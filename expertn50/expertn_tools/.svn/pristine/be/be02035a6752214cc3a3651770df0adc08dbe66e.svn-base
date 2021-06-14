#!/usr/bin/env python

import sys

#Liesst *.cdb file
# return [header,data]
# header: 1d array mit den Headernamen
# data: 2d array mit Daten, dabei ist die erste Spalte mit den Datumswerten im Datetime Format
# daten werden hier nicht in floats umgewandelt!!!!
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
            d="0.0"
            if i==0:
                        try:
                            d=col[i]
                        except:
                            try:
                                d=col[i]
                            except:
                                d="0.0"
            else:
                        try:                        
                            d=col[i]
                        except:
                            d="0.0"
            datas[i].append(d)
  f.close()  
  return headers,datas

def write_output_file(filename,header,data):
    len_header=len(header)
    seperator = ","
    f = open(filename,'w')
    for line in range(len_header):
        str2 = header[line].split("\n")
        str1 = str2[len(str2)-1]
        for i in range(len(data[line])):
            str2 = data[line][i].split("\n")
            str1 = str1 + seperator + str2[len(str2)-1]
        str1 = str1 + "\n"
        f.write(str1)

def transpose_cdb_file(file1,file2):
    [header,data] = read_CDB_File(file1)
    write_output_file(file2,header,data)
    

if __name__ == "__main__":
   #print sys.argv,len(sys.argv)
   if len(sys.argv)<3:
     print "Missing Parameter:"
     print "transpose_cdb_file inputfile.cdb outputfile.out"
   else:
     transpose_cdb_file(sys.argv[1],sys.argv[2])
   
