//      xylemwaterflow_connector.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __XYLEMWATERFLOW_CONNECTOR_H__
#define __XYLEMWATERFLOW_CONNECTOR_H__

#include <expertn_modul_base.h>
#include "xylemwaterflow_connector_class_wrapper.h"

G_BEGIN_DECLS

#define XYLEMWATERFLOW_CONNECTOR_TYPE				(xylemwaterflow_connector_get_type())
#define XYLEMWATERFLOW_CONNECTOR(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			XYLEMWATERFLOW_CONNECTOR_TYPE, xylemwaterflow_connector))
#define XYLEMWATERFLOW_CONNECTOR_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			XYLEMWATERFLOW_CONNECTOR_TYPE, xylemwaterflow_connectorClass))
#define IS_XYLEMWATERFLOW_CONNECTOR(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			XYLEMWATERFLOW_CONNECTOR_TYPE))
#define IS_XYLEMWATERFLOW_CONNECTOR_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			XYLEMWATERFLOW_CONNECTOR_TYPE))

typedef struct _xylemwaterflow_connector			xylemwaterflow_connector;
typedef struct _xylemwaterflow_connectorClass		xylemwaterflow_connectorClass;

struct _xylemwaterflow_connector
{
	expertn_modul_base parent;
	/* add your public declarations here */
	c_xylemwaterflow_connector_class *xwf;
};


// public class member function:
G_MODULE_EXPORT int xylemwaterflow_connector_load(xylemwaterflow_connector *self);
G_MODULE_EXPORT int xylemwaterflow_connector_global_run(xylemwaterflow_connector *self);
G_MODULE_EXPORT int xylemwaterflow_connector_mosaic_run(xylemwaterflow_connector *self);
G_MODULE_EXPORT int xylemwaterflow_connector_run(xylemwaterflow_connector *self);
G_MODULE_EXPORT int xylemwaterflow_connector_done(xylemwaterflow_connector *self);

struct _xylemwaterflow_connectorClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType xylemwaterflow_connector_get_type (void);

G_END_DECLS

#endif /* __XYLEMWATERFLOW_CONNECTOR_H__ */

