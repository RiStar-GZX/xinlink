#include "mainwindow.h"

#include <QApplication>

//extern "C"{
#include <core.h>
#include <dsp.h>
//}


void *monitor_event(XLevent_par * par){
    printf("monitor event!\n\n");
    while(1)
    {
        XLsource source;
        source.net=network_get_local_info();
        source.mode=SOURCE_EVENTID;
        source.id=2;

        int s=1;
        printf("请输入操作\n");
        scanf("%d",&s);
        XLins * ins;
        if(s==1){
            ins=ins_create("xldev","");
            network_send_ins(par->source,&source,ins);
        }
        if(s==2){
            char str[60],ip[60];
            printf("请输入名字\n");
            scanf("%s",str);
            printf("请输入ip\n");
            scanf("%s",ip);
            ins=ins_create("connectout","");
            ins_add_arg(ins,"name",ARG_TYPE_STR,str);
            ins_add_arg(ins,"ip",ARG_TYPE_STR,ip);
            network_send_ins(par->source,&source,ins);
        }
        if(s==3){

            char core_name[60],only[60];
            printf("请输入CORE\n");
            scanf("%s",core_name);
            printf("请输入ONLY\n");
            scanf("%s",only);
            XLcore * core=core_get_by_ip(inet_addr(core_name));
            if(core!=NULL){
                ins=ins_create("connect","");
            }
        }
        if(s==4){
            int core_id;
            printf("请输入CORE\n");
            scanf("%d",core_id);
            send_appinfo(core_id);
        }
        XLpak_ins * ins_recv=monitor_get_pak_ins_delay(par->mon_id,1000);
        if(message(ins_recv,"xldev",0)){

            XLll * ll=INS_get_par_ll(&ins_recv->Ins,"dev");
            if(ll!=NULL){
                printf("\nMONITOR GET DEVICE INFO:\n");
                XLll_member * member=ll->head;
                //printf("member num:%d\n",ll->member_num);
                for(int i=0;i<ll->member_num;i++){
                    arg_device * dev=(arg_device*)member->data;
                    printf("ONLY%d:%s NAME:%s\n",i+1,dev->only_name,dev->show_name);

                    member=member->next;
                }
                printf("\n");
            }
            ll_free(ll);
        }
        monitor_remove_member(par->mon_id);
        //sleep(1);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    if(xinlink_init())printf("xinlink init success!\n");
    app_add("monitor",monitor_event);
    event_create_and_run("monitor");
    dsp_init();

    while(1);/*
    QApplication a(argc, argv);
    MainWindow  w;
    w.show();
    return a.exec();

    /*FILE * file=popen("usbip list -r 192.168.1.5","r");
    char * str;
    file_get_str_front_stop(file,'-',':');
    //fseek(file,-3,SEEK_CUR);
    fgets(str,5,file);

    while(1){
        printf("str:%s\n",str);
        sleep(1);
    }*/
}
