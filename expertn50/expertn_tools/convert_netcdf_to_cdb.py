#!/usr/bin/env python

#from Scientific.IO.NetCDF import *
import numpy as np
from netCDF4 import MFDataset
import sys
import datetime
import time

def get_date_from_netcdf(Time_var):
    year = int(Time_var[0])*1000+int(Time_var[1])*100+int(Time_var[2])*10+int(Time_var[3])
    month = int(Time_var[5])*10+int(Time_var[6])
    day = int(Time_var[8])*10+int(Time_var[9])
    hour = int(Time_var[11])*10+int(Time_var[12])
    minute = int(Time_var[14])*10+int(Time_var[15])
    second = int(Time_var[17])*10+int(Time_var[18])
    return datetime.datetime(year=year, month=month, day=day, hour=hour, minute=minute, second=second)

def rm_komma(name):
  S = name.split(',')
  S2 = ""
  i=0
  for item in S:
        if i == 0:
          S2 = item
        else:
          S2 = S2 + ";" + item
        i = i+1
  return S2

def convert_grid_ij(netcdf_file,cdbfile,pos_i,pos_j):
  #file = NetCDFFile(netcdf_file,'r')
  file =MFDataset(netcdf_file)
  varlistnames = []
  cdbnames = []
  i = 0
  for item in file.variables:
     #if (i % 2) == 0:
       S = ""
       try:
          S2 = rm_komma(file.variables[item].units)
          if S2 != "" :
             S = S+"{" + S2 + "} "
       except:
          S2 = ""
       try:
          S3 = rm_komma(file.variables[item].description)
          S = S+'  -  ' +S3
       except:
          S3 = ""
       cdbnames.append(S)
       varlistnames.append(item)
       #print S
     #print item
       i=i+1
#  var = file.variables["Q2"]
  Times =  file.variables["Times"]
  z_size = Times.shape[0]
  i = 0
  file2 = open(cdbfile,"w")
  while (1):
      if i ==0:
          S = "Time"
      else:
          S = str(time.mktime((get_date_from_netcdf(Times[i-1])).timetuple()))
#             S = str(get_date_from_netcdf)
      i4 = 0
      for item in varlistnames:
          if  len(file.variables[item].shape)==3:
               if i == 0:
                 S = S+','+ item + ' ' + cdbnames[i4]
               else:
                 var = file.variables[item]
                 #print var[0]
                 #print item
                 #print len(var.shape)
                 S = S + ',' + str(file.variables[item] [i-1][pos_j][pos_i])
                 #if item=="XLAT":
                     #print item,file.variables[item] [i-1][pos_j][pos_i]
          i4=i4+1
      i4 = 0
      for item in varlistnames:
          if len(file.variables[item].shape)==4:
               for i2 in range((file.variables[item].shape[1])):
                  if i == 0:
                     S = S + ',' + item + '[' + str(i2)+'] ' + cdbnames[i4]
                  else:
                     #print i2
                     #print file.variables[item] [i-1][pos_j][pos_i][34]
                     #print len(file.variables[item] [i-1][pos_j][pos_i]),(file.variables[item].shape[3])
                     #print len(file.variables[item] [i-1][pos_j])
                    # print item,i-1,pos_j,pos_i,i2
                     S = S + ',' + str(file.variables[item] [i-1][i2][pos_j][pos_i])
          i4=i4+1
      S = S + "\n"
      file2.write(S)
      print float(i)/float(z_size)
      i = i+1
      if i >= z_size: break

#  print len(var.shape)

def convert_grid_ij_2(netcdf_file,cdbfile,pos_i,pos_j):
    netf =MFDataset(netcdf_file)
    netf_time2 = netf.variables["Times"][:,:]
    if np.ma.getmask(netf_time2[0])==True:
        print "No Data availible!"
    netf_time = np.array([])
    for i in range(len(netf_time2)):
        netf_time = np.append(netf_time,time.mktime((get_date_from_netcdf(netf_time2[i])).timetuple()))

    netf_variables_str = []
    netf_variables = []
    netf_variables_str.append("pTi.pSimTime.fTimeY")
    netf_variables.append(netf_time)

    number_var = len(netf.variables)
    i=0
    for item in netf.variables:
        if (item == "Times"):
            continue
        if len(netf.variables[item].shape)==3:
            var = netf.variables[item][:,pos_j,pos_i]
            if np.ma.getmask(var[0])==False:
                netf_variables_str.append(item)
                netf_variables.append(var)
        if len(netf.variables[item].shape)==4:
            for k in range(netf.variables[item].shape[1]):
                var = netf.variables[item][:,k,pos_j,pos_i]
                if np.ma.getmask(var[0])==False:
                    netf_variables_str.append(item+"["+str(k)+"]")
                    netf_variables.append(var)
        i=i+1
        print float(i)/float(number_var), " of vars read!"
    # write cdb file
    file2 = open(cdbfile,"w")
    header_str=""
    for item in netf_variables_str:
        if header_str=="":
            header_str=item
        else:
            header_str=header_str+","+item

    file2.write(header_str+"\n")
    len_var = len(netf_variables[0])
    for i in range(len_var):
        S = ""
        for item in netf_variables:
            if S=="":
                S=str(item[i])
            else:
                S=S+","+str(item[i])
        file2.write(S+"\n")
        print float(i)/float(len_var), " written!"





def convert_grid_xpn_output_ij(netcdf_file,cdbfile,pos_i,pos_j,pos_k):
    netf =MFDataset(netcdf_file)
    #netf_time = netf.variables["pTi.pSimTime.fTimeY"][:,pos_k,pos_j,pos_i]
    netf_time2 = netf.variables["Times"][:,:]
    if np.ma.getmask(netf_time2[0])==True:
        print "No Data availible!"
    netf_time = np.array([])
    for i in range(len(netf_time2)):
        netf_time = np.append(netf_time,time.mktime((get_date_from_netcdf(netf_time2[i])).timetuple()))
    if np.ma.getmask(netf_time[0])==True:
        print "No Data availible at (i,j): (",pos_i,",",pos_j,")"
    netf_variables_str = []
    netf_variables = []
    netf_variables_str.append("pTi.pSimTime.fTime")
    netf_variables.append(netf_time)
    number_var = len(netf.variables)
    i=0
    for item in netf.variables:
        if (item == "pTi.pSimTime.fTime"):
            continue
        if len(netf.variables[item].shape)==4:
            var = netf.variables[item][:,pos_k,pos_j,pos_i]
            if np.ma.getmask(var[0])==False:
                netf_variables_str.append(item)
                netf_variables.append(var)
        i=i+1
        print float(i)/float(number_var), " of vars read!"

    # write cdb file
    file2 = open(cdbfile,"w")
    header_str=""
    for item in netf_variables_str:
        if header_str=="":
            header_str=item
        else:
            header_str=header_str+","+item

    file2.write(header_str+"\n")
    len_var = len(netf_variables[0])
    for i in range(len_var):
        S = ""
        for item in netf_variables:
            if S=="":
                S=str(item[i])
            else:
                S=S+","+str(item[i])
        file2.write(S+"\n")
        print float(i)/float(len_var), " written!"





if __name__ == "__main__":
   #print sys.argv,len(sys.argv)
   if len(sys.argv)<5:
     print "Parameter fehlt:"
     print "find_gird_koord.py netcdfile cdbfile i j k"
     print "k: -1 = WRF output file\n else : Expert-N output file"
   else:
       if int(sys.argv[5])==-1:
           convert_grid_ij_2(sys.argv[1],sys.argv[2],int(sys.argv[3]),int(sys.argv[4]))
       else:
           convert_grid_xpn_output_ij(sys.argv[1],sys.argv[2],int(sys.argv[3]),int(sys.argv[4]),int(sys.argv[5]))


