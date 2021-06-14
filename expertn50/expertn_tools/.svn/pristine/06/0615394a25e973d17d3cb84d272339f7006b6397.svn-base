#!/usr/bin/env python

import gtk, gobject
import os
from string import *
import datetime
import time
import numpy as np
import math


#Liesst *.cdb file
# return [header,data]
# header: 1d array mit den Headernamen
# data: 2d array mit Daten, dabei ist die erste Spalte mit den Datumswerten im Datetime Format
def read_CDB_File(filename):
  headers=[]
  datas=[]
  f = open(filename,'r')
  count = 0
  for line in f:
         col = line.split(',')
         count = count + 1
         if (count==1):
            for header in col:
                headers.append(header.split('\n')[0])
                data=[]
                datas.append(data)
            continue       
         for i in range(len(col)):
            d=0.0               
            if i==0:
                        try:
                            d=datetime.datetime.fromtimestamp(float(col[i]))
                        except:
                            try:
                                d=float(col[i])
                            except:
                                d=0.0
            else:
                        try:                        
                            d=float(col[i])
                        except:
                            d=0.0              
            datas[i].append(d)
  f.close()  
  return headers,datas
  
#Liesst *.csv file
# return [header,data]
# header: 1d array mit den Headernamen
# data: 2d array mit Daten, dabei ist die erste Spalte mit den Datumswerten im Datetime Format
def read_CSV_File(filename,dateformat,spacer):
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
         for i in range(len(col)):
            d=0.0
            if i==0:
              d=datetime.datetime.fromtimestamp(time.mktime(time.strptime(col[i],dateformat)))
            else:
              d=float(col[i])              
            datas[i].append(d)
  f.close()  
  return headers,datas  
  
#Schreibt *.csv file
# return [header,data]
# header: 1d array mit den Headernamen
# data: 2d array mit Daten, dabei ist die erste Spalte mit den Datumswerten im Datetime Format
def write_CSV_File(header,data,filename,dateformat,spacer):
  f = open(filename,'w')
  S = ""
  for head in header:
     if S == "":
        S = head
     else:
        S = S +spacer +head
  f.write(S)
  f.write("\n")
  for i2 in range(len(data[0])):
      S=""
      for i in range(len(data)):
        if i==0:
           S= time.strftime(dateformat,data[i][i2].timetuple())         
        else:
           S=spacer+"%e"  % data[i][i2]
        f.write(S)
        if i== len(data)-1:
          f.write("\n")
          
#Schreibt *.cdb file
# return [header,data]
# header: 1d array mit den Headernamen
# data: 2d array mit Daten, dabei ist die erste Spalte mit den Datumswerten im Datetime Format
def write_CDB_File(header,data,filename):
  f = open(filename,'w')
  S = ""
  for head in header:
     if S == "":
        S = head
     else:
        S = S +"," +head
  f.write(S)
  f.write("\n")
  for i2 in range(len(data[0])):
      S=""
      for i in range(len(data)):
        if i==0:
           S= str(time.mktime(data[i][i2].timetuple()))
        else:
           S=",%e"  % data[i][i2]
        f.write(S)
        if i== len(data)-1:
          f.write("\n")          
          
         
class MainWindow:
    def __init__(self):
        builder = gtk.Builder()
        builder.add_from_file("xpnhelper.ui")
        self.window = builder.get_object("window")
        self.filechooserbuttonDirectory = builder.get_object("filechooserbuttonDirectory")
        self.entryClassName = builder.get_object("entryClassName")
        self.ProgDir = os.path.abspath(".") 
        builder.connect_signals(self)
        self.builder = builder
     
    def __getitem__(self,key):
        return self.builder.get_object(key)   
     
    def on_window_destroy(self,widget, data=None):
        self.window.destroy()
        gtk.main_quit()
        return True
    
    def create_file_from_Template(self,dest_file,src_file,ClassName):
        ClassName_uppercase = upper(ClassName) 
        className_lowercase = lower(ClassName)
        f_src = open(src_file,'r')
        f_dest = open(dest_file,'w')
        for line in f_src:
            s = Template(line)
            s2 = s.safe_substitute(templ_uppercase=ClassName_uppercase,template=className_lowercase)
            f_dest.write(s2)
            
    def create_class(self):         
        ClassName = self.entryClassName.get_text()
        ClassDir = self.filechooserbuttonDirectory.get_current_folder()
        TemplateDir = self.ProgDir + '/template'
        
        dest_file = ClassDir + '/' + lower(ClassName) + '.h'
        src_file = TemplateDir + '/' + 'template.h'
        self.create_file_from_Template(dest_file,src_file,ClassName)
        
        dest_file = ClassDir + '/' + lower(ClassName) + '.c'
        src_file = TemplateDir + '/' + 'template.c'
        self.create_file_from_Template(dest_file,src_file,ClassName)
        
        dest_file = ClassDir + '/' + lower(ClassName) + '_register_modul.c'
        src_file = TemplateDir + '/' + 'template_register_modul.c'
        self.create_file_from_Template(dest_file,src_file,ClassName)   
        return 0
    
    def on_create(self,data=None):
        Tool =  self['notebook_Tool'].get_current_page()
        ret = 0
        if Tool==1:
            ret = self.create_class()
        elif Tool==0:
            ret=self.convert_file()
        elif Tool==2:
            ret=self.run_script()
        if ret==0:                
          self['dialogSuccess'].run()
          self['dialogSuccess'].hide()
        return
        
    def  convert_file(self):
        #print self['comboboxentryDateSource'].get_active_text()
        #print "convert file"
        sourcefile = self["entry_SourceFile"].get_text()
        targetfile = self["entry_TargetFile"].get_text()
        if sourcefile==None or sourcefile=="" or targetfile==None or targetfile=="": return 1
        srcindex = self['combobox_Source_File_Format'].get_active()
        trgindex = self['combobox_Taget_File_Format'].get_active()
        data=[]
        header=[]
        if srcindex==0:
           [header,data] = read_CDB_File(sourcefile)
        elif srcindex==1:
            [header,data] = read_CSV_File(sourcefile,self['comboboxentryDateSource'].get_active_text(),',')
        if trgindex==0:
           write_CDB_File(header,data,targetfile)
        elif trgindex==1:
           write_CSV_File(header,data,targetfile,self['comboboxentryDateTarget'].get_active_text(),',')
        return 0
        
    def change_source_format(self,widget):
       index = self['combobox_Source_File_Format'].get_active()       
       if index==0:
          self['frameOption_Source'].set_sensitive(False)
       elif index==1:
          self['frameOption_Source'].set_sensitive(True)
          
    def change_target_format(self,widget):
       index = self['combobox_Taget_File_Format'].get_active()       
       if index==0:
          self['frameOption_Target'].set_sensitive(False)
       elif index==1:
          self['frameOption_Target'].set_sensitive(True)
          
    def on_convert_open_source(self,data=None):
       id = self["filechooserdialogOpen"].run()
       if id ==1:
            suffix=""
            if (self['combobox_Source_File_Format'].get_active()==0):
                suffix="cdb"
            else:
                suffix="csv"
            text = self["filechooserdialogOpen"].get_filename()
            if text.split(".")[len(text.split("."))-1] != suffix:
                text=text+"."+suffix
            self["entry_SourceFile"].set_text(text)
       self["filechooserdialogOpen"].hide()
          
    def on_convert_open_target(self,data=None):
       id = self["filechooserdialogSave"].run()
       if id ==1:
            suffix=""
            if (self['combobox_Taget_File_Format'].get_active()==0):
                suffix="cdb"
            else:
                suffix="csv"
            text = self["filechooserdialogSave"].get_filename()
            if text.split(".")[len(text.split("."))-1] != suffix:
                text=text+"."+suffix
            self["entry_TargetFile"].set_text(text)
       self["filechooserdialogSave"].hide()

    def run_script(self):
        print "run Script"
        return 0
        
if __name__ == "__main__":
    Mwindow = MainWindow()
    Mwindow.window.show()
    gtk.main()
