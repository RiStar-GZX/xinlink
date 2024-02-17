#ifndef EVENT_H
#define EVENT_H

#include<stdio.h>
#include<malloc.h>
#include<string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>


#include <type.h>
#include <instruction.h>
#include <ll.h>

#define FRONTSIZE_APPCONTAIN_NAME 0
#define FRONTSIZE_APPCONTAIN_ID sizeof(char)*APP_NAME_LENGTH

#define FRONTSIZE_EVENT_ID 0
#define FRONTSIZE_EVENT_NAME sizeof(event_id_t)+sizeof(mon_id_t)

enum SIGN_MODE{
    SIGN_CS_SERVER =1,
    SIGN_CS_CLIENT,
    SIGN_CS_BOTH
};

enum operate_code{
    OPERATE_CODE_CONNECT =1,
    OPERATE_CODE_DISCONNECT,
    OPERATE_CODE_INS ,
};


int app_add(const char* name,EVENT event);
void app_show(void);
int app_remove(app_contain_id_t id);
int app_set_event(const char* name,EVENT event);
XLapp_contain * app_get_by_id(app_contain_id_t id);
XLapp_contain * app_get_by_name(const char* name);

event_id_t event_create(XLapp_info * info,EVENT event);
XLevent * event_get_by_id(event_id_t id);
XLevent * event_get_by_name(const char * name);
int event_set_name(event_id_t id,const char* name);

int event_run(event_id_t id);
int ins_event_run(event_id_t id);
void event_show(void);
event_id_t event_create_and_run(const char* app_name);


int event_set_device_mark(event_id_t id,const char * name,XLsource * source,uint state);
int event_remove_device_mark(event_id_t id,const char * name);

int send_to_link_event(XLpak_info * info);

int soot_create(event_id_t id,const char * op_name,SOOT soot);
int soot_remove(event_id_t id,const char * op_name);
int soot_run(SOOT soot,XLsoot_par * par);

#endif // EVENT_H
