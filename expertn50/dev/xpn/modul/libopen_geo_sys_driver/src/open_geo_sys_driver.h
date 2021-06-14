//      open_geo_sys_driver.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __OPEN_GEO_SYS_DRIVER_H__
#define __OPEN_GEO_SYS_DRIVER_H__

#include <expertn_modul_base.h>
#include "open_geo_sys_driver_class_wrapper.h"

G_BEGIN_DECLS

#define OPEN_GEO_SYS_DRIVER_TYPE                (open_geo_sys_driver_get_type())
#define OPEN_GEO_SYS_DRIVER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj),\
            OPEN_GEO_SYS_DRIVER_TYPE, open_geo_sys_driver))
#define OPEN_GEO_SYS_DRIVER_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST((klass),\
            OPEN_GEO_SYS_DRIVER_TYPE, open_geo_sys_driverClass))
#define IS_OPEN_GEO_SYS_DRIVER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj),\
            OPEN_GEO_SYS_DRIVER_TYPE))
#define IS_OPEN_GEO_SYS_DRIVER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),\
            OPEN_GEO_SYS_DRIVER_TYPE))

typedef struct _open_geo_sys_driver         open_geo_sys_driver;
typedef struct _open_geo_sys_driverClass        open_geo_sys_driverClass;

struct _open_geo_sys_driver
{
    expertn_modul_base parent;
    /* add your public declarations here */
        
    // OGS Class
    c_open_geo_sys_driver_class *ogs;
};


// public class member function:
G_MODULE_EXPORT int open_geo_sys_driver_load(open_geo_sys_driver *self);
G_MODULE_EXPORT int open_geo_sys_driver_run_global(open_geo_sys_driver *self);
G_MODULE_EXPORT int open_geo_sys_driver_run_mosaic(open_geo_sys_driver *self);
G_MODULE_EXPORT int open_geo_sys_driver_run(open_geo_sys_driver *self);
G_MODULE_EXPORT int open_geo_sys_driver_done(open_geo_sys_driver *self);

struct _open_geo_sys_driverClass
{
    expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType open_geo_sys_driver_get_type (void);

G_END_DECLS

#endif /* __OPEN_GEO_SYS_DRIVER_H__ */

