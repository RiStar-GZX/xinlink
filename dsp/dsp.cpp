#include <dsp.h>
#include <core.h>




void *moonlight_event(XLevent_par * par){
    printf("moonlight event!\n\n");
    while(1)
    {
        XLpak_ins * ins_recv=monitor_get_pak_ins_delay(par->mon_id,5000);

        if(message(ins_recv,"xldev",0)){
            XLins * ins=ins_create("xldev","");
            XLll * ll=ll_create(sizeof(arg_device));
            arg_device dev;
            strcpy(dev.only_name,"device1");
            dev.abco=ARG_DEVICE_AB;
            dev.state=ARG_DEVICE_STATE_IN;
            ll_add_member_tail(ll,&dev,sizeof(arg_device));
            strcpy(dev.only_name,"device2");
            ll_add_member_tail(ll,&dev,sizeof(arg_device));
            ins_add_arg(ins,"dev",ARG_TYPE_XLLL,ll);
            printf("moonlight\n");
            network_send_ins(par->source,&ins_recv->sender,ins);
        }
        monitor_remove_member(par->mon_id);
        sleep(1);
    }
    return NULL;
}


char * name_to_chinese(const char * name){
    if(name==NULL)return "未标记设备";
    //printf("name:%s ",name);
    if(catch_keyword(name,"unknow"))return "未知设备";
    if(catch_keyword(name,"touch"))return "触摸设备";
    if(catch_keyword(name,"USB KB"))return "键盘";
    if(catch_keyword(name,"MOUSE"))return "鼠标";
    if(catch_keyword(name,"camera"))return "相机";
    if(catch_keyword(name,"bluetooth"))return "蓝牙";
    if(catch_keyword(name,"disk"))return "U盘";
    return "未标记设备";
}

void *usbip_event(XLevent_par * par){
    printf("usbip event!\n\n");
    popen("sudo modprobe usbip-core","r");
    popen("sudo modprobe usbip-vudc","r");
    popen("sudo modprobe vhci-hcd","r");
    popen("sudo modprobe usbip-host","r");
    popen("sudo usbipd","r");
    XLll * devll=ll_create(sizeof(arg_device));
    XLll * devll_new=ll_create(sizeof(arg_device));
    while(1)
    {
        XLpak_ins * ins_recv=monitor_get_pak_ins_delay(par->mon_id,1000);

        if(message(ins_recv,"xldev",0)){
            printf("usbip\n");
            XLins * ins=ins_create("xldev","");
            FILE * usbip=popen("usbip list -l","r");

            ll_del_member_all(devll_new);
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
                    //printf("cn_name:%s\n",cn_name);
                    //printf("\n%sen_name:%s cn:%s\n",usb_id,en_name,cn_name);
                }
                pclose(lsusb);
                cn_name=name_to_chinese(en_name);
                event_linkinfo_new(devll_new,busid,cn_name,ARG_DEVICE_STATE_OUT,ARG_DEVICE_AB);
            }
            pclose(usbip);

            event_linkinfo_update(devll,devll_new);

            ins_add_arg(ins,"dev",ARG_TYPE_XLLL,devll);
            network_send_ins(par->source,&ins_recv->sender,ins);
        }
        //connect 将本机设备发送给对面
        if(message(ins_recv,"connectout",2,"name","ip")){
            char * name=INS_get_par_str(&ins_recv->Ins,"name");
            char * ip=INS_get_par_str(&ins_recv->Ins,"ip");
            printf("link with:%s %s\n",ip,name);

            popen("sudo usbip bind -b %s",name);

            XLins * ins;
            ins=ins_create("recv","s","opname","connectout");
            XLsource recver;
            recver.net.ip=inet_addr(ip);

            //network_send_ins(re);
        }
        if(message(ins_recv,"connectin",2,"name","ip")){

        }

        monitor_remove_member(par->mon_id);
        //sleep(1);
    }
    return NULL;
}

int dsp_init(void){
    app_add("moonlight",moonlight_event);
    app_add("usbip",usbip_event);
    event_create_and_run("usbip");
    event_create_and_run("moonlight");
    return 1;
}
