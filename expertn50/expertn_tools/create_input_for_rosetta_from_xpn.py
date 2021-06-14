#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
try:
    from iniparse import ConfigParser
except:
    from ConfigParser import ConfigParser


def convert_strlist_to_float(list1):
    list2 = list1.split(";")
    list3 = []
    for item in list2:
        if item != "":
            list3.append(float(item))
    return list3

input_header = 'Code    Description        Sand       Silt       Clay       BD         TH33       TH1500     \n                           %          %          %          gr/cm3     cm3/cm3    cm3/cm3   \n'

def main(argv):
    if len(sys.argv)<3:
        print "Parameter fehlt:"
        print "convert_input_for_rosetta_from_xpn.py xpn.ini rosetta_input.txt"
    else:
        config = ConfigParser()
        config.read(sys.argv[1])
        sand = convert_strlist_to_float(config.get("soil","sand"))
        silt = convert_strlist_to_float(config.get("soil","silt"))
        clay = convert_strlist_to_float(config.get("soil","clay"))
        bulk_density = convert_strlist_to_float(config.get("soil","bulk_density"))

        TH33 = -9.90000
        TH1500 =- 9.90000

        f = open(sys.argv[2], 'w')
        f.write(input_header)
        for i in range(len(sand)):
            S = "%d%9s%26f%11f%11f%10f%11f%11f\n" %(i+1,"NA",sand[i],silt[i],clay[i],bulk_density[i],TH33,TH1500)
            f.write(S)
        f.close()



if __name__ == '__main__':
    main(sys.argv)
