#!/usr/bin/env python

from netCDF4 import MFDataset
from netCDF4 import Dataset
import numpy as np
import time
import datetime
import glob
import sys

# T = Temperature in Grad C
# return vapor pressure of water [hPa]
def get_e(T):
  a0 = 6.10779961
  a1 = 4.436518521e-1
  a2 = 1.428945805e-2
  a3 = 2.650648471e-4
  a4 = 3.031240396e-6
  a5 = 2.034080948e-8
  a6 = 6.136820929e-11
  return a0 + T * (a1 + T*(a2 + T*(a3 + T*(a4 + T * (a5 + T * a6)))))
  
# q = spez. humidity [kg/kg]
# P = Pressure [hPa]
#return P_H20 [hPa]
def get_Ph20(q,P):
  M_dry = 28.9644
  M_h20 = 18.01534
  xh20 = - M_dry * q / ((q-1.0) * M_h20 - M_dry*q)
  return xh20 * P
  
# q = spez. humidity [kg/kg]
# T = Temperature [Grad C]
# P = Pressure [hPa]
# return RH [%]
def calc_rel_hum(q,T,P):
   PH20 = get_Ph20(q,P)
   ret = PH20 / get_e(T) * 100.0
   ret = min(ret,100.0)
   ret = max(ret,0.0)
   return ret 


def dump_wrfnc(ncfile2,outputfile,nc_i,nc_j,vars):

    ncfiles = sorted(glob.glob(ncfile2),key=str.lower)

    f = open(outputfile,"w")
    S='Time and Date'
    vars2 = []
    for i in range(len(vars)):
        if vars[i]!="-":
            vars2.append(vars[i])
    vars = vars2
    for i in range(len(vars)):
      S=S+','+vars[i]

    f.write(S+'\n')

    for ncfile in ncfiles:
      nc = Dataset(ncfile,'r')

      #schauen ob vars existieren:
      for var in vars:
        try:
            S = var.split(":")
            #print S
            #print var,": ",nc.variables[S[0]].shape
        except:
            print "Variable ",var," existiert nicht oder dimension ist falsch!"
            return

      times2 = nc.variables["Times"][:]

      times = []
      for i2 in range(len(times2)):
        S = ''
        for ch in times2[i2]:
          if ch=="_":
            ch=" "
          S = S+ch
        times.append(S)



      for i2 in range(len(times)):
        f.write(times[i2])
        for i in range(len(vars)):
            S = vars[i].split(":")
            if len(S) < 2:
                if S[0] == "T2":
                    f.write(','+str(nc.variables[S[0]][i2,nc_j,nc_i]-273.15))
                elif S[0] == "RH":
                    rh = calc_rel_hum(nc.variables["Q2"][i2,nc_j,nc_i],nc.variables["T2"][i2,nc_j,nc_i]-273.15,nc.variables["PSFC"][i2,nc_j,nc_i]/100.0)
                    f.write(','+str(rh))
                else:
                    f.write(','+str(nc.variables[S[0]][i2,nc_j,nc_i]))
            else:
                k = int(S[1])
                f.write(','+str(nc.variables[S[0]][i2,k,nc_j,nc_i]))

        f.write('\n')



if __name__ == "__main__":
    if len(sys.argv)<6:
     print "Parameter fehlt:"
     print "dump_wrfnc.py netcdfile outputfile i j var1 var2 ..."
    else:
     dump_wrfnc(sys.argv[1],sys.argv[2],int(sys.argv[3]),int(sys.argv[4]),sys.argv[5:])






