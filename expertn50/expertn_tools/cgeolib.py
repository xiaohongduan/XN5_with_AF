#!/usr/bin/env python
# -*- coding: utf-8 -*-
#  Copyright 2013 Christian Klein <christian.klein@helmholtz-muenchen.de>

# This program is free software;you can redistribute it
# and/or modify it under the terms of the GNU General
# Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at
# your option) any later version.

# This program is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

# You should have received a copy of the GNU General
# Public License along with this program; if not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Fifth Floor, Boston, MA  02110-1301, USA.

import sys
import glob
import os
import math
import numpy as np
import datetime
import time
import matplotlib.dates
import codecs
import errno

def convert_svg_to_pdf(file_in,file_out):
    os.system("inkscape -f %s -A %s" % (file_in,file_out))

def convert_svg_to_eps(file_in,file_out):
    os.system("inkscape -f %s -E %s" % (file_in,file_out))

# Bsp.:
# datum_korr = roundTime(datum,30*60) rundet immer auf 30 Minuten
# datum_korr = roundTime(datum,60*60) rundet immer auf die ganze Stunde
def roundTime(dt=None, roundTo=60):
   """Round a datetime object to any time laps in seconds
   dt : datetime.datetime object, default now.
   roundTo : Closest number of seconds to round to, default 1 minute.
   Author: Thierry Husson 2012 - Use it as you want but don't blame me.
   """
   if dt == None : dt = datetime.datetime.now()
   seconds = (dt - dt.min).seconds
   # // is a floor division, not a comment on following line:
   rounding = (seconds+roundTo/2) // roundTo * roundTo
   return dt + datetime.timedelta(0,rounding-seconds,-dt.microsecond)


# get total seconds of a timedelta
def get_total_seconds(td): return (td.microseconds + (td.seconds + td.days * 24 * 3600) * 1e6) / 1e6



# same as arange for date and time:
# example: a = arange_date_and_time(datetime.datetime(year=2012,month=4,day=1),datetime.datetime(year=2014,month=11,day=1))
def arange_date_and_time(start,stop,step=datetime.timedelta(hours=1)):
    #[base - datetime.timedelta(days=x) for x in range(0, numdays)]
    len_ar = (get_total_seconds(stop-start))/get_total_seconds(step)
    ar = np.empty((len_ar),dtype=object)
    act_date = start
    i = 0
    while (act_date<stop):
        ar[i] = act_date
        i+=1
        act_date+=step
    return ar

# progress in % [0..100]
def drawProgressBar(progress, barLen = 20):
    percent = progress/100.0
    sys.stdout.write("\r")
    progress = ""
    for i in range(barLen):
        if i < int(barLen * percent + 0.5):
            progress += "="
        else:
            progress += " "
    sys.stdout.write("[ %s ] %.2f%%" % (progress, percent * 100))
    sys.stdout.flush()


# Kann den Abspand zweier Standpunkte auf 50m genau berechnen. Es wird dabei
# ein WGS86 Ellipsoid zugrundegelegt
# Quelle: http://de.wikipedia.org/wiki/Orthodrome
def get_distanance(lat1, lon1, lat2, lon2):
    rad_factor = math.pi / 180.0
    b1 = lat1
    b2 = lat2
    l1 = lon1
    l2 = lon2
    # Abplattung der Erde:
    f = 1.0 / 298.257223563
    # Aequatoreadius der Erde in Metern:
    a = 6378137.0 / 1000.0
    F = (b1 + b2) / 2.0 * rad_factor
    G = (b1 - b2) / 2.0 * rad_factor
    l = (l1 - l2) / 2.0 * rad_factor
    S = np.sin(G) ** 2 * np.cos(l) ** 2 + np.cos(F) ** 2 * np.sin(l) ** 2
    C = np.cos(G) ** 2 * np.cos(l) ** 2 + np.sin(F) ** 2 * np.sin(l) ** 2
    w = np.arctan(np.sqrt(S / C))
    D = 2.0 * w * a
    R = np.sqrt(S * C) / w
    # Abstand D muss durch die Faktoren H1 und H2 korrigiert werden
    H1 = (3.0 * R - 1.0) / (2.0 * C)
    H2 = (3.0 * R + 1.0) / (2.0 * S)
    # Abstand s in km berechnet sich folgendermassen:
    s = D * (1.0 + f * H1 * np.sin(F) ** 2 * np.cos(G) ** 2 - f * H2 * np.cos(F) ** 2 * np.sin(G) ** 2)
    return s



    #Find closest point in a set of (lat,lon) points to specified point
    #latvar - 2D latitude variable from an open netCDF dataset
    #lonvar - 2D longitude variable from an open netCDF dataset
    #lat0,lon0 - query point
    #Returns iy,ix such that the square of the tunnel distance
    #between (latval[it,ix],lonval[iy,ix]) and (lat0,lon0)
    #is minimum.
    # example:
    #
    #   ncfile = netCDF4.Dataset(filename, 'r')
    #   latvar = ncfile.variables['Latitude']
    #   lonvar = ncfile.variables['Longitude']
    #   iy,ix = tunnel_fast(latvar, lonvar, 50.0, -140.0)
    #   print 'Closest lat lon:', latvar[iy,ix], lonvar[iy,ix]
    #   ncfile.close()
def find_koord(latvar,lonvar,lat0,lon0):
    rad_factor = math.pi/180.0 # for trignometry, need angles in radians
    # Read latitude and longitude from file into numpy arrays
    latvals = latvar[:] * rad_factor
    lonvals = lonvar[:] * rad_factor
    ny,nx = latvals.shape
    lat0_rad = lat0 * rad_factor
    lon0_rad = lon0 * rad_factor
    # Compute numpy arrays for all values, no loops
    clat,clon = np.cos(latvals),np.cos(lonvals)
    slat,slon = np.sin(latvals),np.sin(lonvals)
    delX = np.cos(lat0_rad)*np.cos(lon0_rad) - clat*clon
    delY = np.cos(lat0_rad)*np.sin(lon0_rad) - clat*slon
    delZ = np.sin(lat0_rad) - slat;
    dist_sq = delX**2 + delY**2 + delZ**2
    minindex_1d = dist_sq.argmin()  # 1D index of minimum element
    iy_min,ix_min = np.unravel_index(minindex_1d, latvals.shape)
    return iy_min,ix_min



def get_files_of_path(path,pathname):
    old_path = os.path.abspath(os.curdir)
    #os.path.dirname( os.path.realpath( __file__ ) )
    os.chdir(path)
    output_files= sorted(glob.glob(pathname),key=str.lower)
    os.chdir(old_path)
    output_file_with_path = map(lambda x: path+"/"+x,output_files)
    return output_file_with_path

def make_sure_path_exists(path):
    try:
        os.makedirs(path)
    except OSError as exception:
        if exception.errno != errno.EEXIST:
            raise


#input array (lon,lat):
# coordinates = np.array([
#                    [9.858862713246934,52.24148357918767],
#                    [9.859486005889513,50.84519118924196],
#                    [11.6631815667536,50.8377294606306],
#                    [11.70391076824869,52.23009356842507]
#                  ])
#return  [[lon1,lat1],[lon2,lat2]]
def get_edges_lon_lat(coordinates):
    lons, lats = coordinates.transpose()
    lon1 = np.amin(lons)
    lon2 = np.amax(lons)
    lat1 = np.amin(lats)
    lat2 = np.amax(lats)
    return np.array([[lon1, lat1], [lon2, lat2]])



# a structure implementation:
# example:
# struc = structure(a=4.0,b=1.0)
# print struc.a
class structure(dict):
    def __init__(self, *args, **kwargs):
        dict.__init__(self, *args, **kwargs)
        self.__dict__ = self

# Polynomial Regression
# res = polyfit(x,y,1)
# r² = res.det
# print “y=%.6fx+(%.6f)”%(res.coeffs[0],res.coeffs[1])
# plot(x,res.poly(x))

def polyfit(x, y, degree=1):
    x2 = np.array(x,dtype=float)
    y2 = np.array(y,dtype=float)
    results = {}

    coeffs = np.polyfit(x2, y2, degree)

     # Polynomial Coefficients
    coeffs = coeffs.tolist()

    # r-squared
    p = np.poly1d(coeffs)
    # fit values, and mean
    yhat = p(x2)                         # or [p(z) for z in x2]
    ybar = np.sum(y2)/len(y2)          # or sum(y)/len(y)
    ssreg = np.sum((yhat-ybar)**2)   # or sum([ (yihat - ybar)**2 for yihat in yhat])
    sstot = np.sum((y2 - ybar)**2)    # or sum([ (yi - ybar)**2 for yi in y])
    det = ssreg / sstot

    polynomial = np.poly1d(coeffs)

    results = structure(coeffs=coeffs,det=det,poly=polynomial)
    return results

# xpn Data Klasse:
# Auslesen von Dateien im xno, csv und cdb Format
# z.b:
# noah_output = xpn_data("noah_output.csv",dateformat='%Y-%m-%d %H:%M:%S')
# out = noah_output["Times","SMC:0"]
#output = xpn_data("noah_output.csv",dateformat='no date') falls erste Spalte kein Datum ist
class xpn_data:
    def __init__(self, data=None, dateformat='%Y-%m-%d %H:%M:%S', spacer=',', silent=False, merge_delimiters=False, from_date=None, to_date=None):
        self.silent = silent
        filename = None
        if isinstance(data, np.ndarray):
            if (from_date is not None) or (to_date is not None):
                self.data = self.get_data_from_time_range(data, date_from=from_date, date_to=to_date)
            else:
                self.data = data
        elif isinstance(data, list):
            if (from_date is not None) or (to_date is not None):
                self.data = self.get_data_from_time_range(np.array(data), date_from=from_date, date_to=to_date)
            else:
                self.data = np.array(data)
        else:
            self.data = None
            filename = data
        if filename is not None:
            if dateformat == "xno":
                self.read_xno_file(filename, merge_delimiters, from_date=from_date, to_date=to_date)
            elif dateformat == "cdb":
                self.read_cdb_file(filename, merge_delimiters, from_date=from_date, to_date=to_date)
            else:
                self.read_csv_file(filename, dateformat, spacer, merge_delimiters, from_date=from_date, to_date=to_date)

    def __del__(self):
        del self.data

    # axis: 0 --> rows, 1: --> col
    def get_size(self,axis=0):
        return np.size(self.data,axis)

    # axis: 0 --> rows, 1: --> col
    def size(self,axis=0):
        return np.size(self.data,axis)

    # return np.array
    def add_col(self,number=1,cols_to_add=None,overwrite_data=False):
        data_new = np.resize(self.data.transpose(),(np.size(self.data,1)+number,np.size(self.data,0))).transpose()
        if cols_to_add is not None:
            data_new[:,-(number):] = cols_to_add
        if overwrite_data:
            del self.data
            self.data = data_new
        return data_new

    def convert_col(self,keys,cmd):
        if isinstance(keys, type(())) is False and isinstance(keys, list) is False and isinstance(keys,
                                                                                                  np.ndarray) is False:
            keys = keys,
        indices = np.zeros(len(keys), dtype=int)
        for i in xrange(len(keys)):
            if isinstance(keys[i], int):
                indices[i] = keys[i]
            else:
                indices[i] = list(self.data[0, :]).index(keys[i])

        for i in indices:
            self.data[1:,i] = map(lambda x : cmd(x), self.data[1:,i])

        del indices

    # sortiert nach datum absteigend
    def sort_by_date(self):
        #self.data[1:].sort(axis=0)
        self.data[1:] = self.data[1:][self.data[1:,0].argsort()]


    @staticmethod
    # verbindet eine liste/array aus xpn_data (data müssen den gleichen header haben)
    # und sortiert die Werte anschließend
    # return neuer xpn_data
    def merge_xpn_data(arr):
        data_len = 1
        for item in arr:
            data_len += len(item.data)-1
        header = arr[0].get_header()
        data_raw = np.empty((data_len,len(header)),dtype=object)
        data_raw[0] = header
        start_row = 1
        for item in arr:
            data_raw[start_row:start_row+len(item.data)-1] = item.data[1:]
            start_row += len(item.data)-1

        data_new = xpn_data(data_raw)
        data_new.sort_by_date()
        return data_new


    @staticmethod
    def __rem_more_then_one_ch(str1, ch1):
        a = ''
        last_is_space = False
        for ch in str1:
            if (ch == ch1) and (last_is_space is False):
                a = a + ch
                last_is_space = True
            if ch != ch1:
                a = a + ch
                last_is_space = False
        return a

    @staticmethod
    def __to_str(x, dateformat):
        if type(x) is datetime.datetime:
            if dateformat == 'cdb':
                return str(time.mktime(x.timetuple()))
            else:
                try:
                    return x.strftime(dateformat)
                except:
                    return x.isoformat(" ").split(".")[0]
        return str(x)

    # Removes NaN, which should be marked as "-"
    @staticmethod
    def remove_nan(data):
        condition = (data != "-").all(1)
        return data[condition]

    @staticmethod
    def get_data_from_time_range(data, date_from=None, date_to=None, data_has_header=True):
        org_from, org_to = xpn_data.get_time_range(data, data_has_header)
        if date_from is None:
            date_from = org_from
        if date_to is None:
            date_to = org_to
        if data_has_header:
            condition = (data[1:, 0] >= date_from) & (data[1:, 0] < date_to)
            condition = np.insert(condition, 0, True)
            return data[condition]
        else:
            condition = (data[:, 0] >= date_from) & (data[:, 0] <= date_to)
            return data[condition]

    @staticmethod
    def get_time_range(data, data_has_header=True):
        if data_has_header:
            return data[1, 0], data[-1, 0]
        else:
            return data[0, 0], data[-1, 0]

    @staticmethod
    def get_average(data, data_has_header=True, with_header=True):
        mat_start = 0
        if data_has_header:
            mat_start = 1
        dt = (matplotlib.dates.date2num(data[mat_start+1:, 0])-matplotlib.dates.date2num(data[mat_start:-1, 0]))
        result_without_header = "-"
        if len(dt)!=0:
            dt = np.append(dt, dt[-1])
            dt /= np.sum(dt)
            inter_result = (data[mat_start:, 1:].transpose()*dt).transpose()
            dt2 = matplotlib.dates.date2num(data[1, 0])
            dt1 = matplotlib.dates.date2num(data[-1, 0])
            result_without_header = np.insert(np.sum(inter_result, axis=0), 0, matplotlib.dates.num2date((dt2-dt1)/2.0+dt1))

        if data_has_header and with_header:
            result_with_header = np.array([data[0, :], result_without_header])
            return result_with_header
        else:
            return result_without_header

    def get_averaged_data(self, delta, remove_nans=True):
        data = self.data
        data_without_header = data[1:, :]
        start_time, end_time = xpn_data.get_time_range(data, data_has_header=True)
        count_for_new_array = 1 + np.int(np.floor((end_time-start_time).total_seconds()/delta.total_seconds()))
        new_mat = np.empty((count_for_new_array, len(data[0])), dtype=object)
        #header
        new_mat[0,:] = data[0, :]
        act_time_and_date = start_time
        for i in xrange(1,count_for_new_array):
            next_time_and_date = act_time_and_date + delta
            data_part = xpn_data.get_data_from_time_range(data=data_without_header,date_from=act_time_and_date, date_to=next_time_and_date, data_has_header=False)
            new_mat[i, :] = xpn_data.get_average(data_part,data_has_header=False, with_header=False)
            act_time_and_date = next_time_and_date
        if remove_nans:
            return xpn_data.remove_nan(new_mat)
        return new_mat


    def read_cdb_file(self, filename, merge_delimiters=False,from_date=None, to_date=None):
        return self.read_csv_file(filename, dateformat='cdb', spacer=',', merge_delimiters=merge_delimiters, from_date=from_date, to_date=to_date)


    def read_xno_file(self, filename, merge_delimiters=False, from_date=None, to_date=None):
        return self.read_csv_file(filename, dateformat='%Y-%m-%d %H:%M:%S', spacer=',',
                                  merge_delimiters=merge_delimiters, from_date=from_date, to_date=to_date)

    #Liesst *.csv file
    # return [header,data]
    # header: 1d array mit den Headernamen
    # data: 2d array mit Daten, dabei ist die erste Spalte mit den Datumswerten im Datetime Format
    def read_csv_file(self, filename, dateformat='%Y-%m-%d %H:%M:%S', spacer=',', merge_delimiters=False, from_date=None, to_date=None):
        del self.data
        self.data = None
        f = open(filename, 'rU')
        count = 0
        data = []
        col_count = 0
        for line in f:
            values = []
            dont_write = False
            if merge_delimiters:
                line = self.__rem_more_then_one_ch(line, spacer)
            col = line.split(spacer)
            count += 1
            if count == 1:
                for item in col:
                    values.append(item.rstrip())
                data.append(values)
                col_count = len(col)
                continue
            for i in range(len(col)):
                d = None
                if (i == 0) and (dateformat != "no date"):
                    try:
                        if dateformat == "cdb":
                            d = datetime.datetime.fromtimestamp(float(col[i]))
                        else:
                            d = datetime.datetime.strptime(col[i], dateformat)
                        if from_date is not None:
                            if d < from_date:
                                dont_write = True
                        if to_date is not None:
                            if d >= to_date:
                                dont_write = True
                    except:
                        #print "Problem with Date: ",col
                        dont_write = True
                else:
                    try:
                        d = np.float64(col[i])
                    except:
                        d = "-"
                if dont_write:
                    break
                else:
                    values.append(d)
            if len(values) == col_count and dont_write is False:
                data.append(values)
        f.close()

        self.data = np.array(data, dtype=object)
        del data

    def write_cdb_file(self, filename):
        self.write_csv_file(filename=filename, dateformat='cdb', spacer=',')

    def write_xno_file(self, filename):
        self.write_csv_file(filename=filename, dateformat='%Y-%m-%d %H:%M:%S', spacer=',')

    def write_csv_file(self, filename, dateformat='%Y-%m-%d %H:%M:%S', spacer=','):
        lines = self.data
        f = open(filename, 'w')
        for line in lines:
            f.write(spacer.join(map(lambda x: self.__to_str(x, dateformat), line)) + "\n")
        f.close()
        del lines


    # example: new_data = data.get_array_with_condition(data["Stations_id"]==3)
    def get_array_with_condition(self,condition):
        return xpn_data(self.data[np.concatenate(([True], condition))])

    # numpy array with headers
    def get_header(self):
        return self.data[0, :]

    # get data values (without header)
    def get_data(self):
        return self.data[1:, :]

    def get_column(self, col, with_header=True, remove_nan=False):
        start_row = 0
        if with_header is False:
            start_row = 1
        if isinstance(col, int):
            if remove_nan:
                return self.remove_nan(self.data[start_row:, col])
            else:
                return self.data[start_row:, col]
        else:
            return self.data[1:, list(self.data[0, :]).index(col)]

    def set_column(self, col, arr, with_header=True):
        start_row = 0
        if with_header is False:
            start_row = 1
        if isinstance(col, int):
            self.data[start_row:, col] = arr
        else:
            self.data[1:, list(self.data[0, :]).index(col)]=arr

    def get_columns(self, keys, with_header=True, remove_nan=False):
        start_row = 0
        if with_header is False:
            start_row = 1
        if isinstance(keys, type(())) is False and isinstance(keys, list) is False and isinstance(keys,
                                                                                                  np.ndarray) is False:
            keys = keys,
        indices = np.zeros(len(keys), dtype=int)
        for i in xrange(len(keys)):
            if isinstance(keys[i], int):
                indices[i] = keys[i]
            else:
                indices[i] = list(self.data[0, :]).index(keys[i])
        if remove_nan:
            return self.remove_nan(self.data[start_row:, indices])
        else:
            return self.data[start_row:, indices]

    # rounds the first col
    # ex: 60*60 --> hourly
    #     60*30 --> half hourly
    def round_date_and_time(self,roundTo=60*60):
        self.data[1:, 0] = np.array(map(lambda x: roundTime(x,roundTo), self.data[1:, 0]))

    def __getitem__(self, keys):
        if isinstance(keys, type(())) is False:
            return self.get_column(keys, with_header=False, remove_nan=True)
        else:
            return self.get_columns(keys, with_header=False, remove_nan=True).transpose()

    def __setitem__(self,keys,arr):
        if isinstance(keys, type(())) is False:
            self.set_column(keys, arr,with_header=False)
        else:
            print "not implemented"


    @staticmethod
    def get_common_header_masks(data1, data2):
        dt1 = data1[0, :]
        dt2 = data2[0, :]
        mask1, mask2 = xpn_data.in1d_special(dt1, dt2)
        return mask1, mask2

    @staticmethod
    def get_common_header(data1, data2):
        mask1, mask2 = xpn_data.get_common_header_masks(data1, data2)
        return data1[mask1]


    # simular to numpy.in1d, but use quicksort and returns mask1 and mask2 as result
    @staticmethod
    def in1d_special(ar1, ar2):
        if type(ar1[0]) is datetime.datetime:
            return xpn_data.in1d_date(ar1,ar2)
        ar = np.concatenate((ar1, ar2))
        # We need this to be a stable sort, so always use 'mergesort'
        # here. The values from the first array should always come before
        # the values from the second array.
        order = ar.argsort(kind='quicksort')
        #order = np.lexsort((ar,))
        sar = ar[order]
        equal_adj = (sar[1:] == sar[:-1])
        flag1 = np.concatenate((equal_adj, [False]))
        flag2 = np.concatenate(([False], equal_adj))
        index = order.argsort(kind='mergesort')

        index1 = index[:len(ar1)]
        index2 = index[len(ar1):]

        return flag1[index1], flag2[index2]

    @staticmethod
    def in1d_date(time1,time2,assume_unique=False, invert=False):
        """
        Test whether each element of a 1-D array (type: datetime) is also present in a second array.
        Returns a boolean array the same length as `ar1` that is True
        where an element of `ar1` is in `ar2` and False otherwise.
        Parameters
        ----------
        ar1 : (M,) array_like
            Input array.
        ar2 : array_like
            The values against which to test each value of `ar1`.
        assume_unique : bool, optional
            If True, the input arrays are both assumed to be unique, which
            can speed up the calculation.  Default is False.
        invert : bool, optional
            If True, the values in the returned array are inverted (that is,
            False where an element of `ar1` is in `ar2` and True otherwise).
            Default is False. ``np.in1d(a, b, invert=True)`` is equivalent
            to (but is faster than) ``np.invert(in1d(a, b))``.
            .. versionadded:: 1.8.0
        Returns
        -------
        in1d : (M,) ndarray, bool
            The values `ar1[in1d]` are in `ar2`.
        See Also
        --------
        numpy.lib.arraysetops : Module with a number of other functions for
                            performing set operations on arrays.
        """
        ar1 = np.array(map(matplotlib.dates.date2num, time1),dtype=float)
        ar2 = np.array(map(matplotlib.dates.date2num, time2),dtype=float)
        mask1 = np.in1d(ar1, ar2, assume_unique, invert)
        mask2 = np.in1d(ar2, ar1, assume_unique, invert)

        return mask1,mask2



    @staticmethod
    def get_statistics_of_pair(arg1, arg2):
        arg1 = arg1.astype(np.float)
        arg2 = arg2.astype(np.float)
        P_M_O = arg1-arg2
        P_M_O_Q = P_M_O*P_M_O
        N = len(P_M_O)
        if N == 0:
            return np.array([0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,0.0,0.0,0.0])
        N_1 = 1.0/N
        O_Mean = np.mean(arg2)

        P_M_O_2 = np.abs(arg1-O_Mean) + np.abs(arg2-O_Mean)
        P_M_O_3 = np.sum(P_M_O_2*P_M_O_2)


        MBE = N_1 * np.sum(P_M_O)
        RMSE = np.sqrt(N_1 * np.sum(P_M_O_Q))
        NRMSE = np.divide(RMSE, O_Mean)
        MAE = N_1 * np.sum(np.abs(P_M_O))
        ME = 1.0 - np.sum(P_M_O_Q) / P_M_O_3

        NSE = 1.0 - np.sum(P_M_O_Q)/np.sum((np.subtract(arg2, O_Mean))*(np.subtract(arg2, O_Mean)))

        # correlationskoefficient:
        Cor = np.corrcoef([arg1,arg2])[0,1]
        R_Mean = np.mean(arg1)
        Std1 = arg1.std(ddof=0)
        Std2 = arg2.std(ddof=0)
        #Cor = N_1 * np.sum((arg1-R_Mean)*(arg2-O_Mean))/(Std1*Std2)

        Std1 = np.sum((np.subtract(arg2, O_Mean))*(np.subtract(arg2, O_Mean)))
        Std2 = O_Mean

        return np.array([MBE, RMSE, NRMSE, MAE, ME, NSE,Cor,Std1,Std2,N],dtype=np.float)

    @staticmethod
    def get_statistic(arg1, arg2):
        header_mask1, header_mask2 = xpn_data.get_common_header_masks(arg1, arg2)
        data1 = arg1[:, header_mask1]
        data2 = arg2[:, header_mask2]

        var_count = len(data1[0])

        stat_matrix = np.empty((var_count, 11), dtype=object)

        stat_matrix[0, :] = np.array(["Variable", "MBE", "RMSE", "NRMSE", "MAE", "ME", "NSE","Cor","Std1","Std2","N"])

        for var_index in xrange(1, var_count):
            data1_pair = xpn_data(data1).get_columns((0, var_index), remove_nan=True, with_header=False)
            data2_pair = xpn_data(data2).get_columns((0, data1[0, var_index]), remove_nan=True, with_header=False)

            # mask for common time:

            #data1_pair_mask, data2_pair_mask = xpn_data.in1d_special(data1_pair[:, 0], data2_pair[:, 0])
            data1_pair_mask, data2_pair_mask = xpn_data.in1d_date(data1_pair[:, 0], data2_pair[:, 0])

            #print data1_pair[data1_pair_mask, 1]
            #print data1_pair[data1_pair_mask, 1]
            stat_vector = xpn_data.get_statistics_of_pair(data1_pair[data1_pair_mask, 1], data2_pair[data2_pair_mask, 1])

            stat_matrix[var_index, :] = np.concatenate((np.array([data1[0, var_index]],dtype=object), stat_vector))
            #data2_pair[:, 0]


        return stat_matrix



        #print data2





class xpn_csv():
    def __init__(self, data=None, spacer=',', silent=False, merge_delimiters=False,encoding='utf-8'):
        self.silent = silent
        filename = None
        if isinstance(data, np.ndarray):
                self.data = data
        elif isinstance(data, list):
                self.data = np.array(data)
        else:
            self.data = None
            filename = data
        if filename is not None:
                self.read_csv_file(filename, spacer, merge_delimiters,encoding)

    def __del__(self):
        del self.data

    # for example: met_data.convert_col(("lat","lon","grid_lat","grid_lon"),float)
    def convert_col(self,keys,cmd):
        if isinstance(keys, type(())) is False and isinstance(keys, list) is False and isinstance(keys,
                                                                                                  np.ndarray) is False:
            keys = keys,
        indices = np.zeros(len(keys), dtype=int)
        for i in xrange(len(keys)):
            if isinstance(keys[i], int):
                indices[i] = keys[i]
            else:
                indices[i] = list(self.data[0, :]).index(keys[i])

        for i in indices:
            self.data[1:,i] = map(lambda x : cmd(x), self.data[1:,i])

        del indices


    # axis: 0 --> rows, 1: --> col
    def get_size(self,axis=0):
        return np.size(self.data,axis)

    # numpy array with headers
    def get_header(self):
        return self.data[0, :]

    # axis: 0 --> rows, 1: --> col
    def size(self,axis=0):
        return np.size(self.data,axis)

    # return np.array
    def add_col(self,number=1,cols_to_add=None,overwrite_data=False):
        data_new = np.resize(self.data.transpose(),(np.size(self.data,1)+number,np.size(self.data,0))).transpose()
        if cols_to_add is not None:
            data_new[:,-(number):] = cols_to_add
        if overwrite_data:
            del self.data
            self.data = data_new
        return data_new


    #Liesst *.csv file
    # return [header,data]
    # header: 1d array mit den Headernamen
    # data: 2d array mit Daten, dabei ist die erste Spalte mit den Datumswerten im Datetime Format
    def read_csv_file(self, filename, spacer=',', merge_delimiters=False,encoding='utf-8'):
        del self.data
        self.data = None
        f = codecs.open(filename, 'rU',encoding)
        count = 0
        data = []
        col_count = 0
        for line in f:
            line = line.encode('utf-8')
            values = []
            dont_write = False
            if merge_delimiters:
                line = self.__rem_more_then_one_ch(line, spacer)
            col = line.split(spacer)
            count += 1
            if count == 1:
                for item in col:
                    values.append(item.rstrip().lstrip())
                data.append(values)
                col_count = len(col)
                continue
            for i in range(len(col)):
                d = col[i]
                values.append(d)
            if len(values) == col_count and dont_write is False:
                data.append(values)
        f.close()

        self.data = np.array(data, dtype=object)
        del data

    def set_column(self, col, arr, with_header=True):
        start_row = 0
        if with_header is False:
            start_row = 1
        if isinstance(col, int):
            self.data[start_row:, col] = arr
        else:
            self.data[1:, list(self.data[0, :]).index(col)]=arr

    def get_column(self, col, with_header=True, remove_nan=False):
        start_row = 0
        if with_header is False:
            start_row = 1
        if isinstance(col, int):
            if remove_nan:
                return self.remove_nan(self.data[start_row:, col])
            else:
                return self.data[start_row:, col]
        else:
            return self.data[1:, list(self.data[0, :]).index(col)]

    def get_columns(self, keys, with_header=True, remove_nan=False):
        start_row = 0
        if with_header is False:
            start_row = 1
        if isinstance(keys, type(())) is False and isinstance(keys, list) is False and isinstance(keys,
                                                                                                  np.ndarray) is False:
            keys = keys,
        indices = np.zeros(len(keys), dtype=int)
        for i in xrange(len(keys)):
            if isinstance(keys[i], int):
                indices[i] = keys[i]
            else:
                indices[i] = list(self.data[0, :]).index(keys[i])
        if remove_nan:
            return self.remove_nan(self.data[start_row:, indices])
        else:
            return self.data[start_row:, indices]

    def __getitem__(self, keys):
        if isinstance(keys, type(())) is False:
            return self.get_column(keys, with_header=False, remove_nan=False)
        else:
            return self.get_columns(keys, with_header=False, remove_nan=False).transpose()

    def __setitem__(self,keys,arr):
        if isinstance(keys, type(())) is False:
            self.set_column(keys, arr,with_header=False)
        else:
            print "not implemented"

    def index(self,index_str):
        return list(self.data[0, :]).index(index_str)

    # example: new_data = data.get_array_with_condition(data["Stations_id"]==3)
    def get_array_with_condition(self,condition):
        return xpn_csv(self.data[np.concatenate(([True], condition))])

    @staticmethod
    def __to_str(x, dateformat):
        if type(x) is datetime.datetime:
            if dateformat == 'cdb':
                return str(time.mktime(x.timetuple()))
            else:
                try:
                    return x.strftime(dateformat)
                except:
                    return x.isoformat(" ").split(".")[0]
        return str(x)

    def write_csv_file(self, filename, dateformat='%Y-%m-%d %H:%M:%S', spacer=','):
        lines = self.data
        f = open(filename, 'w')
        for line in lines:
            f.write(spacer.join(map(lambda x: self.__to_str(x, dateformat), line)) + "\n")
        f.close()
        del lines

    @staticmethod
    def __rem_more_then_one_ch(str1, ch1):
        a = ''
        last_is_space = False
        for ch in str1:
            if (ch == ch1) and (last_is_space is False):
                a = a + ch
                last_is_space = True
            if ch != ch1:
                a = a + ch
                last_is_space = False
        return a


def main(argv):
    #print get_distanance(9.463615, 50.615695, 14.418655, 50.551270)
    #coordinates = np.array([
    #    [9.858862713246934, 52.24148357918767],
    #    [9.859486005889513, 50.84519118924196],
    #    [11.6631815667536, 50.8377294606306],
    #    [11.70391076824869, 52.23009356842507]
    #])
    #print get_edges_lon_lat(coordinates)

    data = np.array([["Time and Date", "test", "test2"],
                     [datetime.datetime(year=2000, month=1, day=1), 0.5, 0.1],
                     [datetime.datetime(year=2000, month=1, day=2), 0.7, 0.1],
                     [datetime.datetime(year=2000, month=1, day=3), 0.7, 0.2],
                     [datetime.datetime(year=2000, month=1, day=5), 0.6, 0.15],
                     [datetime.datetime(year=2000, month=1, day=6), 0.7, 0.4]
    ])

    test_data = xpn_data("/home/chrikle/Documents/fluxnet_paper/fluxnet_ch/output/kartier/fluxnet_ch_1_107_51_0.xno")
    mess_data = xpn_data("/home/chrikle/Documents/fluxnet_paper/fluxnet_ch/fluxnet_ch_1_107_51_0_measurement.xno", from_date=datetime.datetime(year=2006, month=1 ,day=1), to_date=datetime.datetime(year=2006, month=12, day=31))

    #test_data.read_xno_file("/home/chrikle/Documents/fluxnet_paper/fluxnet_ch/output/kartier/fluxnet_ch_1_107_51_0.xno")
    #test_data.write_csv_file("/home/chrikle/Desktop/test.csv")
    #test_data = xpn_data(data)

    #a = test_data.get_columns(("Time and Date", 2), with_header=True)

    #b = xpn_data.get_data_from_time_range(a, datetime.datetime(year=2006,month=5,day=1),datetime.datetime(year=2006,month=5,day=3))

    #c = xpn_data(b).get_data()

    #startdate, stopdate = xpn_data.get_time_range(b)

    #dates = pd.DatetimeIndex(b[1:, 0])
    #print dates.year

    #day = datetime.datetime(year=2006,month=5,day=1)
    #filter = (c[:, 0] >= day) & (c[:, 0] < day+datetime.timedelta(days=1))

    #print c[filter]

    #test_data = xpn_data(test_data.get_columns(("Time and Date", 2, 3)))

    #header = xpn_data.get_common_header(test_data.data, mess_data.data)
    #data1 = test_data.get_columns(header, remove_nan=False)
    #data2 = mess_data.get_columns(header, remove_nan=True)


    #data1_d = data1[1:, :]
    #data2_d = data2[1:, :]
    #print data1_d
    #print data2_d

    #print ""



    #get_common_data(data1[1:, 0], data2[1:, 0])
    #index1, index2 = xpn_data.in1d_special(data1_d[:, 0], data2_d[:, 0])


    #print data1_d[:, 0], data2_d[:, 0]
    #print data1_d[index1]
    #print data2_d[index2, :]

    statistic = xpn_data.get_statistic(test_data.data, mess_data.data)

    print statistic

    #

    #erg = test_data.get_averaged_data(datetime.timedelta(days=1))


    #print erg

    #rows = len(self.ph_stat.data)
    #data_output_raw = np.empty((rows,len(header)),dtype=object)
    #data_output_raw[0] = header
    #data_output_raw[1:,0] = time_and_date


    return 0


if __name__ == '__main__':
    main(sys.argv)
