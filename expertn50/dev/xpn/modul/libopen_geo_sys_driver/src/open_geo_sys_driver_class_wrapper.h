#ifndef __OPEN_GEO_SYS_DRIVER_CLASS_WRAPPER_H__
#define __OPEN_GEO_SYS_DRIVER_CLASS_WRAPPER_H__

typedef void c_open_geo_sys_driver_class;

#ifdef __cplusplus
extern "C" {
#endif

c_open_geo_sys_driver_class * c_open_geo_sys_driver_class_new(void *xpn);
int c_open_geo_sys_driver_class_load(const c_open_geo_sys_driver_class *self);
int c_open_geo_sys_driver_class_run_global(const c_open_geo_sys_driver_class *self);
int c_open_geo_sys_driver_class_run_mosaic(const c_open_geo_sys_driver_class *self);
int c_open_geo_sys_driver_class_run(const c_open_geo_sys_driver_class *self);
int c_open_geo_sys_driver_class_done(const c_open_geo_sys_driver_class *self);
void c_open_geo_sys_driver_class_delete(const c_open_geo_sys_driver_class *self);
#ifdef __cplusplus
}
#endif

#endif
