//      xylemwaterflow_connector.c
//      
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//      


#include "xylemwaterflow_connector.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

G_DEFINE_TYPE(xylemwaterflow_connector, xylemwaterflow_connector, EXPERTN_MODUL_BASE_TYPE);


static void xylemwaterflow_connector_class_init(xylemwaterflow_connectorClass *klass){}


static void xylemwaterflow_connector_init(xylemwaterflow_connector *self)
{}



// Our Modul Functions:

int xylemwaterflow_connector_load(xylemwaterflow_connector *self)
{
	expertn_modul_base *xpn = &(self->parent);
	self->xwf = c_xylemwaterflow_connector_class_new((void*)xpn);
		
	return c_xylemwaterflow_connector_class_load(self->xwf);
}

int xylemwaterflow_connector_global_run(xylemwaterflow_connector *self)
{
	return c_xylemwaterflow_connector_class_global_run(self->xwf);
}

int xylemwaterflow_connector_mosaic_run(xylemwaterflow_connector *self)
{
	return c_xylemwaterflow_connector_class_mosaic_run(self->xwf);
}

int xylemwaterflow_connector_run(xylemwaterflow_connector *self)
{
	return c_xylemwaterflow_connector_class_run(self->xwf);
}

int xylemwaterflow_connector_done(xylemwaterflow_connector *self)
{	
	int ret;
	ret = c_xylemwaterflow_connector_class_done(self->xwf);
	c_xylemwaterflow_connector_class_delete(self->xwf);
	return ret;
}
