#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os,sys
import locale
import subprocess
if not subprocess.mswindows:
    locale.setlocale(locale.LC_ALL, 'en_US.UTF-8')
import gtk, gobject, cairo
from gtk import gdk

import errno
import os,shutil
try:
    from iniparse import ConfigParser
except:
    from ConfigParser import ConfigParser
import thread
import signal
from re import findall
from string import find
import urllib
import xnoplot
import time
import fnmatch
import glob
#from xpn_cfg_manager import *
import xpn_cfg_manager

import matplotlib


from numpy import *
import matplotlib.pyplot as plt
import numpy as np

from matplotlib.backends.backend_gtkagg import FigureCanvasGTKAgg as FigureCanvas
from matplotlib.backends.backend_gtkagg import NavigationToolbar2GTKAgg as NavigationToolbar



#Subprocess with Pipes (platformindependent):

if subprocess.mswindows:
    from win32file import ReadFile, WriteFile
    from win32pipe import PeekNamedPipe
    import msvcrt
else:
    import select
    import fcntl

def do_gui_operation(function, *args, **kw):
      def idle_func():
          gtk.threads_enter()
          try:
              function(*args, **kw)
              return False
          finally:
              gtk.threads_leave()
      gobject.idle_add(idle_func)

class Popen(subprocess.Popen):
    def recv(self, maxsize=None):
        return self._recv('stdout', maxsize)

    def recv_err(self, maxsize=None):
        return self._recv('stderr', maxsize)

    def send_recv(self, input='', maxsize=None):
        return self.send(input), self.recv(maxsize), self.recv_err(maxsize)

    def get_conn_maxsize(self, which, maxsize):
        if maxsize is None:
            maxsize = 1024
        elif maxsize < 1:
            maxsize = 1
        return getattr(self, which), maxsize

    def _close(self, which):
        getattr(self, which).close()
        setattr(self, which, None)

    if subprocess.mswindows:
        def send(self, input):
            if not self.stdin:
                return None

            try:
                x = msvcrt.get_osfhandle(self.stdin.fileno())
                (errCode, written) = WriteFile(x, input)
            except ValueError:
                return self._close('stdin')
            except (subprocess.pywintypes.error, Exception), why:
                if why[0] in (109, errno.ESHUTDOWN):
                    return self._close('stdin')
                raise

            return written

        def _recv(self, which, maxsize):
            conn, maxsize = self.get_conn_maxsize(which, maxsize)
            if conn is None:
                return None

            try:
                x = msvcrt.get_osfhandle(conn.fileno())
                (read, nAvail, nMessage) = PeekNamedPipe(x, 0)
                if maxsize < nAvail:
                    nAvail = maxsize
                if nAvail > 0:
                    (errCode, read) = ReadFile(x, nAvail, None)
            except ValueError:
                return self._close(which)
            except (subprocess.pywintypes.error, Exception), why:
                if why[0] in (109, errno.ESHUTDOWN):
                    return self._close(which)
                raise

            if self.universal_newlines:
                read = self._translate_newlines(read)
            return read

    else:
        def send(self, input):
            if not self.stdin:
                return None

            if not select.select([], [self.stdin], [], 0)[1]:
                return 0

            try:
                written = os.write(self.stdin.fileno(), input)
            except OSError, why:
                if why[0] == errno.EPIPE: #broken pipe
                    return self._close('stdin')
                raise

            return written

        def _recv(self, which, maxsize):
            conn, maxsize = self.get_conn_maxsize(which, maxsize)
            if conn is None:
                return None

            flags = fcntl.fcntl(conn, fcntl.F_GETFL)
            if not conn.closed:
                fcntl.fcntl(conn, fcntl.F_SETFL, flags| os.O_NONBLOCK)

            try:
                if not select.select([conn], [], [], 0)[0]:
                    return ''

                r = conn.read(maxsize)
                if not r:
                    return self._close(which)

                if self.universal_newlines:
                    r = self._translate_newlines(r)
                return r
            finally:
                if not conn.closed:
                    fcntl.fcntl(conn, fcntl.F_SETFL, flags)

PIPE = subprocess.PIPE

def recv_some(p, t=.1, e=1, tr=5):
    if tr < 1:
        tr = 1
    x = time.time()+t
    y = []
    y2 = []
    r = ''
    pr = p.recv
    pr_err = p.recv_err
    finish=False
    while time.time() < x or r or r_err:
        r = pr()
        r_err = pr_err()
        if r is None:
            finish=True
        elif r:
            y.append(r)
        else:
            time.sleep(max((x-time.time())/tr, 0))
        if r_err is None:
            nothing = 0
        elif r_err:
            y2.append(r_err)
        else:
            nothing = 0
    return [''.join(y),''.join(y2),finish]

def send_all(p, data):
    while len(data):
        sent = p.send(data)
        if sent is None:
            raise Exception(message)
        data = buffer(data, sent)

#end: Subprocess with Pipes

# return liste der filenames
def filter_filenames(source):
  result = []
  source2 = source
  while (1):
    ssource = source2.split("$<",1)
    if len(ssource) < 2:
      break
    ssource3 = ssource[1].split("$>",1)
    if len(ssource3) < 2:
      break
    result.append(ssource3[0].replace("\\","/"))
    source2 = ssource3[1]
  return result

use_svg=False

try:
    import rsvg
    use_svg=True
except ImportError:
    use_svg=False
    #print"Warning, could not import 'rsvg'"
    #if os.name == 'nt':
        #print "Detected windows, creating rsvg."
        ##some workarounds for windows

        #from ctypes import *

        #l=CDLL('librsvg-2-2.dll')
        #g=CDLL('libgobject-2.0-0.dll')
        #g.g_type_init()

        #class rsvgHandle():
            #class RsvgDimensionData(Structure):
                #_fields_ = [("width", c_int),
                            #("height", c_int),
                            #("em",c_double),
                            #("ex",c_double)]

            #class PycairoContext(Structure):
                #_fields_ = [("PyObject_HEAD", c_byte * object.__basicsize__),
                            #("ctx", c_void_p),
                            #("base", c_void_p)]

            #def __init__(self, path):
                #self.path = path
                #error = ''
                #self.handle = l.rsvg_handle_new_from_file(self.path,error)
                ##self.handle = l.rsvg_handle_new_gz(self.path,error)


            #def get_dimension_data(self):
                #svgDim = self.RsvgDimensionData()
                #l.rsvg_handle_get_dimensions(self.handle,byref(svgDim))
                #return (svgDim.width,svgDim.height,svgDim.width,svgDim.height)

            #def render_cairo(self, ctx):
                #ctx.save()
                #z = self.PycairoContext.from_address(id(ctx))
                #l.rsvg_handle_render_cairo(self.handle, z.ctx)
                #ctx.restore()



        #class rsvgClass():
            #def Handle(self,file):
                #return rsvgHandle(file)

        #rsvg = rsvgClass()




if (os.name=='nt'):
    import win32api, win32con


def get_fullpath_from_relative(basepath,relativepath):
    basepath=basepath.replace("\\","/")
    relativepath=relativepath.replace("\\","/")
    if ((basepath==None) or (basepath=="")):
        return relativepath
    if ((relativepath==None) or (relativepath=="")):
        return relativepath
    if relativepath[1]==':' or relativepath[0]=='/'  :
           return relativepath
    return  basepath+'/'+relativepath

def get_output_main_groups(outputlist):
   groups = []
   for outputitem in outputlist:
      S = outputitem.split('.')
      if len(S)<2: continue
      if S[0]=='output':
        is_in = False
        for item in groups:
          if S[1]==item:
            is_in = True
        if is_in==False:
          groups.append(S[1])
   return groups

def get_output_subgroups(outputlist,group):
  subgroups = []
  for outputitem in outputlist:
    S = outputitem.split('.')
    if len(S)<3: continue
    if ((S[0]=='output') and (S[1] == group)):
        is_in = False
        for item in subgroups:
          if S[2]==item:
            is_in = True
        if is_in==False:
          subgroups.append(S[2])
  return subgroups

def get_output_subgroup_vars(outputlist,group,subgroup):
  varlist = []
  for outputitem in outputlist:
    S = outputitem.split('.')
    if len(S)<4: continue
    if ((S[0]=='output') and (S[1] == group) and (S[2] == subgroup)):
        is_in = False
        for item in varlist:
          if S[3]==item:
            is_in = True
        if is_in==False:
          varlist.append(S[3])
  varliststr = ''
  first = 0
  for varlistitem in varlist:
    if first!=0:
      varliststr=varliststr+','
    first= first+1
    varliststr=varliststr+'output.'+group+'.'+subgroup+'.'+varlistitem
  return varliststr

def get_cfg_values(filename):
    config = ConfigParser()
    # Ini Datei laden:
    valueslist = []
    try:
        config.read(filename)
        for section in config.sections():
            for option in config.options(section):
                val1 = config.get(section,option)
                val2 = val1.split("$or$")
                for val3 in val2:
                    value = [section,option,val3]
                    valueslist.append(value)
    except:
        print "Error: Loading CFG FILE: ", filename
    return valueslist


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


class cModules:
    def __init__(self):
        self.name=""
        self.libname=""
        self.active=False

def get_modules_list(Modules,Model,Submodel):
    active=-1
    modules_list = []
    new_model = cModules()
    new_model.name = "empty"
    modules_list.append(new_model)
    for i in range(len(Modules)):
        if (Modules[i].modul==Model) and (Modules[i].submodul==Submodel):
            new_model = Modules[i]
            modules_list.append(new_model)
            if new_model.active==True:
              active=1
    if active==-1:
      modules_list[0].active=True
    return modules_list

class cModulesList:
    def __init__(self):
        self.name=""
        self.icon=""
        self.submodules=[]


class cResultList:
    def __init__(self):
        self.name=""
        self.subcategory=[]

def item_in_list(item,list):
    for item2 in list:
        if item2==item:
            return True
    return False

def get_modul_list(modules):
    modul_list = []
    for item in modules:
        if (item_in_list(item.modul,modul_list)==False):
            modul_list.append(item.modul)
    return modul_list

def get_submodul_list(modul,modules):
    submodul_list = []
    subm_list2 = []
    for item in modules:
        item2 = item.submodul
        if ((modul==item.modul) and (item_in_list(item2,subm_list2)==False)):
            submodul_list.append(item)
            subm_list2.append(item2)
    return submodul_list

def get_name_list(modul,submodul,modules):
    name_list=[]
    for item in modules:
        if (item.submodul==submodul) and (item.modul==modul):
            name_list.append(item)
    return name_list

def get_filtered_libName(name):
    S = name.split(".")
    S2 = S[len(S)-2]
    S = S2.split("/")
    S2 = S[len(S)-1]
    S = S2.split("\\")
    S2 = S[len(S)-1]
    return S2[3:]


def convert_uri_to_file(uri):
    uri = uri.replace('%20',' ')
    file = uri[7:]
    if find(file,":")==-1:
        return file
    else:
        return file[1:]



def convert_file_to_uri(file):
    file = file.replace('%20',' ')
    if file[0]=='/':
        return('file://'+file)
    else:
        return('file:///'+file)

class c_XPNStartScreen:
    def __init__(self,builder,use_svg=True):
        self.builder=builder
        if use_svg:
            self.use_svg()
        else:
            self.use_png()



    def use_png(self):
        self.temp_width = 0
        self.temp_height = 0
        hboxStartScreen = self['hboxStartScreen']
        image = gtk.Image()
        image.set_from_file('GUI_start_final.png')
        self.pixbuf = image.get_pixbuf()
        org_width=self.pixbuf.get_width()
        org_height=self.pixbuf.get_height()
        box = gtk.ScrolledWindow()
        box.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
        image.connect('expose-event', self.on_image_resize2, box,org_width,org_height)
        box.add(image)
        box.set_size_request(int(float(org_width)/2.0*1.4), int(float(org_height)/2.0*1.4))
        hboxStartScreen.add(box)
        hboxStartScreen.show_all()

    def use_svg(self):
        hboxStartScreen = self['hboxStartScreen']
        self.drawingareaXPNStartScreensvg = rsvg.Handle('GUI_start_final.svg')
        [width,height,widthf,heightf] = self.drawingareaXPNStartScreensvg.get_dimension_data()
        image = gtk.DrawingArea()
        image.connect('expose-event', self.on_image_resize_svg)
        hboxStartScreen.add(image)
        image.set_size_request(int(float(width)*1.4),int(float(height)*1.4))
        hboxStartScreen.show_all()


    def on_image_resize_svg(self,win,event):
        x, y, w, h = win.allocation
        BORDER_WIDTH=10
        cr = win.window.cairo_create()
        cr.set_source_color(win.style.fg[win.state])
        svg = self.drawingareaXPNStartScreensvg
        area = win.get_allocation()

        [width,hight,widthf,hightf] = svg.get_dimension_data()

        if svg != None:
            matrix = cairo.Matrix(float(area.width)/float(width),0,0,float(area.height)/float(hight),0, 0)
            cr.transform (matrix)
            svg.render_cairo(cr)


        return True


    def on_image_resize(self, widget, event, window,org_width,org_height):
        allocation = widget.get_allocation()
        if self.temp_width != allocation.width:
            self.temp_width = allocation.width
            allocation.height = self.temp_width*org_height/org_width
            pixbuf = self.pixbuf.scale_simple(allocation.width, allocation.height, gtk.gdk.INTERP_BILINEAR)
            widget.set_from_pixbuf(pixbuf)

    def on_image_resize2(self, widget, event, window,org_width,org_height):
        allocation = widget.get_allocation()
        if self.temp_height != allocation.height or self.temp_width != allocation.width:
            self.temp_height = allocation.height
            self.temp_width = allocation.width
            pixbuf = self.pixbuf.scale_simple(allocation.width, allocation.height, gtk.gdk.INTERP_BILINEAR)
            widget.set_from_pixbuf(pixbuf)

    def show(self):
        self.dialog_statistic_help.show()

    def hide(self):
        self.dialog_statistic_help.hide()




    def __getitem__(self,key):
        return self.builder.get_object(key)


class MainWindow:
    def __init__(self):
        self.FileAction = []
        builder = gtk.Builder()
        builder.add_from_file("expertn_gui_main.ui")
        self.window = builder.get_object("window")
        self.textoutput = builder.get_object("textoutput")
        self.texterror = builder.get_object("texterror")
        self.textviewoutput = builder.get_object("textviewoutput")
        self.textviewerror = builder.get_object("textviewerror")
        self.labelTime = builder.get_object("labelTime")
        self.frameError = builder.get_object("frameError")
        self.aboutdialog = builder.get_object("aboutdialog")
        self.frameOptions = builder.get_object("frameOptions")
        self.textbufferOptions = builder.get_object("textbufferOptions")
        self.labelOptions = builder.get_object("labelOptions")
        self.dialogxnoplot = builder.get_object("dialogxnoplot")
        self.liststoreCDB = builder.get_object("liststoreCDB")
        self.treeviewCDB = builder.get_object("treeviewCDB")
        self.dialogClose = builder.get_object("dialogClose")
        self.dialog_cfg_general = builder.get_object("dialog_cfg_general");
        self.drawingareaXPNStartScreen = builder.get_object("drawingareaXPNStartScreen")
        #self.project_filename="/home/chrikle/projects/newmatrix/software/WXPN/cfg/expertn_driver_bin.xpn"



        self.XPNStartScreen = c_XPNStartScreen(builder,use_svg)

        self.treeviewXPNInstances_Path=""
        self.buttonSaveDescription_id = -1

        builder.connect_signals(self)
        self.builder =  builder

        self.config = ConfigParser()
        toolsstr = ""
        # Ini Datei laden:
        try:
            self.config.read("expertn_gui.ini")
            self.expertn_bin = self.config.get("PATHS","expertn_bin")
            self.base_path = self.config.get("PATHS","base_path")
            try:
                toolsstr = self.config.get("TOOLS","tool_list")
            except:
                nothing=0

            try:
                self.delay_workaround = double(self.config.get("WINDOWS","delay_workaround"))
            except:
                self.delay_workaround = 0.1

        except:
            print "Error: Loading INI FILE"

        self.tools_list = []
        self.expertn_run_mode="stop"

        for toolitem in toolsstr.split(';'):
            toolitem2 = toolitem.split(',',2)
            if len(toolitem2)>=3:
                self.tools_list.append([toolitem2[0],toolitem2[1],toolitem2[2]])


        self.expertntxtoutlist = [];

        recentfilter = gtk.RecentFilter()
        recentfilter.add_pattern("*.xpn")
        recentfilter.set_name("Expert-N Project Files")
        self["recentchooser1"].add_filter(recentfilter)

        self.read_result_structure()




    def read_result_structure(self):
        config = ConfigParser()
        config.read("result.def")

        result_list=[]
        for name in config.get("results","category").rstrip().split(","):
                results = cResultList()
                results.name = name
                results.subcategory = config.items(name)
                result_list.append(results)

        hboxRestultCategory  = self["hboxRestultCategory"]
        hboxResultSubcategory  = self["hboxResultSubcategory"]
        for item in result_list:
                button = gtk.Button(label=item.name)
                hboxRestultCategory.pack_start(button,True,True,3)
                button.show_all()
        for item in result_list[0].subcategory:
            button = gtk.Button(label=item[0])
            hboxResultSubcategory.pack_start(button,True,True,3)
            button.show_all()


    def retrieve_info(self, widget):
        selected = self["recentchooser1"].get_current_item()
        self.project_filename=convert_uri_to_file(selected.get_uri())
        self.project_filename = unicode(self.project_filename, 'utf-8')
        self["notebook1"].set_current_page(1)
        manager = gtk.recent_manager_get_default()
        manager.add_item(convert_file_to_uri(self.project_filename))
        self.window.set_title("Expert N Control Center    -  "+xnoplot.get_filename_without_path(self.project_filename).split(".")[0] +"  -   ")





    def __getitem__(self,key):
        return self.builder.get_object(key)


    def on_window_destroy(self,widget, data=None):
        id = self.dialogClose.run()
        if (id==1):
            if self.expertn_run_mode!="stop":
                self.expertn_run_mode="stop"
                while (self["buttonExpertNStop"].get_property('sensitive')!=True) and (self["buttonExpertNPause"].get_property('sensitive')!=True) and (self["buttonExpertNStart"].get_property('sensitive')==True):
                    time.sleep(0.1)
            self.window.destroy()
            gtk.main_quit()
        self.dialogClose.hide()
        return True


    def on_config_general(self,widget, data=None):
        self.show_options_text(self.project_filename)




    def act_output_stdout(self,obj):
        return True
        if self.process.poll():
            return False
        else:
            return True
    def act_output_stderr(self,obj):
        if self.process.poll():
            return False
        else:
            return True

    def treeview_output_doubleclick(self,treeview,path,col,userdate=None):
      model = treeview.get_model()
      piter = model.get_iter(path)
      if piter!=None:
        Filename = model.get_value(piter,0)
        Filename_withpath = model.get_value(piter,1)
        Name = model.get_value(piter,2)
        Vars = model.get_value(piter,3)
        filename = model.get_value(piter,1)
        if (Vars!=''):
          self.open_cdb_var_list(xnoplot.get_varname_withoutpath(Name),Vars,filename)
        else:
          if (Filename==xnoplot.get_filename_without_path(Filename_withpath)):
             self.open_cdb_var_list(xnoplot.get_filename_without_path(filename),'',filename)

    def act_output_thread2(self,error):
        self.texterror.set_text("")
        while (1):
            line = error.readline()
            if not line: break
            gtk.gdk.threads_enter()
            self.texterror.insert(self.texterror.get_end_iter(),line)
            self.textviewerror.scroll_to_iter(self.texterror.get_end_iter(),0.0)
            self.frameError.show()
            gtk.gdk.threads_leave()

    def add_file_to_outputlist(self,filename):
      #Falls schon in der Liste, dann wird zuerst dieser Dateiname geloescht:
       Filename_withoutpath = xnoplot.get_filename_without_path(filename)
       treestore = self["treestore_output"]
       treeviewModules = self["treeview_output"]
       selection = treeviewModules.get_selection()
       selection.unselect_all()
       model = treeviewModules.get_model()
       first = None
       i = 0
       while (1):
          if i == 0:
             first = model.get_iter_first()
          else:
            if first==None:
                break
            first = model.iter_next(first)
          if first==None:
            break
          i = i+1
          ListFilename = model.get_value(first,1)
          ListFilename_withpath = model.get_value(first,0)
          if ((ListFilename == filename) and (ListFilename_withpath==Filename_withoutpath)):
              model.remove(first)
              first = model.get_iter_first()
              i =1
       piter = self['treestore_output'].append(None,[xnoplot.get_filename_without_path(filename),filename,'output',''])
       headers = xnoplot.read_cdb_header(filename)
       groups = get_output_main_groups(headers)
       for group in groups:
           piter2 = self['treestore_output'].append(piter, [group,filename,'output.'+group,''])
           subgroup = get_output_subgroups(headers,group)
           for subgroupitem in subgroup:
                  varliststr = get_output_subgroup_vars(headers,group,subgroupitem)
                  self['treestore_output'].append(piter2, [subgroupitem,filename,'output.'+group+'.'+subgroupitem,varliststr])

    def act_output_thread3(self,output):
        while (1):
            output.write(".q\n")
            #if not line:
            #    break


    def act_output_thread(self,shell):
        outputlistcount=0
        self.expertntxtoutlist =  []
        self.textoutput.set_text("")
        self.texterror.set_text("")
        self.texterror.set_text("")

        #shell = ['/home/chrikle/expertn50/built/bin/expertn_bin','-d',"/home/chrikle/expertn50/built",'-n',"--config-file=/home/chrikle/expertn50/built/cfg/modell_vergleich.xpn"]
        lastsign=""
        p = Popen(shell,stdin=PIPE, stdout=PIPE, stderr=PIPE)
        while (1):
            try:
                [text,text_error,finish] = recv_some(p)
                text = text.replace("\r","")
                text_error = text_error.replace("\r","")
                gtk.gdk.threads_enter()
                for line in text.split("\n"):
                    if (find(line,'Simulation Day')!=-1):
                        self.labelTime.set_text(line+"\n")
                    elif (find(line,'<TXTOUTPUT:')!=-1):
                        if outputlistcount>100:
                            continue
                        S = line.split("'")
                        time.sleep(0.1)
                        outputlistcount=outputlistcount+1

                        self.add_file_to_outputlist(S[1])
                        self['frameOutput'].show()
                    elif (find(line,'<COMMAND LINE:')!=-1):
                        if self.expertn_run_mode=="stop":
                            send_all(p, "q")
                        elif self.expertn_run_mode=="pause":
                            gtk.gdk.threads_leave()
                            while (self.expertn_run_mode=="pause"):
                                nothing=0.1
                            send_all(p, "n")
                            self["buttonExpertNStart"].set_sensitive(False)
                            self["buttonExpertNPause"].set_sensitive(True)
                            self["buttonExpertNStop"].set_sensitive(True)
                        else:
                            send_all(p, "n")
                    elif (line=="") and (lastsign==""):
                        nothing=0
                    else:
                        if (os.name=='posix'):
                            self.textoutput.insert(self.textoutput.get_end_iter(),line+"\n")
                            self.textviewoutput.scroll_to_iter(self.textoutput.get_end_iter(),0.0)
                        else:
                            time.sleep(self.delay_workaround)
                            do_gui_operation(self.textoutput.insert, self.textoutput.get_end_iter(),line+"\n")
                            do_gui_operation(self.textviewoutput.scroll_to_iter, self.textoutput.get_end_iter(),0.0)
                        lastsign = line

                if text_error!="":
                    if (os.name=='posix'):
                        self.texterror.insert(self.texterror.get_end_iter(),text_error)
                        self.textviewerror.scroll_to_iter(self.texterror.get_end_iter(),0.0)
                        self.frameError.show()
                    else:
                        time.sleep(self.delay_workaround)
                        do_gui_operation(self.texterror.insert,self.texterror.get_end_iter(),text_error)
                        do_gui_operation(self.textviewerror.scroll_to_iter,self.texterror.get_end_iter(),0.0)
                        self.frameError.show()
                gtk.gdk.threads_leave()
                if finish:
                    self["buttonExpertNStart"].set_sensitive(True)
                    self["buttonExpertNPause"].set_sensitive(False)
                    self["buttonExpertNStop"].set_sensitive(False)
                    self.expertn_run_mode="stop"
                    break

            except:
                self["buttonExpertNStart"].set_sensitive(True)
                self["buttonExpertNPause"].set_sensitive(False)
                self["buttonExpertNStop"].set_sensitive(False)
                self.expertn_run_mode="stop"
                break










        #self.expertntxtoutlist =  []
        #self.textoutput.set_text("")
        #self.texterror.set_text("")
        #p = subprocess.Popen([filename,param1,param2,param3,param4,param5], shell=False,  stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE,close_fds = True)
        #(output,input,error) = (p.stdin, p.stdout, p.stderr)
        #thread.start_new_thread(self.act_output_thread2, (error,))
        ##thread.start_new_thread(self.act_output_thread3, (output,))
        #self.xpn_pid = p.pid
        #while (1):
            #line = input.readline()
            #if not line:
                #break
            #gtk.gdk.threads_enter()
            #if (find(line,'Simulation Day')!=-1):
                #self.labelTime.set_text(line)
            #elif (find(line,'<TXTOUTPUT:')!=-1):
                #S = line.split("'")
                #time.sleep(0.1)
                #self.add_file_to_outputlist(S[1])
                #self['frameOutput'].show()
            #elif (find(line,'<COMMAND LINE:')!=-1):
                #nothing = 0
                ##p.stdin.write(".q\n")
            #elif (find(line,'.q')!=-1):
                #nothing = 0
                ##p.stdin.write(".q\n"
            #elif (find(line,'.n')!=-1):
                #nothing = 0
            #else:
                #self.textoutput.insert(self.textoutput.get_end_iter(),line)
                #self.textviewoutput.scroll_to_iter(self.textoutput.get_end_iter(),0.0)


            #gtk.gdk.threads_leave()




    def on_run_expertn(self,widget, data=None):
        if self.expertn_run_mode=="pause":
            self.expertn_run_mode="start"
            return
        self.expertn_run_mode="start"
        self["buttonExpertNStart"].set_sensitive(False)
        self["buttonExpertNPause"].set_sensitive(True)
        self["buttonExpertNStop"].set_sensitive(True)
        thread.start_new_thread(self.act_output_thread, ([self.expertn_bin,"-d",self.base_path,"-c",self.project_filename,'-n'],))


    def on_run_valgrind_expertn(self,widget, data=None):
        thread.start_new_thread(self.act_output_thread, ("valgrind --leak-check=yes /home/chrikle/projects/newmatrix/expertn_modul/expertn_bin/Debug/expertn_bin",))

    def on_stop_expertn(self,widget, data=None):
        self.expertn_run_mode="stop"
        #  Fuer Linux:
        #if (os.name=='posix'):
        #    os.kill(self.xpn_pid, signal.SIGKILL)

        #  Fuer Windows:
        #if (os.name=='nt'):
        #    handle = win32api.OpenProcess( win32con.PROCESS_TERMINATE, 0, self.xpn_pid )
        #    win32api.TerminateProcess( handle, 0 )
        #    win32api.CloseHandle( handle )

        #os.kill(self.xpnp.pid, signal.SIGKILL)

    def on_pause_expertn(self,widget, data=None):
        self.expertn_run_mode="pause"
        self["buttonExpertNStart"].set_sensitive(True)
        self["buttonExpertNPause"].set_sensitive(False)
        self["buttonExpertNStop"].set_sensitive(True)

    def on_show_error_messages(self,widget, data=None):
        self.frameError.show()
##        if widget.get_active()==True:
##            self.frameError.show()
##        else:
##            self.frameError.hide()

    def on_show_results(self,widget,data=None):
        self['frameOutput'].show()
##        if widget.get_active()==True:
##            self['frameOutput'].show()
##            self['notebook_view'].set_current_page(2)
##        else:
##            self['frameOutput'].hide()
##            self['notebook_view'].set_current_page(0)

    def on_hide_results(self,widget,data=None):
        self['frameOutput'].hide()

    def on_hide_error_messages(self,widget, data=None):
        self.frameError.hide()

    def on_about_dialog(self,widget, data=None):
        self.aboutdialog.run()
        self.aboutdialog.hide()
    def show_options_text(self,file):
        self.openfile = file
        S = '<b>Options: <i>' + file + '</i></b>'
        self.labelOptions.set_markup(S)
        self.frameOptions.show()
        self.textbufferOptions.set_text("")
        f = open(file,'rU')
        for line in f:
            self.textbufferOptions.insert_at_cursor(line)

    def on_options_save(self, data=None):
        f = open(self.openfile,'w')
        f.write(self.textbufferOptions.get_text(self.textbufferOptions.get_start_iter(),self.textbufferOptions.get_end_iter()))
        self.openfile=''
        self.frameOptions.hide()

    def newWin(self,source,page,x,y,user_data):
        wind = gtk.Window()
        noteb = gtk.Notebook()
        noteb.set_show_tabs(False)
        wind.add(noteb)
        wind.set_default_size(600,600)
        #wind.set_title(user_data)
        wind.set_title(source.get_tab_label(page).get_label())
        wind.connect("delete-event",self.delete_output_tab,noteb,source)
        wind.show_all()
        return noteb

    def delete_output_tab(self,widget,data, noteb,source):
        wid = noteb.get_nth_page(0)
        tab = noteb.get_tab_label(wid)
        noteb.remove_page(0)
        #self['notebookPlot'].append_page(wid,tab)
        #self['notebookPlot'].set_tab_reorderable(wid,True)
        #self['notebookPlot'].set_tab_detachable(wid,True)
        source.append_page(wid,tab)
        source.set_tab_reorderable(wid,True)
        source.set_tab_detachable(wid,True)
        gtk.notebook_set_window_creation_hook(self.newWin,tab.label_str)
        return False

    def on_options_cancel(self, data=None):
        self.openfile=''
        self.frameOptions.hide()


    def on_cdb_plot(self,widget, data=None):
        #subprocess.Popen(['xnoplot'])
        #return

        self.liststoreCDB.clear()
        for S in self.expertntxtoutlist:
            iter = self.liststoreCDB.insert(-1)
            self.liststoreCDB.set_value(iter,0,S)
        iter = self.liststoreCDB.insert(-1)
        self.liststoreCDB.set_value(iter,0,"Open no file")

        model = self.liststoreCDB
        first = model.get_iter_first()
        selection = self.treeviewCDB.get_selection()
        selection.select_iter(first)


        id = self.dialogxnoplot.run()
        if (id==1):
            model = self.liststoreCDB
            selection = self.treeviewCDB.get_selection()
            tup = selection.get_selected()
            iter = tup[1] #model.get_iter((int(path),))
            if (iter!=None):
                val = model.get_value(iter,0)
            else:
                val='Open no file'
            if (val!='Open no file'):
                cdb_plot_args=['expertn_gui','--is_lib','--file='+val]
                Mwindow = xnoplot.MainWindow(cdb_plot_args)
                Mwindow.window.show()
                #subprocess.Popen(args=['python','xnoplot.py','--file='+val])
            else:
                #subprocess.Popen(['xnoplot'])
                cdb_plot_args=['expertn_gui','--is_lib']
                Mwindow = xnoplot.MainWindow(cdb_plot_args)
                Mwindow.window.show()
        self.dialogxnoplot.hide()

    def on_temporal_changes(self,widget, data=None):
        self.open_cdb_var_list('Temporal Changes','OUTPUT.Heat.Air Mean Temperature,OUTPUT.Heat.Soil Temperature[0],OUTPUT.Heat.Soil Temperature[1],OUTPUT.Heat.Soil Temperature[2],OUTPUT.Heat.Soil Temperature[3]')

    def on_temporal_changes_water(self,widget, data=None):
        self.open_cdb_var_list('Temporal Changes','OUTPUT.Water.Water Content[0],OUTPUT.Water.Water Content[1],OUTPUT.Water.Water Content[2],OUTPUT.Water.Water Content[3]')

    def on_climate(self,widget, data=None):
        self.open_cdb_var_list('Climate','OUTPUT.Heat.Air Mean Temperature,OUTPUT.Water.Precipitation [mm]')

    def on_matric_potential(self,widget, data=None):
        self.open_cdb_var_list('Matric Potential','OUTPUT.Water.Matrice Potential [kPa][0],OUTPUT.Water.Matrice Potential [kPa][1],OUTPUT.Water.Matrice Potential [kPa][2],OUTPUT.Water.Matrice Potential [kPa][3]')

    def on_matric_hydraulic_conductivity(self,widget, data=None):
        self.open_cdb_var_list('Hydraulic Conductivity','OUTPUT.Water.Hydraulic Conductivity [mm/day][0],OUTPUT.Water.Hydraulic Conductivity [mm/day][1],OUTPUT.Water.Hydraulic Conductivity [mm/day][2],OUTPUT.Water.Hydraulic Conductivity [mm/day][3]')



    def on_Cum_N_Transformations(self,widget, data=None):
        self.open_cdb_var_list('Cum N Transformations','OUTPUT.Nitrate N Leaching,OUTPUT.Mineralization,OUTPUT.Denitrifcation,OUTPUT.Nitrification,OUTPUT.Immobilization')

    def on_Mineral_N_Measurements(self,widget, data=None):
        self.open_cdb_var_list('Mineral N Measurements','OUTPUT.NO3 Profile,OUTPUT.NO3 lower 2/3 Profile,OUTPUT.NO3 lower 1/3 Profile,OUTPUT.NO4 Profile,OUTPUT.Urea  Profile,OUTPUT.Urea Hydrolysis')

    def on_N_Temporal_Changes(self,widget, data=None):
        self.open_cdb_var_list('N Temporal Changes','OUTPUT.NO3 Profile,OUTPUT.NO3 lower 2/3 Profile,OUTPUT.NO3 lower 1/3 Profile,OUTPUT.NO4 Profile,OUTPUT.Urea  Profile,OUTPUT.Urea Hydrolysis')

    def on_N_Transformations(self,widget, data=None):
        self.open_cdb_var_list('N Transformations','OUTPUT.Urea Hydrolysis,OUTPUT.N-Release Fresh Org Matter,OUTPUT.N-Release Soil Org. Matter,OUTPUT.Nitrified N,OUTPUT.Denitrified N,OUTPUT.Immobilized N')

    def on_Mineralisation_Immbob(self,widget, data=None):
        self.open_cdb_var_list('Mineralisation / Immobilisation','OUTPUT.N Immobilization in Humus,OUTPUT.N Immobilzation in Young Soil Org. Matter,OUTPUT.N Immobilization in Litter,OUTPUT.N Release in Humus,OUTPUT.N Release from Young Soil Org. Matter,OUTPUT.N Release from Litter')

    def on_C_and_N_Pools(self,widget, data=None):
        self.open_cdb_var_list('C-  and N- Pools','OUTPUT.C Litter 0 -30 cm depth,OUTPUT.C Manure 0-30 cm depth,OUTPUT.C Humus 0-30 cm depth,OUTPUT.N Litter 0-30 cm depth,OUTPUT.N Manure 0-30 cm depth,OUTPUT.N Humus 0-30 cm depth')

    def on_C_N_Ratios(self,widget, data=None):
        self.open_cdb_var_list('C/N Ratios','OUTPUT.C/N Ratio Litter 0-30 cm depth,OUTPUT.C/N Ratio Manure 0-30 cm depth,OUTPUT.C/N Ratio Humus 0-30 cm depth')

    def on_tab_destroy(self,notebook,vbox):
        notebook.remove_page(notebook.page_num(vbox))

    def open_cdb_var_list(self,title,var_list,filename=''):
        val=''
        if filename!='':
          val= filename
        else:
          if len(self.expertntxtoutlist)==1:
              val=self.expertntxtoutlist[0]
          else:
              self.liststoreCDB.clear()
              for S in self.expertntxtoutlist:
                  iter = self.liststoreCDB.insert(-1)
                  self.liststoreCDB.set_value(iter,0,S)
              model = self.liststoreCDB
              first = model.get_iter_first()
              selection = self.treeviewCDB.get_selection()
              selection.select_iter(first)

              id = self.dialogxnoplot.run()
              self.dialogxnoplot.hide()

              if (id==1):
                  model = self.liststoreCDB
                  selection = self.treeviewCDB.get_selection()
                  tup = selection.get_selected()
                  iter = tup[1] #model.get_iter((int(path),))
                  if (iter!=None):
                      val = model.get_value(iter,0)
                  else:
                      val='Open no file'
        if (val!='Open no file'):
            #subprocess.Popen(args=['python','xnoplot.py','--file='+val,"--plot="+var_list,'--title='+title,'--hide_vars'])
            if var_list=='':
              cdb_plot_args=['expertn_gui','--is_lib','--file='+val,'--title='+title]
            else:
              cdb_plot_args=['expertn_gui','--is_lib','--file='+val,"--plot="+var_list,'--title='+title,'--hide_vars']
            Mwindow = xnoplot.MainWindow(cdb_plot_args)
            hbox = Mwindow["vbox1"]
            notebookPlot = self["notebookPlot"]

            tabbtn = xnoplot.GtkTabButton(0)
            tabbtn.set_label(title)
            tabbtn.set_comment("Filename: "+val + "\n\n" +"Path: "+var_list)


            tabbtn.label.connect_object("button-release-event",Mwindow.on_ChangeFigureName,notebookPlot,None,tabbtn)
            tabbtn.show_all()
            vbox = gtk.VBox(False, 5)
            vbox.show()
            tabbtn.closebutton.connect_object("clicked",self.on_tab_destroy, notebookPlot,vbox)
            notebookPlot.append_page(vbox,tabbtn)
            notebookPlot.set_tab_reorderable(vbox,True)
            notebookPlot.set_tab_detachable(vbox,True)
            gtk.notebook_set_window_creation_hook(self.newWin,tabbtn.label_str)
        #self.notebookPlot.set_default_size(600,600)
            notebookPlot.show()
            notebookPlot.set_current_page(-1)
            hbox.reparent(vbox)
            hbox.show()
            self["notebook_view"] .set_current_page(2)
            #Mwindow.window.show()

            #builder = gtk.Builder()
            #builder.add_from_file("expertn_gui_mdi.ui")
            #mdiwindow = builder.get_object("mdiwindow")
            #backframe = builder.get_object("backframe")
            #backframe2 = builder.get_object("backframe2")
            #frame = builder.get_object("frame")

            #vbox = gtk.VBox(False, 5)
            #vbox.set_size_request(500,500)

            #mdiwindow.reparent(vbox)
            #hbox.reparent(frame)

            #self["hboxGraphs"].add(vbox)

             # Frame Auswaehlen:
             #backframe.modify_bg(gtk.STATE_NORMAL, gtk.gdk.color_parse('#0000ff'))
            #backframe2.modify_bg(gtk.STATE_NORMAL, gtk.gdk.color_parse('#add8e6'))
            #builder.get_object("frame1").modify_bg(gtk.STATE_NORMAL, gtk.gdk.color_parse('#add8e6'))
             #self["hboxGraphs"].add(hbox)
            #vbox.show()
            #hbox.show()
            #mdiwindow.show()
            #Mwindow.window.show()
        else:
            #subprocess.Popen(['xnoplot'])
            cdb_plot_args=['expertn_gui','--is_lib']
            Mwindow = xnoplot.MainWindow(cdb_plot_args)
            Mwindow.window.show()

    def AssistantNewApply(self,assistant, button,entryFile,entryTemplate):

        self.project_filename = entryFile.get_text().replace("\\","/")
        self.FileAction = []
        template = entryTemplate.get_text().replace("\\","/")
        p=xpn_cfg_manager.cxpn_project(base_path=self.base_path,project_filename=self.project_filename)
        p.create_new_project(self.project_filename,template)

        grids=p.project_file_read_intances()
        if len(grids)<=0:
            p.add_new_grid(template,"$BASE_PATH/cfg_template/project.xpi","",1,1,1,1)


        self["notebook1"].set_current_page(1)
        manager = gtk.recent_manager_get_default()
        manager.add_item(convert_file_to_uri(self.project_filename))
        self.window.set_title("Expert N Control Center    -  "+xnoplot.get_filename_without_path(self.project_filename).split(".")[0] +"  -   ")
        assistant.hide_on_delete()





    def AssistantNewCancel(self,assistant, button):
        assistant.hide_on_delete()

    def on_xpnhelp(self,widget,data=None):
        self.on_run_Tool_with_filename(xpn_cfg_manager.replace_std_templates("$BASE_PATH/doc/expertn_doc.pdf",self.project_filename,self.base_path,""))

    def  AssistantChooseFile(self,widget,entryFile,assistant,vbox):
        dialog = gtk.FileChooserDialog("New Expert-N Project",None,gtk.FILE_CHOOSER_ACTION_SAVE,(gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,gtk.STOCK_OPEN, gtk.RESPONSE_OK))
        dialog.set_current_folder(get_fullpath_from_relative(self.base_path,'cfg'))
        filter = gtk.FileFilter()
        filter.set_name("Expert-N Project Files (*.xpn)")
        filter.add_pattern("*.xpn")
        dialog.add_filter(filter)
        dialog.set_filter(filter)
        dialog.set_default_response(gtk.RESPONSE_OK)
        response = dialog.run()
        if response == gtk.RESPONSE_OK:
            #self.project_filename=dialog.get_filename()
            S = dialog.get_filename()
            S2 = S.split('.')
            if S2[len(S2)-1] !='xpn':
              S=S+'.xpn'
            entryFile.set_text(S)
            assistant.set_page_complete(vbox, True)
        dialog.destroy()

    def AssistantChooseTemplate(self,widget,entryFile,assistant,vbox):
        dialog = gtk.FileChooserDialog("Template Filename",None,gtk.FILE_CHOOSER_ACTION_OPEN,(gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,gtk.STOCK_OPEN, gtk.RESPONSE_OK))
        dialog.set_current_folder(get_fullpath_from_relative(self.base_path,'cfg_template'))
        filter = gtk.FileFilter()
        filter.set_name("Expert-N Project Files (*.xpn)")
        filter.add_pattern("*.xpn")
        dialog.add_filter(filter)
        dialog.set_filter(filter)
        dialog.set_default_response(gtk.RESPONSE_OK)
        response = dialog.run()
        if response == gtk.RESPONSE_OK:
            S = dialog.get_filename()
            entryFile.set_text(S)
        dialog.destroy()

    def on_new_project(self,widget, data=None):
        #self['assistantNew'].set_current_page(0)
        #self['assistantNew'].show_all()
        assistant = gtk.Assistant()
        assistant.set_transient_for(self['window'])
        assistant.set_size_request(600,400)

        vbox = gtk.VBox()
        vbox.set_border_width(5)
        page = assistant.append_page(vbox)
        assistant.set_page_title(vbox, "Create a new Expert-N Project")
        assistant.set_page_type(vbox, gtk.ASSISTANT_PAGE_INTRO)
        label = gtk.Label("This Assistant will help you to create a new Expert-N Project.\nThe Assistant is used to guide a user through configuration of an project.")
        label.set_line_wrap(True)
        vbox.pack_start(label, True, True, 0)
        assistant.set_page_complete(vbox, True)

        vbox = gtk.VBox()
        vbox.set_border_width(5)
        assistant.append_page(vbox)
        assistant.set_page_title(vbox, "Choose a Project File")
        assistant.set_page_type(vbox, gtk.ASSISTANT_PAGE_CONTENT)
        label = gtk.Label("Please choose the path and the file of your new Expert-N project!")
        label.set_line_wrap(True)
        hbox = gtk.HBox()
        hbox.set_border_width(5)
        entryFile = gtk.Entry()
        button = gtk.Button()
        button.set_relief(gtk.RELIEF_NONE)
        button.connect('clicked',self.AssistantChooseFile,entryFile,assistant,vbox)
        im = gtk.image_new_from_stock("gtk-save-as",1)
        button.set_image(im)
        hbox2 = gtk.HBox()
        hbox2.set_border_width(5)
        entryFile2 = gtk.Entry()
        template_project = self.base_path+"/cfg_template/project.xpn"
        entryFile2.set_text(template_project)
        button2 = gtk.Button()
        button2.set_relief(gtk.RELIEF_NONE)
        button2.connect('clicked',self.AssistantChooseTemplate,entryFile2,assistant,vbox)
        im2 = gtk.image_new_from_stock("gtk-open",1)
        button2.set_image(im2)
        hbox.pack_start(entryFile, True, True, 0)
        hbox.pack_start(button, False,False, 0)
        hbox2.pack_start(entryFile2, True, True, 0)
        hbox2.pack_start(button2, False, False, 0)
        vbox.pack_start(gtk.Label("Project Filename:"), False, False, 0)
        vbox.pack_start(hbox, False, False, 0)
        vbox.pack_start(gtk.Label("Template Filename:"), False, False, 0)
        vbox.pack_start(hbox2, False, False, 0)
        #vbox.pack_start(hbox2, False, False, 0)
        vbox.pack_start(label, True, True, 0)
        assistant.set_page_complete(vbox, False)

        vbox = gtk.VBox()
        vbox.set_border_width(5)
        assistant.append_page(vbox)
        assistant.set_page_title(vbox, "Configuration Complete")
        assistant.set_page_type(vbox, gtk.ASSISTANT_PAGE_CONFIRM)
        label = gtk.Label("The configuration of your new Expert-N project is done.\nClick to Apply!")
        label.set_line_wrap(True)
        vbox.pack_start(label, True, True, 0)
        assistant.set_page_complete(vbox, True)

        assistant.connect("apply", self.AssistantNewApply, "Apply",entryFile,entryFile2)
        assistant.connect("cancel", self.AssistantNewCancel, "Cancel")

        assistant.show_all()

    def on_open_project(self,widget, data=None):
        dialog = gtk.FileChooserDialog("Open Expert-N Project",None,gtk.FILE_CHOOSER_ACTION_OPEN,(gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,gtk.STOCK_OPEN, gtk.RESPONSE_OK))
        filter = gtk.FileFilter()
        filter.set_name("Expert-N Project Files (*.xpn)")
        filter.add_pattern("*.xpn")
        dialog.add_filter(filter)
        dialog.set_filter(filter)
        dialog.set_default_response(gtk.RESPONSE_OK)
        response = dialog.run()
        if response == gtk.RESPONSE_OK:
            self.project_filename=dialog.get_filename()
            self["notebook1"].set_current_page(1)
            manager = gtk.recent_manager_get_default()
            manager.add_item(convert_file_to_uri(self.project_filename))
            self.window.set_title("Expert N Control Center    -  "+xnoplot.get_filename_without_path(self.project_filename).split(".")[0] +"  -   ")
        dialog.destroy()


    def on_start_screen(self,widget, data=None):
        if self["notebook1"].get_current_page()==0:
            self["notebook1"].set_current_page(1)
        else:
            self["notebook1"].set_current_page(0)

    def on_buttonInstancesGroupDelete(self,widget,data=None,data2=None):
        treestore = self['treestoreXPNInstances']
        treeview = self['treeviewXPNInstances']
        model = treestore
        selection = treeview.get_selection()
        tup = selection.get_selected()
        iter = tup[1] #model.get_iter((int(path),))
        if (iter!=None):
            code = model.get_value(iter,2)
            name = model.get_value(iter,1)
            codearray = self.get_code_array(code)
            id = self['dialog_Question'].run()
            self['dialog_Question'].hide()
            if id == 1:
                self['vboxInstanceundergroup'].hide()
                self['vboxmaingroup'].hide()
                # Delete Grid:
                if model.iter_parent(iter)==None:
                    p=xpn_cfg_manager.cxpn_project(base_path=self.base_path,project_filename=self.project_filename)
                    p.rm_grid(int(codearray[1]),int(codearray[2]),int(codearray[3]))
                # Delete Instance:
                else:
                    number = int(name)
                    p=xpn_cfg_manager.cxpn_project(base_path=self.base_path,project_filename=self.project_filename)
                    p.rm_instance(int(codearray[1]),int(codearray[2]),int(codearray[3]),number)
            self.show_xpn_instances()



    def on_button_New_Template_File_Open(self,widget,data=None):
        dialog = gtk.FileChooserDialog("Choose Template File",None,gtk.FILE_CHOOSER_ACTION_OPEN,(gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,gtk.STOCK_OPEN, gtk.RESPONSE_OK))
        dialog.set_current_folder(get_fullpath_from_relative(self.base_path,'cfg_template'))
        filter = gtk.FileFilter()
        filter.set_name("Expert-N Config Files (*.xpi)")
        filter.add_pattern("*.xpi")
        dialog.add_filter(filter)
        dialog.set_filter(filter)
        dialog.set_default_response(gtk.RESPONSE_OK)
        response = dialog.run()
        if response == gtk.RESPONSE_OK:
            S = dialog.get_filename()
            self["entry_New_Template_File"].set_text(S)

        dialog.destroy()

    def on_NewGridTemplateFileButton(self,widget,data=None):
        dialog = gtk.FileChooserDialog("Choose Template File",None,gtk.FILE_CHOOSER_ACTION_OPEN,(gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,gtk.STOCK_OPEN, gtk.RESPONSE_OK))
        dialog.set_current_folder(get_fullpath_from_relative(self.base_path,'cfg_template'))
        filter = gtk.FileFilter()
        filter.set_name("Expert-N Config Files (*.xpi)")
        filter.add_pattern("*.xpi")
        dialog.add_filter(filter)
        dialog.set_filter(filter)
        dialog.set_default_response(gtk.RESPONSE_OK)
        response = dialog.run()
        if response == gtk.RESPONSE_OK:
            S = dialog.get_filename()
            self["entryNewGridTemplateFile"].set_text(S)

        dialog.destroy()


    def on_buttonInstancesGroupNew(self,widget,data=None,data2=None):
        treestore = self['treestoreXPNInstances']
        treeview = self['treeviewXPNInstances']
        model = treestore
        selection = treeview.get_selection()
        tup = selection.get_selected()
        iter = tup[1] #model.get_iter((int(path),))
        if (iter!=None):
            code = model.get_value(iter,2)
            name = model.get_value(iter,1)
            codearray = self.get_code_array(code)
            #piter = treestoreXPNInstances.append(None,[act,S,group])
            if model.iter_parent(iter)==None:
                self['dialogInstancesEditgroup'].set_title('Create New Grid')
                templatefile="$BASE_PATH/cfg_template/project.xpi"
                templateproject = xpn_cfg_manager.get_project_file_from_cfg_file(templatefile)
                templateregstr = xpn_cfg_manager.get_regstr_from_filename(templatefile)
                templatefile1 = xpn_cfg_manager.replace_std_template_and_path(templatefile,templateproject,self.base_path,templateregstr)

                self['entryNewGridTemplateFile'].set_text(templatefile1)

                id = self['dialogInstancesEditgroup'].run()
                self['dialogInstancesEditgroup'].hide()
                if id==1:
                    codearray = []
                    codearray.append(str(int(self['checkbuttondialogInstancesEditgroupact'].get_active())))
                    codearray.append(self['entrydialogInstancesEditgroupID'].get_text())
                    codearray.append(self['entrydialogInstancesEditgroupi'].get_text())
                    codearray.append(self['entrydialogInstancesEditgroupj'].get_text())

                    p=xpn_cfg_manager.cxpn_project(base_path=self.base_path,project_filename=self.project_filename)
                    templatefile=self["entryNewGridTemplateFile"].get_text()
                    templateproject = xpn_cfg_manager.get_project_file_from_cfg_file(templatefile)
                    templateregstr = xpn_cfg_manager.get_regstr_from_filename(templatefile)

                    cover = 100.0
                    p.add_new_grid(templateproject,templatefile,templateregstr,int(codearray[0]),int(codearray[1]),int(codearray[2]),int(codearray[3]),cover)

                self.show_xpn_instances()

            else:# Instance:
                self['dialogInstancesEditSubgroup'].set_title('Create New Instance')
                templatefile="$BASE_PATH/cfg_template/project.xpi"
                templateproject = xpn_cfg_manager.get_project_file_from_cfg_file(templatefile)
                templateregstr = xpn_cfg_manager.get_regstr_from_filename(templatefile)
                templatefile1 = xpn_cfg_manager.replace_std_template_and_path(templatefile,templateproject,self.base_path,templateregstr)

                self['entry_New_Template_File'].set_text(templatefile1)
                id = self['dialogInstancesEditSubgroup'].run()
                self['dialogInstancesEditSubgroup'].hide()
                if id==1:
                    p=xpn_cfg_manager.cxpn_project(base_path=self.base_path,project_filename=self.project_filename)
                    templatefile=self["entry_New_Template_File"].get_text()
                    templateproject = xpn_cfg_manager.get_project_file_from_cfg_file(templatefile)
                    templateregstr = xpn_cfg_manager.get_regstr_from_filename(templatefile)

                    cover = float(self['entrydialogInstancesEditSubgroupCover'].get_text())

                    p.add_new_instance(templateproject,templatefile,templateregstr,int(codearray[1]),int(codearray[2]),int(codearray[3]),cover)


                self.show_xpn_instances()


    def checkbuttonInstancesAct_toggled_cb_toggled(self,widget,data=None):
        treestore = self['treestoreXPNInstances']
        treeview = self['treeviewXPNInstances']
        model = treestore
        selection = treeview.get_selection()
        tup = selection.get_selected()
        iter = tup[1] #model.get_iter((int(path),))
        if (iter!=None):
            self.treeviewXPNInstances_Path = model.get_string_from_iter(iter)
            code = model.get_value(iter,2)
            name = model.get_value(iter,1)
            codearray = self.get_code_array(code)
            p=xpn_cfg_manager.cxpn_project(base_path=self.base_path,project_filename=self.project_filename)
            if widget.get_active():
                codearray[0]="1"
            else:
                codearray[0]="0"
            p.change_active(int(codearray[1]),int(codearray[2]),int(codearray[3]),int(codearray[0]))
            self.show_xpn_instances()







    def on_buttonInstancesUnderGroup(self,widget,event=None,data2=None):
        treestore = self['treestoreXPNInstances']
        treeview = self['treeviewXPNInstances']
        model = treestore
        selection = treeview.get_selection()
        tup = selection.get_selected()
        iter = tup[1] #model.get_iter((int(path),))
        if (iter!=None):
            code = model.get_value(iter,2)
            name = model.get_value(iter,1)

            self.treeviewXPNInstances_Path = model.get_string_from_iter(iter)


            # [akt,id,i,j,filename_rel1,cover2,filename_rel1,cover2,...] = codearray
            codearray = self.get_code_array(code)
            codearrayold = []
            for item in codearray:
                codearrayold.append(item)
            #Grid Editieren Dialog):
            if model.iter_parent(iter)==None:
                self['entrydialogInstancesEditgroupID'].set_text(codearray[1])
                self['entrydialogInstancesEditgroupi'].set_text(codearray[2])
                self['entrydialogInstancesEditgroupj'].set_text(codearray[3])
                self['checkbuttondialogInstancesEditgroupact'].set_active(int(codearray[0]))
                self['dialogInstancesEditgroup'].set_title('Edit Grid')
                self['vbox42TemplateFile'].hide()
                id = self['dialogInstancesEditgroup'].run()
                if id==1:
                    codearray[1] = self['entrydialogInstancesEditgroupID'].get_text()
                    codearray[2] = self['entrydialogInstancesEditgroupi'].get_text()
                    codearray[3] =  self['entrydialogInstancesEditgroupj'].get_text()
                    codearray[0] = str(int(self['checkbuttondialogInstancesEditgroupact'].get_active()))
                    lencodearray = len(codearray)

                    p=xpn_cfg_manager.cxpn_project(base_path=self.base_path,project_filename=self.project_filename)
                    p.change_active(int(codearrayold[1]),int(codearrayold[2]),int(codearrayold[3]),int(codearray[0]))
                    p.rename_grid(int(codearrayold[1]),int(codearrayold[2]),int(codearrayold[3]),int(codearray[1]),int(codearray[2]),int(codearray[3]))

                    codestr = self.set_code_str(codearray)
                    rootiter = iter
                    i = 0
                    while (True):
                        iter = model.iter_nth_child(rootiter,i)
                        if iter==None:
                            break
                        model.set_value(iter,2,codestr)
                        i=i+1

                    model.set_value(rootiter,2,codestr)
                    name='('+codearray[1]+','+codearray[2]+','+codearray[3]+')'
                    model.set_value(rootiter,1,name)
                self['vbox42TemplateFile'].show()
                self['dialogInstancesEditgroup'].hide()
                self.show_xpn_instances()


            # Cover Verstellen: (Edit Subgroup)
            else:
                self['entrydialogInstancesEditSubgroupCover'].set_text(codearray[5+int(name)*2])
                self['dialogInstancesEditSubgroup'].set_title('Edit Instance')
                self['vbox43TemplateFile'].hide()

                id = self['dialogInstancesEditSubgroup'].run()
                if id==1:
                    p=xpn_cfg_manager.cxpn_project(base_path=self.base_path,project_filename=self.project_filename)
                    p.change_cover_frac(int(codearray[1]),int(codearray[2]),int(codearray[3]),int(name),float(self['entrydialogInstancesEditSubgroupCover'].get_text()))
                    codearray[5+int(name)*2]=self['entrydialogInstancesEditSubgroupCover'].get_text()
                    codestr = self.set_code_str(codearray)
                    rootiter = model.iter_parent(iter)
                    i = 0
                    while (True):
                        iter = model.iter_nth_child(rootiter,i)
                        if iter==None:
                            break
                        model.set_value(iter,2,codestr)
                        i=i+1

                    model.set_value(rootiter,2,codestr)

                self['dialogInstancesEditSubgroup'].hide()
                self['vbox43TemplateFile'].show()
                self.show_xpn_instances()

            treeviewXPNInstances = self['treeviewXPNInstances']
            treestoreXPNInstances = self['treestoreXPNInstances']
            self.on_treeviewXPNInstances_Select(None,treestoreXPNInstances,treeviewXPNInstances)

    def on_activate_all(self,widget):
        p=xpn_cfg_manager.cxpn_project(base_path=self.base_path,project_filename=self.project_filename)
        p.change_active_all(1)
        self.show_xpn_instances()
        treeviewXPNInstances = self['treeviewXPNInstances']
        treestoreXPNInstances = self['treestoreXPNInstances']
        self.on_treeviewXPNInstances_Select(None,treestoreXPNInstances,treeviewXPNInstances)


    def on_deactivate_all(self,widget):
        p=xpn_cfg_manager.cxpn_project(base_path=self.base_path,project_filename=self.project_filename)
        p.change_active_all(0)
        self.show_xpn_instances()
        treeviewXPNInstances = self['treeviewXPNInstances']
        treestoreXPNInstances = self['treestoreXPNInstances']
        self.on_treeviewXPNInstances_Select(None,treestoreXPNInstances,treeviewXPNInstances)


    def on_instace_press(self,treeview,event):
         if event.button==3:
                #Popup Menueliste fuer VarListe
                menu = gtk.Menu()
                menu_item = gtk.CheckMenuItem("activate all")
                menu.append(menu_item)
                menu_item.connect("activate", self.on_activate_all)
                menu_item.show()
                menu_item = gtk.CheckMenuItem("deactivate all")
                menu.append(menu_item)
                menu_item.connect("activate", self.on_deactivate_all)
                menu_item.show()
                x = int(event.x)
                y = int(event.y)
                time = event.time
                menu.popup( None, None, None, event.button, time)



    def model_options_opt_edit_cell_Value(self,cellrenderertext, path,new_text):
        liststore_model_options = self["liststore_model_options_global_opt"]
        iter = liststore_model_options.get_iter(path)
        liststore_model_options.set(iter, 1, new_text);

    def model_options_opt_global_edit_cell_Option(self,cellrenderertext, path,new_text):
        liststore_model_options = self["liststore_model_options_global_opt"]
        iter = liststore_model_options.get_iter(path)
        liststore_model_options.set(iter, 0, new_text);
        self.dialog_project_options_save()

    def Add_Global_opt_Option(self,widget, data=None):
       id = self["dialog_Add_Options"].run()
       if (id==1):
         liststore_model_options_global = self["liststore_model_options_global_opt"]
         iter = liststore_model_options_global.insert(-1)
         liststore_model_options_global.set_value(iter,0,self["entry_Add_Option_Option"].get_text())
         liststore_model_options_global.set_value(iter,1,self["entry_Add_Option_Value"].get_text())
       self["dialog_Add_Options"].hide()
       self.dialog_project_options_save()

    def Delete_Global_opt_Option(self,widget, data=None):
       treeview_model_options_global = self["treeview_model_options_global_opt"]
       selection = treeview_model_options_global.get_selection()
       model, treeiter = selection.get_selected()
       # If there's no selection, treeiter will be None
       if treeiter is None: return
       id = self["dialog_Question"].run()
       if (id==1):
          model.remove(treeiter)
       self["dialog_Question"].hide()
       self.dialog_project_options_save()

    def treeview_model_options_global_opt_activated(self,widget,data=None):
        liststoreModel = self["liststore_model_options_global_opt"]
        treeviewModel = self["treeview_model_options_global_opt"]
        model = liststoreModel
        selection = treeviewModel.get_selection()
        tup = selection.get_selected()
        iter = tup[1] #model.get_iter((int(path),))
        act = False
        if (iter!=None):
                val = model.get_value(iter,1) #Nummer
                filenames = filter_filenames(val)
                if len(filenames) != 0:
                    act = True
        if act:
            self['buttonGlobalOptionsOptEditFilename'].set_sensitive(True)
        else:
            self['buttonGlobalOptionsOptEditFilename'].set_sensitive(False)

    def on_global_options_opt_open_file_with_extern_editor(self,widget):
        liststoreModel = self["liststore_model_options_global_opt"]
        treeviewModel = self["treeview_model_options_global_opt"]
        model = liststoreModel
        selection = treeviewModel.get_selection()
        tup = selection.get_selected()
        iter = tup[1] #model.get_iter((int(path),))
        if (iter!=None):
                val = model.get_value(iter,1) #Nummer
                filenames = filter_filenames(val)
                for filename in filenames:
                    REG_STR = ''
                    PROJECT_NAME = xnoplot.get_filename_without_path(self.project_filename)[:-4]
                    BASE_PATH = self.base_path;
                    self.on_run_Tool_with_filename(xpn_cfg_manager.replace_std_templates(filename,self.project_filename,BASE_PATH,REG_STR))

    def on_button_dialog_options_standard_values_global_cellrenderertoggle_toggled(self,cell,path, data=None):
        liststore = self["liststore_options_standard_values_global"]
        model = liststore
        iter = model.get_iter((int(path),))
        val = model.get_value(iter,0)
        val = not val
        model.set(iter,0,val)


    def get_all_filenames_from_cfg_file(self,cfg_name):
        filenames = []
        cfg_values=get_cfg_values(cfg_name)
        for cfg_value in cfg_values:
            [section,subsection,val] = cfg_value
            filenames2 = filter_filenames(val)
            for filename in filenames2:
                filenames.append(filename)
        return filenames




    def run_dialog_options_standard_values_global(self,widget,data=None):

        #configureation in list schreiben:

        list_value_model_options = []
        treeview_model_options = self["treeview_model_options_global_opt"]
        liststore_model_options = self["liststore_model_options_global_opt"]
        treemodel_model_options = treeview_model_options.get_model()
        first = liststore_model_options.get_iter_first()
        if first!=None:
            val = treemodel_model_options.get_value(first,0)
            val2 = treemodel_model_options.get_value(first,1)
            list_value_model_options.append([val,val2])
            iter = first
            while (1):
                iter = treemodel_model_options.iter_next(iter)
                if iter==None:
                    break
                val = treemodel_model_options.get_value(iter,0)
                val2 =  treemodel_model_options.get_value(iter,1)
                list_value_model_options.append([val,val2])



        treeview = self["treeview_options_standard_values_global"]
        liststore = self["liststore_options_standard_values_global"]

        # Librarys:
        liblistcfg = []
        liblist = []
        liststoreModules = self["liststoreModules"]
        treeviewModules = self["treeviewModules"]
        selection = treeviewModules.get_selection()
        selection.unselect_all()
        model = treeviewModules.get_model()
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
            Listlibname = model.get_value(first,0)
            liblist.append(Listlibname)
            liblistcfg.append(xpn_cfg_manager.replace_std_templates("$BASE_PATH/cfg_template/lib"+Listlibname+".cfg",self.project_filename,self.base_path,""))

        try:
            self.template_file_list = glob.glob(xpn_cfg_manager.replace_std_templates("$BASE_PATH/cfg_template/*.cfg",self.project_filename,self.base_path,""))
        except:
            self.template_file_list = []

        for i in range(len(self.template_file_list)):
            self.template_file_list[i] = self.template_file_list[i].replace("\\","/")

        # falls lib templatefile hat, dann parameter laden:

        values_list = []
        for i in range(len(self.template_file_list)):
            for i2 in range(len(liblistcfg)):
                if liblistcfg[i2]==self.template_file_list[i]:
                    values_list2 = get_cfg_values(liblistcfg[i2])
                    for values in values_list2:
                        [section,option,value] = values
                        lib = liblist[i2]
                        values2 = [section,option,value,lib]
                        values_list.append(values2)

        liststore.clear()

        i=0
        options_temp_list = []
        for values in values_list:
            [section,option,value,lib] = values
            if section=="global":
                if option not in options_temp_list:
                    options_temp_list.append(option)
                    iter = liststore.insert(-1)
                    set_true = True
                    for list_value in list_value_model_options:
                        [option2,val] = list_value
                        if option2==option:
                            set_true=False
                    i2 = 0
                    for val2 in values_list:
                        [section2,option2,value2,lib2] = val2
                        if section2==section and option2==option and lib == lib2 and i>i2:
                            set_true=False
                            break
                        i2+=1
                    if (value.find("$-") != -1):
                        set_true=False
                        value = value.replace("$-","")

                    liststore.set_value(iter,0,set_true)
                    liststore.set_value(iter,1,lib)
                    liststore.set_value(iter,2,option)
                    liststore.set_value(iter,3,value)
                i+=1

        id = self["dialog_options_standard_values_global"].run()

        self["dialog_options_standard_values_global"].hide()

        if (id==1):
            newoptions = []
            model_options = treeview.get_model()
            first = model_options.get_iter_first()
            if first!=None:
                val = [model_options.get_value(first,0),model_options.get_value(first,1),model_options.get_value(first,2),model_options.get_value(first,3)]
                if val[0]==True:
                    newoptions.append([val[1],val[2],val[3]])
                while (1):
                    first = model_options.iter_next(first)
                    if first==None:
                        break
                    val = [model_options.get_value(first,0),model_options.get_value(first,1),model_options.get_value(first,2),model_options.get_value(first,3)]
                    if val[0]==True:
                        newoptions.append([val[1],val[2],val[3]])

            for value in newoptions:
                [section,option,value2] = value
                add_var=True
                i=0
                for list_value in list_value_model_options:
                    [option2,val] = list_value
                    if option2==option:
                        list_value = [option2,value2]
                        list_value_model_options[i] = list_value
                        add_var = False
                        break
                    i=i+1
                if add_var:
                    list_value = [option,value2]
                    list_value_model_options.append(list_value)
                    file_list = filter_filenames(value2)
                    for file1 in file_list:
                            REG_STR_OLD = ""
                            PROJECT_NAME_OLD = "project"
                            BASE_PATH_OLD=self.base_path+"/cfg_template"
                            PROJECT_FILENAME_OLD =BASE_PATH_OLD+"/"+PROJECT_NAME_OLD+".xpn"
                            REG_STR_NEW = ""
                            PROJECT_NAME_NEW = xnoplot.get_filename_without_path(self.project_filename)[:-4]
                            BASE_PATH_NEW = self.base_path;
                            src = get_fullpath_from_relative(self.base_path,xpn_cfg_manager.replace_std_templates(file1,PROJECT_FILENAME_OLD,BASE_PATH_OLD,REG_STR_OLD))
                            dst = get_fullpath_from_relative(self.base_path,xpn_cfg_manager.replace_std_templates(file1,self.project_filename,BASE_PATH_NEW,REG_STR_NEW))
                            self.file_list_memory.append([section,option,value2,True])
                            if src!=dst:
                                  self.FileAction.append(['copy',src,dst])


            liststore_model_options.clear()
            for list_value in list_value_model_options:
                [option2,val] = list_value
                iter1 = liststore_model_options.insert(-1)
                liststore_model_options.set_value(iter1,0,option2)
                liststore_model_options.set_value(iter1,1,val)

        self.dialog_project_options_save()

    def show_xpn_instances(self):
        config = ConfigParser()
        config.read(self.project_filename)
        model_grid = config.get("model","grid")
        treeviewXPNInstances = self['treeviewXPNInstances']
        treestoreXPNInstances = self['treestoreXPNInstances']
        treestoreXPNInstances.clear()
        instance_group = model_grid.split(';')
        for group in instance_group:
            subgroup = group.split(',')
            if len(subgroup) < 6:
              break;
            lensubgroup = len(subgroup)
            act = subgroup[0]
            S ='('+ str(subgroup[1])+','+str(subgroup[2])+',' + str(subgroup[3]) + ')'
            piter = treestoreXPNInstances.append(None,[act,S,group])
            i=4
            i2=0
            while(1):
              if i+1>lensubgroup:
                break
              #S = str(subgroup[i+1])
              S = str(i2)
              piter2 = treestoreXPNInstances.append(piter,[act,S,group])
              i=i+2
              i2=i2+1

        piter = treestoreXPNInstances.append(None,[False,"Environment",""])
        piter = treestoreXPNInstances.append(None,[False,"Plugins and System",""])
        treeviewXPNInstances.expand_all()
        #self['hboxInstanceButtons'].set_sensitive(False)
        model = treeviewXPNInstances.get_model()

        if self.treeviewXPNInstances_Path=="":
            self['hboxInstanceButtons'].set_sensitive(False)
        else:
            iter1 = model.get_iter_from_string(self.treeviewXPNInstances_Path)
            selection = treeviewXPNInstances.get_selection()
            selection.select_iter(iter1)






    def on_cfg_general_dialog(self,widget, data=None):
        treeviewXPNInstances = self['treeviewXPNInstances']
        treestoreXPNInstances = self['treestoreXPNInstances']
        config = ConfigParser()
        config.read(self.project_filename)
        cpus = config.get("system","num_proc")
        debug_level = config.get("system","debug_level")

        modules=config.get("modul","libs")
        module_path="moduls"#config.get("modul","path")

        model_lat = 0#config.get("model","lat")
        model_lon = 0#config.get("model","lon")
        model_cfg = 0#config.get("model","cfg")
        model_var_list = ""#config.get("model","var_list")
        model_act = 0#config.get("model","active")


        self["spinbutton_cpus"].set_value(int(cpus))
        self["spinbutton_debug_level"].set_value(int(debug_level))

        #self["entryTextFilename"].set_text(output_txtfile)
        #self["checkbuttonTextFile"].set_active(int(output_use_txt))

        #self["checkbuttonDatabase"].set_active(int(output_use_database))
        #self["entryProvider"].set_text(output_database_provider)
       # self["entryDatabaseString"].set_text(output_database_string)

        #self["entry_ModulPath"].set_text(module_path)

        liststoreModules = self["liststoreModules"]
        liststoreInstances = self["liststoreInstances"]
        treeviewInstances = self["treeviewInstances"]


        liststoreModules.clear()
        for modul in modules.split(";"):
            if modul!='':
                iter = liststoreModules.insert(-1)
                liststoreModules.set_value(iter,0,modul)

        model_var_list_split = model_var_list.split(";")


        self.show_xpn_instances()


        self.FileAction = []

        treeviewXPNInstances.connect('cursor-changed', self.on_treeviewXPNInstances_Select,treestoreXPNInstances,treeviewXPNInstances)
        self['vboxInstanceundergroup'].hide()
        self['vboxmaingroup'].hide()

        #GlobalOptions:
        self.To_Delete_1 = []
        self.To_Delete_2 = []
        self.To_Delete_3 = []

        treeview_model_options_global_opt = self['treeview_model_options_global_opt']
        liststore_model_options_global_opt = self['liststore_model_options_global_opt']
        liststore_model_options_global_opt.clear()


        try:
            for items in config.items('options'):
                iter = liststore_model_options_global_opt.insert(-1)
                liststore_model_options_global_opt.set_value(iter,0,items[0])
                liststore_model_options_global_opt.set_value(iter,1,items[1])
                self.To_Delete_1.append('options')
                self.To_Delete_2.append(items[0])
                self.To_Delete_3.append(items[1])
        except:
            nothing=0

        #Parameter Merken, die Dateien enthalten:
        # Reihenfolge: [libname,paramter,value,dateimanipulation getan]
        self.file_list_memory = []
        for i in range(len(self.To_Delete_3)):
            file_list = filter_filenames(self.To_Delete_3[i])
            if len(file_list)>0:
                    self.file_list_memory.append([self.To_Delete_1[i],self.To_Delete_2[i],self.To_Delete_3[i],False])


        id1 = self["buttonGlobalOptionsOptEditFilename"].connect("clicked",self.on_global_options_opt_open_file_with_extern_editor)

        id = self.dialog_cfg_general.run()
        id = 1

        self["buttonGlobalOptionsOptEditFilename"].disconnect(id1)

        self.dialog_cfg_general.hide()

        self.dialog_project_options_save()


    def dialog_project_options_save(self):
            treeviewXPNInstances = self['treeviewXPNInstances']
            treestoreXPNInstances = self['treestoreXPNInstances']
            config = ConfigParser()
            config.read(self.project_filename)

            try:
               config.add_section("system")
            except:
               dummy=0
            config.set("system","num_proc",int(self["spinbutton_cpus"].get_value()))
            config.set("system","debug_level",int(self["spinbutton_debug_level"].get_value()))
           # config.set("system","output_use_txt",int(self["checkbuttonTextFile"].get_active()))
           # config.set("system","output_use_database",int(self["checkbuttonDatabase"].get_active()))
           # config.set("system","output_txtfile",self["entryTextFilename"].get_text())
           # config.set("system","output_database_provider",self["entryProvider"].get_text())
           # config.set("system","output_database_string",self["entryDatabaseString"].get_text())

            try:
               config.add_section("modul")
            except:
               dummy=0

            #config.set("modul","path",self["entry_ModulPath"].get_text())
            #with open(self.project_filename, 'wb') as configfile:
            #    config.write(configfile)

            # Librarys:
            liblist = []
            liststoreModules = self["liststoreModules"]
            treeviewModules = self["treeviewModules"]
            selection = treeviewModules.get_selection()
            selection.unselect_all()
            model = treeviewModules.get_model()
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
                  Listlibname = model.get_value(first,0)
                  liblist.append(Listlibname)
            libnamesstr = ""
            for  libname in liblist:
              libnamesstr = libnamesstr + libname + ';'
            config.set("modul","libs",libnamesstr)

            #Instances:
            # treeviewXPNInstances = self['treeviewXPNInstances']
            #treestoreXPNInstances
            model = treeviewXPNInstances.get_model()
            first = None
            codelist= []
            i = 0
            while (1):
                  if i == 0:
                     first = model.get_iter_first()
                  else:
                     first = model.iter_next(first)
                  if first==None:
                     break
                  i = i+1
                  code = model.get_value(first,2)
                  if model.iter_parent(first)==None:
                       codelist.append(code)
            #list to string
            codestr=''
            for codeitem in codelist:
                codestr=codestr+codeitem+';'

            try:
               config.add_section("model")
            except:
               dummy=0
            config.set("model","grid",codestr)


            #Globel Options:
            for i in range(len(self.To_Delete_1)):
                config.remove_option(self.To_Delete_1[i],self.To_Delete_2[i])

            list_value_Option = []
            list_value_Value = []

            treeview_model_options_global_opt = self['treeview_model_options_global_opt']
            liststore_model_options_global_opt = self['liststore_model_options_global_opt']
            treemodel_model_options_global_opt = treeview_model_options_global_opt.get_model()
            first = liststore_model_options_global_opt.get_iter_first()

            if first!=None:
                val = treemodel_model_options_global_opt.get_value(first,0)
                val2 = treemodel_model_options_global_opt.get_value(first,1)
                list_value_Option.append(val)
                list_value_Value.append(val2)
                iter = first
                while (1):
                    iter = treemodel_model_options_global_opt.iter_next(iter)
                    if iter==None:
                        break
                    val = treemodel_model_options_global_opt.get_value(iter,0)
                    val2 =  treemodel_model_options_global_opt.get_value(iter,1)
                    list_value_Option.append(val)
                    list_value_Value.append(val2)
            try:
                config.add_section('options')
            except:
                  nothing=0
            for i in range(len(list_value_Option)):
                  config.set('options', list_value_Option[i],list_value_Value[i])
                  for i2 in range(len(self.file_list_memory)):
                      [lib,v1,v2,man] = self.file_list_memory[i2]
                      if lib=='options' and v1==list_value_Option[i]:
                          file_list2 = filter_filenames(list_value_Value[i])
                          file_list1 = filter_filenames(v2)
                          filelen = min(len(file_list1),len(file_list2))
                          self.file_list_memory[i2][3] = True
                          for i3 in range(filelen):
                              REG_STR = ""
                              PROJECT_NAME = xnoplot.get_filename_without_path(self.project_filename)[:-4]
                              BASE_PATH = self.base_path;
                              src = get_fullpath_from_relative(self.base_path,xpn_cfg_manager.replace_std_templates(file_list1[i3],self.project_filename,BASE_PATH,REG_STR))
                              dst = get_fullpath_from_relative(self.base_path,xpn_cfg_manager.replace_std_templates(file_list2[i3],self.project_filename,BASE_PATH,REG_STR))
                              if src!=dst:
                                  self.FileAction.append(['rename',src,dst])

            for i in range(len(self.file_list_memory)):
                   [lib,v1,v2,man] = self.file_list_memory[i]
                   if man==False:
                       REG_STR = ""
                       PROJECT_NAME = xnoplot.get_filename_without_path(self.project_filename)[:-4]
                       BASE_PATH = self.base_path;
                       file_list1 = filter_filenames(v2)
                       for i3 in range(len(file_list1)):
                           src = get_fullpath_from_relative(self.base_path,xpn_cfg_manager.replace_std_templates(file_list1[i3],self.project_filename,BASE_PATH,REG_STR))
            # TODO: Was habe ich mir dabei gedacht????
            #               self.FileAction.append(['rm',src,""])

            configfile = open(self.project_filename, 'wb')
            config.write(configfile)

            # Kopier, Loesch und umbenenn Aktionen

            self.do_fileoperations(self.FileAction)
            self.FileAction=[]
            self.To_Delete_1 = []
            self.To_Delete_2 = []
            self.To_Delete_3 = []

    def do_fileoperations(self,FileAction):
        if len(FileAction)==0:
            return

        liststore_fileoperations = self["liststore_fileoperations"]
        treeview_fileoperations = self["treeview_fileoperations"]
        treeview_fileoperations_model = treeview_fileoperations.get_model()


        liststore_fileoperations.clear()
        for item in FileAction:
            [act,src,dst] = item
            iter1 = liststore_fileoperations.insert(-1)
            liststore_fileoperations.set_value(iter1,0,True)
            liststore_fileoperations.set_value(iter1,1,act)
            liststore_fileoperations.set_value(iter1,2,xnoplot.get_filename_without_path(src))
            liststore_fileoperations.set_value(iter1,3,xnoplot.get_filename_without_path(dst))
            liststore_fileoperations.set_value(iter1,4,src)
            liststore_fileoperations.set_value(iter1,5,dst)


        id = self["dialog_fileoperation"].run()
        self["dialog_fileoperation"].hide()
        id = 1;
        if id==1:
            file_action_do = []
            iter1 = treeview_fileoperations_model.get_iter_first()
            while (1):
                if iter1==None:
                    break
                execute=treeview_fileoperations_model.get_value(iter1,0)
                action=treeview_fileoperations_model.get_value(iter1,1)
                file1=treeview_fileoperations_model.get_value(iter1,4)
                file2=treeview_fileoperations_model.get_value(iter1,5)
                if execute:
                    file_action_do.append([action,file1,file2])
                iter1 = treeview_fileoperations_model.iter_next(iter1)

            for item in file_action_do:
                [act,src,dst] = item
                if act=='copy':
                    try:
                        shutil.copy(src,dst)
                    except:
                        print "Error: copy file: ",src,dst
                if act=='rm':
                    try:
                        os.remove(src)
                    except:
                        print "Error: remove file: ",src
                if act=='rename':
                    try:
                        shutil.move(src,dst)
                    except:
                        print "Error: rename File: ",src,dst




    def on_add_module(self,widget, data=None):
        dialog = gtk.FileChooserDialog("Add Moduls",None,gtk.FILE_CHOOSER_ACTION_OPEN,(gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,gtk.STOCK_OPEN, gtk.RESPONSE_OK))
        filter = gtk.FileFilter()
        filter.set_name("Modul Files (*.so, *.dll)")
        filter.add_pattern("*.dll")
        filter.add_pattern("*.so")
        dialog.add_filter(filter)
        dialog.set_filter(filter)
        dialog.set_default_response(gtk.RESPONSE_OK)
        dialog.set_current_folder(get_fullpath_from_relative(self.base_path,"models"))
        dialog.set_select_multiple(True)
        response = dialog.run()
        if response == gtk.RESPONSE_OK:
           filenames = dialog.get_filenames()
           liststoreModules = self["liststoreModules"]
           treeviewModules = self["treeviewModules"]
           for filename in filenames:
                filtered_libname = get_filtered_libName(filename)
                selection = treeviewModules.get_selection()
                selection.unselect_all()
                model = treeviewModules.get_model()
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
                  Listlibname = model.get_value(first,0)
                  if (Listlibname==filtered_libname):
                     model.remove(first)
                     first = liststoreModules.get_iter_first()
                     i =1

                iter = liststoreModules.insert(-1)
                liststoreModules.set_value(iter,0,get_filtered_libName(filename))
        dialog.hide()
        self.dialog_project_options_save()


    def on_result_file_add(self,widget, data=None):
        dialog = gtk.FileChooserDialog("Add Result File",None,gtk.FILE_CHOOSER_ACTION_OPEN,(gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,gtk.STOCK_OPEN, gtk.RESPONSE_OK))
        filter = gtk.FileFilter()
        filter.set_name("Expert-N 5.0 (*.xno)")
        filter.add_pattern("*.xno")
        dialog.add_filter(filter)
        dialog.set_filter(filter)
        filter = gtk.FileFilter()
        filter.set_name("C Debug File (*.cdb)")
        filter.add_pattern("*.cdb")
        dialog.add_filter(filter)
        dialog.set_default_response(gtk.RESPONSE_OK)
        #dialog.set_current_folder(get_fullpath_from_relative(self.base_path,self["entry_ModulPath"].get_text()))
        dialog.set_select_multiple(True)
        response = dialog.run()
        if response == gtk.RESPONSE_OK:
           filenames = dialog.get_filenames()
           for filename in filenames:
              self.add_file_to_outputlist(filename)
        dialog.hide()

    def on_result_file_remove(self,widget, data=None):
          treeviewModules = self["treeview_output"]
          selection = treeviewModules.get_selection()
          model, treeiter = selection.get_selected()
          # If there's no selection, treeiter will be None
          if treeiter is None: return
          id = self["dialog_Question"].run()
          if (id==1):
             model.remove(treeiter)
          self["dialog_Question"].hide()

    def on_delete_module(self,widget, data=None):
          treeviewModules = self["treeviewModules"]
          selection = treeviewModules.get_selection()
          model, treeiter = selection.get_selected()
         # If there's no selection, treeiter will be None
          if treeiter is None: return
          id = self["dialog_Question"].run()
          if (id==1):
             model.remove(treeiter)
          self["dialog_Question"].hide()
          self.dialog_project_options_save()

    def on_xpninstances_toggled(self,cell,path,liststore):
        model = liststore
        path2 = path.split(':')[0]
        iter = model.get_iter((int(path2),))
        val = model.get_value(iter,0)
        val = not val
        model.set(iter,0,val)

    def on_spinbutton_debug_level_change_value(self,data):
        i=1
        #self.dialog_project_options_save()

    def on_instances_toggled(self,cell,path,liststore):
        model = liststore
        iter = model.get_iter((int(path),))
        val = model.get_value(iter,2)
        val = not val
        model.set(iter,2,val)

    def set_code_str(self,codearray):
        codestr = ""
        first=0
        for code in codearray:
            first = first+1
            if first==1:
                codestr=code
            else:
                codestr=codestr+','+code
        return codestr

    def get_code_array(self,code):
       return code.split(',')

    def on_treeviewInstances_click(self,treeview,treeviewcolumn,data=None):
        treeviewXPNInstances = self['treeviewXPNInstances']
        treestoreXPNInstances = self['treestoreXPNInstances']
        model = self['treeviewInstancesFiles'].get_model()
        selection = treeview.get_selection()
        tup = selection.get_selected()
        iter = tup[1] #model.get_iter((int(path),))
        if (iter!=None):
            if model.iter_parent(iter)==None:
                pathstr = model.get_value(iter,1)
                self.on_cfg_modul_dialog(None,filechoosen=pathstr)
            else:
                pathstr = model.get_value(iter,1)
                self.on_run_Tool_with_filename(pathstr)
            self.on_treeviewXPNInstances_Select(None,treestoreXPNInstances,treeviewXPNInstances)


    def set_description_text(self,cfgname):
        cfg_name = xpn_cfg_manager.replace_std_template_and_path(cfgname,self.project_filename,self.base_path,"")
        reg_str = xpn_cfg_manager.get_regstr_from_filename(cfg_name)
        textbufferDescription = self["textbufferDescription"]
        config = ConfigParser()
        config.read(cfg_name)
        description = ""
        try:
            description = config.get("doc","description")
        except:
            description = ""

        textbufferDescription.set_text(description.replace("$n$","\n"))
        if self.buttonSaveDescription_id != -1:
            self["buttonSaveDescription"].disconnect(self.buttonSaveDescription_id)

        self.buttonSaveDescription_id = self["buttonSaveDescription"].connect("clicked",self.on_buttonSaveDescription,textbufferDescription,cfg_name)

    def on_buttonSaveDescription(self,widget,textbufferDescription,cfg_name):
        text = textbufferDescription.get_text(textbufferDescription.get_start_iter(),textbufferDescription.get_end_iter()).replace("\n","$n$")
        config = ConfigParser()
        config.read(cfg_name)

        try:
               config.add_section("doc")
        except:
               pass

        config.set("doc","description",text)


        configfile = open(cfg_name, 'wb')
        config.write(configfile)
        configfile.close()



    def set_treeviewInstancesFiles(self,cfgname):
        cfg_name = xpn_cfg_manager.replace_std_template_and_path(cfgname,self.project_filename,self.base_path,"")
        reg_str = xpn_cfg_manager.get_regstr_from_filename(cfg_name)
        cfg_name_op = xpn_cfg_manager.get_filename_without_path(cfg_name)
        cfg_file_list = xpn_cfg_manager.get_all_filenames_from_cfg_file(cfg_name)
        treeviewInstancesFiles = self['treeviewInstancesFiles']
        treestoreInstancesFiles = self['treestoreInstancesFiles']
        treestoreInstancesFiles.clear()


        piter = treestoreInstancesFiles.append(None,[cfg_name_op,cfg_name,cfgname])
        for file1 in cfg_file_list:
            file1_path = xpn_cfg_manager.replace_std_template_and_path(file1,self.project_filename,self.base_path,reg_str)
            file1_op = xpn_cfg_manager.get_filename_without_path(file1_path)
            piter2 = treestoreInstancesFiles.append(piter,[file1_op,file1_path,file1])
        treeviewInstancesFiles.expand_all()


        #for group in instance_group:
            #subgroup = group.split(',')
            #if len(subgroup) < 6:
              #break;
            #lensubgroup = len(subgroup)
            #act = subgroup[0]
            #S ='('+ str(subgroup[1])+','+str(subgroup[2])+',' + str(subgroup[3]) + ')'
            #piter = treestoreXPNInstances.append(None,[act,S,group])
            #i=4
            #i2=0
            #while(1):
              #if i+1>lensubgroup:
                #break
              ##S = str(subgroup[i+1])
              #S = str(i2)
              #piter2 = treestoreXPNInstances.append(piter,[act,S,group])
              #i=i+2
              #i2=i2+1


    def on_treeviewXPNInstances_Select(self,widget,treestore,treeview):
        model = treestore
        selection = treeview.get_selection()
        if (selection == None):
            return
        tup = selection.get_selected()
        iter = tup[1] #model.get_iter((int(path),))
        if (iter!=None):
            code = model.get_value(iter,2)
            name = model.get_value(iter,1)
            if name=="Environment":
                self['frameGlobalOptions'].show()
                self['vboxInstanceundergroup'].hide()
                self['vboxmaingroup'].hide()
                self['vboxPlugins'].hide()
                self['hboxInstanceButtons'].set_sensitive(False)
                return
            if name=="Plugins and System":
                self['frameGlobalOptions'].hide()
                self['vboxInstanceundergroup'].hide()
                self['vboxmaingroup'].hide()
                self['vboxPlugins'].show()
                self['hboxInstanceButtons'].set_sensitive(False)
                return
            codearray = self.get_code_array(code)
            if (len(name.split(','))>1):
                self['entryInstanceID'].set_text(codearray[1]);
                self['entryInstancesi'].set_text(codearray[2]);
                self['entryInstancesj'].set_text(codearray[3]);
                self['checkbuttonInstancesAct'].set_active(int(codearray[0]))
                self['vboxInstanceundergroup'].hide()
                self['vboxmaingroup'].show()
                self['frameGlobalOptions'].hide()
                self['vboxPlugins'].hide()
                self['hboxInstanceButtons'].set_sensitive(True)
                self.Create_Instance_Statistic(codearray[1],codearray[2],codearray[3])
            else:   #Instance ist gewaehlt
                #self['entryInstancesConfigFilename'].set_text(codearray[4+int(name)*2])
                self['entryInstancesCover'].set_text(codearray[5+int(name)*2]);
                self.set_treeviewInstancesFiles(codearray[4+int(name)*2])
                self.set_description_text(codearray[4+int(name)*2])
                self['vboxInstanceundergroup'].show()
                self['vboxmaingroup'].hide()
                self['frameGlobalOptions'].hide()
                self['vboxPlugins'].hide()
                self['hboxInstanceButtons'].set_sensitive(True)


    def Create_Instance_Statistic(self,id1,gi,gj):
        vboxinstanceStatistic = self['vboxinstanceStatistic']
        vboxGridStatistic = self['vboxGridStatistic']
        p=xpn_cfg_manager.cxpn_project(base_path=self.base_path,project_filename=self.project_filename)
        grids = p.project_file_read_intances()
        fracs=[]
        labels=[]
        for grid in grids:
            [act,idnr,grid_i,grid_j,instances] = grid
            if (grid_i==gi) and (grid_j==gj) and (idnr==id1):
                for i in range(len(instances)):
                    [instancefilenamestr,frac] = instances[i]
                    fracs.append(float(frac)/100.0)
                    labels.append(str(i))
                break
        sum_frac=0
        for frac in fracs:
            sum_frac=sum_frac+frac

        try:
          vboxinstanceStatistic.remove(self.instanceStatisticcanvas)
          vboxinstanceStatistic.remove(self.instanceStatisticLabel)
          #self.PlotBox.remove(self.Navtoolbar)
        except:
          nothing=0

        fig1 = plt.figure(figsize=(5,4), dpi=100,facecolor='white')
        ax = fig1.add_subplot(111)
        ax.pie(fracs, labels=labels, autopct='%1.1f%%', shadow=True)
        self.instanceStatisticcanvas = FigureCanvas(fig1)  # a gtk.DrawingArea
        vboxinstanceStatistic.pack_start(self.instanceStatisticcanvas,True,True)
        if sum_frac!=1.0:
            self.instanceStatisticLabel=gtk.Label("Error: Sum of Fraction is not 100 %!")
        else:
            self.instanceStatisticLabel=gtk.Label("")

        vboxinstanceStatistic.pack_start(self.instanceStatisticLabel,False,False)
        vboxinstanceStatistic.show_all()

        [i1,j1,i2,j2] =  xpn_cfg_manager.get_grid_size(grids,int(id1))
        i = range(i1-1,i2+2)
        j = range(j1-1,j2+2)

        matr = np.zeros((i2-i1+3,j2-j1+3))


        for k2 in range(len(matr)):
            for k1 in range(len(matr[k2])):
                matr[k2,k1] = (((k1 % 2) + (k2 % 2)) % 2) * 0.0


        for grid in grids:
            [act,idnr,grid_i,grid_j,instances] = grid
            if int(idnr)!=int(id1): continue
            k2 = int(grid_i)-i1+1
            k1 = int(grid_j)-j1+1
            act1 = int(act)
            if act1==1:
                matr[k2,k1] = (((k1-1 % 2) + (k2-1 % 2)) % 2) * 0.1 + 1.0

        matr[int(gi)-i1+1,int(gj)-j1+1] = 5.0


        try:
          vboxGridStatistic.remove(self.GridStatisticccanvas)
          vboxGridStatistic.remove(self.GridStatisticLabel)

        except:
          nothing=0

        fig2 = plt.figure(figsize=(5,4), dpi=100,facecolor='white')
        ax2 = fig2.add_subplot(111)
        im = matplotlib.image.NonUniformImage(ax2, interpolation='nearest', extent=(-5,5,-5,5))
        mtr = matr.transpose()
        im.set_data(i, j, mtr)
        ax2.images.append(im)
        ax2.set_xlim(i1-1,i2+1)
        ax2.set_ylim(j1-1,j2+1)
        n1 = len(i)/8
        i_=[]
        if n1==0:
            i_=i
        else:
            pos = 0
            while(1):
                i_.append(i[pos])
                pos = pos + n1
                if (pos > len(i)-1): break;

        n1 = len(j)/10
        j_=[]
        if n1==0:
            j_=j
        else:
            pos = 0
            while(1):
                j_.append(j[pos])
                pos = pos + n1
                if (pos > len(j)-1): break;
        ax2.set_xticks(i_)
        ax2.set_yticks(j_)
        #ax2.imshow(matr, interpolation='nearest',origin=[1,1],extent=[i1,i2,j1+1,j2+1])
        #ax2.grid(True)
        self.GridStatisticccanvas = FigureCanvas(fig2)  # a gtk.DrawingArea
        vboxGridStatistic.pack_start(self.GridStatisticccanvas,True,True)
        self.GridStatisticLabel=gtk.Label("")

        vboxGridStatistic.pack_start(self.GridStatisticLabel,False,False)

        vboxGridStatistic.show_all()






    def on_Treeview_Instances_Select(self,widget,liststoreInstances,treeviewInstances,model_lat_split,model_lon_split,model_cfg_split,model_var_list_split):
        model = liststoreInstances
        selection = treeviewInstances.get_selection()
        tup = selection.get_selected()
        iter = tup[1] #model.get_iter((int(path),))
        if (iter!=None):
            i = model.get_value(iter,1)
            self["spinbuttonLat"].set_value(float(model_lat_split[i]))
            self["spinbuttonLon"].set_value(float(model_lon_split[i]))
            self["entryInstanceConfigFile"].set_text(model_cfg_split[i])
            self["entryInstancesVarList"].set_text(model_var_list_split[i])




    def on_cfg_modul_dialog(self,widget, data=None,filechoosen=""):

        # List of HelpFiles:
        try:
           self.help_file_list = glob.glob(xpn_cfg_manager.replace_std_templates("$BASE_PATH/doc/*.pdf",self.project_filename,self.base_path,""))
        except:
           self.help_file_list = []

        try:
            self.template_file_list = glob.glob(xpn_cfg_manager.replace_std_templates("$BASE_PATH/cfg_template/*.cfg",self.project_filename,self.base_path,""))
        except:
            self.template_file_list = []

        for i in range(len(self.template_file_list)):
            self.template_file_list[i] = self.template_file_list[i].replace("\\","/")

        for i in range(len(self.help_file_list)):
            self.help_file_list[i] = self.help_file_list[i].replace("\\","/")

        self.on_change_instances_combo_Last_SubModelChange=False
        liststoreModel = self["liststoreModel"]
        treeviewModel = self["treeviewModel"]
        config = ConfigParser()
        config.read(self.project_filename)
        #model_cfg = config.get("model","cfg")
        #model_cfg_split = model_cfg.split(";")
        #for i in range(len(model_cfg_split)):
         #  model_cfg_split[i]=get_fullpath_from_relative(self.base_path,model_cfg_split[i])
        #ModelsConfgFilename=model_cfg_split[0]
        model_grid = config.get("model","grid")
        instance_group = model_grid.split(';')
        model_cfg_split=[]
        for group in instance_group:
            subgroup = group.split(',')
            if len(subgroup) < 6:
              break;
            lensubgroup = len(subgroup)
            act = subgroup[0]
            S = str(subgroup[1])+','+str(subgroup[2])+',' + str(subgroup[3])
            i=4
            while(1):
              if i+1>lensubgroup:
                break
              model_cfg_split.append(subgroup[i])
              S = str(subgroup[i+1])
              i=i+2

        combobox_Instance_active=0
        liststoreInstance=self["liststoreInstance"]
        liststoreInstance.clear()

        for i in range(len(model_cfg_split)):
            S = xpn_cfg_manager.replace_std_template_and_path(model_cfg_split[i],self.project_filename,self.base_path,"")
            if S!="":
              iter = liststoreInstance.insert(-1)

              liststoreInstance.set_value(iter,0,S)
              if filechoosen==S:
                  combobox_Instance_active=i


        xpn_modules=[]





            #def File lesen:
        config = ConfigParser()
        config.read("models.def")

        modules_list=[]
        for name in config.get("modules","names").rstrip().split(","):
                model = cModulesList()
                model.name = name
                model.icon = config.get(name,"icon")
                model.submodules = config.get(name,"submodul").rstrip().split(",")
                modules_list.append(model)

        liststoreModel.clear()
        for i in range(len(modules_list)):
                iter = liststoreModel.insert(-1)
                liststoreModel.set_value(iter,0,modules_list[i].name)
                #img = cairo.ImageSurface.create_from_png("xnoplot64x64.png")
                pbuf = gdk.pixbuf_new_from_file(modules_list[i].icon)
                liststoreModel.set_value(iter,1,pbuf)
                liststoreModel.set_value(iter,2,i)

        id_treeviewModel = treeviewModel.connect('cursor-changed',self.on_Model_Selected,modules_list,xpn_modules)

        model = liststoreModel
        first = model.get_iter_first()
        selection = treeviewModel.get_selection()
        selection.set_mode(gtk.SELECTION_SINGLE)
        selection.select_iter(first)



            #event = gtk.gdk.Event(gtk.gdk.CURSOR_CHANGED)
            #event.send_event = True
            #self.quit.emit("cursor-changed", event)

        id_combo=self["combobox_Instance"].connect('changed', self.on_change_instances_combo, xpn_modules,modules_list)


        self["combobox_Instance"].set_active(combobox_Instance_active)

        result = self["dialogModel"].run()

        if (result==1):
               ModelsConfgFilename = self["combobox_Instance"].get_model().get_value(self["combobox_Instance"].get_active_iter(),0)
               self.Save_Modul_CFG_File(ModelsConfgFilename,modules_list,xpn_modules)
        self["dialogModel"].hide()
        self["combobox_Instance"].disconnect(id_combo)
        treeviewModel.disconnect(id_treeviewModel)

    def Save_Modul_CFG_File(self,ModelsConfgFilename,modules_list,xpn_modules):
         modul_cfg = ConfigParser()
         modul_cfg.read(ModelsConfgFilename)
         for i in range(len(modules_list)):
                  for i2 in range(len(modules_list[i].submodules)):
                    try:
                           modul_cfg.remove_option(modules_list[i].name,modules_list[i].submodules[i2])
                    except:
                           nothing=True

         for i in range(len(xpn_modules)):
                  if (xpn_modules[i].active==True):
                       try:
                          modul_cfg.add_section(xpn_modules[i].modul)
                       except:
                          nothing=0
                       modul_cfg.set(xpn_modules[i].modul,xpn_modules[i].submodul,xpn_modules[i].name)
         configfile = open(ModelsConfgFilename, 'wb')
         modul_cfg.write(configfile)

    def on_change_instances_combo(self,combobox,xpn_modules,modules_list):
        model = combobox.get_model()
        iter = combobox.get_active_iter()
        if iter!=None:
           ModelsConfgFilename = model.get_value(iter,0)
           try:
                if self.on_change_instances_combo_Last_SubModelChange==True:
                    id = self["dialog_Save_Changes"].run()
                    if (id==1):
                        self.Save_Modul_CFG_File(self.on_change_instances_combo_Last_ModelsConfgFilename,modules_list,xpn_modules)
                self["dialog_Save_Changes"].hide()

           except:
                nothing=True
           del xpn_modules[:]
           self.on_change_instances_combo_Last_SubModelChange=False
           self.on_change_instances_combo_Last_ModelsConfgFilename = ModelsConfgFilename
           liststoreModel = self["liststoreModel"]
           treeviewModel = self["treeviewModel"]
           modul_cfg = ConfigParser()
           modul_cfg.read(ModelsConfgFilename)
           self.expertn_bin = self.expertn_bin.replace("\\","/")
           p = subprocess.Popen([self.expertn_bin,"-d",self.base_path,"-c",self.project_filename,"-m"], shell=False,  stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
           (output,input,error) = (p.stdin, p.stdout, p.stderr)
           while (1):
                lines = input.readline()
                # irgendwie scheint das folgende unter windows nicht zu funktionieren (daher werden fehler erstmal nicht ausgelesen:
                if os.name != 'nt':
                    errline = error.readline()
                    if errline:
                        self.texterror.insert(self.texterror.get_end_iter(),errline)
                        self.textviewerror.scroll_to_iter(self.texterror.get_end_iter(),0.0)
                        self.frameError.show()
                        break

                if not lines:
                    break
                line = lines.split("\n")[0].split("\r")[0].split(",")
                xpn_modul = cModules()
                #xpn_modul.modul = line[0].replace("_"," ")
                xpn_modul.modul = line[0]
                #xpn_modul.submodul = line[1].replace("_"," ")
                xpn_modul.submodul = line[1]
                xpn_modul.libname = line[2]
                xpn_modul.name = line[3].split("\n")[0]
                try:
                    name = modul_cfg.get(line[0],line[1])
                    if name == xpn_modul.name:
                        xpn_modul.active = True
                    else:
                        xpn_modul.active = False
                except:
                    xpn_modul.active = False
                xpn_modules.append(xpn_modul)


           self.on_Model_Selected(None, modules_list,xpn_modules)

    def show_libhelp(self,widget,docfilename):
        self.on_run_Tool_with_filename(docfilename)



    def on_Model_Selected(self,data, modules_list,xpn_modules):
        ModelsConfgFilename = self["combobox_Instance"].get_model().get_value(self["combobox_Instance"].get_active_iter(),0)
        liststoreModel = self["liststoreModel"]
        treeviewModel = self["treeviewModel"]
        vboxSubmodels = self["vboxSubmodels"]
        model = liststoreModel
        selection = treeviewModel.get_selection()
        tup = selection.get_selected()
        iter = tup[1] #model.get_iter((int(path),))
        if (iter!=None):
                val = model.get_value(iter,2) #Nummer
                children = vboxSubmodels.get_children()
                children = vboxSubmodels.get_children()
                if (len(children)!=0):
                    for child in children:
                        vboxSubmodels.remove(child)
                tablex=5
                table_submodul = gtk.VBox()
                table_submodul2 = gtk.VBox()


                hbox21 = gtk.HBox()
                hbox21.pack_start(table_submodul2,True,True,10)
                hbox21.pack_start(table_submodul,True,True,10)
                hbox21.set_homogeneous(True)
                hbox21.show()
                #self.vboxSubmodels
                scrolled_window2 = gtk.ScrolledWindow()
                scrolled_window2.set_border_width(0)
                scrolled_window2.set_policy(gtk.POLICY_NEVER, gtk.POLICY_NEVER)#gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
                scrolled_window2.add_with_viewport(hbox21)
                scrolled_window2.show()
                vbox20 = gtk.HBox() #gtk.Table(len(submodellist),2,True)
                #self.table_submodul.pack_end(scrolled_window2)
                #vbox20.pack_start(scrolled_window2)
                vboxSubmodels.add(scrolled_window2)
                table_submodul.show()
                table_submodul2.show()
                i=0
                i2=0
                for submodules in modules_list[val].submodules:
                    #scrolled_window = gtk.ScrolledWindow()
                    #scrolled_window.set_border_width(0)
                    #scrolled_window.set_policy(gtk.POLICY_NEVER, gtk.POLICY_NEVER)
                    frame = gtk.Frame()
                    #scrolled_window.add_with_viewport(frame)
                    #table_submodul.attach(scrolled_window,i,i+1,i2,i2+1, xoptions=gtk.EXPAND|gtk.FILL, yoptions=gtk.EXPAND|gtk.FILL, xpadding=5, ypadding=5)
                    if i2 % 2:
                        table_submodul.pack_start(frame,False,True,10)
                    else:
                        table_submodul2.pack_start(frame,False,False,10)
                    #scrolled_window.show()
                    i2=i2+1
                    #if i > 2:
                     #   i=0
                     #   i2=i2+1

                    frame.set_label(submodules)
                    name_list = []

                    comboactive=0
                    xpn_module_list = get_modules_list(xpn_modules,modules_list[val].name,submodules)
                    for i in range(len(xpn_module_list)):
                        name_list.append(xpn_module_list[i].name)
                        if xpn_module_list[i].active==True: comboactive=i


                    combobox1 = gtk.combo_box_new_text()
                    i5=0


                    for item in name_list:
                        combobox1.append_text(item)

                    combobox1.set_active(comboactive)

                    OptionButton = gtk.Button()
                    OptionButton.set_relief(gtk.RELIEF_NONE)
                    im = gtk.image_new_from_stock(gtk.STOCK_EDIT,1)
                    OptionButton.add(im)
                    model = combobox1.get_model()
                    index = combobox1.get_active()
                    iter = combobox1.get_active_iter()
                    val2 = model.get_value(iter,0)
                    if val2=="empty":
                       OptionButton.set_sensitive(False)
                    else:
                       OptionButton.set_sensitive(True)
                    OptionButton.show_all()
                    HelpButton = gtk.Button()
                    HelpButton.set_relief(gtk.RELIEF_NONE)
                    im = gtk.image_new_from_stock(gtk.STOCK_HELP,1)
                    HelpButton.add(im)
                    HelpButton.set_sensitive(False)



                    #combobox1.connect('changed',self.on_Model_Selected_combobox1,HelpButton,OptionButton,xpn_module_list,combobox1,ModelsConfgFilename,False)
                    combobox1.connect('changed',self.on_Model_Selected_combobox12, modules_list,xpn_modules,combobox1,xpn_module_list)
                    self.on_Model_Selected_combobox1(None,HelpButton,OptionButton,xpn_module_list,combobox1,ModelsConfgFilename,True)



                    vbox1 = gtk.VBox()
                    hbox5=gtk.HBox()
                    hbox5.pack_start(combobox1,True,True,10)
                    hbox5.pack_end(HelpButton,False,False)
                    hbox5.pack_end(OptionButton,False,False)
                    vbox1.pack_start(hbox5,False,False)
                    vbox2 = gtk.VBox()
                    vbox1.pack_end(vbox2,True,True,0)
                    frame.add(vbox1)
                    frame.show_all()

    def on_Model_Selected_combobox12(self,data, modules_list,xpn_modules,combobox1,xpn_module_list):
        iter = combobox1.get_active_iter()
        model = combobox1.get_model()
        val2 = model.get_value(iter,0)
        choosen=0
        for i in range(len(xpn_module_list)):
           if (xpn_module_list[i].name==val2):
              choosen=i
              xpn_module_list[i].active=True
           else:
              xpn_module_list[i].active=False
        self.on_Model_Selected(data, modules_list,xpn_modules)


    def on_Model_Selected_combobox1(self,widget,HelpButton,OptionButton,xpn_module_list,combobox1,ModelsConfgFilename,first_time):
        if first_time==False:
            self.on_change_instances_combo_Last_SubModelChange=True
        iter = combobox1.get_active_iter()
        model = combobox1.get_model()
        val2 = model.get_value(iter,0)
        choosen=0
        for i in range(len(xpn_module_list)):
           if (xpn_module_list[i].name==val2):
              choosen=i
              xpn_module_list[i].active=True
           else:
              xpn_module_list[i].active=False
        if (choosen==0):
            OptionButton.set_sensitive(False)
        else:
            OptionButton.set_sensitive(True)

        HelpButton.set_sensitive(False)
        docfilename = ""
        if val2!="empty":
            docname = "lib"+get_filtered_libName(xpn_module_list[choosen].libname)+".pdf"
            docfilename=xpn_cfg_manager.replace_std_templates("$BASE_PATH/doc/"+docname,self.project_filename,self.base_path,"")
            help_exists=False
            for help_file in self.help_file_list:
                if help_file==docfilename:
                    help_exists=True
                    break
            if help_exists:
                HelpButton.set_sensitive(True)
                HelpButton.connect("clicked",self.show_libhelp,docfilename)
            else:
                docfilename=""

        if val2!="empty":
            cfg_template_name = "lib"+get_filtered_libName(xpn_module_list[choosen].libname)+".cfg"
            cfg_template_filename=xpn_cfg_manager.replace_std_templates("$BASE_PATH/cfg_template/"+cfg_template_name,self.project_filename,self.base_path,"")
            cfg_exists=False
            for template_file in self.template_file_list:
                if template_file==cfg_template_filename:
                    cfg_exists=True
                    break
            if cfg_exists:
                OptionButton.set_sensitive(True)
                id = gobject.signal_lookup("clicked",OptionButton)
                #if id!=0:
                #    OptionButton.disconnect(id)
                OptionButton.connect("clicked", self.model_options_edit, None,xpn_module_list,combobox1,ModelsConfgFilename,docfilename,cfg_template_filename)
            else:
                OptionButton.set_sensitive(False)




    def on_run_Tool_with_filename(self,filename):
        self.do_fileoperations(self.FileAction)
        self.FileAction=[]
        not_run=True
        for item in self.tools_list:
            [title,command,filetypes] = item
            if filetypes!="":
                for filetype2 in filetypes.split(";"):
                    for filetype in filetype2.split(","):
                        if fnmatch.fnmatch(filename,filetype):
                            if os.name == 'nt':
                                subprocess.Popen(args=[command,filename],shell=False)
                            else:
                                subprocess.Popen(args=[command+" "+'"'+filename+'"'],shell=True)

                            not_run=False
        if not_run:
            self['labelInfo'].set_label('No Application defined for this filetype!')
            self["dialogInfo"].run()
            self["dialogInfo"].hide()


    def get_REG_STR_from_filename(self,filename):
        result= ""
        flen =  len(filename)
        endstr = flen-4
        if endstr<=0:return result
        startstr = endstr
        unterstr=0
        while (1):
           startstr = startstr-1
           if startstr<=0:return result
           if (filename[startstr] >= '0' and filename[startstr] <= '9') or filename[startstr]=='_':
             if filename[startstr]=='_':
                unterstr=unterstr+1
             if unterstr>=4:
                  startstr=startstr+1
                  break
             continue
           else:
             startstr=startstr+1
             break;
        result = filename[startstr:endstr]
        return result


    #def replace_std_templates(self,source,PROJECT_NAME,BASE_PATH,REG_STR):
    #    result = source
    #    result = result.replace("$BASE_PATH",BASE_PATH)
    #    result = result.replace("$REG_STR",REG_STR)
    #    result = result.replace("$PROJECT_NAME",PROJECT_NAME)
    #    result = result.replace("$<","")
    #    result = result.replace("$>","")
    #    return result

    def on_global_options_open_file_with_extern_editor(self,widget,ModelsConfgFilename=None):
        liststoreModel = self["liststore_model_options_global"]
        treeviewModel = self["treeview_model_options_global"]
        model = liststoreModel
        selection = treeviewModel.get_selection()
        tup = selection.get_selected()
        iter = tup[1] #model.get_iter((int(path),))
        if (iter!=None):
                val = model.get_value(iter,1) #Nummer
                filenames = filter_filenames(val)
                for filename in filenames:
                    REG_STR = self.get_REG_STR_from_filename(ModelsConfgFilename)
                    PROJECT_NAME = xnoplot.get_filename_without_path(self.project_filename)[:-4]
                    BASE_PATH = self.base_path;
                    self.on_run_Tool_with_filename(xpn_cfg_manager.replace_std_templates(filename,self.project_filename,BASE_PATH,REG_STR))

    def on_local_options_open_file_with_extern_editor(self,widget,ModelsConfgFilename=None):
        liststoreModel = self["liststore_model_options"]
        treeviewModel = self["treeview_model_options"]
        model = liststoreModel
        selection = treeviewModel.get_selection()
        tup = selection.get_selected()
        iter = tup[1] #model.get_iter((int(path),))
        if (iter!=None):
                val = model.get_value(iter,1) #Nummer
                filenames = filter_filenames(val)
                for filename in filenames:
                    REG_STR = self.get_REG_STR_from_filename(ModelsConfgFilename)
                    PROJECT_NAME = xnoplot.get_filename_without_path(self.project_filename)[:-4]
                    BASE_PATH = self.base_path;
                    self.on_run_Tool_with_filename(xpn_cfg_manager.replace_std_templates(filename,self.project_filename,BASE_PATH,REG_STR))


    def treeview_model_options_global_row_activated(self,widget,data=None):
        liststoreModel = self["liststore_model_options_global"]
        treeviewModel = self["treeview_model_options_global"]
        model = liststoreModel
        selection = treeviewModel.get_selection()
        tup = selection.get_selected()
        iter = tup[1] #model.get_iter((int(path),))
        act = False
        if (iter!=None):
                val = model.get_value(iter,1) #Nummer
                filenames = filter_filenames(val)
                if len(filenames) != 0:
                    act = True
        if act:
            self['buttonGlobalOptionsEditFilename'].set_sensitive(True)
        else:
            self['buttonGlobalOptionsEditFilename'].set_sensitive(False)

    def treeview_model_options_local_row_activated(self,widget,data=None):
        liststoreModel = self["liststore_model_options"]
        treeviewModel = self["treeview_model_options"]
        model = liststoreModel
        selection = treeviewModel.get_selection()
        tup = selection.get_selected()
        iter = tup[1] #model.get_iter((int(path),))
        act = False
        if (iter!=None):
                val = model.get_value(iter,1) #Nummer
                filenames = filter_filenames(val)
                if len(filenames) != 0:
                    act = True
        if act:
            self['buttonLocalOptionsEditFilename'].set_sensitive(True)
        else:
            self['buttonLocalOptionsEditFilename'].set_sensitive(False)

    def on_button_dialog_options_standard_values_cellrenderertoggle_toggled(self,cell,path, data=None):
        liststore = self["liststore_options_standard_values"]
        model = liststore
        iter = model.get_iter((int(path),))
        val = model.get_value(iter,0)
        val = not val
        model.set(iter,0,val)

    def on_dialog_fileoperation_toggled(self,cell,path, data=None):
        liststore = self["liststore_fileoperations"]
        model = liststore
        iter = model.get_iter((int(path),))
        val = model.get_value(iter,0)
        val = not val
        model.set(iter,0,val)

    def on_button_dialog_options_standard_values(self,widget,cfg_template_filename,globalname,localname,ModelsConfgFilename):
        list_value_Value = []
        liststore_model_options = self["liststore_model_options"]
        treeview_model_options = self["treeview_model_options"]
        treemodel_model_options = treeview_model_options.get_model()
        first = liststore_model_options.get_iter_first()
        if first!=None:
            val = treemodel_model_options.get_value(first,0)
            val2 = treemodel_model_options.get_value(first,1)
            list_value_Value.append([val,val2])
            iter = first
            while (1):
                iter = treemodel_model_options.iter_next(iter)
                if iter==None:
                    break
                val = treemodel_model_options.get_value(iter,0)
                val2 =  treemodel_model_options.get_value(iter,1)
                list_value_Value.append([val,val2])
        list_value_global_Option = []
        liststore_model_options_global = self["liststore_model_options_global"]
        treeview_model_options_global = self["treeview_model_options_global"]
        treemodel_model_options = treeview_model_options_global.get_model()
        first = liststore_model_options_global.get_iter_first()
        if first!=None:
            val = treemodel_model_options.get_value(first,0)
            val2 = treemodel_model_options.get_value(first,1)
            list_value_global_Option.append([val,val2])
            iter = first
            while (1):
                iter = treemodel_model_options.iter_next(iter)
                if iter==None:
                    break
                val = treemodel_model_options.get_value(iter,0)
                val2 =  treemodel_model_options.get_value(iter,1)
                list_value_global_Option.append([val,val2])
        values_list = get_cfg_values(cfg_template_filename)
        liststore = self["liststore_options_standard_values"]
        treeview = self["treeview_options_standard_values"]
        liststore.clear()
        i = 0
        for values in values_list:
            [section,option,value] = values
            if section==globalname or section==localname:
                iter = liststore.insert(-1)
                set_true=True
                for val in list_value_global_Option:
                   [val1,val2] = val
                   if section==globalname and val1==option:
                       set_true=False
                       break
                for val in list_value_Value:
                   [val1,val2] = val
                   if section==localname and val1==option:
                       set_true=False
                       break
                i2 = 0
                for val in values_list:
                    [section2,option2,value2] = val
                    if section2==section and option2==option and i>i2:
                        set_true=False
                        break
                    i2+=1

                if (value.find("$-") != -1):
                    set_true=False
                    value = value.replace("$-","")
                liststore.set_value(iter,0,set_true)
                liststore.set_value(iter,1,section)
                liststore.set_value(iter,2,option)
                liststore.set_value(iter,3,value)
            i+=1

        id = self["dialog_options_standard_values"].run()
        self["dialog_options_standard_values"].hide()

        if id==1:
            newoptions = []
            model_options = treeview.get_model()
            first = model_options.get_iter_first()
            if first!=None:
                val = [model_options.get_value(first,0),model_options.get_value(first,1),model_options.get_value(first,2),model_options.get_value(first,3)]
                if val[0]==True:
                    newoptions.append([val[1],val[2],val[3]])
                while (1):
                    first = model_options.iter_next(first)
                    if first==None:
                        break
                    val = [model_options.get_value(first,0),model_options.get_value(first,1),model_options.get_value(first,2),model_options.get_value(first,3)]
                    if val[0]==True:
                        newoptions.append([val[1],val[2],val[3]])

            for value in newoptions:
                [section,option,value2] = value
                if section==globalname:
                    add_var=True
                    i=0
                    for list_value in list_value_global_Option:
                        [option2,val] = list_value
                        if option2==option:
                            list_value = [option2,value2]
                            list_value_global_Option[i] = list_value
                            add_var = False
                            break
                        i=i+1
                    if add_var:
                        list_value = [option,value2]
                        list_value_global_Option.append(list_value)
                        file_list = filter_filenames(value2)
                        for file1 in file_list:
                            REG_STR_OLD = ""
                            PROJECT_NAME_OLD = "project"
                            BASE_PATH_OLD=self.base_path+"/cfg_template"
                            PROJECT_FILENAME_OLD = BASE_PATH_OLD+"/"+PROJECT_NAME_OLD+".xpn"
                            REG_STR_NEW = self.get_REG_STR_from_filename(ModelsConfgFilename)
                            PROJECT_NAME_NEW = xnoplot.get_filename_without_path(self.project_filename)[:-4]
                            BASE_PATH_NEW = self.base_path;
                            src = get_fullpath_from_relative(self.base_path,xpn_cfg_manager.replace_std_templates(file1,PROJECT_FILENAME_OLD,BASE_PATH_OLD,REG_STR_OLD))
                            dst = get_fullpath_from_relative(self.base_path,xpn_cfg_manager.replace_std_templates(file1,self.project_filename,BASE_PATH_NEW,REG_STR_NEW))
                            self.file_list_memory.append([section,option,value2,True])
                            if src!=dst:
                                  self.FileAction.append(['copy',src,dst])
                if section==localname:
                    add_var=True
                    i=0
                    for list_value in list_value_Value:
                        [option2,val] = list_value
                        if option2==option:
                            list_value = [option2,value2]
                            list_value_Value[i] = list_value
                            add_var = False
                            break
                        i=i+1
                    if add_var:
                        list_value = [option,value2]
                        list_value_Value.append(list_value)
                        file_list = filter_filenames(value2)
                        for file1 in file_list:
                            REG_STR_OLD = ""
                            PROJECT_NAME_OLD = "project"
                            BASE_PATH_OLD=self.base_path+"/cfg_template"
                            PROJECT_FILENAME_OLD = BASE_PATH_OLD+"/"+PROJECT_NAME_OLD+".xpn"
                            REG_STR_NEW = self.get_REG_STR_from_filename(ModelsConfgFilename)
                            PROJECT_NAME_NEW = xnoplot.get_filename_without_path(self.project_filename)[:-4]
                            BASE_PATH_NEW = self.base_path;
                            src = get_fullpath_from_relative(self.base_path,xpn_cfg_manager.replace_std_templates(file1,PROJECT_FILENAME_OLD,BASE_PATH_OLD,REG_STR_OLD))
                            dst = get_fullpath_from_relative(self.base_path,xpn_cfg_manager.replace_std_templates(file1,self.project_filename,BASE_PATH_NEW,REG_STR_NEW))
                            self.file_list_memory.append([section,option,value2,True])
                            if src!=dst:
                                  self.FileAction.append(['copy',src,dst])


            liststore_model_options.clear()
            for list_value in list_value_Value:
                [option2,val] = list_value
                iter1 = liststore_model_options.insert(-1)
                liststore_model_options.set_value(iter1,0,option2)
                liststore_model_options.set_value(iter1,1,val)

            liststore_model_options_global.clear()
            for list_value in list_value_global_Option:
                [option2,val] = list_value
                iter1 = liststore_model_options_global.insert(-1)
                liststore_model_options_global.set_value(iter1,0,option2)
                liststore_model_options_global.set_value(iter1,1,val)




    def model_options_edit(self, widget,data=None,xpn_module=None,combobox=None,ModelsConfgFilename=None,docfilename="",cfg_template_filename=""):
        model = combobox.get_model()
        comboactive = combobox.get_active()
        To_Delete_1 = []
        To_Delete_2 = []
        To_Delete_3 = []
        if comboactive==0: return
        liststore_model_options = self["liststore_model_options"]
        liststore_model_options.clear()
        modul_cfg = ConfigParser()
        modul_cfg.read(ModelsConfgFilename)
        try:
          for items in modul_cfg.items(xpn_module[comboactive].name):
             iter = liststore_model_options.insert(-1)
             liststore_model_options.set_value(iter,0,items[0])
             liststore_model_options.set_value(iter,1,items[1])
             To_Delete_1.append(xpn_module[comboactive].name)
             To_Delete_2.append(items[0])
             To_Delete_3.append(items[1])

        except:
             nothing=0

        liststore_model_options_global = self["liststore_model_options_global"]
        liststore_model_options_global.clear()
        libname = get_filtered_libName(xpn_module[comboactive].libname)
        try:
          for items in modul_cfg.items(libname):
             iter = liststore_model_options_global.insert(-1)
             liststore_model_options_global.set_value(iter,0,items[0])
             liststore_model_options_global.set_value(iter,1,items[1])
             To_Delete_1.append(libname)
             To_Delete_2.append(items[0])
             To_Delete_3.append(items[1])

        except:
             nothing = 0

        #Parameter Merken, die Dateien enthalten:
        # Reihenfolge: [libname,paramter,value,dateimanipulation getan]
        self.file_list_memory = []
        for i in range(len(To_Delete_3)):
            file_list = filter_filenames(To_Delete_3[i])
            if len(file_list)>0:
                    self.file_list_memory.append([To_Delete_1[i],To_Delete_2[i],To_Delete_3[i],False])



        id1 = self["buttonGlobalOptionsEditFilename"].connect("clicked",self.on_global_options_open_file_with_extern_editor,ModelsConfgFilename)
        id2 = self["buttonLocalOptionsEditFilename"].connect("clicked",self.on_local_options_open_file_with_extern_editor,ModelsConfgFilename)

        id3 = -1
        if len(docfilename)>3:
            id3 = self["model_option_help_button"].connect("clicked",self.show_libhelp,docfilename)
            self["model_option_help_button"].set_sensitive(True)
        else:
            self["model_option_help_button"].set_sensitive(False)

        self["label_dialog_modeloption_global"].set_label(" (" + libname + ")")
        self["label_dialog_modeloption_local"].set_label(" (" + xpn_module[comboactive].name + ")")


        id4 = self["button_dialog_options_standard_values"].connect("clicked",self.on_button_dialog_options_standard_values,cfg_template_filename,libname,xpn_module[comboactive].name,ModelsConfgFilename)

        self['buttonGlobalOptionsEditFilename'].set_sensitive(False)
        self['buttonLocalOptionsEditFilename'].set_sensitive(False)

        self.FileAction = []


        id = self["dialog_modeloption"].run()
        self["dialog_modeloption"].hide()
        self["buttonGlobalOptionsEditFilename"].disconnect(id1)
        self["buttonLocalOptionsEditFilename"].disconnect(id2)
        if id3 != -1:
            self["model_option_help_button"].disconnect(id3)

        self["button_dialog_options_standard_values"].disconnect(id4)

        if id==1:
               configfile = open(ModelsConfgFilename, 'wb')

               for i in range(len(To_Delete_1)):
                   modul_cfg.remove_option(To_Delete_1[i],To_Delete_2[i])

               list_value_Option = []
               list_value_Value = []
               liststore_model_options = self["liststore_model_options"]
               treeview_model_options = self["treeview_model_options"]
               treemodel_model_options = treeview_model_options.get_model()
               first = liststore_model_options.get_iter_first()
               if first!=None:
                  val = treemodel_model_options.get_value(first,0)
                  val2 = treemodel_model_options.get_value(first,1)
                  list_value_Option.append(val)
                  list_value_Value.append(val2)
                  iter = first
                  while (1):
                     iter = treemodel_model_options.iter_next(iter)
                     if iter==None:
                        break
                     val = treemodel_model_options.get_value(iter,0)
                     val2 =  treemodel_model_options.get_value(iter,1)
                     list_value_Option.append(val)
                     list_value_Value.append(val2)
               try:
                  modul_cfg.add_section(xpn_module[comboactive].name)
               except:
                  nothing=0
               for i in range(len(list_value_Option)):
                  modul_cfg.set(xpn_module[comboactive].name, list_value_Option[i],list_value_Value[i])
                  for i2 in range(len(self.file_list_memory)):
                      [lib,v1,v2,man] = self.file_list_memory[i2]
                      if lib==xpn_module[comboactive].name and v1==list_value_Option[i]:
                          file_list2 = filter_filenames(list_value_Value[i])
                          file_list1 = filter_filenames(v2)
                          filelen = min(len(file_list1),len(file_list2))
                          self.file_list_memory[i2][3] = True
                          for i3 in range(filelen):
                              REG_STR = self.get_REG_STR_from_filename(ModelsConfgFilename)
                              PROJECT_NAME = xnoplot.get_filename_without_path(self.project_filename)[:-4]
                              BASE_PATH = self.base_path;
                              src = get_fullpath_from_relative(self.base_path,xpn_cfg_manager.replace_std_templates(file_list1[i3],self.project_filename,BASE_PATH,REG_STR))
                              dst = get_fullpath_from_relative(self.base_path,xpn_cfg_manager.replace_std_templates(file_list2[i3],self.project_filename,BASE_PATH,REG_STR))
                              if src!=dst:
                                  self.FileAction.append(['rename',src,dst])

               list_value_Option = []
               list_value_Value = []
               liststore_model_options = self["liststore_model_options_global"]
               treeview_model_options = self["treeview_model_options_global"]
               treemodel_model_options = treeview_model_options.get_model()
               first = liststore_model_options.get_iter_first()
               if first!=None:
                  val = treemodel_model_options.get_value(first,0)
                  val2 = treemodel_model_options.get_value(first,1)
                  list_value_Option.append(val)
                  list_value_Value.append(val2)
                  iter = first
                  while (1):
                     iter = treemodel_model_options.iter_next(iter)
                     if iter==None:
                        break
                     val = treemodel_model_options.get_value(iter,0)
                     val2 =  treemodel_model_options.get_value(iter,1)
                     list_value_Option.append(val)
                     list_value_Value.append(val2)
               try:
                  modul_cfg.add_section(libname)
               except:
                  nothing=0
               for i in range(len(list_value_Option)):
                  modul_cfg.set(libname, list_value_Option[i],list_value_Value[i])
                  for i2 in range(len(self.file_list_memory)):
                      [lib,v1,v2,man] = self.file_list_memory[i2]
                      if lib==libname and v1==list_value_Option[i]:
                          file_list2 = filter_filenames(list_value_Value[i])
                          file_list1 = filter_filenames(v2)
                          filelen = min(len(file_list1),len(file_list2))
                          self.file_list_memory[i2][3] = True
                          for i3 in range(filelen):
                              REG_STR = self.get_REG_STR_from_filename(ModelsConfgFilename)
                              PROJECT_NAME = xnoplot.get_filename_without_path(self.project_filename)[:-4]
                              BASE_PATH = self.base_path;
                              src = get_fullpath_from_relative(self.base_path,xpn_cfg_manager.replace_std_templates(file_list1[i3],self.project_filename,BASE_PATH,REG_STR))
                              dst = get_fullpath_from_relative(self.base_path,xpn_cfg_manager.replace_std_templates(file_list2[i3],self.project_filename,BASE_PATH,REG_STR))
                              if src!=dst:
                                  self.FileAction.append(['rename',src,dst])

               for i in range(len(self.file_list_memory)):
                   [lib,v1,v2,man] = self.file_list_memory[i]
                   if man==False:
                       REG_STR = self.get_REG_STR_from_filename(ModelsConfgFilename)
                       PROJECT_NAME = xnoplot.get_filename_without_path(self.project_filename)[:-4]
                       BASE_PATH = self.base_path;
                       file_list1 = filter_filenames(v2)
                       for i3 in range(len(file_list1)):
                           src = get_fullpath_from_relative(self.base_path,xpn_cfg_manager.replace_std_templates(file_list1[i3],self.project_filename,BASE_PATH,REG_STR))
                           self.FileAction.append(['rm',src,""])

               modul_cfg.write(configfile)
               self.do_fileoperations(self.FileAction)
               self.FileAction=[]


    def model_options_edit_cell_Option(self,cellrenderertext, path,new_text):
        liststore_model_options = self["liststore_model_options"]
        iter = liststore_model_options.get_iter(path)
        liststore_model_options.set(iter, 0, new_text);

    def model_options_edit_cell_Value(self,cellrenderertext, path,new_text):
        liststore_model_options = self["liststore_model_options"]
        iter = liststore_model_options.get_iter(path)
        liststore_model_options.set(iter, 1, new_text);

    def model_options_global_edit_cell_Option(self,cellrenderertext, path,new_text):
        liststore_model_options = self["liststore_model_options_global"]
        iter = liststore_model_options.get_iter(path)
        liststore_model_options.set(iter, 0, new_text);

    def model_options_global_edit_cell_Value(self,cellrenderertext, path,new_text):
        liststore_model_options = self["liststore_model_options_global"]
        iter = liststore_model_options.get_iter(path)
        liststore_model_options.set(iter, 1, new_text);

    def Add_Option(self,widget, data=None):
       id = self["dialog_Add_Options"].run()
       if (id==1):
         liststore_model_options_global = self["liststore_model_options"]
         iter = liststore_model_options_global.insert(-1)
         liststore_model_options_global.set_value(iter,0,self["entry_Add_Option_Option"].get_text())
         liststore_model_options_global.set_value(iter,1,self["entry_Add_Option_Value"].get_text())
       self["dialog_Add_Options"].hide()

    def Delete_Option(self,widget, data=None):
       treeview_model_options_global = self["treeview_model_options"]
       selection = treeview_model_options_global.get_selection()
       model, treeiter = selection.get_selected()
       # If there's no selection, treeiter will be None
       if treeiter is None: return
       id = self["dialog_Question"].run()
       if (id==1):
          model.remove(treeiter)
       self["dialog_Question"].hide()

    def Add_Global_Option(self,widget, data=None):
       id = self["dialog_Add_Options"].run()
       if (id==1):
         liststore_model_options_global = self["liststore_model_options_global"]
         iter = liststore_model_options_global.insert(-1)
         liststore_model_options_global.set_value(iter,0,self["entry_Add_Option_Option"].get_text())
         liststore_model_options_global.set_value(iter,1,self["entry_Add_Option_Value"].get_text())
       self["dialog_Add_Options"].hide()

    def Delete_Global_Option(self,widget, data=None):
       treeview_model_options_global = self["treeview_model_options_global"]
       selection = treeview_model_options_global.get_selection()
       model, treeiter = selection.get_selected()
       # If there's no selection, treeiter will be None
       if treeiter is None: return
       id = self["dialog_Question"].run()
       if (id==1):
          model.remove(treeiter)
       self["dialog_Question"].hide()


    def on_submodel_change(self,combobox,OptionButton,xpn_module_list):
        self.on_change_instances_combo_Last_SubModelChange=True
        model = combobox.get_model()
        index = combobox.get_active()
        iter = combobox.get_active_iter()
        val = model.get_value(iter,0)
        choosen=-1
        for i in range(len(xpn_module_list)):
           if (xpn_module_list[i].name==val):
              choosen=i
              xpn_module_list[i].active=True
           else:
              xpn_module_list[i].active=False
        if (choosen==0):
          OptionButton.set_sensitive(False)
        else:
              OptionButton.set_sensitive(True)
        return
        item = cModules()
        for item2 in name_list:
            if item2.name==val:
                item=item2
        if item.description=='':
            text1.set_markup("\n<i>No Description</i>")
        else:
            textlist = item.description.split("\\n")
            text = ""
            for txt in textlist:
                text = text + '\n' + txt
            text1.set_markup('<i>'+text+'</i>')
        text1.set_line_wrap(True)
        text1.set_single_line_mode(False)
        text1.show()
        children = vbox2.get_children()
        if (len(children)!=0):
                    for child in children:
                        vbox2.remove(child)
        i=0

        hbox_1=gtk.HBox()
        vbox_1=gtk.VBox()
        vbox_2=gtk.VBox()
        vbox_3=gtk.VBox()
        hbox_1.pack_start(vbox_1)
        hbox_1.pack_start(vbox_2)
        hbox_1.pack_start(vbox_3)

        for param in item.params:
            text2=gtk.Label()
            S = '<b>'+param+':</b>'
            text2.set_markup(S)
            textentry1 = gtk.Entry()
            textentry1.set_text(item.params_act_val[i])
            text3=gtk.Label()
            text3.set_label(item.params_descr[i])
            hbox1=gtk.HBox()
            hbox1.pack_start(text3,False,True,10)
            hbox2=gtk.HBox()
            hbox2.pack_start(text2,False,True,10)

            vbox_1.pack_start(hbox2)
            vbox_2.pack_start(textentry1);
            vbox_3.pack_start(hbox1,False,False,5)
            i=i+1
        if len(item.params)!=0:
            text5=gtk.Label()
            text5.set_markup('<span foreground="blue"><b>Options:</b> </span>')
            text4=gtk.Label()
            text4.set_markup("")
            text3=gtk.Label()
            text3.set_markup("")
            vbox2.pack_start(text4,True,True,5)
            vbox2.pack_start(text5,True,True,5)
            vbox2.pack_end(text3,True,True,5)
        vbox2.pack_end(hbox_1)
        vbox2.show_all()

    def tool_options_edit_cell_Title(self,cellrenderertext, path,new_text):
        liststoreToolsOption = self["liststoreToolsOption"]
        iter = liststoreToolsOption.get_iter(path)
        liststoreToolsOption.set(iter, 0, new_text);

    def tool_options_edit_cell_Command(self,cellrenderertext, path,new_text):
        liststoreToolsOption = self["liststoreToolsOption"]
        iter = liststoreToolsOption.get_iter(path)
        liststoreToolsOption.set(iter, 1, new_text);

    def tool_options_edit_cell_FileType(self,cellrenderertext, path,new_text):
        liststoreToolsOption = self["liststoreToolsOption"]
        iter = liststoreToolsOption.get_iter(path)
        liststoreToolsOption.set(iter, 2, new_text);

    def on_tools_add_tools_to_list(self,widget,data=None):
        liststoreToolsOption = self['liststoreToolsOption']
        id = self["dialogAddTool"].run()
        if (id==1):
            iter = liststoreToolsOption.insert(-1)
            liststoreToolsOption.set_value(iter,0,self["entryToolTitle"].get_text())
            liststoreToolsOption.set_value(iter,1,self["entryToolCommand"].get_text())
            liststoreToolsOption.set_value(iter,2,self["entryToolFileType"].get_text())
        self["dialogAddTool"].hide()

    def on_tools_delete_tools_to_list(self,widget, data=None):
       treeview_ToolsOption = self["treeview_ToolsOption"]
       selection = treeview_ToolsOption.get_selection()
       model, treeiter = selection.get_selected()
       # If there's no selection, treeiter will be None
       if treeiter is None: return
       id = self["dialog_Question"].run()
       if (id==1):
          model.remove(treeiter)
       self["dialog_Question"].hide()

    def on_cfg_dialog_SystemSettings(self,widget, data=None):
       liststoreToolsOption= self["liststoreToolsOption"]
       treeview_ToolsOption= self["treeview_ToolsOption"]
       liststoreToolsOption.clear()
       try:
          i = 0
          for items in self.tools_list:
             [title,command,filetype] = items
             iter = liststoreToolsOption.insert(-1)
             liststoreToolsOption.set_value(iter,0,title)
             liststoreToolsOption.set_value(iter,1,command)
             liststoreToolsOption.set_value(iter,2,filetype)
             i=i+1
       except:
             nothing=0

       entry_Working_Directory = self['entry_Working_Directory']
       entry_Expert_N_Exec = self['entry_Expert_N_Exec']
       entry_Expert_N_Delay_Workaround = self['entry_Expert_N_Delay_Workaround']
       try:
          entry_Working_Directory.set_text(self.base_path)
       except:
          a=0
       try:
         entry_Expert_N_Exec.set_text(self.expertn_bin)
       except:
          a=0

       entry_Expert_N_Delay_Workaround.set_text(str(self.delay_workaround))

       xnoplotconfig = ConfigParser()
       try:
            xnoplotconfig.read("xnoplot.ini")
            self['dialogoptionspath'].set_text(xnoplotconfig.get("spreadsheet","path"))
            self['dialogoptionscommand'].set_text(xnoplotconfig.get("spreadsheet","command"))
       except:
            self['dialogoptionspath'].set_text("")
            self['dialogoptionscommand'].set_text("gnumeric")
       id = self["dialog_SystemSettings"].run()
       if (id==1):
           configfile = open("expertn_gui.ini", 'wb')
           self.base_path = entry_Working_Directory.get_text().replace("\\","/")
           self.expertn_bin = entry_Expert_N_Exec.get_text().replace("\\","/")
           self.delay_workaround = double(entry_Expert_N_Delay_Workaround.get_text())
           modul_cfg = ConfigParser()
           try:
              modul_cfg.add_section("PATHS")
           except:
              dummy=0
           modul_cfg.set("PATHS","expertn_bin",self.expertn_bin)
           modul_cfg.set("PATHS","base_path",self.base_path)

           try:
              modul_cfg.add_section("WINDOWS")
           except:
              dummy=0

           modul_cfg.set("WINDOWS","delay_workaround",str(self.delay_workaround))

           cdbconfigfile=open("xnoplot.ini",'w')
           try:
                xnoplotconfig.add_section("spreadsheet")
           except:
                dummy=0
           xnoplotconfig.set("spreadsheet","path",self['dialogoptionspath'].get_text())
           xnoplotconfig.set("spreadsheet","command",self['dialogoptionscommand'].get_text())
           xnoplotconfig.write(cdbconfigfile)
           cdbconfigfile.close()

           self.tools_list = []
       #     liststoreToolsOption= self["liststoreToolsOption"]
      # treeview_ToolsOption= self["treeview_ToolsOption"]
           treemodel = treeview_ToolsOption.get_model()
           first = liststoreToolsOption.get_iter_first()
           if first!=None:
                  title = treemodel.get_value(first,0)
                  command = treemodel.get_value(first,1).replace("\\","/")
                  filetype= treemodel.get_value(first,2).replace(";",",")
                  self.tools_list.append([title,command,filetype])
                  iter = first
                  while (1):
                     iter = treemodel.iter_next(iter)
                     if iter==None:
                        break
                     title = treemodel.get_value(iter,0)
                     command = treemodel.get_value(iter,1).replace("\\","/")
                     filetype= treemodel.get_value(iter,2).replace(";",",")
                     self.tools_list.append([title,command,filetype])

           try:
                modul_cfg.add_section("TOOLS")
           except:
                nothing=0

           toolstr = ""
           for item in self.tools_list:
                [title,command,filetype] = item
                toolstr = toolstr + title + "," + command + ',' + filetype + ';'
           modul_cfg.set("TOOLS","tool_list",toolstr)
           modul_cfg.write(configfile)


       self["dialog_SystemSettings"].hide()

    def on_run_Tool(self,widget,data):
        [title,command,filetype] = data
        subprocess.Popen(args=[command],shell=True)

    def on_ToolsMenueActivated(self,widget,data=None):
        menu = gtk.Menu()
        for item in self.tools_list:
            [title,command,filetype] = item
            menu_item = gtk.MenuItem(title)
            menu.append(menu_item)
            menu_item.connect("activate", self.on_run_Tool,item)
            menu_item.show()


        #menu.popup( None, None, None, 1,0)
        menu.allocation.x = 5
        menu.allocation.y = 5
        menu.reposition
        menu.show()
        menu.attach_to_widget(widget,None)
        menu.popup( None, None, None, 1,0)




gobject.threads_init()
if (os.name=='posix'):
    gtk.gdk.threads_init()
lock = thread.allocate_lock()


if __name__ == "__main__":
    #thread.start_new_thread(testthread, ())
    Mwindow = MainWindow()
    Mwindow.window.show()
    gtk.gdk.threads_enter
    gtk.main()
    gtk.gdk.threads_leave()

