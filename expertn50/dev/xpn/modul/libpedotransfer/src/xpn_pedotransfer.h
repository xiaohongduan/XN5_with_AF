//      xpn_pedotransfer.h
//      
//      Copyright 2010 Christian Klein <chrikle@berlios>
//      



#ifndef __XPN_PEDOTRANSFER_H__
#define __XPN_PEDOTRANSFER_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

#define XPN_PEDOTRANSFER_TYPE				(xpn_pedotransfer_get_type())
#define XPN_PEDOTRANSFER(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj),\
			XPN_PEDOTRANSFER_TYPE, xpn_pedotransfer))
#define XPN_PEDOTRANSFER_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),\
			XPN_PEDOTRANSFER_TYPE, xpn_pedotransferClass))
#define IS_XPN_PEDOTRANSFER(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),\
			XPN_PEDOTRANSFER_TYPE))
#define IS_XPN_PEDOTRANSFER_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass),\
			XPN_PEDOTRANSFER_TYPE))

typedef struct _xpn_pedotransfer			xpn_pedotransfer;
typedef struct _xpn_pedotransferClass		xpn_pedotransferClass;

struct _xpn_pedotransfer
{
	expertn_modul_base parent;
	/* add your public declarations here */
	double Plant_Mass;
};


// public class member function:

int  xpn_pedotransfer_set_pedotransfer(xpn_pedotransfer *self,void *pedo);

G_MODULE_EXPORT int xpn_pedotransfer_set_campbell(xpn_pedotransfer *self);
G_MODULE_EXPORT int xpn_pedotransfer_set_RawlsBrakensiek(xpn_pedotransfer *self);
G_MODULE_EXPORT int xpn_pedotransfer_set_Vereecken(xpn_pedotransfer *self);

struct _xpn_pedotransferClass
{
	expertn_modul_baseClass parent_class;
};

G_MODULE_EXPORT GType xpn_pedotransfer_get_type (void);

G_END_DECLS

#endif /* __XPN_PEDOTRANSFER_H__ */

