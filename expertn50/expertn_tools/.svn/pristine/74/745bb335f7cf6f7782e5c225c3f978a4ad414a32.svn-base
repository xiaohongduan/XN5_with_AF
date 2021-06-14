#!/usr/bin/env python

#from Scientific.IO.NetCDF import *
from netCDF4 import MFDataset
import sys

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

def get_OutputLatLon(out,Lat,Lon,x_size,y_size,OutputLat,OutputLon):
  OutX=0
  OutY=0
  latw = OutputLat-Lat[OutX][OutY]
  latq = latw*latw
  lonw = OutputLon-Lon[OutX][OutY]
  lonq = lonw*lonw
  rqold = latq+lonq
  for j in range(0,y_size):
    out.writeln(str(float(j)/float(y_size)*100.0)+ "%   "+ str(j)+" "+str(y_size))
    for i in range (0,x_size):
      latw = OutputLat-Lat[i][j]
      latq = latw*latw
      lonw = OutputLon-Lon[i][j]
      lonq = lonw*lonw
      rq = latq+lonq
      if rq <= rqold:
        rqold = rq
        OutX = i
        OutY = j
  return OutX,OutY
  

def get_grid_ij(out,netcdf_file,lat,lon):
  #file = NetCDFFile(netcdf_file,'r')
  file =  MFDataset(netcdf_file)
  latstr = "XLAT"
  lonstr = "XLONG"
  try:
    Lat2 = file.variables[latstr]
    Lon2 = file.variables[lonstr]
  except:
    latstr = "XLAT_M"
    lonstr = "XLONG_M" 
  
  Lat = file.variables[latstr][0,:,:]
  Lon = file.variables[lonstr][0,:,:]
  Lato = file.variables[latstr]
  Lono = file.variables[lonstr]
  z_size = Lato.shape[0]
  x_size = Lato.shape[1]
  y_size = Lato.shape[2]
  out.writeln("Suche nach Grid fuer Koordinaten Lat/Lon: "+str(lat)+"/"+str(lon))
  OutputX,OutputY = get_OutputLatLon(out,Lat,Lon,x_size,y_size,lat,lon)
  out.writeln("Grid Koordinaten sind (i,j)             : "+str(OutputY)+"/"+str(OutputX))
  out.writeln("Die entsprechenden Koordinaten Lat/Lon  : "+str(Lat[OutputX][OutputY])+"/"+str(Lon[OutputX][OutputY]))
  return [OutputY,OutputX,Lat[OutputX][OutputY],Lon[OutputX][OutputY]]
  
if __name__ == "__main__":
   #print sys.argv,len(sys.argv)
   if len(sys.argv)<4:
     print "Parameter fehlt:"
     print "find_gird_koord.py netcdfile lat lon"
   else:
     out = c_output()
     get_grid_ij(out,sys.argv[1],float(sys.argv[2]),float(sys.argv[3]))
   
