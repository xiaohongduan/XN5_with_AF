//      xpn_pedotransfer.c
//      
//      Copyright 2010 Christian Klein <chrikle@berlios.de>
//      


#include "xpn_pedotransfer.h"
#include "xpn_pedotransfer_campbell.h"
#include "xpn_pedotransfer_vereecken.h"
#include "xpn_pedotransfer_RawlsBrakensiek.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

G_DEFINE_TYPE(xpn_pedotransfer, xpn_pedotransfer, EXPERTN_MODUL_BASE_TYPE);


static void xpn_pedotransfer_class_init(xpn_pedotransferClass *klass){}


static void xpn_pedotransfer_init(xpn_pedotransfer *self)
{}


int  xpn_pedotransfer_set_pedotransfer(xpn_pedotransfer *self,void *pedo)
  {
	  xpn_register_var_add_none(self->parent.pXSys->var_list,"xpn_pedotransfer.function",pedo,-1,TRUE);	  
	  return RET_SUCCESS;
  }


int xpn_pedotransfer_set_campbell(xpn_pedotransfer *self)
{
	xpn_pedotransfer_set_pedotransfer(self,&xpn_pedotransfer_campbell);
	return RET_SUCCESS;
}


int xpn_pedotransfer_set_Vereecken(xpn_pedotransfer *self)
{
	xpn_pedotransfer_set_pedotransfer(self,&xpn_pedotransfer_Vereecken);
	return RET_SUCCESS;
}

int xpn_pedotransfer_set_RawlsBrakensiek(xpn_pedotransfer *self)
{
	xpn_pedotransfer_set_pedotransfer(self,&xpn_pedotransfer_RawlsBrakensiek);
	return RET_SUCCESS;
}
