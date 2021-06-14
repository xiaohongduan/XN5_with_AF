#!/usr/bin/env python
# -*- coding: utf-8 -*-

import glob
import sys
import shutil
import os
import subprocess
import shutil

dll_ext = ".so"
exe_ext = ""

if subprocess.mswindows:
    dll_ext = ".dll"
    exe_ext = ".exe"


class ccopy_files:
    def __init__(self,dst_path,act_path='.'):
        self.act_path = act_path
        self.src_path = act_path+"/.."
        self.dst_path = dst_path



        self.src_built=self.src_path+"/built"
        self.dst_built=dst_path+"/built"

        self.src_built_standalone = self.src_path+"/built_ohne_compiler"
        self.dst_built_standalone = dst_path+"/built"

        self.src_expertn_tools=self.src_path+"/gui/dist/expertn_gui"
        self.dst_expertn_tools=dst_path+"/gui"

        self.src_dev=self.src_path+"/dev"
        self.dst_dev=dst_path+"/dev"


        self.create_bin()
        self.create_cfg()
        self.create_cfg_system()
        self.cfg_template()
        self.cfg_treemix()
        self.doc()
        self.models()
        self.expertn_tools()
        self.built_standalone()
        self.dev()

    def create_bin(self):
        file_list = [
                        "expertn_bin"+exe_ext,
                        "libexpertn_class"+dll_ext,
                        "libexpertn_modul_base"+dll_ext
                    ]
        if subprocess.mswindows:
                        print "windows \n"
                        file_list.append("expertn_bin_term"+exe_ext)
        else:
                        print "linux \n"
                        file_list.append("expertn_environment.sh")

        self.mkdir(self.dst_built+"/bin")
        for f in file_list:
            self.copy(self.src_built+"/bin/"+f,self.dst_built+"/bin/"+f)

    def create_cfg(self):
        self.mkdir(self.dst_built+"/cfg")

    def create_cfg_system(self):
        self.mkdir(self.dst_built+"/cfg_system")
        self.copy_all(self.src_built+"/cfg_system",self.dst_built+"/cfg_system","*")

    def cfg_template(self):
        self.mkdir(self.dst_built+"/cfg_template")
        self.copy_all(self.src_built+"/cfg_template",self.dst_built+"/cfg_template","*")

    def cfg_treemix(self):
        self.mkdir(self.dst_built+"/cfg_treemix")
        self.copy_all(self.src_built+"/cfg_treemix",self.dst_built+"/cfg_treemix","*")

    def doc(self):
        self.mkdir(self.dst_built+"/doc")
        self.copy_all(self.src_built+"/doc",self.dst_built+"/doc","*")

    def models(self):
        self.mkdir(self.dst_built+"/models")
        self.copy_all(self.src_built+"/models",self.dst_built+"/models","*")

    def expertn_tools(self):
        self.mkdir(self.dst_expertn_tools)
        self.copy_all_subfolders(self.src_expertn_tools,self.dst_expertn_tools,"*")

    def built_standalone(self):
        #self.mkdir(self.dst_expertn_tools)
        self.copy_all_subfolders(self.src_built_standalone,self.dst_built_standalone,"*")

        #self.mkdir(self.dst_expertn_tools+"/template")
        #self.copy_all(self.src_expertn_tools+"/template",self.dst_expertn_tools+"/template","*")

        #self.mkdir(self.dst_expertn_tools+"/noah_soils")
        #self.copy_all(self.src_expertn_tools+"/noah_soils",self.dst_expertn_tools+"/noah_soils","*")

        #self.mkdir(self.dst_expertn_tools+"/noah_soils/doc")
        #self.copy_all(self.src_expertn_tools+"/noah_soils/doc",self.dst_expertn_tools+"/noah_soils/doc","*")


    def dev(self):

        # root path:
        dev_root_files = [
                            "waf",
                            "waf.bat",
                            "readme.txt",
                            "wscript",
                            "expertn_doc.html",
                            "module_example.mod"
                         ]
        self.mkdir(self.dst_dev)
        self.copy_file_list(self.src_dev,self.dst_dev,dev_root_files)

        #xpn
        self.mkdir(self.dst_dev+"/xpn")

        #xpn/libexpertn_class
        self.mkdir(self.dst_dev+"/xpn/libexpertn_class/include")
        self.copy_all(self.src_dev+"/xpn/libexpertn_class/include",self.dst_dev+"/xpn/libexpertn_class/include","*.h")

        #xpn/libexpertn_modul_base
        self.mkdir(self.dst_dev+"/xpn/libexpertn_modul_base/include")
        self.copy_all(self.src_dev+"/xpn/libexpertn_modul_base/include",self.dst_dev+"/xpn/libexpertn_modul_base/include","*.h")
        self.mkdir(self.dst_dev+"/xpn/libexpertn_modul_base/doc")
        self.copy_all(self.src_dev+"/xpn/libexpertn_modul_base/doc",self.dst_dev+"/xpn/libexpertn_modul_base/doc","*")

        #xpn/modul/libmodule_example
        self.mkdir(self.dst_dev+"/xpn/modul/libmodule_example/src")
        self.copy_all(self.src_dev+"/xpn/modul/libmodule_example/src",self.dst_dev+"/xpn/modul/libmodule_example/src","*")

        #xpn/modul/llibmpmas_coupling
        #self.mkdir(self.dst_dev+"/xpn/modul/libmpmas_coupling/src")
        #self.copy_all(self.src_dev+"/xpn/modul/libmpmas_coupling/src",self.dst_dev+"/xpn/modul/libmpmas_coupling/src","*")







    def copy(self,src,dst):
        try:
            shutil.copy(src,dst)
            print "copy: ", src , " ", dst
        except IOError, exc:
            print "Error: copy file: ",src,dst

    def mkdir(self,path):
        try:
            os.makedirs(path)
            print "mkdir: ", path
        except OSError, msg:
            print "Error: ",msg," mkdir: ",path

    def dir(self,path,pattern):
        old_path = os.getcwd()
        os.chdir(path)
        files = sorted(glob.glob(pattern),key=str.lower)
        os.chdir(old_path)
        return files

    def dir_subfolders(self,path,pattern):
        old_path = os.getcwd()
        os.chdir(path)
        files = sorted(glob.glob(pattern),key=str.lower)
        os.chdir(old_path)
        return files

    def copy_all(self,from1,to,pattern="*"):
        files = self.dir(from1,pattern)
        for f in files:
            self.copy(from1+"/"+f,to+"/"+f)

    def copy_all_subfolders(self,from1,to,pattern="*"):
        for src_dir, dirs, files in os.walk(from1):
                dst_dir = src_dir.replace(from1, to, 1)
                if not os.path.exists(dst_dir):
                        os.makedirs(dst_dir)
                for file_ in files:
                        src_file = os.path.join(src_dir, file_)
                        dst_file = os.path.join(dst_dir, file_)
                        if os.path.exists(dst_file):
                                os.remove(dst_file)
                        shutil.copy(src_file, dst_dir)
        #files = self.dir_subfolders(from1,pattern)
        #for f in files:
        #    self.copy(from1+"/"+f,to+"/"+f)

    def copy_file_list(self,from1,to,files):
        for f in files:
            self.copy(from1+"/"+f,to+"/"+f)




def main(argv):
    if len(argv)<2:
        print "Parameter fehlt:"
        print "copy_bin_and_dev.py ~/expertn50_Rev222"
    else:
        ccopy_files(argv[1])


    return 0

if __name__ == '__main__':
    main(sys.argv)

