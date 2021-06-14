#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  untitled.py
#  
#  Copyright 2012 Christian Klein <chrikle@chrikle-Helmholtz>
#  
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#  MA 02110-1301, USA.


import sys
import gtk, gobject

import matplotlib
matplotlib.use('GTKAgg')
#from mpl_toolkits.basemap import Basemap
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_gtkagg import FigureCanvasGTKAgg as FigureCanvas
from  matplotlib.dates import num2date,date2num,DateFormatter
import numpy as N
from pylab import *
import datetime
from matplotlib import rc
import datetime


from matplotlib.backends.backend_gtkagg import NavigationToolbar2GTKAgg as NavigationToolbar

import glob

import find_grid_koord
import dump_wrfnc

try:
    from iniparse import ConfigParser
except:
    from ConfigParser import ConfigParser
    
    
from netCDF4 import MFDataset

# Abstract struct class       
class Struct:
    def __init__ (self, *argv, **argd):
        if len(argd):
            # Update by dictionary
            self.__dict__.update (argd)
        else:
            # Update by position
            attrs = filter (lambda x: x[0:2] != "__", dir(self))
            for n in range(len(argv)):
                setattr(self, attrs[n], argv[n])

# Specific class

class struct_station (Struct):
    station = ""
    site_code = ""
    real_lat = 0.0
    real_lon = 0.0
    lat = 0.0
    lon = 0.0
    i = -1
    j = -1    
    dump_filename = ""
    dump_datas = []
    vars = []
    
class struct_wrf_file(Struct):    
    mess_filename = ""
    mess_date_format = "%Y-%m-%d_%H:%M:%S"
    mess_data = []
    stations = []
    
#Liesst *.csv file
# return [header,data]
# header: 1d array mit den Headernamen
# data: 2d array mit Daten, dabei ist die erste Spalte mit den Datumswerten im Datetime Format
def read_CSV_File(filename,dateformat='%Y-%m-%d_%H:%M:%S',spacer=','):
  headers=[]
  datas=[]
  f = open(filename,'r')
  count = 0
  for line in f:
         col = line.split(spacer)
         count = count + 1
         if (count==1):
             for header in col:
                headers.append(header.split('\n')[0])
                data=[]
                datas.append(data)
             continue
         dont_write=False
         for i in range(len(col)):
            d=None
            if i==0:
              try:
                d=datetime.datetime.strptime(col[i],dateformat)
              except:
                #print "Problem with Date: ",col
                dont_write=True
            else:
              try:
                  d=float(col[i])
              except:
                  d="-"
            if dont_write==False:
                datas[i].append(d)
  f.close()  
  return headers,datas  
    
def erase_strings(tim,data,date_start,date_end):    
    tim2 = []
    data2 = []
    for i in range(len(tim)):
        if ((tim[i]=='-') or (data[i]=='-')):
            continue
        if ((tim[i]<date_start) or (tim[i] >date_end)):
            continue
        tim2.append(tim[i])
        data2.append(data[i])
    return [tim2,data2]
    

class c_output:
    def __init__(self,outputstreams=[sys.stdout]):
        self.outputstreams = outputstreams 
        
    def write(self,data):
        for stream in self.outputstreams:
            stream.write(data)
            stream.flush()
            
    def writeln(self,data):
        for stream in self.outputstreams:
            stream.write(data+"\n")
            stream.flush()
        

class cxpn_statistic:
    def __init__(self,ini_filename):
        self.load_ini(ini_filename)
        self.dump_nc_files()
        self.read_mess_values()
        self.show_statistics()
        self.paint_figures()

    def load_ini(self,ini_filename):
        config = ConfigParser()
        config.read(ini_filename)
        stations = config.get("general","names").split(";")
        self.netcdffiles = config.get("general","netcdffile").split(";")        
        self.temp_path = config.get("general","temp_path")
        output = config.get("general","output")
        
        try:
          if (int(config.get("general","use_latex"))==1):
            self.use_Latex=True
          else:
            self.use_Latex=False
        except:
          self.use_Latex=False
          
        try:
          if (int(config.get("general","show_grid"))==1):
            self.show_grid=True
          else:
            self.show_grid=False
        except:
          self.show_grid=False
          
        try:
          if (int(config.get("general","show_legend"))==1):
            self.show_legend=True
          else:
            self.show_legend=False
        except:
          self.show_legend=False
          
        
        
        self.outputstream= open(output,"w")
        self.out = c_output([sys.stdout,self.outputstream])
        
        
        
        self.ncfiles = []
        for files in self.netcdffiles:
            # liste aller netcdf dateien (sortiert):
            files2 = sorted(glob.glob(files.split(",")[0]),key=str.lower)
            self.ncfiles.append(files2)
            #ncfiles = [netcdffile]        
        
        # alle messstationen infos einlesen:
        self.wrfs = []
        
        for wrf_i,netcdffile in enumerate(self.netcdffiles):
            wrf = struct_wrf_file()
            wrf.stations = []            
            for station in stations:
                st = struct_station()
                st.station = station
                st.site_code = config.get(station,"site_code")
                st.real_lat = float(config.get(station,"lat"))
                st.real_lon = float(config.get(station,"lon"))
                
                try:
                   st.i =  int(config.get(station,"i"))
                   st.j =  int(config.get(station,"j"))
                except:
                    st.i = -1
                    st.j = -1
                    
                wrf.mess_filename = config.get(station,"filename")
                try:
                    wrf.mess_date_format = config.get(station,"filename_date_format")
                except:
                    wrf.mess_date_format = "%Y-%m-%d_%H:%M:%S"
                st.vars = config.get(station,"vars").split(";")
            
                # i und j bzw. lat/ lon des gridpunktes   
                self.out.writeln("\nFind Grid Koord for: "+st.site_code+ " " +self.netcdffiles[wrf_i].split(",")[1]+" Lat: "+ str(st.real_lat)+" Lon: " + str(st.real_lon))
                if (st.i==-1) or (st.j==-1):
                    erg = find_grid_koord.get_grid_ij(self.out,self.ncfiles[wrf_i][0],st.real_lat,st.real_lon)
                    st.i = erg[0]
                    st.j = erg[1]
                    st.lat = erg[2]
                    st.lon = erg[3]
                else:
                    st.lat = st.real_lat
                    st.lon = st.real_lon
                st.dump_filename = self.temp_path + "/" + netcdffile.split(",")[1] +'_'+ st.site_code + "_"+ str(st.i) + "_"+ str(st.j)+ ".csv"
                wrf.stations.append(st)            
            self.wrfs.append(wrf)
            
            
    def dump_nc_files(self):
        for wrf_i,wrf in enumerate (self.wrfs):
            station2 = []            
            for station in wrf.stations:
                var_liste = []
                for item in station.vars:
                    var_liste.append(item.split(",")[1])                
                self.out.writeln("Dump: "+station.site_code+ " " +self.netcdffiles[wrf_i].split(",")[1]+" "+str(station.i)+" "+str(station.j)+" "+str(var_liste))
                dump_wrfnc.dump_wrfnc(self.netcdffiles[wrf_i].split(",")[0],station.dump_filename,station.i,station.j,var_liste)            
                station.dump_datas = read_CSV_File(station.dump_filename)
                station2.append(station)
            wrf.stations=station2
        
    def read_mess_values(self):
        wrf2 = []
        for wrf in self.wrfs:
            self.out.writeln("Read: "+ wrf.mess_filename)
            wrf.mess_data = read_CSV_File(wrf.mess_filename,wrf.mess_date_format)
            wrf2.append(wrf)
        self.wrfs=wrf2
        
        
    def paint_figures(self):
        rc('text', usetex=self.use_Latex)
        for wrf_i,wrf in enumerate (self.wrfs):
            for station_number,station in enumerate(wrf.stations):
                for number,value in enumerate(station.vars):
                    [var_mess,var_sim,ylabel] = value.split(",")
                    var_mess_row = -1
                    var_dump_row = -1
                    [dump_data_header,dump_data_data] = station.dump_datas
                                    
                    for index,item in enumerate(dump_data_header):
                        if item==var_sim:
                            var_dump_row=index
                
                    
                        #station.site_code + " " + str(number)
                    fig = plt.figure(number+station_number*100,figsize=(15,6), dpi=100,facecolor='white')
                    if var_dump_row != -1:
                        ax = fig.add_subplot(111)
                        ax.plot(dump_data_data[0],dump_data_data[var_dump_row],label=var_sim+"\_"+self.netcdffiles[wrf_i].split(",")[1])
                        if self.show_legend==True:
                           ax.legend(shadow=False,fancybox=True)
                        ax.xaxis.set_major_formatter(DateFormatter("%d.%m %H:%M"))
                
                    if wrf_i==len(self.wrfs)-1:
                        [mess_data_header,mess_data_data] = wrf.mess_data
                        for index,item in enumerate(mess_data_header):
                            if item==var_mess:
                                var_mess_row=index
                        if var_mess_row != -1:
                            [tim,data] = erase_strings(mess_data_data[0],mess_data_data[var_mess_row],dump_data_data[0][0],dump_data_data[0][len(dump_data_data[0])-1])                
                            ax = fig.add_subplot(111)
                            ax.plot(tim,data,'.',label=var_mess)
                            if self.show_legend==True:
                               ax.legend(shadow=False,fancybox=True)
                            ax.xaxis.set_major_formatter(DateFormatter("%d.%m %H:%M"))
                
                    if (self.show_grid==True):
                       plt.grid()
                    #X-Achsen Datum um 40 Grad Rotieren
                    fig.autofmt_xdate(rotation=40)
                    # Beschriftung x - Achse
                    plt.ylabel(ylabel)                    
                    plt.savefig(station.site_code + " " + str(number)+".png")
        plt.show()
        #plt.savefig("output.pdf")
        
    def show_statistics(self):
        for wrf_i,wrf in enumerate (self.wrfs):
            for station in wrf.stations:
                for number,value in enumerate(station.vars):
                    [var_mess,var_sim,ylabel] = value.split(",")
                    var_mess_row = -1
                    var_dump_row = -1
                    [dump_data_header,dump_data_data] = station.dump_datas                                    
                    for index,item in enumerate(dump_data_header):
                        if item==var_sim:
                            var_dump_row=index
        
                    if var_dump_row == -1:
                        continue
                                                    
                    [mess_data_header,mess_data_data] = wrf.mess_data
                    for index,item in enumerate(mess_data_header):
                        if item==var_mess:
                            var_mess_row=index
                    if var_mess_row == -1:
                        continue
                    [tim,data] = erase_strings(mess_data_data[0],mess_data_data[var_mess_row],dump_data_data[0][0],dump_data_data[0][len(dump_data_data[0])-1])                
                    
                    self.out.writeln("")
                    self.out.writeln("")
                    self.out.writeln(station.site_code+" :")
                    self.out.writeln(self.netcdffiles[wrf_i].split(",")[1]+" :")
                    self.out.writeln(var_sim+" :")                    
                    self.calc_statistics(dump_data_data[0],dump_data_data[var_dump_row],tim,data)
                    
        
    def calc_statistics(self,time1,val1,time2,val2):
        [time_arr,values_arr,mess_arr] = self.calc_statistic_get_values(time1,val1,time2,val2)
        # p= predicted, O = observed
        # N Number of Values
        # MBE = Mean Bias Error
        # RMSE = Root Mean Square Error
        # Mean Average Error
        # ME Model efficiency Index
        # NRMSE Normalized Root Mean Square Error
        P_M_O = values_arr-mess_arr
        P_M_O_Q = P_M_O*P_M_O        
        N = len(P_M_O)
        N_1 = 1.0/N
        O_Mean = np.mean(mess_arr)
        P_M_O_2 = np.abs(values_arr-O_Mean) + np.abs(mess_arr-O_Mean)
        P_M_O_3 = np.sum(P_M_O_2*P_M_O_2)
        
        
        MBE = N_1 * np.sum(P_M_O)
        RMSE = np.sqrt(N_1 * np.sum(P_M_O_Q))
        NRMSE = RMSE / O_Mean
        MAE = N_1 * np.sum(np.abs(P_M_O))
        ME = 1.0 - np.sum(P_M_O_Q) / P_M_O_3
        E = 1.0 - np.sum(P_M_O_Q)/np.sum(mess_arr-O_Mean)
        

        self.out.writeln("MBE: "+str(MBE))
        self.out.writeln("RMSE: "+str(RMSE))
        self.out.writeln("MAE: " + str(MAE))
        self.out.writeln("NRMSE: "+str(NRMSE))
        self.out.writeln("ME: "+str(ME))
        self.out.writeln("E: " + str(E))
        
        

    def calc_statistic_get_values(self,time1,val1,time2,val2):
        time_arr = np.array([])
        values_arr = np.array([])
        mess_arr = np.array([])
        i3=0
        for i in range(len(time1)):
            for i2 in range(i3,len(time2)):
               if time1[i] == time2[i2]:
                   i3=i2+1       
                   time_arr  = np.append(time_arr,time1[i])
                   values_arr = np.append(values_arr,val1[i])
                   mess_arr = np.append(mess_arr,val2[i2])
                   break
        return [time_arr,values_arr,mess_arr]
                        
     
        
                            
            
            
            
        

if __name__ == "__main__":
    if len(sys.argv)<2:
        print "Parameter fehlt:"
        print "create_project_from_netcdf.py cfgfile"
    else:
        ini_file=sys.argv[1]
        pr = cxpn_statistic(ini_file)
       

