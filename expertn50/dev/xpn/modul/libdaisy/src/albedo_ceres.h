//      albedo_ceres.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __ALBEDO_CERES_H__
#define __ALBEDO_CERES_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define ALBEDO_CERES_TYPE               (albedo_ceres_get_type())
#define ALBEDO_CERES(obj)               (G_TYPE_CHECK_INSTANCE_CAST((obj),\
            ALBEDO_CERES_TYPE, albedo_ceres))
#define ALBEDO_CERES_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST((klass),\
            ALBEDO_CERES_TYPE, albedo_ceresClass))
#define IS_ALBEDO_CERES(obj)            (G_TYPE_CHECK_INSTANCE_TYPE((obj),\
            ALBEDO_CERES_TYPE))
#define IS_ALBEDO_CERES_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE((klass),\
            ALBEDO_CERES_TYPE))

typedef struct _albedo_ceres            albedo_ceres;
typedef struct _albedo_ceresClass       albedo_ceresClass;

struct _albedo_ceres
{
    expertn_modul_base parent;
    /* add your public declarations here */
    
};


// public class member function:
G_MODULE_EXPORT int albedo_ceres_Water(albedo_ceres *self);
G_MODULE_EXPORT int albedo_ceres_Vegetation(albedo_ceres *self);
G_MODULE_EXPORT int albedo_ceres_pasture(albedo_ceres *self);



struct _albedo_ceresClass
{
    expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType albedo_ceres_get_type (void);

G_END_DECLS

#endif /* __ALBEDO_CERES_H__ */

