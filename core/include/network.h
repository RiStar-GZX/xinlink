#ifndef NETWORK_H
#define NETWORK_H

#include <malloc.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <instruction.h>
#include <type.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

#ifdef PLATFORM_LINUX
#include <pthread.h>
#include <arpa/inet.h>
#endif

#ifdef PLATFORM_ESP32
#include <WiFi.h>
#include <WiFiUdp.h>
#include <sys/socket.h>
#include <RTOS.h>
#endif

#define SEND_QUEUE          1
#define RECIVE_QUEUE        2

#define PAK_MAX_SIZE        2048

#define CONNECT_REQUEST     1
#define CONNECT_ACCPET      0

#define NETWORK_MODE_INS        1
#define NETWORK_MODE_CONNECT    2
#define NETWORK_MODE_EVENT_INFO       3
#define NETWORK_MODE_EVENT_LINK_INFO  4

//CONNECT
enum pak_connect_mode{
    PAK_CONNECT_SEND_CORE       =0,
    PAK_CONNECT_SEND_CORE_REQUIRE ,
    PAK_CONNECT_REQUEST           ,
    PAK_CONNECT_ACCEPT            ,
    PAK_CONNECT_DISACCEPT         ,
    PAK_DISCONNECT                ,
};
#define SEE_CONNECT_ONLY        15       //0010 0000 0000 0000

//SOURCE MODE
enum source_mode{
    SOURCE_EVENTID           =1,
    SOURCE_APPNAME             ,
    //SOURCE_ACCESS              ,
    SOURCE_SYSTEM              ,
    SOURCE_DEVICE              ,
};
//SIGN

#define SIGN_REQUIRE        1          //---- ---- ---- 0001
#define SIGN_SEND           2          //---- ---- ---- 0010
#define SEE_FOUR_ONLY       15         //0010 0000 0000 0000

enum queue_type{
    QUEUE_TYPE_NONE=0,
    QUEUE_TYPE_INS,
    QUEUE_TYPE_CONNECT,
    QUEUE_TYPE_SIGN,
    QUEUE_TYPE_INFO,
    //QUEUE_TYPE_LINK_INFO,
};

#define OUT_TIME 10000

void data_show(uint8_t *data,uint size);
void data_add(uint8_t *data,int * p,void * _struct,uint size);
void data_add_str(uint8_t *data,int * p,char* str);
void data_get(uint8_t *data,int * p,void * _struct,uint size);
int data_get_str(uint8_t *data,int *p,char * str);
char * data_get_str_p(uint8_t *data,int *p);
/*--------------*/
XLnet network_get_local_info(void);
int TCP_send(XLnet * net,uint8_t * data,int datasize);
int Broadcast_send (void *buf,int bufsize);

int queue_del_head(XLqueue_head *head);
int queue_del_queue(XLqueue_head *head,XLqueue * queue);
int queue_remove_all(XLqueue_head * head);
int queue_add(int mode,XLqueue_head * head,XLqueue_in * in,LEVEL level);
int queue_add_ins(XLqueue_head * head,XLpak_ins * ins,LEVEL level);
int queue_add_connect(XLqueue_head * head,XLpak_connect * connect,LEVEL level);
void queue_show(XLqueue_head * head);
XLqueue_head * queue_total(void);
XLqueue_head * queue_send(void);
XLqueue_head * queue_broadcast(void);
XLqueue_head * queue_recv(void);
int queue_init(void);

int network_init(void);

//XLpak_ins * buf_to_pak_ins(uint8_t * buf);
//uint8_t * pak_ins_to_buf(XLpak_ins *pak_ins,int * size);
uint8_t * pak_ins_to_buf(XLpak_ins *pak_ins,int * size);
XLpak_ins * buf_to_pak_ins(uint8_t * buf);
XLpak_connect * buf_to_pak_connect(uint8_t* buf);
uint8_t * pak_connect_to_buf(XLpak_connect * pak_connect,int * size);
//uint8_t * pak_sign_to_buf(XLpak_sign * pak_sign,int * size);
//XLpak_sign * buf_to_pak_sign(uint8_t * buf);

int network_core_find_send(void);
int network_core_connect(XLpak_connect * pak_connect);
int network_core_connect_require_send(core_id_t core_id);
int network_core_disconnect_send(core_id_t core_id);

int network_safe(XLnet *net);
int network_send_ins(XLsource * sender,XLsource *receiver,XLins * ins);

int network_core_find_receive(XLpak_connect * pak_connect);
int network_core_connect_require_receive(XLpak_connect * pak_connect);

//uint network_send_event_info(XLnet core);

uint8_t * pak_info_to_buf(XLpak_info * info,uint * size);
int network_get_event_info(uint8_t * buf);

XLpak_info * buf_to_pak_info(uint8_t * buf);

int event_send_ins(event_id_t id,XLsource *receiver,XLins * ins);
int event_send_ins_soot(event_id_t id,XLsource *receiver,XLins * ins,SOOT soot);
int dev_send_ins(dev_id_t id,XLsource *receiver,XLins * ins);
int dev_send_ins_soot(dev_id_t id,XLsource *receiver,XLins * ins,SOOT soot);
XLpak_ins * wait_ins_return(event_id_t event_id,uint mark,uint time);
int event_return_ins(event_id_t id,XLsource * receiver,uint mark,XLins * ins);

int send_inss(unknow_id_t id,bool event0_dev1,XLsource *receiver,XLins * ins,SOOT soot,uint out_time);

void * out_times(void * arg);

#endif // NETWORK_H
