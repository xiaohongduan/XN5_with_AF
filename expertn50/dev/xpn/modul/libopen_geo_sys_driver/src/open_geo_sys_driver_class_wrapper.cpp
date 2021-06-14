#include "ogs_class.h"
#include "open_geo_sys_driver_class_wrapper.h"

extern "C" {

c_open_geo_sys_driver_class * c_open_geo_sys_driver_class_new(void *xpn)
{
	OGS_CLASS *self;
	self = new OGS_CLASS(xpn);
	return (c_open_geo_sys_driver_class *)self;
}

int c_open_geo_sys_driver_class_load(const c_open_geo_sys_driver_class *self)
{
	OGS_CLASS *s = (OGS_CLASS*)self;
	return s->load();
}

int c_open_geo_sys_driver_class_run_global(const c_open_geo_sys_driver_class *self)
{
	OGS_CLASS *s = (OGS_CLASS*)self;
	return s->run_global();
}

int c_open_geo_sys_driver_class_run_mosaic(const c_open_geo_sys_driver_class *self)
{
	OGS_CLASS *s = (OGS_CLASS*)self;
	return s->run_mosaic();
}

int c_open_geo_sys_driver_class_run(const c_open_geo_sys_driver_class *self)
{
	OGS_CLASS *s = (OGS_CLASS*)self;
	return s->run();
}

int c_open_geo_sys_driver_class_done(const c_open_geo_sys_driver_class *self)
{
	OGS_CLASS *s = (OGS_CLASS*)self;
	return s->done();
}

void c_open_geo_sys_driver_class_delete(const c_open_geo_sys_driver_class *self)
{
	OGS_CLASS *s = (OGS_CLASS*)self;
	delete s;
}

}
