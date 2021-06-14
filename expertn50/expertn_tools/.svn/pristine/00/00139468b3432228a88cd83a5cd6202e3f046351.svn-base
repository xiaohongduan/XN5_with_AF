#!/usr/bin/env python
# -*- coding: utf-8 -*-
import numpy as np


class xpnstatistic:
    def __init__(self,data1,data2):
        self.MBE=0.0
        self.RMSE=0.0
        self.NRMSE=0.0
        self.MAE=0.0
        self.ME=0.0
        self.E=0.0

        self.calc_statistics(data1[0],data1[1],data2[0],data2[1])

    def time_compare(self,tim1,tim2):
        if ((tim1.year==tim2.year) and (tim1.month==tim2.month) and (tim1.day==tim2.day) and (tim1.hour==tim2.hour) and (tim1.minute==tim2.minute) and (tim1.second==tim2.second)):
                return True
        else:
                return False


    def calc_statistic_get_values(self,time1,val1,time2,val2):
        time_arr = np.array([])
        values_arr = np.array([])
        mess_arr = np.array([])
        i3=0
        for i in range(len(time1)):
            for i2 in range(i3,len(time2)):
               if self.time_compare(time1[i],time2[i2]):
                   i3=i2+1
                   time_arr  = np.append(time_arr,time1[i])
                   values_arr = np.append(values_arr,val1[i])
                   mess_arr = np.append(mess_arr,val2[i2])
                   break
        return [time_arr,values_arr,mess_arr]

    def print_statistic(self):
        print "MBE: ",self.MBE
        print "RMSE: ",self.RMSE
        print "MAE: ",self.MAE
        print "NRMSE: ",self.NRMSE
        print "ME: ",self.ME
        print "NSE: ",self.NSE
        print "N: ",self.N

    def print_statistic_to_S(self):
        S =  "MBE: %f\n" % self.MBE
        S += "RMSE: %f\n" % self.RMSE
        S += "MAE: %f\n" % self.MAE
        S += "NRMSE: %f\n" % self.NRMSE
        S += "IA/ME: %f\n" % self.ME
        S += "NSE: %f\n" % self.NSE
        S += "N: %d\n" % self.N
        return S

    def calc_statistics(self,time1,val1,time2,val2):
        [time_arr,values_arr,mess_arr] = self.calc_statistic_get_values(time1,val1,time2,val2)
        # p= predicted, O = observed
        # N Number of Values
        # MBE = Mean Bias Error
        # RMSE = Root Mean Square Error
        # Mean Average Error
        # ME Model efficiency Index
        # NRMSE Normalized Root Mean Square Error
        #print values_arr
        #print mess_arr
        P_M_O = values_arr-mess_arr
        P_M_O_Q = P_M_O*P_M_O
        N = len(P_M_O)
        N_1 = 1.0/N
        O_Mean = np.mean(mess_arr)
        P_M_O_2 = np.abs(values_arr-O_Mean) + np.abs(mess_arr-O_Mean)
        P_M_O_3 = np.sum(P_M_O_2*P_M_O_2)


        self.MBE = N_1 * np.sum(P_M_O)
        self.RMSE = np.sqrt(N_1 * np.sum(P_M_O_Q))
        self.NRMSE = self.RMSE / O_Mean
        self.MAE = N_1 * np.sum(np.abs(P_M_O))
        self.ME = 1.0 - np.sum(P_M_O_Q) / P_M_O_3
        self.NSE = 1.0 - np.sum(P_M_O_Q)/np.sum((mess_arr-O_Mean)*(mess_arr-O_Mean))
        self.N = N


        #print "MBE: ",MBE
        #print "RMSE: ",RMSE
        #print "MAE: ",MAE
        #print "NRMSE: ",NRMSE
        #print "ME: ",ME
        #print "E: ",E
