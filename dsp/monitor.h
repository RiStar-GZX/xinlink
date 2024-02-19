#ifndef MONITOR_H
#define MONITOR_H

#include <core.h>
#include <type.h>

enum monitor_operate_mode{
    MONITOR_OPMODE_NONE=0,
    MONITOR_OPMODE_LSDEV ,
    MONITOR_OPMODE_CONNECT,
    MONITOR_OPMODE_CONNECTWITH,
    MONITOR_OPMODE_DISCONNECTWITH,
    MONITOR_OPMODE_DISCONNECT,
    MONITOR_OPMODE_SYSLSDEV,
};

enum monitor_operate_STATE{
    OPSTATE_DOING=0,
    OPSTATE_FINISH,
};

typedef struct monitor_device
{
    XLsource source;
    XLll arg_ll;
}monitor_device;

typedef struct monitor_operate
{
    int mode;
    bool state;
    unknow_id_t id[5];
    char device_name[64];
    char ip[64];
}monitor_operate;

typedef struct monitor_return
{
    char ret[64];
}monitor_return;


typedef struct monitor_contain
{
    XLll local_device_ll;   //monitor_device
    XLll device_ll;
    monitor_operate operate;
    monitor_return ret;
}monitor_contain;

void monitor_init(void);

#endif // MONITOR_H
