#ifndef __XYLEMWATERFLOW_CONNECTOR_CLASS_WRAPPER_H__
#define __XYLEMWATERFLOW_CONNECTOR_CLASS_WRAPPER_H__

typedef void c_xylemwaterflow_connector_class;

#ifdef __cplusplus
extern "C" {
#endif

#include <expertn_modul_base.h>

c_xylemwaterflow_connector_class * c_xylemwaterflow_connector_class_new(expertn_modul_base *xpn);
int c_xylemwaterflow_connector_class_load(const c_xylemwaterflow_connector_class *self);
int c_xylemwaterflow_connector_class_global_run(const c_xylemwaterflow_connector_class *self);
int c_xylemwaterflow_connector_class_mosaic_run(const c_xylemwaterflow_connector_class *self);
int c_xylemwaterflow_connector_class_run(const c_xylemwaterflow_connector_class *self);
int c_xylemwaterflow_connector_class_done(const c_xylemwaterflow_connector_class *self);
void c_xylemwaterflow_connector_class_delete(const c_xylemwaterflow_connector_class *self);
#ifdef __cplusplus
}
#endif

#endif // __XYLEMWATERFLOW_CONNECTOR_CLASS_WRAPPER_H__
