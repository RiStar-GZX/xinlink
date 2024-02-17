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
    //event_return_ins(par->event_id,&par->ins->sender,par->ins->mark,ins_create("test",""));
    return NULL;
}

void * test1(XLevent_par *par){
    soot_create(par->event_id,"test",test1_soot);
    return NULL;
}

void * test2_soot(XLsoot_par * par){
    if(par->error_code==SOOT_ERROR_CODE_NONE)printf("YES\n");
    if(par->error_code==SOOT_ERROR_CODE_TIMEOUT)printf("TIMEOUT!\n");
    return NULL;
}

void * test2(XLevent_par *par){
    while(1){
        XLsource recv;
        recv.id=1;
        recv.mode=SOURCE_EVENTID;
        recv.net=network_get_local_info();
        event_send_ins_soot(par->event_id,&recv,ins_create("test",""),test2_soot);
        sleep(1);
/*
        int mark=event_send_ins(par->event_id,&recv,ins_create("test",""));
        XLpak_ins * ins_recv=wait_ins_return(par->event_id,mark,1000);
        if(ins_recv==NULL)printf("test2:NONE\n");
        else printf("test2:YES\n");

    }
    return NULL;
}

int main(){
    xinlink_init();

    app_add("test1",test1);
    app_add("test2",test2);
    event_create_and_run("test1");
    event_create_and_run("test2");

    while(1);
    return 0;
}


*/
