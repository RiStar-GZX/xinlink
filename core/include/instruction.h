#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <type.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

XLins * ins_create3(int num,...);
XLins * ins_create(const char *op_name,const char * format,...);
int message(XLpak_ins *pak_ins,const char * op_name,uint num,...);
uint8_t * arg_type_xlll_to_data(XLll * ll,uint * data_size);
XLll * ll_to_arg_type_xlll(uint8_t* data);
int ins_add_arg(XLins *ins,const char * name,uint type,void * data);
int ins_del_arg(XLins * ins,const char * name);
void ins_printf(XLins * ins);
int INS_cpy(XLins * dect,XLins * src);
int ins_send_to_event(XLpak_ins * ins);

mon_id_t monitor_create(XLsource * receiver);
void monitor_show(void);
int monitor_remove(mon_id_t id);
XLmonitor * monitor_get_by_id(mon_id_t id);
//XLpak_ins * monitor_get_member(mon_id_t monitor_id);
XLqueue * monitor_get_queue(mon_id_t monitor_id);
XLqueue * monitor_get_queue_delay(mon_id_t monitor_id,uint time);
//XLins * monitor_get_ins(mon_id_t monitor_id);
XLpak_ins * monitor_get_pak_ins(mon_id_t monitor_id);
XLpak_ins * monitor_get_pak_ins_delay(mon_id_t monitor_id,uint time);
int monitor_remove_member(mon_id_t monitor_id);
int monitor_remove_all_member(mon_id_t monitor_id);

XLsource *monitor_get_source(mon_id_t id);

XLins_arg * INS_get_par(XLins * ins,const char * name);
int INS_get_par_int(XLins * ins,const char * name);
char * INS_get_par_str(XLins * ins,const char * name);
XLll * INS_get_par_ll(XLins * ins,const char * name);

int source_cmp(XLsource *source1,XLsource *source2);
int source_cpy(XLsource * source1,XLsource * source2);


int file_get_str(FILE * file,const char * str);
int catch_keyword(const char * str,const char * s);
char * file_get_str_front_stop(FILE * file,char front,char stop);
void str_del_char(const char * str,int p);

int event_linkinfo_update(XLll * ll_old,XLll * ll_new);
int event_linkinfo_new(XLll * ll,const char * only_name,const char * show_name,int state,int abco);
int event_linkinfo_del(XLll * ll,const char * only_name);
arg_device * event_linkinfo_get(XLll * ll,const char * only_name);

int send_appinfo(core_id_t core_id);
int send_appinfo_req(core_id_t core_id);
void recv_appinfo(XLpak_ins * pak_ins);
void recv_appinfo_req(XLpak_ins * pak_ins);

#endif // CINSTRUCTION_H
