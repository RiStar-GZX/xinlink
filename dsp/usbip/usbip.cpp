#include <usbip.h>

char * name_to_chinese(const char * name){
    if(name==NULL)return (char*)"未标记设备";
    //printf("name:%s ",name);
    if(catch_keyword(name,"unknow"))return (char*)"未知设备";
    if(catch_keyword(name,"touch"))return (char*)"触摸设备";
    if(catch_keyword(name,"USB KB"))return (char*)"键盘";
    if(catch_keyword(name,"MOUSE"))return (char*)"鼠标";
    if(catch_keyword(name,"camera"))return (char*)"相机";
    if(catch_keyword(name,"bluetooth"))return (char*)"蓝牙";
    if(catch_keyword(name,"disk"))return (char*)"U盘";
    return (char*)"未标记设备";
}

void connectwith_soot(XLsoot_par *par){
    XLdevice_source * source=INS_get_par_device_source(&par->ins->Ins,"device");
    if(source==NULL){
        event_return_ins(par->event_id,par->ins,ins_create("recv","s","state","cw:on source"));
        return;
    }
    XLdevice_info * info=device_info_get_by_source(*source);
    if(info==NULL){
        event_return_ins(par->event_id,par->ins,ins_create("recv","s","state","cw:no info"));
        return;
    }
    else printf("device info:\n|name:%s \n|only name:%s \n|proto:%s \n",info->showname,info->onlyname,info->protocol);


    device_return_ins(par->device_id,par->ins,ins_create("recv","s","state","cw:success"));
}

void connect_soot(XLsoot_par *par){
    XLdevice_source * source=INS_get_par_device_source(&par->ins->Ins,"device");
    if(source==NULL){
        event_return_ins(par->event_id,par->ins,ins_create("recv","s","state","c:no source"));
        return;
    }
    uint mark=device_send_connectwith(par->device_id,source,NULL);
    XLpak_ins * ins=wait_ins_return(par->event_id,mark,OUT_TIME);
    if(ins==NULL){
        event_return_ins(par->event_id,par->ins,ins_create("recv","s","state","c:target timeout"));
        return;
    }
    char * state=INS_get_par_str(&ins->Ins,"state");
    if(state==NULL||strcmp(state,"cw:success")!=0){
        event_return_ins(par->event_id,par->ins,ins_create("recv","s","state","c:fail to connect"));
        return;
    }
    event_return_ins(par->event_id,par->ins,ins_create("recv","s","state","c:success"));
}

void disconnectwith_soot(XLsoot_par *par){
    device_return_ins(par->device_id,par->ins,ins_create("recv",""));
}

void disconnect_soot(XLsoot_par *par){
    XLdevice_source * source=INS_get_par_device_source(&par->ins->Ins,"device");
    if(source==NULL)printf("NO!\n");
    uint mark=device_send_connectwith(par->device_id,source,NULL);
    XLpak_ins * ins=wait_ins_return(par->event_id,mark,OUT_TIME);
    if(ins==NULL){
        printf("OUTTIME!\n");
        event_return_ins(par->event_id,par->ins,ins_create("recv","s","state","timeout"));
        return;
    }
    event_return_ins(par->event_id,par->ins,ins_create("recv","s","state","success"));
}

void *usbip_event(XLevent_par * par){
    printf("usbip event!\n\n");
    popen("sudo modprobe usbip-core","r");
    popen("sudo modprobe usbip-vudc","r");
    popen("sudo modprobe vhci-hcd","r");
    popen("sudo modprobe usbip-host","r");
    popen("sudo usbipd","r");

    soot_create(par->event_id,"connect",connect_soot);
    soot_create(par->event_id,"disconnect",disconnect_soot);
    soot_create(par->event_id,"connectwith",connectwith_soot);
    soot_create(par->event_id,"disconnectwith",disconnectwith_soot);
    while(1)
    {
        FILE * usbip=popen("usbip list -l","r");

        device_set_update_mode(par->event_id);

        for(;file_get_str(usbip,"busid");){
            char * busid=file_get_str_front_stop(usbip,' ',' ');
            str_del_char(busid,0);
            str_del_char(busid,strlen(busid)-1);
            char * usb_id=file_get_str_front_stop(usbip,'(',')');
            str_del_char(usb_id,0);
            str_del_char(usb_id,strlen(usb_id)-1);
            file_get_str_front_stop(usbip,':','(');

            FILE * lsusb=popen("lsusb","r");
            char * en_name=NULL,* cn_name;
            if(file_get_str(lsusb,usb_id)){
                en_name=file_get_str_front_stop(lsusb,' ','\n');
                str_del_char(en_name,strlen(en_name)-1);
            }
            pclose(lsusb);
            cn_name=name_to_chinese(en_name);
            //printf("en_name:%s cn:%s\n",en_name,cn_name);
            XLdevice *device=device_get_update(par->event_id,busid);
            if(device==NULL){
                device=device_create_and_get(par->event_id,busid);
                strcpy(device->protocol,"USBIP");
                device->direction=DEVICE_DIRECTION_OUT;
                strcpy(device->showname,cn_name);
            }
        }
        pclose(usbip);
        XLdevice * device=device_get_update(par->event_id,"usbip");
        if(device==NULL){
            device=device_create_and_get(par->event_id,"usbip");
            strcpy(device->protocol,"USBIP");
            device->direction=DEVICE_DIRECTION_IN;
            strcpy(device->showname,"usbip server");
        }
        device_del_not_update(par->event_id);
        sleep(1);
    }
    return NULL;
}

void usbip_init(void){
    app_add("usbip",usbip_event);
    event_create_and_run("usbip");
}
