//      open_geo_sys_driver.c
//      
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//      


#include "open_geo_sys_driver.h"
#include <string.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

G_DEFINE_TYPE(open_geo_sys_driver, open_geo_sys_driver, EXPERTN_MODUL_BASE_TYPE);


static void open_geo_sys_driver_class_init(open_geo_sys_driverClass *klass){}


static void open_geo_sys_driver_init(open_geo_sys_driver *self)
{
}



// Our Modul Functions:

int open_geo_sys_driver_load(open_geo_sys_driver *self)
{
	expertn_modul_base *xpn = &(self->parent);
	self->ogs = c_open_geo_sys_driver_class_new((void*)xpn);
	return c_open_geo_sys_driver_class_load(self->ogs);
}

int open_geo_sys_driver_run_global(open_geo_sys_driver *self)
{
	return c_open_geo_sys_driver_class_run_global(self->ogs);
}

int open_geo_sys_driver_run_mosaic(open_geo_sys_driver *self)
{
	return c_open_geo_sys_driver_class_run_mosaic(self->ogs);
}

int open_geo_sys_driver_run(open_geo_sys_driver *self)
{
	return c_open_geo_sys_driver_class_run(self->ogs);
}

int open_geo_sys_driver_done(open_geo_sys_driver *self)
{
	int ret;
	ret = c_open_geo_sys_driver_class_done(self->ogs);
	c_open_geo_sys_driver_class_delete(self->ogs);
	return ret;
}

