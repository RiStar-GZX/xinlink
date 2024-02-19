#include "mainwindow.h"
#include <QApplication>

#include <core.h>
#include <dsp.h>
#include <monitor.h>



int main(int argc, char *argv[])
{
    if(xinlink_init())printf("xinlink init success!\n");
    monitor_init();
    dsp_init();

    QApplication a(argc, argv);
    MainWindow  w;
    w.show();
    return a.exec();
}


/*
void * test1_soot(XLsoot_par * par){
    printf("test1 connectwith soot!\n");
    event_return_ins(par->event_id,par->ins,ins_create("recv",""));
    return NULL;
}

void * test1(XLevent_par *par){
    XLdevice * device=device_create_and_get(par->event_id,"dev_t1");
    strcpy(device->protocol,"OK");
    strcpy(device->showname,"dev_t1");
    soot_create(par->event_id,OPNAME_CONNECTWITH,test1_soot);

    return NULL;
}

void * test2_soot(XLsoot_par * par){
    if(par->error_code==SOOT_ERROR_CODE_NONE)printf("YES\n");
    if(par->error_code==SOOT_ERROR_CODE_TIMEOUT)printf("TIMEOUT!\n");
    return NULL;
}

void * test2(XLevent_par *par){
    XLdevice * device=device_create_and_get(par->event_id,"dev_t2");
    strcpy(device->protocol,"OK");
    strcpy(device->showname,"dev_t2");
    while(1){
        int s;
        scanf("%d",&s);
        XLdevice_source recv;
        recv.core_net=network_get_local_info();
        recv.id=1;
        int u=device_send_connectwith(device->id,&recv,test2_soot);
        printf("send!%d\n",u);
    }
    return NULL;
}

int main(){
    xinlink_init();

    app_add("test1",test1);
    app_add("test2",test2);
    event_create_and_run("test1");
    event_create_and_run("test2");

    while(1){
        connect
    }
    return 0;
}
*/
