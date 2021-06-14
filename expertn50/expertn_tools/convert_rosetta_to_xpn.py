#!/usr/bin/env python
# -*- coding: utf-8 -*-
from xnoplot import *
import sys

try:
    from iniparse import ConfigParser
except:
    from ConfigParser import ConfigParser

def create_string_list(list1):
    str1 = ""
    for item in list1:
        if str1=="":
            str1 = str(item)+';'
        else:
            str1 = str1 + str(item) + ";"
    return str1

def main(argv):
    if len(sys.argv)<3:
        print "Parameter fehlt:"
        print "convert_rossetta_to_xpn.py rosetta_output.txt xpn.ini"
    else:
         output=xpn_data(filename=sys.argv[1],dateformat="no date",spacer=' ',merge_delimiters=True)
         Theta_r = output["Code","Theta_r"][1]
         Theta_s = output["Code","Theta_s"][1]
         Alpha = output["Code","Alpha"][1]
         N = output["Code","N"][1]
         Ks = output["Code","Ks"][1]
         
         config = ConfigParser()
         try:
             config.read(sys.argv[2])
         except:
             pass
         try:
             config.add_section("soil")
         except:
             pass
                  
             
         config.set("soil","res_water_cont",create_string_list(Theta_r*100.0))
         config.set("soil","porosity",create_string_list(Theta_s*100.0))
         config.set("soil","van_gen_a",create_string_list(Alpha))
         config.set("soil","van_gen_n",create_string_list(N))
         config.set("soil","cond_sat",create_string_list(Ks*10.0))
         configfile=open(sys.argv[2],"w")
         config.write(configfile)
         configfile.close()
         

if __name__ == '__main__':
    main(sys.argv)
