#!/usr/bin/env python
# -*- coding: utf-8 -*-
import time
#from datetime import datetime
import datetime
import string
import gtk, gobject
from gtk import *
import tempfile
import subprocess
import os
import getopt


from numpy import *
import numpy as np

from matplotlib.figure import Figure
import matplotlib.mlab as mlab
from  matplotlib.dates import num2date,date2num,DateFormatter
from numpy import arange, sin, pi
import ConfigParser
from threading import Thread
import thread
import datetime
from pylab import *
#from datetime import datetime

window_instances=0

from xpnstatistic import xpnstatistic

lock = thread.allocate_lock()

# uncomment to select /GTK/GTKAgg/GTKCairo
#from matplotlib.backends.backend_gtk import FigureCanvasGTK as FigureCanvas
from matplotlib.backends.backend_gtkagg import FigureCanvasGTKAgg as FigureCanvas
#from matplotlib.backends.backend_gtkcairo import FigureCanvasGTKCairo as FigureCanvas

# or NavigationToolbar for classic
#from matplotlib.backends.backend_gtk import NavigationToolbar2GTK as NavigationToolbar
from matplotlib.backends.backend_gtkagg import NavigationToolbar2GTKAgg as NavigationToolbar

def get_varname_withoutpath(varname):
   varname2 = varname.split(".")
   return varname2[len(varname2)-1]

def isNaN(x):
    return (x!=x)

def isInfinite(x):
    inf=1e10000
    return not(x not in (inf,-inf))

def get_formated_date(tim2):
    str1 = "%04d" % tim2.year + "-" + "%02d" % tim2.month + "-" + "%02d" % tim2.day + " " + "%02d" % tim2.hour + ":" + "%02d" % tim2.minute + ":" + "%02d" % tim2.second
    return str1

##class ThreadExternalProg(Thread):
##    def __init__(self,process,filename,plotwin,window):
##        Thread.__init__(self)
##        self.process = process
##        self.filename = filename
##        self.plotwin = plotwin
##        self.window = window
##        self.lock=thread.allocate_lock()
##    def run(self):
##        self.process.wait()
##        print "ja ja"
##        self.
##        try:
##            self.plotwin.open_file(self.filename)
##        except:
##            print "Error open file"
##        print "non no"
##        self.lock.release()
##        os.remove (self.filename)
##        print "Exit Thread"
##

#Liesst *.cdb file header
#return header: 1d array mit den Headernamen
def read_cdb_header(filename):
  headers=[]
  f = open(filename,'rU')
  count = 0
  for line in f:
        col = line.split(',')
        count = count + 1
        if (count==1):
             for header in col:
                headers.append(header.rstrip())
        break
  f.close()
  #print headers
  return headers

def get_filename_without_path(name):
    S2 = name
    S = S2.split("/")
    S2 = S[len(S)-1]
    S = S2.split("\\")
    S2 = S[len(S)-1]
    return S2

def convert_sec_to_str(sec):
    time_struc = time.localtime(sec)
    if (time_struc.tm_mon<10):
        mon_str="0"+str(time_struc.tm_mon)
    else:
        mon_str=str(time_struc.tm_mon)
    if (time_struc.tm_mday<10):
        mday_str = "0"+str(time_struc.tm_mday)
    else:
        mday_str = str(time_struc.tm_mday)
    if (time_struc.tm_hour<10):
        hour_str="0"+str(time_struc.tm_hour)
    else:
        hour_str=str(time_struc.tm_hour)
    if time_struc.tm_min<10:
        min_str = "0"+str(time_struc.tm_min)
    else:
        min_str = str(time_struc.tm_min)
    if time_struc.tm_sec<10:
        sec_str = "0"+str(time_struc.tm_sec)
    else:
        sec_str = str(time_struc.tm_sec)
    time_str = str(time_struc.tm_year)+"."+ mon_str+"."+mday_str+" "+hour_str+"."+min_str+"."+sec_str
    return time_str

def get_indices_of_date_range(start_date,end_date,data):
        start_date2 = matplotlib.dates.date2num(start_date)
        end_date2 = matplotlib.dates.date2num(end_date)

        start_date_i=0
        end_date_i=-1
        for i,act_date in enumerate(data[0]):
            act_date2 = matplotlib.dates.date2num(act_date)
            #if (act_date2 >= start_date2) and (start_date_i == -1):
            if (act_date2 <= start_date2):
                start_date_i = i
                if start_date_i<0:
                    start_date_i=0
            if (act_date2 > end_date2):
                end_date_i = i
                if end_date_i > len(data[0])-1:
                    end_date_i=len(data[0])-1
                return [start_date_i,end_date_i]
        end_date_i=len(data[0])
        return [start_date_i,end_date_i]
        #print "ERROR: reached end date !"
        #if start_date_i==-1:
        #    start_date_i=0
        #end_date_i=len(data[0])
        #return [start_date_i,end_date_i]

def convert_str_to_datetime(S):
    S2 = S.split(" ")
    datearr = []
    timearr = []
    for S3 in S2[0].split("."):
        datearr.append(int(S3))
    for S3 in S2[1].split(":"):
        timearr.append(int(S3))
    d = datetime.datetime(year=datearr[0],month=datearr[1],day=datearr[2],hour=timearr[0],minute=timearr[1],second=timearr[2])
    return d

# xpn Data Klasse:
# Auslesen von Dateien im xno, csv und cdb Format
# z.b:
# noah_output = xpn_data("noah_output.csv",dateformat='%Y-%m-%d %H:%M:%S')
# out = noah_output["Times","SMC:0"]
#output = xpn_data("noah_output.csv",dateformat='no date') falls erste Spalte kein Datum ist
class xpn_data:
    def __init__(self,filename,dateformat='%Y-%m-%d %H:%M:%S',spacer=',',read_vars=None,silent=False,merge_delimiters=False):
        self.silent = silent
        if dateformat=="xno":
            self.csv_data = self.read_XNO_File(filename,read_vars,merge_delimiters)
        elif dateformat=="cdb":
            self.csv_data = self.read_CDB_File(filename,read_vars)
        else:
            self.csv_data = self.read_CSV_File(filename,dateformat,spacer,read_vars,merge_delimiters)

    def rem_more_then_one_ch(self,str1,ch1):
        a = ''
        last_is_space=False
        for ch in str1:
            if (ch==ch1) and (last_is_space==False):
                a=a+ch
                last_is_space=True
            if ch!=ch1:
                a=a+ch
                last_is_space=False
        return a

    def read_XNO_File(self,filename,read_vars=None,merge_delimiters=False):
        return self.read_CSV_File(filename,dateformat='%Y-%m-%d %H:%M:%S',spacer=',',merge_delimiters=merge_delimiters)

    #Liesst *.csv file
    # return [header,data]
    # header: 1d array mit den Headernamen
    # data: 2d array mit Daten, dabei ist die erste Spalte mit den Datumswerten im Datetime Format
    def read_CSV_File(self,filename,dateformat='%Y-%m-%d %H:%M:%S',spacer=',',read_vars=None,merge_delimiters=False):
      headers=[]
      datas=[]
      f = open(filename,'rU')
      count = 0
      for line in f:
             if merge_delimiters:
                 line=self.rem_more_then_one_ch(line,spacer)
             col = line.split(spacer)
             count = count + 1
             if (count==1):
                 for header in col:
                    headers.append(header.rstrip())
                    data=[]
                    datas.append(data)
                 continue
             dont_write=False
             for i in range(len(col)):
                d=None
                if (i==0) and (dateformat!="no date"):
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
      return [headers,datas]

    #Liesst *.cdb file
    # return [header,data]
    # header: 1d array mit den Headernamen
    # data: 2d array mit Daten, dabei ist die erste Spalte mit den Datumswerten im Datetime Format
    def read_CDB_File(self,filename,read_vars=None):
      headers=[]
      datas=[]
      f = open(filename,'rU')
      count = 0
      for line in f:
             col = line.split(',')
             count = count + 1
             if (count==1):
                 for header in col:
                    headers.append(header.rstrip())
                    data=[]
                    datas.append(data)
                 continue
             for i in range(len(col)):
                d=0.0
                if i==0:
                  d=datetime.datetime.fromtimestamp(float(col[i]))
                else:
                  d=float(col[i])
                datas[i].append(d)
      f.close()
      return headers,datas

    def get_len(self):
        [header,data] = self.csv_data
        return len(header)

    def get_header(self):
        [header,data] = self.csv_data
        return header

    def get_data(self):
        [header,data] = self.csv_data
        return data


    def get_indices_of_date_range(self,start_date,end_date,data):
        start_date2 = matplotlib.dates.date2num(start_date)
        end_date2 = matplotlib.dates.date2num(end_date)
        start_date_i=-1
        end_date_i=-1
        for i,act_date in enumerate(data[0]):
            act_date2 = matplotlib.dates.date2num(act_date)
            if (act_date2 >= start_date2) and (start_date_i == -1):
                start_date_i = i
            if (act_date2 >= end_date2) and (end_date_i == -1):
                end_date_i = i
                return [start_date_i,end_date_i]
        print "ERROR: reached end date !"
        if start_date_i==-1:
            start_date_i=0
        end_date_i=len(data[0])
        return [start_date_i,end_date_i]

    def get_average_of_time_range(self,start_date,end_date,data):
        [i1,i2] = self.get_indices_of_date_range(start_date,end_date,data)
        data2 = []
        if i1==i2 or i2 > len(data[0])-1:
            for i,item in enumerate(data):
                data2.append(data[i][i1])
            return data2

        start = matplotlib.dates.date2num(data[0][i1])
        stop = matplotlib.dates.date2num(data[0][i2])
        time_span = stop-start
        i3 = 0
        for item in data:
            data2.append(0.0)
        for i in range(i1,i2):
            for i2 in range(len(data)):
                diff = matplotlib.dates.date2num(data[0][i+1])-matplotlib.dates.date2num(data[0][i])
                dat=0.0
                if i2==0:
                    dat =  matplotlib.dates.date2num(data[i2][i])
                else:
                    dat = data[i2][i]
                data2[i2] = data2[i2] + dat*diff/time_span
            i3=i3+1

        data2[0] = matplotlib.dates.num2date(data2[0])
        return data2

    # example:
    #out = xpn_data("ck_CH-FRU_2010_flx_met.xno")
    #dat = out.get_averaged_data(("Time and Date","P 1"),datetime.datetime(year=2010,month=1,day=1),datetime.datetime(year=2010,month=1,day=5),datetime.timedelta(days=1))

    def get_averaged_data(self,keys,start_date,end_date,delta):
        data = self.get_items_range(keys,start_date,end_date+datetime.timedelta(milliseconds=5))
        act_date=start_date
        act_date2=start_date+delta

        new_data=[]
        for i in range(len(data)):
            dat = []
            new_data.append(dat)

        while (1):
            new_data_row=self.get_average_of_time_range(act_date,act_date2,data)
            for i2,item in enumerate(new_data_row):
                new_data[i2].append(item)
            if act_date2>=end_date:
                break
            act_date=act_date+delta
            act_date2=act_date2+delta
            if act_date2>end_date:
                act_date2=end_date

        # new_data in numpy format verwandeln:
        new_data2 = []
        for item in new_data:
            new_data2.append(np.array(item))
        return new_data2





    def add_data_rows(self,time_and_date,data_array,header_array):
         [header,data] = self.csv_data
         size = len(data[0])
         data_new=[]
         for i in range(size):
             if i == 0:
                 print "to do"

    def get_items(self,keys):
        [headers,datas] = self.csv_data
        if isinstance(keys,type(()))==False:
            keys=keys,
        keynumbers=[]
        for key in keys:
            for i,item in enumerate(headers):
                if isinstance(item,type(key)):
                    if item==key:
                        keynumbers.append(i)
                        break
                else:
                    if key==i:
                        keynumbers.append(i)
                        break
        if len(keys)!=len(keynumbers):
            if self.silent==False:
                print "Key(s) could not be found: ", keys
            return None


        # find values with no value:
        no_numbers = []
        no_numbers_mark = np.zeros(len(datas[keynumbers[0]]),bool)
        for keynumber in keynumbers:
            for i, item in enumerate(datas[keynumber]):
                if isinstance(item,type("-")):
                    no_numbers.append(i)
                    no_numbers_mark[i]=True

        if len(no_numbers)==0:
            ret=[]
            if len(keynumbers)==1:
                return np.array(datas[keynumbers[0]])
            for keynumber in keynumbers:
                ret.append(np.array(datas[keynumber]))
            return ret
        else:
            ret=[]
            if len(keynumbers)==1:
                return np.array(datas[keynumbers[0]])
            for keynumber in keynumbers:
                data = []
                for i, item in enumerate(datas[keynumber]):
                    if no_numbers_mark[i]==False:
                        data.append(item)
                ret.append(np.array(data))
            return ret

    def get_items_range(self,keys,start_date,end_date):
        data = self.get_items(keys)
        [i_from,i_to] = self.get_indices_of_date_range(start_date,end_date,data)
        data2 = []
        for item in data:
            data2.append(item[i_from:i_to])
        return data2


    def __getitem__(self,keys):
        return self.get_items(keys)

class c_dialog_statistic_help():
    def __init__(self,builder):
        self.builder=builder
        self.dialog_statistic_help = self["dialog_statistic_help"]
        self.temp_width = 0
        viewport_image = self['scrolled_image']
        image = gtk.Image()
        image.set_from_file('statistic.png')        
        self.pixbuf = image.get_pixbuf()
        org_width=self.pixbuf.get_width()
        org_height=self.pixbuf.get_height()
        image.connect('expose-event', self.on_image_resize, viewport_image,org_width,org_height)
        viewport_image.add(image)
        viewport_image.set_size_request(org_width, org_height)
        viewport_image.show_all()
        
    def on_image_resize(self, widget, event, window,org_width,org_height):
        allocation = widget.get_allocation()
        if self.temp_width != allocation.width:
            self.temp_width = allocation.width
            allocation.height = self.temp_width*org_height/org_width     
            pixbuf = self.pixbuf.scale_simple(allocation.width, allocation.height, gtk.gdk.INTERP_BILINEAR)
            widget.set_from_pixbuf(pixbuf)
        
    def show(self):
        self.dialog_statistic_help.show()
        
    def hide(self):
        self.dialog_statistic_help.hide()
        
    
        
    
    def __getitem__(self,key):
        return self.builder.get_object(key)
    


class PlotWindow(gtk.VBox):
    def __init__(self,main_class,window,figure_count,filename,liststore,externspreadsheet,open_whole_file=False,x_axis_choice=0,x_axis_choice_name="Time",messfilelist=None):
        gtk.VBox.__init__(self)
        self.Figure_Count = figure_count
        self.filename = filename
        self.externspreadsheet = externspreadsheet
        self.x_axis_choice=x_axis_choice
        self.x_axis_choice_name = x_axis_choice_name
        iter = liststore.get_iter_first()
        First = liststore.get_value(iter,1)
        self.messfilelist = messfilelist
        self.Parentwindow = window
        self.main_class = main_class
                

        if self.main_class.x_axis_automatic==True:
            self.x_axis_automatic=True
        else:
            self.x_axis_automatic=False
            self.xFrom=self.main_class.xFrom
            self.xTo=self.main_class.xTo

        if self.main_class.y_axis_automatic==True:
            self.y_axis_automatic=True
        else:
            self.y_axis_automatic=False
            self.yFrom=self.main_class.yFrom
            self.yTo=self.main_class.yTo

        self.grid_on=self.main_class.grid_on
        self.angle=self.main_class.angle
        self.linestyle=self.main_class.linestyle



        builder = gtk.Builder()
        builder.add_from_file("xnoplotWindow.ui")
        builder.connect_signals(self)
        self.builder =  builder
        self.dialog_statistic_help = c_dialog_statistic_help(self.builder)
        count2 = 0
        self.valuelist = []
        readingerror=0
        if open_whole_file==False:
            if ".xno" in self.filename or ".cdb" in self.filename:
              if ".xno" in self.filename:
                  self.dateformat = "xno"
              if ".cdb" in self.filename:
                  self.dateformat = "cdb"
              if self.x_axis_choice==0:
                self.xachseformat = 0
              else:
                self.xachseformat = 0
              # alle namen rauslesen:
              plot_names=[]

              iter = liststore.get_iter_first()
              i = 0
              while 1:
                if iter is None:
                        break
                if i==self.x_axis_choice:
                  self.x_axis_choice_name=liststore.get_value(iter,1)
                akt = liststore.get_value(iter,0)
                if akt==True:
                  S = liststore.get_value(iter,1)
                  plot_names.append(S)
                iter=liststore.iter_next(iter)
                i=i+1

              xpn_values = xpn_data(self.filename,self.dateformat)

              self.valuelist = []
              for i, item in enumerate(plot_names):
                values = xpn_values[self.x_axis_choice_name,item]
                list2=[]
                list2.append(values[0])
                list2.append(values[1])
                list2.append(item)
                self.valuelist.append(list2)
        else:
            self.open_file(self.filename)
            if self.x_axis_choice==0:
                self.xachseformat = 0
            else:
                self.xachseformat = 0
        self.toolbar = gtk.Toolbar()
        self.toolbar.set_orientation(gtk.ORIENTATION_HORIZONTAL)
        self.toolbar.set_style(gtk.TOOLBAR_ICONS)
        self.toolbar.set_icon_size(2)
        self.toolbar.set_show_arrow(False)
        #self.toolbar.set_resize_mode(True)
        #self.toolbar.set_size_request(True)


        self.legend=False
        self.paint_allowed=False


        #Open Button
        image = gtk.Image()
        image.set_from_stock(gtk.STOCK_OPEN,3)
        close_button = gtk.ToolButton(image,"Open")
        close_button.connect_object("clicked",self.on_open_file,None)
        self.toolbar.insert(close_button,-1)

        #Save Button
        image = gtk.Image()
        image.set_from_stock(gtk.STOCK_SAVE_AS,3)
        close_button = gtk.ToolButton(image,"Save")
        close_button.connect_object("clicked",self.on_save_file,None)
        self.toolbar.insert(close_button,-1)


        #Edit Button
        image = gtk.Image()
        image.set_from_stock(gtk.STOCK_EDIT,3)
        close_button = gtk.ToolButton(image,"Edit")
        close_button.connect_object("clicked",self.on_extern_edit,None)
        self.toolbar.insert(close_button,-1)

        self.toolbar.insert(gtk.SeparatorToolItem(),-1)

        #Second legend:
        image = gtk.Image()
        image.set_from_stock(gtk.STOCK_INFO,3)
        self.legend_button = gtk.ToggleToolButton(None)
        self.legend_button.set_icon_widget(image)
        self.legend_button.connect_object("clicked",self.on_toggle_legend,self.legend_button)
        self.toolbar.insert(self.legend_button,-1)

        #List:
        image = gtk.Image()
        image.set_from_stock(gtk.STOCK_INDEX,3)
        self.list_button = gtk.ToggleToolButton(None)
        self.list_button.set_icon_widget(image)
        self.list_button.connect_object("clicked",self.on_toggle_list,self.list_button)
        self.toolbar.insert(self.list_button,-1)

        #Axen Eigenschaften:
        image = gtk.Image()
        image.set_from_stock(gtk.STOCK_PREFERENCES,3)
        self.axes_button = gtk.ToolButton(None)
        self.axes_button.set_icon_widget(image)
        self.axes_button.connect_object("clicked",self.on_Axis,None)
        self.toolbar.insert(self.axes_button,-1)


        image = gtk.Image()
        image.set_from_stock(gtk.STOCK_ITALIC,3)
        self.axes_button = gtk.ToolButton(None)
        self.axes_button.set_icon_widget(image)
        self.axes_button.connect_object("clicked",self.on_calc_statistics,None)
        self.toolbar.insert(self.axes_button,-1)



        #self.toolbar.insert(gtk.SeparatorToolItem(),-1)

        #Plot Style:
        #comboboxentry = gtk.ComboBoxEntry()
        #liststore = gtk.ListStore(str)
        #comboboxentry.set_model(liststore)
        #liststore.append(['-'])
        #liststore.append(['o'])
        #liststore.append(['o-'])
        #liststore.append(['--'])
        #liststore.append(['.-'])
        #liststore.append(['.'])
        #liststore.append([':'])
        #comboboxentry.set_text_column(0)
        #comboboxentry.child.connect('changed', self.set_linestyle)
        #comboboxentry.set_active(0)
        #self.linestyle='-'
        #toolitem = gtk.ToolItem()
        #vbox = gtk.VBox()
        #vbox.pack_start(comboboxentry,True,True)
        #toolitem.add(vbox)
        #toolitem.set_expand(True)
        #toolitem.set_homogeneous(True)
        #self.toolbar.insert(toolitem,-1)

        combobox = gtk.combo_box_new_text()
        combobox.append_text('x - Automatic')
        combobox.append_text('x - Date')
        combobox.append_text('x - Time')
        combobox.append_text('x - Date and Time')
        combobox.connect('changed', self.set_xAchse)

        self.toolbar.insert(gtk.SeparatorToolItem(),-1)

        combobox.set_active(self.xachseformat)
        toolitem = gtk.ToolItem()
        toolitem.add(combobox)
        toolitem.set_expand(True)
        toolitem.set_homogeneous(True)
        self.toolbar.insert(toolitem,-1)

        self.vpaned = gtk.VPaned()
        self.PlotBox = gtk.VBox()
        self.vpaned.pack1(self.PlotBox,True,True)
        self.list = self.create_list()
        self.vpaned.pack2(self.list,False,True)


        vbox = gtk.VBox()
        vbox.pack_start(self.toolbar,False,True)
        self.pack_start(vbox,False,True)
        self.pack_start(self.vpaned)

        #MessListe auslesen

        self.mess_data_list=[]
        try:
            self.mess_data_list = self.readmesslistfiles()
        except:
            nothing =0

        self.paint_allowed=True
        self.paintPlot()
        self.show_all()
        self.list.hide()

    def __getitem__(self,key):
        return self.builder.get_object(key)

    def readmesslistfiles(self):
        messdateformat = "xno"
        mess_data_list = []
        for filename in self.messfilelist:
            if ".xno" in self.filename:
                messdateformat = "xno"
            if ".cdb" in self.filename:
                messdateformat = "cdb"
            mess_data = xpn_data(filename,messdateformat)
            mess_data_list.append(mess_data)
        return mess_data_list


    def on_Axis(self,data):
         #self.window.show_all()
        xlim0_str = str(self.fig.get_axes()[0].get_xlim()[0])
        xlim1_str = str(self.fig.get_axes()[0].get_xlim()[1])
        ylim0_str = str(self.fig.get_axes()[0].get_ylim()[0])
        ylim1_str = str(self.fig.get_axes()[0].get_ylim()[1])
        dialogScale = self["dialogScale"]
        if self.xachseformat==0:
            self["entry_xFrom"].set_text(xlim0_str);
            self["entry_xTo"].set_text(xlim1_str);
        else:
            #datestr = num2date(self.fig.get_axes()[0].get_xlim()[0])
            #str1 = datestr.strftime('%Y.%m.%d %H:%M:%S')
            tim2 = num2date(self.fig.get_axes()[0].get_xlim()[0])
            str1 = "%04d" % tim2.year + "-" + "%02d" % tim2.month + "-" + "%02d" % tim2.day + " " + "%02d" % tim2.hour + ":" + "%02d" % tim2.minute + ":" + "%02d" % tim2.second
            self["entry_xFrom"].set_text(str1)
            #datestr = num2date(self.fig.get_axes()[0].get_xlim()[1])
            #str1 = datestr.strftime('%Y.%m.%d %H:%M:%S')
            tim2 = num2date(self.fig.get_axes()[0].get_xlim()[1])
            str1 = "%04d" % tim2.year + "-" + "%02d" % tim2.month + "-" + "%02d" % tim2.day + " " + "%02d" % tim2.hour + ":" + "%02d" % tim2.minute + ":" + "%02d" % tim2.second
            self["entry_xTo"].set_text(str1)

        self["entry_yFrom"].set_text(ylim0_str);
        self["entry_yTo"].set_text(ylim1_str);

        #self['checkbutton_x_axis_automatic'].set_active(self.x_axis_automatic)
        #self['checkbutton_y_axis_automatic'].set_active(self.y_axis_automatic)
        if self.x_axis_automatic==True:
            self['checkbutton_x_axis_automatic'].set_label("Automatic (o)")
        else:
            self['checkbutton_x_axis_automatic'].set_label("Automatic ( )")
        if self.y_axis_automatic==True:
            self['checkbutton_y_axis_automatic'].set_label("Automatic (o)")
        else:
            self['checkbutton_y_axis_automatic'].set_label("Automatic ( )")
        self['checkbutton_x_axis_automatic'].set_active(False)
        self['checkbutton_y_axis_automatic'].set_active(False)
        self['checkbutton_Grid_on'].set_active(self.grid_on)
        self['entry_angle'].set_text(str(self.angle))
        self['entry_linestyle'].set_text(self.linestyle)
        self['checkbutton_x_axis_to_new_plot'].set_active(False)
        self['checkbutton_y_axis_to_new_plot'].set_active(False)
        self['checkbutton_prop_to_new_plot'].set_active(False)



        #self.grid_on=self.main_class.grid_on
        #self.angle=self.main_class.angle

        #liststoreVars = self["liststoreVars"]
        #liststoreVars.clear()

        #for lists in self.valuelist:
        #    iter = liststoreVars.insert(-1)
        #    liststoreVars.set_value(iter,0,lists[2])

        #list2 = self.valuelist[0]
        #print list2[2]

        #try:

        #  for items in modul_cfg.items(libname):
             #print items
         #    iter = liststore_model_options_global.insert(-1)
           #  liststore_model_options_global.set_value(iter,0,items[0])

        id = dialogScale.run()
        if (id==1):
            self.x_axis_automatic = self['checkbutton_x_axis_automatic'].get_active()
            self.y_axis_automatic = self['checkbutton_y_axis_automatic'].get_active()

            if self.xachseformat==0:
                #self.fig.get_axes()[0].set_xlim(double(self["entry_xFrom"].get_text()), double(self["entry_xTo"].get_text()))
                self.xFrom = double(self["entry_xFrom"].get_text())
                self.xTo = double(self["entry_xTo"].get_text())
            else:
                #datestr = datetime.datetime(1981,12,07)
                #datetime(*(time.strptime(self["entry_xFrom"].get_text(),'%Y.%m.%d %H:%M:%S')))
                #datestr = convert_str_to_datetime(self["entry_xFrom"].get_text())
                datestr = datetime.datetime.strptime(self["entry_xFrom"].get_text(),'%Y-%m-%d %H:%M:%S')
                num1 = date2num(datestr)

                #datestr = datetime.datetime(1981,12,07)
                #datestr = datetime(*(time.strptime('1981','%Y.%m.%d %H:%M:%S')))
                #datestr = datetime(*(time.strptime(self["entry_xTo"].get_text(),'%Y.%m.%d %H:%M:%S')))
                #datestr = convert_str_to_datetime(self["entry_xTo"].get_text())
                datestr = datetime.datetime.strptime(self["entry_xTo"].get_text(),'%Y-%m-%d %H:%M:%S')
                num2 = date2num(datestr)
                #self.fig.get_axes()[0].set_xlim(num1,num2)
                self.xFrom = num1
                self.xTo = num2

            #self.fig.get_axes()[0].set_ylim(double(self["entry_yFrom"].get_text()), double(self["entry_yTo"].get_text()))
            self.yFrom = double(self["entry_yFrom"].get_text())
            self.yTo = double(self["entry_yTo"].get_text())


            if self['checkbutton_x_axis_to_new_plot'].get_active()==True:
                if self.x_axis_automatic==True:
                    self.main_class.x_axis_automatic=True
                else:
                    self.main_class.x_axis_automatic=False
                    self.main_class.xFrom = self.xFrom
                    self.main_class.xTo = self.xTo

            if self['checkbutton_y_axis_to_new_plot'].get_active()==True:
                if self.y_axis_automatic==True:
                    self.main_class.y_axis_automatic=True
                else:
                    self.main_class.y_axis_automatic=False
                    self.main_class.yFrom = self.yFrom
                    self.main_class.yTo = self.yTo



            self.grid_on = self['checkbutton_Grid_on'].get_active()
            self.angle  =  double(self['entry_angle'].get_text())
            self.linestyle = self['entry_linestyle'].get_text()

            if self['checkbutton_prop_to_new_plot'].get_active()==True:
                self.main_class.grid_on=self.grid_on
                self.main_class.angle=self.angle
                self.main_class.linestyle = self.linestyle

            #set_ylim(left=)
            #self.canvas.draw()
            self.paintPlot()
        dialogScale.hide()

        return True


    def open_file(self,filename):
        del self.valuelist[0:len(self.valuelist)]

        self.filename = filename

        if ".xno" in self.filename or ".cdb" in self.filename:
              if ".xno" in self.filename:
                  self.dateformat = "xno"
              if ".cdb" in self.filename:
                  self.dateformat = "cdb"

        xpn_values = xpn_data(self.filename,self.dateformat)

        self.valuelist = []
        for i in range(1,xpn_values.get_len()):
            values = xpn_values[0,i]
            list2=[]
            list2.append(values[0])
            list2.append(values[1])
            list2.append(xpn_values.get_header()[i])
            self.valuelist.append(list2)

        try:
            child2 = self.vpaned.get_child2()
            self.list = self.create_list()
            child2 = self.vpaned.get_child2()
            child2.hide()
            self.vpaned.remove(child2)
            child2.destroy()
            self.vpaned.pack2(self.list,False,True)
            if self.list_button.get_active()==True:
                self.vpaned.get_child2().show()
        except:
            return


    def on_calc_statistics(self,data):
        textbuffer_statistic = self["textbuffer_statistic"]
        dialogstatistic = self["dialogstatistic"]
        textview_statistic = self["textview_statistic"]

        dialogstatistic.set_title("Statistics: %s" % self.tabbtn.get_label())
        dialogstatistic.show()


        xlim0 = num2date(self.fig.get_axes()[0].get_xlim()[0])
        xlim1 = num2date(self.fig.get_axes()[0].get_xlim()[1])

        str_date0 = get_formated_date(xlim0)
        str_date1 = get_formated_date(xlim1)

        S = ""
        S = textbuffer_statistic.get_text(textbuffer_statistic.get_start_iter(),textbuffer_statistic.get_end_iter())


        for lists in self.valuelist:

            mess_values = None
            for mess_data in self.mess_data_list:
                mess_values = mess_data[0,lists[2]]
                if mess_values!=None:
                    break

            if mess_values!=None:
                [dat1_i_from,dat1_i_to] = get_indices_of_date_range(xlim0,xlim1,[lists[0],lists[1]])
                [dat2_i_from,dat2_i_to] = get_indices_of_date_range(xlim0,xlim1,[mess_values[0],mess_values[1]])

                #[i_from,i_to] = xpn_data.get_indices_of_date_range(xlim0,xlim1,[mess_values[0],mess_values[1]])
                #print i_from,i_to

                dat1 = [lists[0][dat1_i_from:dat1_i_to],lists[1][dat1_i_from:dat1_i_to]]
                dat2 = [mess_values[0][dat2_i_from:dat2_i_to],mess_values[1][dat2_i_from:dat2_i_to]]
                stat = xpnstatistic(dat1,dat2)
                stat_S = stat.print_statistic_to_S()
                S += "Var: %s\n" % lists[2]
                str_date0 = get_formated_date(dat2[0][0])
                str_date1 = get_formated_date(dat2[0][-1])
                S += "From: %s\nTo: %s\n" % (str_date0,str_date1)
                #for item in dat2[0]:
                    #S += "%s\n" % get_formated_date(item)

                S += stat_S+"\n\n"



        textbuffer_statistic.set_text(S)
        textview_statistic.scroll_to_iter(textbuffer_statistic.get_end_iter(),0.0)

        #id = dialogstatistic.run()
        #if (id==1):
        #self.textbufferOptions.set_text("")

        #dialogstatistic.hide()



    def on_statistic_close_clicked(self,widget,data=None,data2=None):
        dialogstatistic = self["dialogstatistic"]
        dialogstatistic.hide()        

    def on_statistic_help_clicked(self,widget,data=None,data2=None):
        #dialog_statistic_help = self["dialog_statistic_help"]
        #dialog_statistic_help.show()
        self.dialog_statistic_help.show()

    def on_statistic_help_close_clicked(self,widget,data=None,data2=None):
        #dialog_statistic_help = self["dialog_statistic_help"]
        #dialog_statistic_help.hide()
        self.dialog_statistic_help.hide()

    def on_open_file(self,widget):
        filechooser =  gtk.FileChooserDialog(action=gtk.FILE_CHOOSER_ACTION_OPEN,buttons=(gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,gtk.STOCK_OPEN, gtk.RESPONSE_OK))
        # if we want to apply filters

        filter2 = gtk.FileFilter()
        filter2.set_name("Expert-N 5.0 (*.xno)")
        filter2.add_pattern("*.xno")
        filechooser.add_filter(filter2)

        filter2 = gtk.FileFilter()
        filter.set_name("C Debug File (*.cdb)")
        filter.add_pattern("*.cdb")
        dialog.add_filter(filter)

        filter1 = gtk.FileFilter()
        filter1.set_name("All Files (*.*)")
        filter1.add_pattern("*")
        filechooser.add_filter(filter1)

        response = filechooser.run()
        if response==gtk.RESPONSE_OK:
            filename=filechooser.get_filename()
            self.open_file(filename)
            self.paintPlot()
        filechooser.hide()

    def save_file(self,filename):
        f = open(filename,'w')
        i=0
        for lists in self.valuelist:
            i=i+1
            if (i==1):
                f.write(self.x_axis_choice_name+",")
            if i==len(self.valuelist):
                f.write(lists[2].rstrip())
            else:
                f.write(lists[2]+',')
        f.write("\n")
        lists=self.valuelist[0]
        listcount = len(lists[0])
        for i in range(0, listcount):
            i2 = 0
            for lists in self.valuelist:
                if (i2==0):
                    tim2 = lists[0][i]
                    S = "%04d" % tim2.year + "-" + "%02d" % tim2.month + "-" + "%02d" % tim2.day + " " + "%02d" % tim2.hour + ":" + "%02d" % tim2.minute + ":" + "%02d" % tim2.second
                    S = S + ","
                    #S = "%e,"% (lists[0][i])
                    f.write(S)
                if i2 == len(self.valuelist)-1:
                    S = "%e" % (lists[1][i])
                else:
                    S = "%e," % (lists[1][i])
                f.write(S)
                i2 = i2 + 1
            f.write("\n")
        f.close()


    def on_save_file(self,widget):
        filechooser =  gtk.FileChooserDialog(action=gtk.FILE_CHOOSER_ACTION_SAVE,buttons=(gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,gtk.STOCK_SAVE, gtk.RESPONSE_OK))
        # if we want to apply filters

        filter2 = gtk.FileFilter()
        filter2.set_name("Expert-N 5.0 Output File (*.xno)")
        filter2.add_pattern("*.xno")
        filechooser.add_filter(filter2)

        filter1 = gtk.FileFilter()
        filter1.set_name("All Files (*.*)")
        filter1.add_pattern("*")
        filechooser.add_filter(filter1)

        response = filechooser.run()
        if response==gtk.RESPONSE_OK:
            filename=filechooser.get_filename()
            if filechooser.get_filter()==filter2:
                filename=filename+'.xno'
            self.save_file(filename)
        filechooser.hide()

    #def set_linestyle(self,entry):
    #    self.linestyle = entry.get_text()
    #    self.paintPlot()
    def set_xAchse(self,combobox):
        model = combobox.get_model()
        index = combobox.get_active()
        if index > -1:
            self.lastxachseformat=self.xachseformat
            self.xachseformat = index
            self.paintPlot(dontautoscale=True)
    
    def create_list_with_equal_len(self,valuelist):
        valuelist2 = []
        #find lowest date:
        date1 = valuelist[0][0]
        for i in range(len(valuelist)):
            if date1 > valuelist[i][0]:
                date1 = valuelist[i][0]
        row = []
        row.append(date1)
        #for i2 in range(len(valuelist)):
            
    def create_list(self):
        scrolled_window = gtk.ScrolledWindow()
        scrolled_window.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)

        modelarray = []
        modelarray.append(gobject.TYPE_STRING)
        for lists in self.valuelist:
            modelarray.append(gobject.TYPE_DOUBLE)
        model = gtk.ListStore(*modelarray)
        #model.append([gobject.TYPE_DOUBLE])
        tree_view = gtk.TreeView(model)
        scrolled_window.add_with_viewport (tree_view)
        tree_view.show()
        i=0
        for lists in self.valuelist:
            i=i+1
            if (i==1):
                cell = gtk.CellRendererText()
                column = gtk.TreeViewColumn(self.x_axis_choice_name, cell, text=i-1)
                column.set_resizable(True)
                tree_view.append_column(column)
            cell = gtk.CellRendererText()
            column = gtk.TreeViewColumn(lists[2], cell, text=i)
            column.set_resizable(True)
            tree_view.append_column(column)
            
        #print len(self.valuelist[0]),len(self.valuelist[1])
        #print self.valuelist[0]
        #ToDo: Wenn Wertetabellen nicht vollständig --> List muss mit - ergaenst werden --> create_list_with_equal_len
        no_of_items = len(self.valuelist[0][0])
        for i in range(1,len(self.valuelist)):
            if no_of_items!=len(self.valuelist[i][0]):
                # hier muessten die Listen aufgefuellt werden!
                return scrolled_window
        lists=self.valuelist[0]
        listcount = len(lists[0])        
        for i in range(0, listcount):
            iter = model.append()
            i2 = 0
            for lists in self.valuelist:
                if (i2==0):
                    if self.xachseformat==0:
                        model.set(iter,i2,str(lists[0][i]))
                    else:
                        tim2 = lists[0][i]
                        time_str = "%04d" % tim2.year + "-" + "%02d" % tim2.month + "-" + "%02d" % tim2.day + " " + "%02d" % tim2.hour + ":" + "%02d" % tim2.minute + ":" + "%02d" % tim2.second
                        model.set(iter,i2,time_str)                
                model.set(iter,i2+1,lists[1][i])
                i2 = i2 + 1

        return scrolled_window

##    class ThreadExternalProg(Thread):
##    def __init__(self,process,filename,plotwin,window):
##        Thread.__init__(self)
##        self.process = process
##        self.filename = filename
##        self.plotwin = plotwin
##        self.window = window
##        self.lock=thread.allocate_lock()
##    def run(self):
##        self.process.wait()
##        print "ja ja"
##        self.
##        try:
##            self.plotwin.open_file(self.filename)
##        except:
##            print "Error open file"
##        print "non no"
##        self.lock.release()
##        os.remove (self.filename)
##        print "Exit Thread"

    def thread_extern_prog(self,filename):
        print "Process Start"
        if os.name == 'nt':
            subprocess.Popen([self.externspreadsheet,filename],shell=False).wait()
        else:
            subprocess.Popen([self.externspreadsheet+" "+'"'+filename+'"'],shell=True).wait()
        #os.system(self.externspreadsheet+" "+filename)
        print "Process exit"
        lock.acquire()
        self.main_class.open_file_name= filename;
        print "File opened"
        lock.release()
        time.sleep(2)



    def open_with_extern_prog(self,filename):
        thread.start_new_thread(self.thread_extern_prog,(filename,))

    def on_extern_edit(self, widget):
        print "extern edit"
        tmpfile2 = tempfile.NamedTemporaryFile()
        tmpfile = tmpfile2.name + '.xno'
        tmpfile2.close()

        self.save_file(tmpfile)


        print tmpfile

        self.open_with_extern_prog(tmpfile)

#


    def on_toggle_legend(self,widget):
        self.legend=widget.get_active()
        self.paintPlot()
    def on_toggle_list(self,widget):
        if (widget.get_active()==True):
            self.list.show()
        else:
            self.list.hide()


    def paintPlot(self,dontautoscale=False):
        colors = ['red', 'blue','green','cyan','orange','yellow','brown','pink','magenta']
        colornumber = 0
        colormaxnumber = len(colors)

        if self.paint_allowed==False:
            return
        try:
            self.PlotBox.remove(self.canvas)
            repaint=1

        except:
            #print "First Time Drawing Plot"
            repaint=0
        try:
            self.PlotBox.remove(self.Navtoolbar)
        except AttributeError:
            nothing=0

        xlim0 = 0.0
        xlim1 = 0.0
        ylim0 = 0.0
        ylim1 = 0.0

        try:
            xlim0 = self.fig.get_axes()[0].get_xlim()[0]
            xlim1 = self.fig.get_axes()[0].get_xlim()[1]
            ylim0 = self.fig.get_axes()[0].get_ylim()[0]
            ylim1 = self.fig.get_axes()[0].get_ylim()[1]
        except:
            nothing = 0

        self.fig = Figure(figsize=(5,4), dpi=100,facecolor='white')
        for lists in self.valuelist:
            ax = self.fig.add_subplot(111)
            d = []
            for entry in lists[0]:
                if self.xachseformat==0:
                    d.append(entry)
                else:
                    d2 = entry
                    d.append(d2)

            mess_values = None
            for mess_data in self.mess_data_list:
                mess_values = mess_data[0,lists[2]]
                if mess_values!=None:
                    break

            if self.xachseformat==0:
                pl=ax.plot(lists[0],lists[1], self.linestyle,label=get_varname_withoutpath(lists[2]),color=colors[colornumber])
                labels = ax.get_xticklabels()
                for label in labels:
                    label.set_rotation(self.angle)
                if mess_values!=None:
                    ax.plot(mess_values[0],mess_values[1],'.',label=get_varname_withoutpath(lists[2]),color=colors[colornumber])
                    #ax.plot(messdata[0],messdata[messindex], '.',label=get_varname_withoutpath(lists[2]))
                    #self.fig.autofmt_xdate(rotation=self.angle)
            elif self.xachseformat==1:
                pl=ax.plot_date(d,lists[1],self.linestyle,label=get_varname_withoutpath(lists[2]),color=colors[colornumber])
                if mess_values!=None:
                    ax.plot(mess_values[0],mess_values[1],'.',color=colors[colornumber])
                ax.xaxis.set_major_formatter(DateFormatter("%d %b %Y"))
                self.fig.autofmt_xdate( rotation=self.angle)
            elif self.xachseformat==2:
                pl=ax.plot_date(d,lists[1],self.linestyle,label=get_varname_withoutpath(lists[2]),color=colors[colornumber])
                if mess_values!=None:
                    ax.plot(mess_values[0],mess_values[1],'.',label=get_varname_withoutpath(lists[2]),color=colors[colornumber])
                ax.xaxis.set_major_formatter(DateFormatter("%H:%M:%S"))
                self.fig.autofmt_xdate(rotation=self.angle)
            else :
                pl=ax.plot_date(d,lists[1],self.linestyle,label=get_varname_withoutpath(lists[2]),color=colors[colornumber])
                if mess_values!=None:
                    ax.plot(mess_values[0],mess_values[1],'.',label=get_varname_withoutpath(lists[2]),color=colors[colornumber])
                ax.xaxis.set_major_formatter(DateFormatter("%d %b %Y - %H:%M:%S"))
                self.fig.autofmt_xdate(rotation=self.angle)
            ax.grid(self.grid_on)
            if (self.legend==True):
                ax.legend(shadow=True)

            colornumber=colornumber+1
            if colornumber>=colormaxnumber:
                colornumber=0

        if dontautoscale==True:
            self.fig.get_axes()[0].set_xlim(xlim0,xlim1)
            self.fig.get_axes()[0].set_ylim(ylim0,ylim1)
        else:
            if (self.x_axis_automatic==True) and (self.y_axis_automatic==True):
                try:
                    self.fig.get_axes()[0].autoscale(enable=True, axis='both')
                except:
                    pass
            elif (self.x_axis_automatic==True):
                try:
                    self.fig.get_axes()[0].autoscale(enable=True, axis='x')
                except:
                    pass
                self.fig.get_axes()[0].set_ylim(self.yFrom,self.yTo)
            elif (self.y_axis_automatic==True):
                try:
                    self.fig.get_axes()[0].autoscale(enable=True, axis='y')
                except:
                    pass
                self.fig.get_axes()[0].set_xlim(self.xFrom,self.xTo)
            else:
                self.fig.get_axes()[0].set_xlim(self.xFrom,self.xTo)
                self.fig.get_axes()[0].set_ylim(self.yFrom,self.yTo)

        self.canvas = FigureCanvas(self.fig)  # a gtk.DrawingArea
        self.Navtoolbar = NavigationToolbar(self.canvas, self.Parentwindow)
        self.PlotBox.pack_start(self.canvas,True,True)
        self.PlotBox.pack_start(self.Navtoolbar, False, False)
        if repaint==1:
            #if ((self.lastxachseformat!=0) and (self.xachseformat!=0)) or (self.xachseformat==self.lastxachseformat):
                #if (self.x_axis_automatic==True) and (self.y_axis_automatic==True):
                    #self.fig.get_axes()[0].autoscale(enable=True, axis='both')
                #elif (self.x_axis_automatic==True):
                    #self.fig.get_axes()[0].autoscale(enable=True, axis='x')
                    #self.fig.get_axes()[0].set_ylim(self.yFrom,self.yTo)
                #elif (self.y_axis_automatic==True):
                    #self.fig.get_axes()[0].autoscale(enable=True, axis='y')
                    #self.fig.get_axes()[0].set_xlim(self.xFrom,self.xTo)
                #else:
                    #self.fig.get_axes()[0].set_xlim(self.xFrom,self.xTo)
                    #self.fig.get_axes()[0].set_ylim(self.yFrom,self.yTo)
                self.canvas.draw()
        self.lastxachseformat=self.xachseformat
        self.PlotBox.show_all()




class GtkTabButton(gtk.HBox):
    def __init__(self,Count):
        gtk.HBox.__init__(self)
        self.label_str="Figure "+ str(Count)
        label = gtk.Label(self.label_str)
        self.lab=label
        e = gtk.EventBox()
        e.add(label)
        e.add_events(gtk.gdk.BUTTON_RELEASE_MASK)
        self.closebutton = gtk.Button()
        self.closebutton.set_relief(gtk.RELIEF_NONE)
        im = gtk.image_new_from_stock("gtk-close",1)
        self.closebutton.set_image(im)
        self.pack_start(e,True,False)
        self.pack_start(self.closebutton,False,False)
        self.label = e
        self.comment=""
        self.show_all()
    def get_label(self):
        return self.label_str
    def get_comment(self):
        return self.comment
    def set_comment(self,text):
        self.comment=text
    def set_label(self,text):
        self.label_str=text
        self.lab.set_label(text)

class MainWindow:

    def newWin(self,source,page,x,y,user_data):
        wind = gtk.Window()
        noteb = gtk.Notebook()
        noteb.set_show_tabs(False)
        wind.add(noteb)
        wind.set_default_size(600,600)
        #wind.set_title(user_data)
        wind.set_title(source.get_tab_label(page).get_label())
        wind.connect("delete-event",self.freewindestroy,noteb,source)
        wind.show_all()
        return noteb

    def freewindestroy(self,widget,data, noteb,source):
        wid = noteb.get_nth_page(0)
        tab = noteb.get_tab_label(wid)
        noteb.remove_page(0)
        source.append_page(wid,tab)
        source.set_tab_reorderable(wid,True)
        source.set_tab_detachable(wid,True)
        gtk.notebook_set_window_creation_hook(self.newWin,tab.label_str)
        return False

    def update_timer(self):
        if self.open_file_name!='':
            args = ""
            if self.dialogoptionspath.get_text()=="":
                args=self.dialogoptionscommand.get_text()
            else:
                args=self.dialogoptionspath.get_text()+"/"+self.dialogoptionscommand.get_text()
            self.open_plot_window(args,self.open_file_name,True)
            os.remove (self.open_file_name)
            self.open_file_name=""
        return True # run again in one second


    def open_plot_window(self,args,filename,open_whole_file=False):
        self.Figure_Count = self.Figure_Count + 1
        messlist = []
        liststore_Messure = self["liststore_Messure"]
        treeview_Messure = self["treeview_Messure"]
        selection = treeview_Messure.get_selection()
        selection.unselect_all()
        model = treeview_Messure.get_model()
        first = None
        i = 0
        while (1):
            if i == 0:
                first = model.get_iter_first()
            else:
                first = model.iter_next(first)
            if first==None:
                break
            i = i+1
            Listname = model.get_value(first,1)
            messlist.append(Listname)
        vbox = PlotWindow(self,self.window,self.Figure_Count,filename,self.liststore,args,open_whole_file,self.x_axis_choice,self.x_axis_choice_name,messlist)
        tabbtn = GtkTabButton(self.Figure_Count)
        tabbtn.closebutton.connect_object("clicked",self.on_tab_destroy, self.notebookPlot,vbox)
        vbox.tabbtn=tabbtn
        #tabbtn.label.connect_object("button-press-event",self.on_ChangeFigureName,self.notebookPlot,vbox)
        tabbtn.label.connect_object("button-release-event",self.on_ChangeFigureName,self.notebookPlot,vbox,tabbtn)
        if len(vbox.valuelist)==1:
           tabbtn.set_label(get_varname_withoutpath(vbox.valuelist[0][2])+ " (" +str(self.Figure_Count)+")")
        tabbtn.show_all()
        self.notebookPlot.append_page(vbox,tabbtn)
        self.notebookPlot.set_tab_reorderable(vbox,True)
        self.notebookPlot.set_tab_detachable(vbox,True)
        gtk.notebook_set_window_creation_hook(self.newWin,tabbtn.label_str)
        #self.notebookPlot.set_default_size(600,600)
        self.notebookPlot.show()
        self.notebookPlot.set_current_page(-1)

    def on_button_matplot(self,widget, data=None):
        args = ""
        if self.dialogoptionspath.get_text()=="":
            args=self.dialogoptionscommand.get_text()
        else:
            args=self.dialogoptionspath.get_text()+"/"+self.dialogoptionscommand.get_text()
        self.open_plot_window(args,self.filename,False)



    def on_window_destroy(self, widget, data=None):
        #self.window.show_all()
        global window_instances
        self.dialogClose = self['dialogClose']
        id = self.dialogClose.run()
        if (id==1):
            self.window.destroy()
            gobject.source_remove(self.timer_id)
            window_instances=window_instances-1
            if (self.is_lib==False):
                if window_instances<=0:
                    gtk.main_quit()
        self.dialogClose.hide()
        self.SaveXNC()
        return True

    def on_tab_destroy(self,notebook,vbox):
        notebook.remove_page(notebook.page_num(vbox))
        #if (notebook.get_n_pages()==0):
        #    notebook.hide()
    def on_ChangeFigureName(self,notebook,button,PlotWindow,tabbtn):
        if button.button==3:
            dialogFigure = self["dialogChangeFigureName"]
            self["entry_FigureName"].set_text(tabbtn.get_label())
            self["textbufferFigureComment"].set_text(tabbtn.get_comment())
            id = dialogFigure.run()
            if (id==1):
                tabbtn.set_label(self["entry_FigureName"].get_text())
                tabbtn.set_comment(self["textbufferFigureComment"].get_text(self["textbufferFigureComment"].get_start_iter(),self["textbufferFigureComment"].get_end_iter()))
            dialogFigure.hide()

    def on_filechooser_file_set(self,widget,data=None):
        dialog = gtk.FileChooserDialog("Select A File ",None,gtk.FILE_CHOOSER_ACTION_OPEN,(gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,gtk.STOCK_OPEN, gtk.RESPONSE_OK))
        self.SetFileFilters(dialog)
        dialog.set_default_response(gtk.RESPONSE_OK)
        response = dialog.run()
        if response == gtk.RESPONSE_OK:
            self.filename = dialog.get_filename()
            self.window.set_title('XNO Plot'+' - '+get_filename_without_path(self.filename))
            self.on_toolbutton_refresh(self, widget)
        dialog.hide()


    def on_menu_import_Expert_N_file(self,widget, data=None):
        dialog = gtk.FileChooserDialog("Open..",None,gtk.FILE_CHOOSER_ACTION_OPEN,(gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,gtk.STOCK_OPEN, gtk.RESPONSE_OK))
        dialog.set_default_response(gtk.RESPONSE_OK)
        filter = gtk.FileFilter()
        filter.set_name("All files")
        filter.add_pattern("*")
        dialog.add_filter(filter)

        response = dialog.run()
        if response != gtk.RESPONSE_OK:
            dialog.destroy()
            return
        self.read_Expert_N_file(dialog.get_filename())
        dialog.destroy()

    def on_toolbutton_new_instance(self,widget):
        global window_instances
        window_instances=window_instances+1
        Mwindow = MainWindow()
        Mwindow.window.show()


    def on_toolbutton_options(self,widget):
        response = self.dialogOptions.run()
        if response==1:
            configfile=open("xnoplot.ini",'w')
            try:
                self.config.add_section("spreadsheet")
            except:
                print "Section exists"
            self.config.set("spreadsheet","path",self.dialogoptionspath.get_text())
            self.config.set("spreadsheet","command",self.dialogoptionscommand.get_text())
            self.config.write(configfile)
            configfile.close()
        self.dialogOptions.hide()

    def on_toolbutton_about(self,widget):
        self.aboutdialog.run()
        self.aboutdialog.hide()


    def on_toolbutton_refresh(self, widget, data=None):
        #print "Read C Debug / CDV File"
        file = open(self.filename,'rU')
        line = file.readline()
        file.close()
        if not line:
            return
        self.liststore.clear()
        names=string.split(line,",")
        nr = 0
        for S in names:
            iter = self.liststore.insert(-1)
            self.liststore.set_value(iter,1,S.rstrip())
            self.liststore.set_value(iter,2,nr)
            nr = nr + 1

        self.SaveXNC()
        self.ReadXNC()
         # versuche Konfig Datei fuer xno Datei zu laden:


    def ReadXNC(self):
        try:
            #print self.filename[:-3]+"xnc"
            config = ConfigParser.ConfigParser()
            config.read(self.filename[:-3]+"xnc")
            self.x_axis_automatic=config.getboolean("axes properties","x_axis_automatic")
            self.y_axis_automatic=config.getboolean("axes properties","y_axis_automatic")
            self.xFrom=config.getfloat("axes properties","xFrom")
            self.xTo=config.getfloat("axes properties","xTo")
            self.yFrom=config.getfloat("axes properties","yFrom")
            self.yTo=config.getfloat("axes properties","yTo")
            self.grid_on=config.getboolean("axes properties","grid_on")
            self.angle=config.getfloat("axes properties","angle")
            self.linestyle=config.get("axes properties","linestyle")
            messliststr = config.get("axes properties","measurement")
            messlist = messliststr.split(",")


            liststore_Messure = self["liststore_Messure"]
            treeview_Messure = self["treeview_Messure"]
            liststore_Messure.clear()
            model = treeview_Messure.get_model()
            for filename in messlist:
                iter = liststore_Messure.insert(-1)
                liststore_Messure.set_value(iter,1,filename)
                liststore_Messure.set_value(iter,0,get_filename_without_path(filename))



        except:
            self.x_axis_automatic=True
            self.y_axis_automatic=True
            self.xFrom=0.0
            self.xTo=10.0
            self.yFrom=0.0
            self.yTo=100.0
            self.grid_on=True
            self.angle=30
            self.linestyle="-"


    def SaveXNC(self):
        # Messliste:
        messlist = []

        liststore_Messure = self["liststore_Messure"]
        treeview_Messure = self["treeview_Messure"]
        model = treeview_Messure.get_model()
        first = None
        i = 0
        while (1):
            if i == 0:
                first = model.get_iter_first()
            else:
                first = model.iter_next(first)
            if first==None:
                break
            i = i+1
            Listname = model.get_value(first,1)
            messlist.append(Listname)


        #print messlist

        messliststr=""
        for item in messlist:
            if messliststr=="":
                messliststr=item
            else:
                messliststr=messliststr+","+item


        try:
            config = ConfigParser.ConfigParser()
            try:
                config.add_section("axes properties")
            except:
                nothing=0
            config.set("axes properties","x_axis_automatic",self.x_axis_automatic)
            config.set("axes properties","y_axis_automatic",self.y_axis_automatic)
            config.set("axes properties","xFrom",self.xFrom)
            config.set("axes properties","xTo",self.xTo)
            config.set("axes properties","yFrom",self.yFrom)
            config.set("axes properties","yTo",self.yTo)
            config.set("axes properties","grid_on",self.grid_on)
            config.set("axes properties","angle",self.angle)
            config.set("axes properties","linestyle",self.linestyle)
            config.set("axes properties","measurement",messliststr)

            configfile=open(self.filename[:-3]+"xnc","w")
            config.write(configfile)
            configfile.close()
        except:
            nothing=0


    def on_toolbutton_gnumeric(self, widget, data=None):
        args = ""
        if self.dialogoptionspath.get_text()=="":
            args=self.dialogoptionscommand.get_text()
        else:
            args=self.dialogoptionspath.get_text()+"/"+self.dialogoptionscommand.get_text()
        if os.name == 'nt':
            subprocess.Popen([args,self.filename],shell=False)
        else:
            subprocess.Popen([args+' '+self.filename],shell=True)
        #p1=subprocess.Popen('gnumeric ' + '"'+self.filename+'"')



    def on_cellrenderertoggle_toggled(self,cell,path, data=None):
        model = self.liststore
        iter = model.get_iter((int(path),))
        val = model.get_value(iter,0)
        val = not val
        model.set(iter,0,val)

    def SetFileFilters(self,dialog):
        filter = gtk.FileFilter()
        filter.set_name("Expert-N 5.0 Output File (*.xno)")
        filter.add_pattern("*.xno")
        dialog.add_filter(filter)
        dialog.set_filter(filter)

        filter = gtk.FileFilter()
        filter.set_name("C Debug File (*.cdb)")
        filter.add_pattern("*.cdb")
        dialog.add_filter(filter)

        filter = gtk.FileFilter()
        filter.set_name("All Files (*.*)")
        filter.add_pattern("*")
        dialog.add_filter(filter)

    def on_var_activated(self,treeview,event):
        if event.button==3:
            #Popup Menueliste fuer VarListe
            menu = gtk.Menu()
            menu_item = gtk.CheckMenuItem("x - Axis")
            menu.append(menu_item)
            menu_item.connect("activate", self.on_popup_menu_var)
            menu_item.show()
            self.popup = menu
            self.popup_x_achis = menu_item
            x = int(event.x)
            y = int(event.y)
            time = event.time
            pthinfo = treeview.get_path_at_pos(x, y)
            if pthinfo is not None:
                path, col, cellx, celly = pthinfo
                treeview.grab_focus()
                treeview.set_cursor( path, col, 0)
                model = self.liststore
                iter = model.get_iter(path)
                val = model.get_value(iter,2)
                if self.x_axis_choice==val:
                    self.popup_x_achis.set_active(True)
                else:
                    self.popup_x_achis.set_active(False)
                self.popup.popup( None, None, None, event.button, time)

    def on_popup_menu_var(self,widget):
        model = self.liststore
        selection = self.treeview.get_selection()
        tup = selection.get_selected()
        iter = tup[1]
        if (iter!=None):
            val = model.get_value(iter,2)
            self.x_axis_choice=val
            val = model.get_value(iter,1)
            self.x_axis_choice_name=val

    def on_messurements_add_file(self,widget,data=None):
        dialog = gtk.FileChooserDialog("Add Messurement File",None,gtk.FILE_CHOOSER_ACTION_OPEN,(gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,gtk.STOCK_OPEN, gtk.RESPONSE_OK))
        filter = gtk.FileFilter()
        filter.set_name("Expert-N 5.0 Output File (*.xno)")
        filter.add_pattern("*.xno")
        dialog.add_filter(filter)
        dialog.set_filter(filter)
        filter = gtk.FileFilter()
        filter.set_name("C Debug File (*.cdb)")
        filter.add_pattern("*.cdb")
        dialog.add_filter(filter)
        dialog.set_default_response(gtk.RESPONSE_OK)
        dialog.set_select_multiple(True)
        response = dialog.run()
        if response == gtk.RESPONSE_OK:
           filenames = dialog.get_filenames()
           liststore_Messure = self["liststore_Messure"]
           treeview_Messure = self["treeview_Messure"]
           for filename in filenames:
              #print get_filtered_libName(filename)
                selection = treeview_Messure.get_selection()
                selection.unselect_all()
                model = treeview_Messure.get_model()
                first = None
                i = 0
                while (1):
                  if i == 0:
                     first = model.get_iter_first()
                  else:
                     first = model.iter_next(first)
                  if first==None:
                     break
                  i = i+1
                  Listlibname = model.get_value(first,1)
                  if (Listlibname==filename):
                     model.remove(first)
                     first =liststore_Messure.get_iter_first()
                     i =1
                iter = liststore_Messure.insert(-1)
                liststore_Messure.set_value(iter,1,filename)
                liststore_Messure.set_value(iter,0,get_filename_without_path(filename))
        dialog.hide()

    def on_messurements_delete_file(self,widget,data=None):
         treeviewModules = self["treeview_Messure"]
         selection = treeviewModules.get_selection()
         model, treeiter = selection.get_selected()
         # If there's no selection, treeiter will be None
         if treeiter is None: return
         id = self["dialog_Question"].run()
         if (id==1):
             model.remove(treeiter)
         self["dialog_Question"].hide()

    def on_Measurement_Window_Show(self,widget,data=None):
        self['frameMeasurement'].show()


    def on_Measurement_Window_Hide(self,widget,data=None):
        self['frameMeasurement'].hide()

    def __getitem__(self,key):
        return self.builder.get_object(key)

    def __init__(self,xnoplot_argv=[]):

        #print self,xnoplot_argv
        builder = gtk.Builder()
        builder.add_from_file("xnoplot.ui")

        self.Figure_Count=0
        self.window = builder.get_object("window")
        self.liststore = builder.get_object("liststore")
        self.treeview = builder.get_object("treeview")
        self.comboboxLineStyle = builder.get_object("comboboxLineStyle")
        self.liststoreLineStyle = builder.get_object("liststoreLineStyle")
        self.notebookPlot = builder.get_object("notebookPlot")
        self.WidgetTab = builder.get_object("WidgetTab")
        self.togglebuttonGrid = builder.get_object("togglebuttonGrid")
        self.togglebuttonLegend = builder.get_object("togglebuttonLegend")
        self.dialogOptions = builder.get_object("dialogOptions")
        self.dialogoptionscommand = builder.get_object("dialogoptionscommand")
        self.dialogoptionspath =  builder.get_object("dialogoptionspath")
        self.aboutdialog = builder.get_object("aboutdialog")
        self.hboxVars = builder.get_object("hboxVars")





        builder.connect_signals(self)
        self.open_file_name=''
        self.timer_id = gobject.timeout_add(1000, self.update_timer)

        self.config = ConfigParser.ConfigParser()

        self.builder = builder


        # Time wird fuer x-Achse ausgewaehlt
        self.x_axis_choice=0
        self.x_axis_choice_name="Time"




        try:
            self.config.read("xnoplot.ini")
            self.dialogoptionspath.set_text(self.config.get("spreadsheet","path"))
            self.dialogoptionscommand.set_text(self.config.get("spreadsheet","command"))
        except:
            self.dialogoptionspath.set_text("")
            self.dialogoptionscommand.set_text("gnumeric")

        self.ploted_vars=[]
        self.is_lib=False
        if len(xnoplot_argv) > 1:
            options, args = getopt.getopt(xnoplot_argv[1:], 'h:p:f', ['help','plot=','file=','hide_vars','title=','is_lib'])
            for o, a in options:
                if o in ('--help','-h'):
                    print "XNO Plot Software Copyright: Christian Klein"
                    print options
                    quit()
                elif o in ('--plot','-p'):
                    print "Vars: ", a
                    self.ploted_vars = a.split(',')
                elif o in ('--hide_vars'):
                    self.hboxVars.hide()
                    self.notebookPlot.set_show_tabs(False)
                elif o in ('--file','-f'):
                    print 'Open: ',a
                    self.filename=a
                    self.on_toolbutton_refresh(self, None)
                elif o in ('--title'):
                    self.window.set_title(self.window.get_title()+' - '+a)
                elif o in ('--is_lib'):
                    self.is_lib=True
                    self['toolbuttonNew'].hide()
                    self['toolbuttonprefs'].hide()
                    self['toolbuttonAbout'].hide()
                else:
                    print "Nothing "

        set_plot=False;
        if len(self.ploted_vars)>0:
            #print "Plot Vars:"
            iter = self.liststore.get_iter_first()
            val = self.liststore.get_value(iter,1)
            if val in self.ploted_vars:
                model = self.liststore
                model.set(iter,0,True)
            while(1):
                iter=self.liststore.iter_next(iter)
                if iter==None:
                    break
                val = self.liststore.get_value(iter,1)
                if val in self.ploted_vars:
                    model = self.liststore
                    model.set(iter,0,True)
                    set_plot=True
        if set_plot: self.on_button_matplot(None)




if __name__ == "__main__":
    #global window_instances
    print sys.argv
    window_instances=window_instances+1
    Mwindow = MainWindow(sys.argv)
    Mwindow.window.show()
    gtk.main()

