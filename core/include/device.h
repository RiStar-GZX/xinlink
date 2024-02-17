#ifndef DEVICE_H
#define DEVICE_H

#include <core.h>
#include <type.h>

#define OPNAME_CONNECT "connect"
#define OPNAME_CONNECTWITH "connectwith"
#define OPNAME_DISCONNECT "disconnect"
#define OPNAME_DISCONNECTWITH "disconnectwith"


dev_id_t device_create(event_id_t event_id,const char * onlyname);
XLdevice * device_create_and_get(event_id_t id,const char * only_name);
void device_show(void);
int device_remove(dev_id_t id);
XLdevice * device_get_local(dev_id_t id);
XLdevice_info * device_info_get(core_id_t core_id,dev_id_t dev_id);
XLll *  event_get_device(event_id_t event_id);

/*
int device_info_update(event_id_t id,XLll * ll_new);
int device_info_new(XLll * ll,const char * only_name,const char * show_name,int state);
int event_linkinfo_del(XLll * ll,const char * only_name);
XLdevice * event_linkinfo_get(XLll * ll,const char * only_name);
*/

void device_set_update_mode(event_id_t id);
XLdevice * device_get_update(event_id_t id,char * only_name);
void device_del_not_update(event_id_t id);
XLll * device_get_arg_ll(event_id_t id);

connect_info * get_device_op_ins(event_id_t event_id,XLpak_ins * ins);

XLdevice * get_device_connect_ins(event_id_t event_id,XLpak_ins * ins);
connect_info * get_device_connectwith(event_id_t event_id,XLpak_ins * ins);
XLdevice * get_device_disconnect_ins(event_id_t event_id,XLpak_ins * ins);

int send_deviceinfo(core_id_t core_id);
void recv_deviceinfo(XLpak_ins * pak_ins);
int send_deviceinfo_req(core_id_t core_id);
void recv_deviceinfo_req(XLpak_ins * pak_ins);


int device_send_connect(dev_id_t dev_id,XLdevice_source * device1,XLdevice_source * device2,SOOT soot);

int device_send_connectwith(dev_id_t dev_id,XLdevice_source * device,SOOT soot);

int device_send_disconnect(dev_id_t dev_id,XLdevice_source * device1,XLdevice_source * device2,SOOT soot);

int device_send_disconnectwith(dev_id_t dev_id,XLdevice_source * device,SOOT soot);

int event_send_connect(event_id_t event_id,XLdevice_source * device1,XLdevice_source * device2,SOOT soot);

int event_send_disconnect(event_id_t event_id,XLdevice_source * device1,XLdevice_source * device2,SOOT soot);

#endif // DEVICE_H
