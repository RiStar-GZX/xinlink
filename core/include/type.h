#ifndef TYPE_H
#define TYPE_H

//config
#define PLATFORM_LINUX
//#define PLATFORM_ESP32

#define NETWORK_PORT           8081
#define NETWORK_BROADCAST_PORT 8088

#define ENABLE   1
#define DISABLE  0

#define NAME_LENGTH 64
#define CORE_NAME_LENGTH 64
#define APP_NAME_LENGTH 64
#define DEV_NAME_LENGTH 64
#define APP_NAME_LENGTH 64
/* base */
#ifndef uint_8_t
typedef unsigned              char     uint8_t;
#endif

#ifndef uint_16_t
typedef unsigned    short     int      uint16_t;
#endif

#ifndef uint_32_t
typedef unsigned              int      uint32_t;
#endif

#ifndef uint
typedef unsigned              int      uint;
#endif


#ifndef int8_t
typedef               char     int8;
#endif

#ifndef int16_t
typedef    short      int      int16;
#endif

#ifndef int32_t
typedef               int      int32;
#endif

#ifndef core_id_t
typedef int core_id_t;
#endif

#ifndef dev_id_t
typedef int      dev_id_t;
#endif

#ifndef IP
typedef uint32_t IP;
#endif

#ifndef MAC
typedef uint8_t MAC;
#endif

#ifndef PORT
typedef uint16_t PORT;
#endif

#ifndef LEVEL
typedef uint8_t LEVEL;
#endif

#ifndef app_id_t
typedef unsigned int      app_contain_id_t;
#endif

#ifndef event_id_t
typedef unsigned int      event_id_t;
#endif

#ifndef mon_id_t
typedef unsigned int      mon_id_t;
#endif

#ifndef sign_id_t
typedef unsigned int      sign_id_t;
#endif


//链表成员结构体
typedef struct XLll_member
{
    //uint size;
    void * data;
    struct XLll_member * front;
    struct XLll_member * next;
}XLll_member;

//链表结构体
typedef struct XLll
{
    uint member_num;
    uint member_size;
    struct XLll_member * head;
    struct XLll_member * tail;
}XLll;


//解码后的指令(一个一个字符串)的结构体
typedef  struct XLins_decoded{
    uint8_t ** argv;
    int argc;
}XLins_decoded;

//网络信息结构体
typedef struct XLnet {
    IP ip;
    MAC mac[6];
    PORT port;
}XLnet;

//来源结构体
typedef struct XLsource{
    uint32_t mode;
    uint32_t id;
    XLnet net;
    char name[NAME_LENGTH];
}XLsource;

#define SIZE_SOURCE_WITHOUT_NAME sizeof(uint32_t)*2+sizeof(XLnet)

//----------网络包部分----------//

//通用网络包头部
typedef struct XLpak_base{
    uint16_t mode;
    uint16_t pak_size;
    XLnet net_sender;
    XLnet net_receiver;
}XLpak_base;

enum arg_type{
    ARG_TYPE_INT =1,
    ARG_TYPE_FLOAT,
    ARG_TYPE_STR ,
    ARG_TYPE_XLLL
};

typedef struct XLins_arg
{
    char  name[NAME_LENGTH];
    //char * name;    //only ins ca char*
    uint8_t type;
    uint data_size;
    void * data;
}XLins_arg;


enum device_mark_state{
    DEVICE_MARK_STATE_NONE=0,
    DEVICE_MARK_STATE_CONNECT,
    DEVICE_MARK_STATE_ABILIABLE
};

typedef struct XLdevice_mark{
    char name[NAME_LENGTH];
    uint state;
    bool use_source;
    XLsource source; //biao ji sp
}XLdevice_mark;

typedef struct XLins
{
    char op_name[NAME_LENGTH];
    XLll arg_ll;    //XLins_arg
    //XLll device_mark;   //XLdevice_mark
}XLins;

//指令网络包结构体
typedef struct XLpak_ins{
    XLpak_base base;
    XLsource sender;
    XLsource receiver;
    XLins Ins;
}XLpak_ins;

//核心对接网络包结构体
typedef struct XLpak_connect{
    XLpak_base base;
    char core_name[NAME_LENGTH];
}XLpak_connect;

enum pak_info_mode{
    PAK_INFO_MODE_REQUIRE=0,
    PAK_INFO_MODE_RETURN,
    PAK_INFO_MODE_REFUSE,
};

typedef struct XLpak_info{
    XLpak_base base;
    XLsource sender;
    XLsource receiver;
    uint mode;
    char op_name[NAME_LENGTH];
    XLll device_mark;
}XLpak_info;


//----------队列----------//

//网络包队列共用体
typedef union XLqueue_in{
    XLpak_ins pak_ins;
    XLpak_connect pak_connect;
    XLpak_info pak_info;
    //XLpak_sign pak_sign;
}XLqueue_in;

//队列成员
typedef struct XLqueue {
    int mode;
    XLqueue_in in;
    LEVEL level;
    struct XLqueue * front;
    struct XLqueue * next;
}XLqueue;

//队列头信息
typedef struct XLqueue_head {
    XLqueue * queue;
}XLqueue_head;

/*
typedef struct XLevent_item {   //only other XLcore use
    uint type;
    char name[NAME_LENGTH];
}XLevent_item;*/

//核心结构体
typedef struct XLcore {
    char name[CORE_NAME_LENGTH];
    core_id_t id;
    XLnet net;
    uint8_t safety;
    //XLll * sign_ll;
    XLll * app_info_ll;   //XLapp_info
}XLcore;


//事件标签的类型
enum event_type{
    EVENT_TYPE_NONE=0,
    EVENT_TYPE_SERVER,
    EVENT_TYPE_DEVICE,
    EVENT_TYPE_BOTH
};


//传入事件函数的参数
typedef struct XLevent_par{
    XLsource * source;
    //XLsign * sign;
    //int mode;
    mon_id_t mon_id;
    //XLqueue_in pak_in;
}XLevent_par;

//事件函数指针
typedef  void *(*EVENT)(XLevent_par * par);

typedef struct XLevent_arg{
    XLins ins;
}XLevent_arg;

typedef struct XLapp_info{
    char name[NAME_LENGTH];
    uint type;          //详细值在event_type

    //char protocol[NAME_LENGTH];
    //char protocol2[NAME_LENGTH]; //
    //char manufacturer[NAME_LENGTH];

    EVENT event;
}XLapp_info;

//事件结构体
typedef struct XLevent{
    event_id_t id;
    mon_id_t mon_id;

    XLapp_info info;
    EVENT event;
}XLevent;



//应用结构体=APP_contain
typedef struct XLapp_contain{
    XLapp_info info;
    app_contain_id_t id;
    EVENT event;
}XLapp_contain;

//监视器结构体
typedef struct XLmonitor{
    mon_id_t id;
    //XLsource sender;
    XLll source_ll;
    XLsource receiver;
    XLqueue_head queue_head;
}XLmonitor;


enum arg_device_state{
    ARG_DEVICE_STATE_IN=1,
    ARG_DEVICE_STATE_OUT=2,
    ARG_DEVICE_STATE_BOTH=4,
};

enum arg_device_ab_or_co{
    ARG_DEVICE_AB=1,
    ARG_DEVICE_CO,
};

typedef struct arg_device
{
    char only_name[NAME_LENGTH];
    char show_name[NAME_LENGTH];
    uint8_t state;
    uint8_t abco;
}arg_device;


#endif // TYPE_H
