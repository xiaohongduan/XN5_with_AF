#include "xylemwaterflow.h"
#include "xylemwaterflow_connector_class_wrapper.h"

extern "C" {

c_xylemwaterflow_connector_class * c_xylemwaterflow_connector_class_new(expertn_modul_base *xpn)
{
	XWF_CLASS *self;
	self = new XWF_CLASS(xpn);
	return (c_xylemwaterflow_connector_class *)self;
}

int c_xylemwaterflow_connector_class_load(const c_xylemwaterflow_connector_class *self)
{
	XWF_CLASS *s = (XWF_CLASS*)self;
	return s->load();
}

int c_xylemwaterflow_connector_class_global_run(const c_xylemwaterflow_connector_class *self)
{
	XWF_CLASS *s = (XWF_CLASS*)self;
	return s->run_global();
}

int c_xylemwaterflow_connector_class_mosaic_run(const c_xylemwaterflow_connector_class *self)
{
	XWF_CLASS *s = (XWF_CLASS*)self;
	return s->run_mosaic();
}

int c_xylemwaterflow_connector_class_run(const c_xylemwaterflow_connector_class *self)
{
	XWF_CLASS *s = (XWF_CLASS*)self;
	return s->run();
}

int c_xylemwaterflow_connector_class_done(const c_xylemwaterflow_connector_class *self)
{
	XWF_CLASS *s = (XWF_CLASS*)self;
	return s->done();
}

void c_xylemwaterflow_connector_class_delete(const c_xylemwaterflow_connector_class *self)
{
	XWF_CLASS *s = (XWF_CLASS*)self;
	delete s;
}

}
