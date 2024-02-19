#include <monitor.h>

monitor_contain contain;

void xldev_soot(XLsoot_par *par){
    if(par->error_code==SOOT_ERROR_CODE_TIMEOUT){
        printf("time out!!\n");
        contain.operate.state=OPSTATE_FINISH;
        return;
    }
    XLpak_ins * ins_recv=par->ins;
    XLll * ll=INS_get_par_ll(&ins_recv->Ins,"dev");
    if(ll!=NULL){
        printf("\nMONITOR GET DEVICE INFO:\n");
        XLll * contain_ll;
        if(ins_recv->sender.net.ip==network_get_local_info().ip)contain_ll=&contain.local_device_ll;
        else contain_ll=&contain.device_ll;

        XLll_member * member=ll->head;
        for(int i=0;i<ll->member_num;i++){
            arg_device * arg=(arg_device*)member->data;
            printf("ONLY%d:%s NAME:%s\n",i+1,arg->only_name,arg->show_name);
            member=member->next;
        }
        printf("\n");
        XLll_member * member_dev;
        member_dev=ll_get_member_compare(contain_ll,
                                           sizeof(uint32_t)*2+sizeof(XLnet),strlen(ins_recv->sender.name)+1
                                           ,ins_recv->sender.name);
        if(member_dev!=NULL){
            monitor_device *device=(monitor_device*)member_dev->data;
            //ll_free(&device->arg_ll);
            device->arg_ll=*ll;
        }
        else {
            monitor_device device;
            device.source=ins_recv->sender;
            device.arg_ll=*ll;
            ll_add_member_tail(contain_ll,&device,sizeof(monitor_device));
        }
    }
    contain.operate.state=OPSTATE_FINISH;
}

void monitor_return_soot(XLsoot_par * par){
    if(par->error_code==SOOT_ERROR_CODE_TIMEOUT){
        strcpy(contain.ret.ret,"Fail(Time Out)!\n");
        return;
    }
    //strcpy(contain.ret.ret,"success!\n");
    char * str=INS_get_par_str(&par->ins->Ins,"state");
    sleep(1);
    printf("return soot:%s\n",str);
}

void *monitor_event(XLevent_par * par){
    while(1)
    {
        int mode=contain.operate.mode;

        if(mode==MONITOR_OPMODE_SYSLSDEV){
            send_deviceinfo_req(contain.operate.id[1]);
            contain.operate.mode=MONITOR_OPMODE_NONE;
            contain.operate.state=OPSTATE_FINISH;
        }
        if(mode==MONITOR_OPMODE_CONNECT||mode==MONITOR_OPMODE_DISCONNECT){
            XLdevice_source device1,device2;
            device1.id=contain.operate.id[1];
            XLcore *core1=core_get_by_id(contain.operate.id[2]);
            device2.id=contain.operate.id[3];
            XLcore *core2=core_get_by_id(contain.operate.id[4]);
            if(core1!=NULL&&core2!=NULL){
                device1.core_net=core1->net;
                device2.core_net=core2->net;
                int u;
                if(mode==MONITOR_OPMODE_CONNECT)
                    u=event_send_connect(par->event_id,&device1,&device2,monitor_return_soot);
                if(mode==MONITOR_OPMODE_DISCONNECT)
                    u=event_send_disconnect(par->event_id,&device1,&device2,monitor_return_soot);
                printf("MONI CONNECT!:%d\n",u);
            }
            contain.operate.mode=MONITOR_OPMODE_NONE;
            contain.operate.state=OPSTATE_FINISH;
        }

        usleep(500000);
    }
    return NULL;
}

void monitor_init(void){
    contain.local_device_ll=*ll_create(sizeof(monitor_device));
    contain.device_ll=*ll_create(sizeof(monitor_device));
    contain.operate.mode=MONITOR_OPMODE_NONE;
    app_add("monitor",monitor_event);
    event_create_and_run("monitor");
}
