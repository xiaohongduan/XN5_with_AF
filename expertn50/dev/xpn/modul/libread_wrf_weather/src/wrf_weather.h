//      wrf_weather.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __WRF_WEATHER_H__
#define __WRF_WEATHER_H__

#include <expertn_modul_base.h>
#include <expertn_noah_driver.h>

G_BEGIN_DECLS

#define WRF_WEATHER_TYPE				(wrf_weather_get_type())
#define WRF_WEATHER(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			WRF_WEATHER_TYPE, wrf_weather))
#define WRF_WEATHER_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			WRF_WEATHER_TYPE, wrf_weatherClass))
#define IS_WRF_WEATHER(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			WRF_WEATHER_TYPE))
#define IS_WRF_WEATHER_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			WRF_WEATHER_TYPE))

typedef struct _wrf_weather			wrf_weather;
typedef struct _wrf_weatherClass		wrf_weatherClass;

typedef struct
{
	void* pointer;
	int vartype;
} struct_connection;

struct _wrf_weather
{
	expertn_modul_base parent;
	/* add your public declarations here */
	char **header;
	double **valuelist;
	int size_of_values;
	int size_of_valuelist;	
	struct_connection *connection;
	struct_WRF *wrf;
	int sim_start;
	int counter;
	int wrf_is_driver;
};


// public class member function:
G_MODULE_EXPORT int wrf_weather_load(wrf_weather *self);
G_MODULE_EXPORT int wrf_weather_run(wrf_weather *self);
G_MODULE_EXPORT int wrf_weather_done(wrf_weather *self);

G_MODULE_EXPORT int wrf_weather_coupling_load(wrf_weather *self);
G_MODULE_EXPORT int wrf_weather_coupling_run(wrf_weather *self);
G_MODULE_EXPORT int wrf_weather_coupling_done(wrf_weather *self);


struct _wrf_weatherClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType wrf_weather_get_type (void);

G_END_DECLS

#endif /* __WRF_WEATHER_H__ */

