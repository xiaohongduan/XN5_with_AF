#!/usr/bin/env python

import os,sys
import shutil
try:
    from iniparse import ConfigParser
except:
    from ConfigParser import ConfigParser

import numpy as np

ConfigParser.optionxform = str

def get_filename_without_path(name):
    S2 = name.replace("\\","/")
    S = S2.split("/")
    S2 = S[len(S)-1]
    return S2

def get_project_name(name):
    S =  get_filename_without_path(name)
    S = S[:-4]
    return S

def get_project_file_from_cfg_file(name):
    prjname=get_project_name(name)
    reg_str=get_regstr_from_filename(name)
    if reg_str!="":
        prjname = prjname.replace("_"+reg_str,"")
    filename=get_file_path(name)+"/"+prjname+".xpn"
    return filename


def get_file_path(name):
    S2 = name.replace("\\","/")
    S = S2.split("/")
    S3 = ""
    for i in range(len(S)-1):
        if i==0:
            S3 = S[i]
        else:
            S3 = S3 + "/" + S[i]
    return S3

def get_grid_info(regstr):
    reg_id = ""
    reg_i = ""
    reg_j = ""
    S = regstr.split("_")
    for i in range(len(S)):
        if i==0:
            reg_id=S[i]
        elif i==1:
            reg_i =S[i]
        elif i==2:
            reg_j=S[i]
    return [reg_id,reg_i,reg_j]

def get_regstr_from_filename(filename):
    filename1 = get_project_name(filename)
    filename2 = filename1.split("_")
    filename2len = len(filename2)
    if filename2len<4:
        return ""
    k = filename2[filename2len-1]
    j = filename2[filename2len-2]
    i = filename2[filename2len-3]
    Id =  filename2[filename2len-4]
    regstr = Id+"_"+i+"_"+j+"_"+k
    return regstr

def get_grid_size(grids,id1):
    if len(grids) <= 0: return [0,0,0,0]
    i1 = 0
    i2 = 0
    j1 = 0
    j2 = 0
    for grid in grids:
        [act,idnr,grid_i,grid_j,instances] = grid
        if int(idnr)==int(id1):
            i2 = int(grid_i)
            i1 = int(grid_i)
            j2 = int(grid_j)
            j1 = int(grid_j)
            break
    for grid in grids:
        [act,idnr,grid_i,grid_j,instances] = grid
        if int(idnr) != int(id1): continue
        if (int(grid_i)) > i2:
            i2 = int(grid_i)
        if int(grid_i) < i1:
            i1 = int(grid_i)
        if int(grid_j) > j2:
            j2 = int(grid_j)
        if int(grid_j) < j1:
            j1 = int(grid_j)
    return [i1,j1,i2,j2]




def replace_std_templates(filename,project_filename,BASE_PATH,REG_STR):
        PROJECT_NAME = get_project_name(project_filename)
        PROJECT_PATH = get_file_path(project_filename)
        [REG_ID,REG_I,REG_J] = get_grid_info(REG_STR)
        result = filename
        result = result.replace("$PROJECT_PATH",PROJECT_PATH)
        result = result.replace("$BASE_PATH",BASE_PATH)
        result = result.replace("$REG_STR",REG_STR)
        result = result.replace("$REG_ID",REG_ID)
        result = result.replace("$REG_I",REG_I)
        result = result.replace("$REG_J",REG_J)
        result = result.replace("$PROJECT_NAME",PROJECT_NAME)
        result = result.replace("$<","")
        result = result.replace("$>","")
        return result

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

def replace_std_template_and_path(filename,project_filename,BASE_PATH,REG_STR):
    return get_fullpath_from_relative(BASE_PATH,replace_std_templates(filename,project_filename,BASE_PATH,REG_STR))


def get_cfg_values(filename):
    config = ConfigParser()
    # Ini Datei laden:
    valueslist = []
    try:
        config.read(filename)
        for section in config.sections():
            for option in config.options(section):
                value = [section,option,config.get(section,option)]
                valueslist.append(value)
    except:
        print "Error: Loading CFG FILE: ", filename
    return valueslist

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

def get_all_filenames_from_cfg_file(cfg_name):
        filenames = []
        cfg_values=get_cfg_values(cfg_name)
        for cfg_value in cfg_values:
            [section,subsection,val] = cfg_value
            filenames2 = filter_filenames(val)
            for filename in filenames2:
                filenames.append(filename)
        return filenames


class cxpn_project:
    def __init__(self,project_filename="",base_path=""):
        self.FileAction=[]
        self.project_filename = ""
        self.base_path=base_path
        if project_filename!="":
            self.project_filename = replace_std_template_and_path(project_filename,"",self.base_path,"")


    def __del__(self):
        if len(self.FileAction)>0:
            self.do_fileoperations_in_terminal()


    def remove_cfg_grids(self):
        config = ConfigParser()
        config.read(self.project_filename)
        try:
            config.add_section("model")
        except:
            dummy=0
        #config.set("model","grid","")
        config.remove_option("model","grid")
        configfile = open(self.project_filename, 'wb')
        config.write(configfile)

    # [projectfilename]
    def create_new_project(self,newname,templatename):
        try:
            os.makedirs(get_file_path(newname))
        except:
            pass
        templatefilename = replace_std_template_and_path(templatename,"",self.base_path,"")
        newfilename = replace_std_template_and_path(newname,"",self.base_path,"")
        self.FileAction.append(['copy',templatefilename,newfilename])
        all_cfg_filenames = get_all_filenames_from_cfg_file(templatefilename)
        for filename in all_cfg_filenames:
            old_filename = replace_std_template_and_path(filename,templatefilename,self.base_path,"")
            new_filename = replace_std_template_and_path(filename,newfilename,self.base_path,"")
            #if self.copy_allowed(new_filename,old_filename):
            self.FileAction.append(['copy',old_filename,new_filename])

        self.project_filename = newfilename
        # falls schon instanzen definiert sind --> kopieren
        grids = self.project_file_read_intances(templatefilename)

        self.remove_cfg_grids()

        for grid in grids:
            [act,idnr,grid_i,grid_j,instances] = grid
            for k in range(len(instances)):
                [instancefilename,frac_str] = instances[k]
                REG_STR=str(idnr)+'_'+str(grid_i)+'_'+str(grid_j)+'_'+str(k)
                if k==0:
                    self.add_new_grid(templatefilename,instancefilename,template_reg_str=REG_STR,Grid_Act=int(act),Grid_ID=int(idnr),Grid_i=int(grid_i),Grid_j=int(grid_j),frac=float(frac_str))
                else:
                    self.add_new_instance(templatefilename,instancefilename,template_reg_str=REG_STR,Grid_ID=int(idnr),Grid_i=int(grid_i),Grid_j=int(grid_j),frac=float(frac_str))



        return newfilename

    def add_new_grid(self,template_project,template_name,template_reg_str="",Grid_Act=1,Grid_ID=0,Grid_i=0,Grid_j=0,frac=100.0,check_copy_allowed=True):
        #zuerst mal grids/instancen im projectfile laden:
        grids=self.project_file_read_intances()

        #checken, ob das schon ein grid existiert:
        for grid in grids:
            [act,idnr,grid_i,grid_j,instances] = grid
            if (int(idnr)==Grid_ID) and (int(grid_i)==Grid_i) and (int(grid_j) ==Grid_j):
                # das scheint es wohl schon zu geben --> instance addieren:
                print "Error: Grid exists, try to add the instance!"
                self.add_new_instance(template_project,template_name,template_reg_str,Grid_ID,Grid_i,Grid_j,frac)
                return

        REG_STR=str(Grid_ID)+'_'+str(Grid_i)+'_'+str(Grid_j)+'_'+'0'
        PROJECT_NAME = replace_std_templates("$PROJECT_NAME",self.project_filename,self.base_path,"")
        instancefilenamestr='$PROJECT_PATH/'+PROJECT_NAME+'_'+REG_STR+'.xpi'

        src = replace_std_template_and_path(template_name,template_project,self.base_path,template_reg_str)
        dst = replace_std_template_and_path(instancefilenamestr,self.project_filename,self.base_path,REG_STR)

        ret_str=dst

        self.FileAction.append(['copy',src,dst])
        all_cfg_filenames = get_all_filenames_from_cfg_file(src)
        for filename in all_cfg_filenames:
            old_filename = replace_std_template_and_path(filename,template_project,self.base_path,template_reg_str)
            new_filename = replace_std_template_and_path(filename,self.project_filename,self.base_path,REG_STR)
            if check_copy_allowed:
                if self.copy_allowed(new_filename,old_filename):
                    self.FileAction.append(['copy',old_filename,new_filename])
            else:
                self.FileAction.append(['copy',old_filename,new_filename])


        #neues grid addieren:
        act = str(Grid_Act)
        idnr = str(Grid_ID)
        grid_i = str(Grid_i)
        grid_j = str(Grid_j)
        frac_str = str(frac)
        instance = [instancefilenamestr,frac_str]
        instances = [instance]
        grids.append([act,idnr,grid_i,grid_j,instances])
        #grid in projectfile schreiben:
        self.project_file_write_intances(grids)
        return ret_str


    def add_new_instance(self,template_project,template_name,template_reg_str,Grid_ID,Grid_i,Grid_j,frac):
        #grids laden:
        grids = self.project_file_read_intances()
        grids2 = []
        fits = False
        for grid in grids:
            [act,idnr,grid_i,grid_j,instances] = grid
            if (int(idnr)==Grid_ID) and (int(grid_i)==Grid_i) and (int(grid_j) ==Grid_j):
                fits=True
                k = len(instances)
                REG_STR=str(Grid_ID)+'_'+str(Grid_i)+'_'+str(Grid_j)+'_'+str(k)
                PROJECT_NAME = replace_std_templates("$PROJECT_NAME",self.project_filename,self.base_path,"")
                instancefilenamestr='$PROJECT_PATH/'+PROJECT_NAME+'_'+REG_STR+'.xpi'
                src = replace_std_template_and_path(template_name,template_project,self.base_path,template_reg_str)
                dst = replace_std_template_and_path(instancefilenamestr,self.project_filename,self.base_path,REG_STR)

                self.FileAction.append(['copy',src,dst])
                all_cfg_filenames = get_all_filenames_from_cfg_file(src)
                for filename in all_cfg_filenames:
                    old_filename = replace_std_template_and_path(filename,template_project,self.base_path,template_reg_str)
                    new_filename = replace_std_template_and_path(filename,self.project_filename,self.base_path,REG_STR)
                    if self.copy_allowed(new_filename,old_filename):
                        self.FileAction.append(['copy',old_filename,new_filename])
                frac_str=str(frac)
                instance = [instancefilenamestr,frac_str]
                instances.append(instance)
                grid = [act,idnr,grid_i,grid_j,instances]
                grids2.append(grid)

            else:
                grids2.append(grid)

        if fits==False:
            print "Error: Grid doesn't fit in, try to add a new grid!"
            self.add_new_grid(template_project,template_name,template_reg_str,1,Grid_ID,Grid_i,Grid_j,frac)
        else:
            #grid in projectfile schreiben:
            self.project_file_write_intances(grids2)


    def rename_grid(self,old_grid_id,old_grid_i,old_grid_j,new_grid_id,new_grid_i,new_grid_j):
        if (old_grid_id==new_grid_id) and (old_grid_i==new_grid_i) and (old_grid_j == new_grid_j):
            print "Error: Grids are equal!"
            return
        #grids laden:
        grids = self.project_file_read_intances()
        for grid in grids:
            [act,idnr,grid_i,grid_j,instances] = grid
            if (int(idnr)==new_grid_id) and (int(grid_i)==new_grid_i) and (int(grid_j) == new_grid_j):
                print "Error: Grid Exists!"
                return

        grids2 = []
        for grid in grids:
            [act,idnr,grid_i,grid_j,instances] = grid
            if (int(idnr)==old_grid_id) and (int(grid_i)==old_grid_i) and (int(grid_j) == old_grid_j):
                instances_len = len(instances)
                instances2 = []
                for k in range(instances_len):
                    [instancefilenamestr_old,frac] = instances[k]
                    reg_str_old=str(old_grid_id)+'_'+str(old_grid_i)+'_'+str(old_grid_j)+'_'+str(k)
                    reg_str_new=str(new_grid_id)+'_'+str(new_grid_i)+'_'+str(new_grid_j)+'_'+str(k)

                    PROJECT_NAME = replace_std_templates("$PROJECT_NAME",self.project_filename,self.base_path,"")
                    instancefilenamestr_new = '$PROJECT_PATH/'+PROJECT_NAME+'_'+reg_str_new+'.xpi'
                    src = replace_std_template_and_path(instancefilenamestr_old,self.project_filename,self.base_path,reg_str_old)
                    dst = replace_std_template_and_path(instancefilenamestr_new,self.project_filename,self.base_path,reg_str_new)
                    self.FileAction.append(['rename',src,dst])
                    all_cfg_filenames = get_all_filenames_from_cfg_file(src)
                    #print src,dst
                    for filename in all_cfg_filenames:
                        old_filename = replace_std_template_and_path(filename,self.project_filename,self.base_path,reg_str_old)
                        new_filename = replace_std_template_and_path(filename,self.project_filename,self.base_path,reg_str_new)
                        if self.copy_allowed(new_filename,old_filename):
                            self.FileAction.append(['rename',old_filename,new_filename])
                    instance2 = [instancefilenamestr_new,frac]
                    instances2.append(instance2)
                grid = [act,str(new_grid_id),str(new_grid_i),str(new_grid_j),instances2]
                grids2.append(grid)
            else:
                grids2.append(grid)
        self.project_file_write_intances(grids2)


    def rm_instance(self,del_grid_id,del_grid_i,del_grid_j,del_grid_k):
        #grids laden:
        grids = self.project_file_read_intances()
        grid_instance_exists=False
        grids2 = []
        ignorelist=[]
        for grid in grids:
            [act,idnr,grid_i,grid_j,instances] = grid
            if (int(idnr)==del_grid_id) and (int(grid_i)==del_grid_i) and (int(grid_j) == del_grid_j):
                grid_instance_exists=True
                len_instances = len(instances)
                if len_instances<del_grid_k:
                    print "The instance doesen't exist: "
                    return 1
                instances2=[]
                for k in range(len_instances):
                    [instancefilenamestr,frac] = instances[k]
                    if k==del_grid_k:
                        #loeschen:
                        REG_STR=str(del_grid_id)+'_'+str(del_grid_i)+'_'+str(del_grid_j)+'_'+str(k)
                        src = replace_std_template_and_path(instancefilenamestr,self.project_filename,self.base_path,REG_STR)
                        self.FileAction.append(['rm',src,""])
                        ignorelist.append(src)
                        # alle datein durchgehen und loeschen:
                        all_cfg_filenames = get_all_filenames_from_cfg_file(src)
                        for filename1 in all_cfg_filenames:
                            filename = replace_std_template_and_path(filename1,self.project_filename,self.base_path,REG_STR)
                            if self.rm_allowed(filename,ignorelist)<=1:
                                self.FileAction.append(['rm',filename,""])
                                ignorelist.append(filename)
                        self.do_fileoperations_in_terminal()
                        continue
                    if k>del_grid_k:
                        #umbenennen:
                        REG_STR_OLD=str(del_grid_id)+'_'+str(del_grid_i)+'_'+str(del_grid_j)+'_'+str(k)
                        REG_STR_NEW=str(del_grid_id)+'_'+str(del_grid_i)+'_'+str(del_grid_j)+'_'+str(k-1)
                        PROJECT_NAME = replace_std_templates("$PROJECT_NAME",self.project_filename,self.base_path,"")
                        instancefilenamestr_new = '$PROJECT_PATH/'+PROJECT_NAME+'_'+REG_STR_NEW+'.xpi'
                        src = replace_std_template_and_path(instancefilenamestr,self.project_filename,self.base_path,REG_STR_OLD)
                        dst = replace_std_template_and_path(instancefilenamestr_new,self.project_filename,self.base_path,REG_STR_NEW)
                        self.FileAction.append(['rename',src,dst])
                        ignorelist.append(src)
                        all_cfg_filenames = get_all_filenames_from_cfg_file(src)

                        for filename in all_cfg_filenames:
                            old_filename = replace_std_template_and_path(filename,self.project_filename,self.base_path,REG_STR_OLD)
                            new_filename = replace_std_template_and_path(filename,self.project_filename,self.base_path,REG_STR_NEW)
                            if self.copy_allowed(new_filename,old_filename,ignorelist):
                                self.FileAction.append(['rename',old_filename,new_filename])
                                ignorelist.append(old_filename)

                        instancefilenamestr = instancefilenamestr_new

                    instances2.append([instancefilenamestr,frac])
                if len(instances2)>0:
                    grid = [act,idnr,grid_i,grid_j,instances2]
                    grids2.append(grid)
            else:
                grids2.append(grid)
        self.project_file_write_intances(grids2)


    def rm_grid(self,del_grid_id,del_grid_i,del_grid_j):
        #grids laden:
        grids = self.project_file_read_intances()
        grid_instance_exists=False
        grids2 = []
        len_instances=0
        for grid in grids:
            [act,idnr,grid_i,grid_j,instances] = grid
            if (int(idnr)==del_grid_id) and (int(grid_i)==del_grid_i) and (int(grid_j) == del_grid_j):
                grid_instance_exists=True
                len_instances = len(instances)
        if len_instances==0 or grid_instance_exists==False:
            print "Error: Grid doesn't exist!"
        else:
            rrange= range(len_instances)
            rrange.reverse()
            for k in rrange:
                self.rm_instance(del_grid_id,del_grid_i,del_grid_j,k)




    def copy_allowed(self,new_filename,old_filename="",ignorelist=[]):
        if old_filename==new_filename:
            return False
        #ueberpruefen, ob diese Datei in der Project File schon vorhanden ist:
        files = get_all_filenames_from_cfg_file(self.project_filename)
        for file1 in files:
            file2 = replace_std_template_and_path(file1,self.project_filename,self.base_path,"")
            if file2==new_filename:
                inignorelist=False
                for file3 in ignorelist:
                    if file3==file2:
                        inignorelist=True
                if inignorelist==False:
                    return False

        #ueberpruefen, ob diese Datei in einer Instance schon vorhanden ist:
        grids = self.project_file_read_intances()
        for grid in grids:
            [act,idnr,grid_i,grid_j,instances] = grid
            for instance in instances:
                [cfg_name,frac] = instance
                name = replace_std_template_and_path(cfg_name,self.project_filename,self.base_path,"")
                files = get_all_filenames_from_cfg_file(name)
                for file1 in files:
                    file2 = replace_std_template_and_path(file1,self.project_filename,self.base_path,"")
                    if file2==new_filename:
                        inignorelist=False
                        for file3 in ignorelist:
                            if file3==file2:
                                inignorelist=True
                        if inignorelist==False:
                            return False
        return True

    def rm_allowed(self,filename,ignorelist=[]):
        filename_count=0
        #ueberpruefen, ob diese Datei in der Project File schon vorhanden ist:
        files = get_all_filenames_from_cfg_file(self.project_filename)
        for file1 in files:
            file2 = replace_std_template_and_path(file1,self.project_filename,self.base_path,"")
            if file2==filename:
                inignorelist=False
                for file3 in ignorelist:
                    if file3==file2:
                        inignorelist=True
                if inignorelist==False:
                    filename_count=filename_count+1


        #ueberpruefen, ob diese Datei in einer Instance schon vorhanden ist:
        grids = self.project_file_read_intances()
        for grid in grids:
            [act,idnr,grid_i,grid_j,instances] = grid
            for instance in instances:
                [cfg_name,frac] = instance
                name = replace_std_template_and_path(cfg_name,self.project_filename,self.base_path,"")
                files = get_all_filenames_from_cfg_file(name)
                for file1 in files:
                    file2 = replace_std_template_and_path(file1,self.project_filename,self.base_path,"")
                    if file2==filename:
                        inignorelist=False
                        for file3 in ignorelist:
                            if file3==file2:
                                inignorelist=True
                        if inignorelist==False:
                            filename_count=filename_count+1
        return filename_count

    #return grids
    def project_file_read_intances(self,project_file=""):
        if project_file=="":
            project_file=self.project_filename
        #sicherheithalber alle file_operationen abschliessen:
        self.do_fileoperations_in_terminal()
        grids = []
        try:
            config = ConfigParser()
            config.read(project_file)
            model_grid = config.get("model","grid")
        except:
            return grids

        model_grids=model_grid.split(";")
        len_model_grids = len(model_grids)

        for i in range(len_model_grids):
            code = model_grids[i].split(",")
            code_len=len(code)
            if code_len < 6:
                continue
            act = code[0]
            idnr = code[1]
            grid_i = code[2]
            grid_j = code[3]
            instances = []
            for i2 in range(4,code_len,2):
                # code[i2] == Pfad, code[i2+1] == Frac
                instance = [code[i2],code[i2+1]]
                instances.append(instance)
            grids.append([act,idnr,grid_i,grid_j,instances])
        return grids

    def change_cover_frac(self,Grid_ID,Grid_i,Grid_j,Grid_k,new_frac):
        grids = self.project_file_read_intances()
        grids2 = []
        fits = False
        for grid in grids:
            [act,idnr,grid_i,grid_j,instances] = grid
            if (int(idnr)==Grid_ID) and (int(grid_i)==Grid_i) and (int(grid_j) ==Grid_j):
                for k in range(len(instances)):
                    if k==Grid_k:
                        [cfg_name,frac] = instances[k]
                        frac = str(new_frac)
                        instances[k] = [cfg_name,frac]
            grid2 = [act,idnr,grid_i,grid_j,instances]
            grids2.append(grid2)
        self.project_file_write_intances(grids2)


    def change_active(self,Grid_ID,Grid_i,Grid_j,new_active):
        grids = self.project_file_read_intances()
        grids2 = []
        fits = False
        for grid in grids:
            [act,idnr,grid_i,grid_j,instances] = grid
            if (int(idnr)==Grid_ID) and (int(grid_i)==Grid_i) and (int(grid_j) ==Grid_j):
                act=str(new_active)
            grid2 = [act,idnr,grid_i,grid_j,instances]
            grids2.append(grid2)
        self.project_file_write_intances(grids2)

    def change_active_all(self,new_active):
        grids = self.project_file_read_intances()
        grids2 = []
        fits = False
        for grid in grids:
            [act,idnr,grid_i,grid_j,instances] = grid
            act=str(new_active)
            grid2 = [act,idnr,grid_i,grid_j,instances]
            grids2.append(grid2)
        self.project_file_write_intances(grids2)







    # schreibt den grid/instance kombination in den Abschnit model.grid in der project file
    def project_file_write_intances(self,grids,project_file=""):
        if project_file=="":
            project_file=self.project_filename
        #sicherheithalber alle file_operationen abschliessen:
        self.do_fileoperations_in_terminal()
        codestr = ""
        for grid in grids:
            [act,idnr,grid_i,grid_j,instances] = grid
            codestr = codestr + act + ","+idnr+","+grid_i+","+grid_j
            for instance in instances:
                [cfg_name,frac] = instance
                codestr = codestr + "," + cfg_name + "," + frac
            codestr = codestr + ";"

        config = ConfigParser()
        config.read(project_file)
        try:
            config.add_section("model")
        except:
            dummy=0
        config.set("model","grid",codestr)
        configfile = open(project_file, 'wb')
        config.write(configfile)




    def do_fileoperations_in_terminal(self):
        if len(self.FileAction)==0:
            return

        for item in self.FileAction:
            [act,src,dst] = item
            if act=='copy':
                try:
                    shutil.copy(src,dst)
                    print "copy: ", src , " ", dst
                except:
                    print "Error: copy file: ",src,dst
            if act=='rm':
                try:
                    os.remove(src)
                    print "rm: ", src
                except:
                    print "Error: remove file: ",src
            if act=='rename':
                try:
                    shutil.move(src,dst)
                    print "rename: ", src , " ", dst
                except:
                    print "Error: rename File: ",src,dst
        self.FileAction=[]


if __name__ == "__main__":
    print "xpn_cfg_manager.py"
    #p=cxpn_project(base_path="/home/chrikle/expertn50/built",project_filename="/home/chrikle/expertn50/built/cfg3/test.xpn")
    #p.create_new_project("$BASE_PATH/cfg3/test.xpn","$BASE_PATH/cfg_template/project.xpn")
    #p.add_new_grid("$BASE_PATH/cfg_template/project.xpn","$BASE_PATH/cfg_template/project.xpi","",1,3,20,21)
    #p.add_new_grid("$BASE_PATH/cfg_template/project.xpn","$BASE_PATH/cfg_template/project.xpi","",1,3,20,22)
    #p.add_new_grid("$BASE_PATH/cfg_template/project.xpn","$BASE_PATH/cfg_template/project.xpi","",1,3,20,21)
    #p.add_new_instance("$BASE_PATH/cfg_template/project.xpn","$BASE_PATH/cfg_template/project.xpi","",3,20,21,50.0)
    #for i2 in range(1,34):
        #for i in range(1,34):
            #if np.random.rand()> 0.9:
                #p.add_new_grid("$BASE_PATH/cfg_template/project.xpn","$BASE_PATH/cfg_template/project.xpi","",1,2,i2,i)



    #print "\n\n\n"
    #p.rename_grid(3,20,21,5,10,15)
    #print "\n\n\n"
    #p.rm_instance(5,10,15,0)


    #p=cxpn_project(base_path="/home/chrikle/expertn50/built",project_filename="/home/chrikle/expertn50/built/cfg/coupling.xpn")
    #grids = p.project_file_read_intances()
    #print grids


