#ifndef __GECROS__TOOLS_H__
#define __GECROS__TOOLS_H__

#include <expertn_modul_base.h>

G_BEGIN_DECLS

double AFGENERATOR(double fInput, RESPONSE* pfResp);
double FCNSW(double x, double v1, double v2, double v3);
double LIMIT(double v1, double v2, double x);
double NOTNUL(double x);
double INSW(double x, double y1, double y2);
double REAAND(double x, double y);
double REANOR(double x, double y);

double  abspowerDBL(double x, double y);

GDate *gecros_convert_str_to_gdate(char* S);

G_END_DECLS

#endif /* __GECROS_H__ */
