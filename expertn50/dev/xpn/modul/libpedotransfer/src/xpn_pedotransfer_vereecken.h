#ifndef __XPN_PEDOTRANSFER_VEREECKEN_H__
#define __XPN_PEDOTRANSFER_VEREECKEN_H__
#include <glib.h>
#include <gmodule.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

G_BEGIN_DECLS

G_MODULE_EXPORT double xpn_pedotransfer_Vereecken(double,double,double,double,double,double, int);

G_END_DECLS

#endif
