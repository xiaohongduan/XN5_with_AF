#!/usr/bin/env python
# -*- coding: utf-8 -*-
#  Copyright 2014 Christian Klein <christian.klein@helmholtz-muenchen.de>
import sys

try:
    from iniparse import ConfigParser
except:
    from ConfigParser import ConfigParser
    
ConfigParser.optionxform = str

import cgeolib as cg
import numpy as np
import os
import stat
import matplotlib
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button, RadioButtons

ini_example = '''
[system]
# Schicht 2,3

# psi file im xno format: psi hat die Einheit [mm] und ist negeativ
psi_filename = ../xno_measurements/meas_pot_a17_30cm.xno

#Psi Spaltenname:
psi_col_name = output.Water.Matrix Potential.psi [mm] 30 cm



# theta file im xno format: theta hat die Einheit []
theta_filename = ../xno_measurements/meas_theta_a17_xpn_format_30cm.xno

# theta Spaltenname:
theta_col_name = output.Water.Water Content.theta [] 30 cm
        

[soil]
# Der letzte Wert wird immer genommen

#Feld Kapazität (WC at 33kPa=-330mm) [Vol%] (wird nur geschrieben):
field_capacity= 32.0;40.1043532681;36.5999379289;

# permanenter Welkepunkt (WC at 1500 kPa) [Vol%] (wird nur geschrieben):
wilting_point = 24.0;21.9104048453;18.5287556898;

# Modellgroesse Wasser Res [%]
res_water_cont = 7.462;18.1640625;14.8076923077;

# Modellgröße Van Genuchten Alpha
van_gen_a = 0.00866;0.00048828125;0.00048828125;

# Modellgröße Van Genuchten N
van_gen_n = 1.46006;1.8896484375;1.8896484375;

# Gesamtporenvolumen [Vol%]:
porosity =      38.468;40.4296875;36.9230769231;
'''


    
    
class combine_theta_and_psi:
    def __init__(self,ini_file):
        
        self.ini_file = ini_file
        self.read_ini_file(ini_file)        
        self.load_data()
        self.plot_data()
        
    def read_ini_file(self,ini_file):        
        config = ConfigParser()
        config.read(ini_file)
        self.psi_filename = config.get("system","psi_filename")
        self.theta_filename = config.get("system","theta_filename")
        self.psi_col_name = config.get("system","psi_col_name")
        self.theta_col_name = config.get("system","theta_col_name")
        
        
        self.alpha = float(config.get("soil","van_gen_a").split(";")[-2])
        self.n = float(config.get("soil","van_gen_n").split(";")[-2])
        self.theta_res = float(config.get("soil","res_water_cont").split(";")[-2])
        self.theta_sat = float(config.get("soil","porosity").split(";")[-2])
        
        del config
        
        
    def load_data(self):
        psi_data = cg.xpn_data(self.psi_filename)
        theta_data = cg.xpn_data(self.theta_filename)
        
        filter_psi,filter_theta = cg.xpn_data.in1d_special(psi_data["Time and Date"],theta_data["Time and Date"])
        
        self.psi = psi_data[self.psi_col_name][filter_psi]
        self.theta = theta_data[self.theta_col_name][filter_theta]*100.0
        
    def plot_data(self):
        self.fig, ax = plt.subplots(num=self.ini_file)
        plt.subplots_adjust(bottom=0.35)        
        ax.set_xscale('log')
        plt.plot(-1.0*self.psi,self.theta,".",label=r"")    
        
        #self.psi_c = np.arange(-10.0**5.0,0.0,0.1)
        self.psi_c = -np.logspace(8.0,0.0,num=1000)
        theta_c = self.calc_f(self.psi_c,self.n,self.alpha,self.theta_res,self.theta_sat)
        self.curve_plot, = plt.plot(-1.0*self.psi_c,theta_c,"-",label=r"")
            
        
        n_ax = plt.axes([0.1, 0.1, 0.8, 0.03], axisbg='lightgoldenrodyellow')
        self.n_sl = Slider(n_ax, 'n', 1.0, 2.0, valinit=self.n)
        self.n_sl.on_changed(self.update)
        
        alpha_ax = plt.axes([0.1, 0.15, 0.8, 0.03], axisbg='lightgoldenrodyellow')
        self.alpha_sl = Slider(alpha_ax, 'alpha * 10^(-4)', 0.0, 100.0, valinit=self.alpha*10.0**(4))
        self.alpha_sl.on_changed(self.update)
        
        
        theta_res_ax = plt.axes([0.1, 0.2, 0.8, 0.03], axisbg='lightgoldenrodyellow')
        self.theta_res_sl = Slider(theta_res_ax, 'theta_res', 0.0, 100.0, valinit=self.theta_res)
        self.theta_res_sl.on_changed(self.update)
        
        theta_sat_ax = plt.axes([0.1, 0.25, 0.8, 0.03], axisbg='lightgoldenrodyellow')
        self.theta_sat_sl = Slider(theta_sat_ax, 'theta_sat', 0.0, 100.0, valinit=self.theta_sat)
        self.theta_sat_sl.on_changed(self.update)
        
        saveax= plt.axes([0.8, 0.025, 0.1, 0.04])
        button = Button(saveax, 'Save', color='lightgoldenrodyellow', hovercolor='0.975')
        button.on_clicked(self.save_data)
        
        self.wc33kPa = self.calc_f(-330.0,self.n,self.alpha,self.theta_res,self.theta_sat)
        self.wc1500kPa = self.calc_f(-15000.0,self.n,self.alpha,self.theta_res,self.theta_sat)
        
        self.textwc33andwc1500 = self.fig.text(0.5, 0.03,'33 kPa WC (field capacity): %0.2f %%\n1500 kPa WC (permanent wilting point): %0.2f %%' % (self.wc33kPa,self.wc1500kPa),ha='center')

        
        
        plt.show()
        
    def calc_f(self,psi_c,n,alpha,theta_res,theta_sat):
        m = 1.0 - 1.0/n
        theta_c = theta_res + (theta_sat- theta_res) * (1.0 + (alpha * np.abs(psi_c))**n)**(-m)
        return theta_c
        
    def update(self,val):
        self.n = self.n_sl.val
        self.alpha = self.alpha_sl.val*10.0**(-4)
        self.theta_res = self.theta_res_sl.val
        self.theta_sat = self.theta_sat_sl.val
        theta_c = self.calc_f(self.psi_c,self.n,self.alpha,self.theta_res,self.theta_sat)
        self.curve_plot.set_ydata(theta_c)
        
        self.wc33kPa = self.calc_f(-330.0,self.n,self.alpha,self.theta_res,self.theta_sat)
        self.wc1500kPa = self.calc_f(-15000.0,self.n,self.alpha,self.theta_res,self.theta_sat)      
        self.textwc33andwc1500.set_text('33 kPa WC (field capacity): %0.2f %%\n1500 kPa WC (permanent wilting point): %0.2f %%' % (self.wc33kPa,self.wc1500kPa))
        self.fig.canvas.draw_idle()
        
    def save_data(self,event):
        print "Save Values"
        config = ConfigParser()
        config.read(self.ini_file)
        config.set("soil","van_gen_a",config.get("soil","van_gen_a")+str(self.alpha)+";")
        config.set("soil","van_gen_n",config.get("soil","van_gen_n")+str(self.n)+";")
        config.set("soil","res_water_cont",config.get("soil","res_water_cont")+str(self.theta_res)+";")
        config.set("soil","porosity",config.get("soil","porosity")+str(self.theta_sat)+";")
        config.set("soil","field_capacity",config.get("soil","field_capacity")+str(self.wc33kPa)+";")
        config.set("soil","wilting_point",config.get("soil","wilting_point")+str(self.wc1500kPa)+";")
        configfile = open(self.ini_file, 'wb')
        config.write(configfile)
        configfile.close()
        del config
        
    


def main(argv):
    
    if len(argv) < 2:
        print "No INI file"
        print "python ret_van_genuchten.py file.ini"
        print "Syntax: %s" % (ini_example)
    else:
        combine_theta_and_psi(argv[1])


    
    #combine_theta_and_psi("a17_10cm.ini")
    #combine_theta_and_psi("a17_30cm.ini")
    #combine_theta_and_psi("a17_50cm.ini")
    
    #combine_theta_and_psi("a18_10cm.ini")
    #combine_theta_and_psi("a18_30cm.ini")
    #combine_theta_and_psi("a18_50cm.ini")
    #plt.show()

    return 0

if __name__ == '__main__':
    main(sys.argv)
    




