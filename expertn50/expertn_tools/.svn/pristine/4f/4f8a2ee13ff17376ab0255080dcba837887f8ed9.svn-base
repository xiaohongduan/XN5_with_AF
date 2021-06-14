#!/usr/bin/env python

try:
    from iniparse import ConfigParser
except:
    from ConfigParser import ConfigParser

from netCDF4 import MFDataset
import xpn_cfg_manager as xpn
import sys
import math
import shutil
import os

ConfigParser.optionxform = str

class cxpn_create_prj_netcdf:
    def __init__(self,ini_filename):
        self.load_ini(ini_filename)

    def load_ini(self,ini_filename):
        config = ConfigParser()
        config.read(ini_filename)
        self.base_path = config.get("system","base_path")
        self.project_path = xpn.replace_std_template_and_path(config.get("system","project_path"),"",self.base_path,"")
        self.project_filename=self.project_path+"/"+config.get("system","project_name")
        self.template_project = config.get("system","project_template")
        self.domain_id = int(config.get("system","domain_id"))
        self.netcdf_file = config.get("grid","netcdf_file")
        self.source_format = int(config.get("grid","source_format"))
        self.veg_type_list = self.get_vegetations_config_types(config)
        try:
           self.rm_grids_after = []
           del_grids = config.get("grid","delete_grids_after")
           for del_grid in del_grids.split(";"):
                items = del_grid.split(",")
                if len(items) >= 3:
                  del_grid_id = int(items[0])
                  del_grid_i = int(items[1])
                  del_grid_j = int(items[2])
                  item = [del_grid_id,del_grid_i,del_grid_j]
                  self.rm_grids_after.append(item)
        except:
           self.rm_grids_after = []


    def get_vegetations_config_types(self,config):
        veg_type_list=[]
        section = "vegetation"
        for option in config.options(section):
                S = option.split("type_")
                if len(S)>1:
                        veg_type=int(S[1])
                        cfg_file= config.get(section,option)
                        veg_type_list.append([veg_type,cfg_file,-1,-1])
                else:
                        opt = config.get(section,option)
                        opt_list = opt.split(";")
                        veg_type_i = int(opt_list[0])
                        veg_type_j = int(opt_list[1])
                        cfg_file = opt_list[2]
                        veg_type = -1
                        veg_type_list.append([veg_type,cfg_file,veg_type_i,veg_type_j])
                
                

        return veg_type_list

    def load_netcdf(self):
        netf =  MFDataset(self.netcdf_file)

        if (self.source_format==1):
            self.vegtyp = netf.variables["LU_INDEX"][0,:,:]
            self.xlat = netf.variables["XLAT_M"][0,:,:]
            self.xlon = netf.variables["XLONG_M"][0,:,:]
            self.alt = netf.variables["HGT_M"][0,:,:]
            self.slopeX = netf.variables["SLPX"][0,:,:]
            self.slopeY = netf.variables["SLPY"][0,:,:]
            # Es gibt noch die Variable SCB_DOM (Soil Category Bottom dominant) --> die wird im NOAH nicht verwendet
            self.soil_type = netf.variables["SCT_DOM"][0,:,:]
            # Fuer die folgende variable braucht man die met_em....nc, die geo_em enthaelt diese Daten nicht!
            self.soil_moisture = netf.variables["SM"][0,:,:,:]*100.0 # moisture [m3 m-3] --> %
            self.soil_temperature_C = netf.variables["ST"][0,:,:,:]-273.15 # Grad C (urspruenglich in K)
        else:
            print "Error: netcdf source format"
            exit()


        self.j_size = len(self.xlat)
        self.i_size = len(self.xlat[0])


    def create_project(self):

        self.load_netcdf()

        p=xpn.cxpn_project(base_path=self.base_path,project_filename=self.project_filename)
        p.create_new_project(self.project_filename,self.template_project)

        count = 0

        for j in range(self.j_size):
            for i in range(self.i_size):
                for veg_type_item in self.veg_type_list:
                    [veg_type,templatefile,veg_type_i,veg_type_j] = veg_type_item
                    if self.vegtyp[j][i]==veg_type or ((veg_type==-1) and (veg_type_i==i) and (veg_type_j ==j)):
                        soil_type_filename = "noah_soils/soil_type_"+str(int(self.soil_type[j][i]))+".ini"
                        if os.path.isfile(soil_type_filename)==False:
                            print "Error: File ",soil_type_filename," for soil type ",str(int(self.soil_type[j][i]))," does not exist!"
                            print "Info: (i,j): ",i,j, "Lat / Lon: ",self.xlat[j][i],self.xlon[j][i]
                            continue

                        templateregstr = xpn.get_regstr_from_filename(templatefile)
                        templateproject = xpn.get_project_file_from_cfg_file(templatefile)
                        config_file = p.add_new_grid(self.template_project,templatefile,templateregstr,1,self.domain_id,i,j,check_copy_allowed=False)

                        # INI Datei bearbeiten:
                        regstr=xpn.get_regstr_from_filename(config_file)

                        #Dateiname der Database INI Datei ermitteln
                        config = ConfigParser()
                        config.read(config_file)
                        standard_ini_filename_temp = config.get("Expert N Standard Read INI","filename")
                        standard_ini_filename = xpn.replace_std_template_and_path(standard_ini_filename_temp,self.project_filename,self.base_path,regstr)

                        shutil.copy(soil_type_filename,standard_ini_filename)


                        #Database INI Datei bearbeiten:
                        config = ConfigParser()
                        config.read(standard_ini_filename)
                        config.set("location","lat",self.xlat[j][i])
                        config.set("location","lon",self.xlon[j][i])
                        config.set("location","alt",self.alt[j][i])
                        sly = self.slopeY[j][i]
                        slx = self.slopeY[j][i]
                        slpq = sly*sly + slx*slx
                        slp = math.sqrt(slpq)

                        #StartWerte:
                        #
                        water_content = ""
                        soil_temp = ""
                        for n in range(4):
                            water_content = water_content+str(self.soil_moisture[n][j][i])+";"
                            soil_temp = soil_temp+str(self.soil_temperature_C[n][j][i])+";"

                        config.set("start values","water_content",water_content)
                        config.set("start values","soil_temperature",soil_temp)

                        config.set("location","inclination",slp*100.0)
                        configfile = open(standard_ini_filename, 'wb')
                        config.write(configfile)



                        count = count + 1


        print "Number of Expert-N Instances: ", count


    def delete_grids_if_nec(self):
        p=xpn.cxpn_project(base_path=self.base_path,project_filename=self.project_filename)
        for item in self.rm_grids_after:
            [del_grid_id,del_grid_i,del_grid_j] = item
            print "Delete Template Grid: Id=",del_grid_id,", i=",del_grid_i,", j=",del_grid_j
            p.rm_grid(del_grid_id,del_grid_i,del_grid_j)






if __name__ == "__main__":
    if len(sys.argv)<2:
        print "Parameter fehlt:"
        print "create_project_from_netcdf.py cfgfile"
    else:
        ini_file=sys.argv[1]
        pr = cxpn_create_prj_netcdf(ini_file)
        pr.create_project()
        pr.delete_grids_if_nec()



