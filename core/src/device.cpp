#include <device.h>

XLll * device_ll=ll_create(sizeof(XLdevice));

dev_id_t device_create(event_id_t event_id,const char * onlyname)
{
    if(onlyname==NULL)return 0;
    XLevent * event=event_get_by_id(event_id);
    if(event==NULL)return NULL;

    XLdevice device_new;
    device_new.id=1;
    device_new.event_id=event_id;
    device_new.reset=DEVICE_ALREADY_RESET;
    device_new.device_source_ll=*ll_create(sizeof(XLdevice_source));
    strcpy(device_new.onlyname,onlyname);

    extern XLll * device_ll;

    //添加首个成员
    if(device_ll->head==NULL){
        ll_add_member_head(device_ll,&device_new,sizeof(XLdevice));
        return device_new.id;
    }
    //添加成员
    XLll_member * member_now=device_ll->head;
    int mode=0,mem_front_id=-1;//-1代表位于首位
    //获得新的成员的ID和要插入的位置
    for(int i=0;i<device_ll->member_num;i++){
        XLdevice * device_now=(XLdevice*)member_now->data;
        if(mode==0&&device_new.id==device_now->id){
            device_new.id++;
            mem_front_id=i;
        }
        else if(mode==1&&device_new.id==device_now->id){
            device_new.id++;
            mem_front_id=i;
            mode=0;
        }
        else if(mode==0)mode=1;

        member_now=member_now->next;
    }

    if(mem_front_id==-1)ll_insert_member_front(device_ll,&device_new,sizeof(XLdevice),0);
    else ll_insert_member_next(device_ll,&device_new,sizeof(XLdevice),mem_front_id);
    return device_new.id;
}

XLdevice * device_create_and_get(event_id_t id,const char * only_name){
    dev_id_t dev_id=device_create(id,only_name);
    return device_get_local(dev_id);
}

void device_show(void)
{
    printf("device show:\n");
    extern XLll * device_ll;
    if(device_ll==NULL)return;
    XLll_member * member_now=device_ll->head;
    if(member_now==NULL)return;
    for(int i=0;i<device_ll->member_num;i++)
    {
        XLdevice * device_now=(XLdevice*)member_now->data;
        printf("%d ",device_now->id);
        member_now=member_now->next;
    }
    printf("\n");
}

int device_remove(dev_id_t id)
{
    extern XLll * device_ll;
    if(device_ll==NULL)return -1;
    XLll_member * member_now=device_ll->head;
    for(int i=0;i<device_ll->member_num;i++){
        XLdevice * device_now=(XLdevice*)member_now->data;
        if(device_now->id==id){
            ll_del_member_all(&device_now->device_source_ll);
            member_now=member_now->next;
            ll_del_member_num(device_ll,i);
            return 1;
        }
        else member_now=member_now->next;
    }
    return -1;
}

XLdevice * device_get_local(dev_id_t id)
{
    extern XLll * device_ll;
    XLll_member * member=ll_get_member_compare(device_ll,0,sizeof(dev_id_t),&id);
    if(member==NULL)return NULL;
    return (XLdevice*)member->data;
}

XLdevice_info * device_info_get(core_id_t core_id,dev_id_t dev_id){
    if(core_id==CORE_MYSELF_ID)return NULL;
    XLcore * core=core_get_by_id(core_id);
    if(core==NULL)return NULL;
    XLll_member * member=ll_get_member_compare(&core->device_ll,0,sizeof(dev_id_t),&dev_id);
    if(member==NULL)return NULL;
    return (XLdevice_info*)member->data;
}

XLdevice_info * device_info_get_by_source(XLdevice_source source){
    XLcore * core=core_get_by_net(&source.core_net);
    if(core==NULL)return NULL;
    if(core->id==CORE_MYSELF_ID){
        XLdevice * device=device_get_local(source.id);
        if(device==NULL)return NULL;
        XLdevice_info * info=(XLdevice_info*)malloc(sizeof(XLdevice_info));
        info->direction=device->direction;
        strcpy(info->onlyname,device->onlyname);
        strcpy(info->protocol,device->protocol);
        strcpy(info->showname,device->showname);
        info->source.id=device->id;
        info->source.core_net=network_get_local_info();
        return info;
    }
    XLll_member * member=ll_get_member_compare(&core->device_ll,0,sizeof(dev_id_t),&source.id);
    if(member==NULL)return NULL;
    return (XLdevice_info*)member->data;
}


XLll *  event_get_device(event_id_t event_id){
    extern XLll * device_ll;
    XLll * ll=ll_create(sizeof(XLdevice));
    XLll_member * member=device_ll->head;
    for(int i=0;i<device_ll->member_num;i++){
        XLdevice * device=(XLdevice*)member->data;
        if(device->event_id==event_id){
            ll_add_member_tail(ll,device,sizeof(XLdevice));
        }
    }
    if(ll->member_num==0){
        free(ll);
        return NULL;
    }
    return ll;
}

void device_set_update_mode(event_id_t id){
    XLevent * event=event_get_by_id(id);
    if(event==NULL)return;
    extern XLll * device_ll;
    XLll_member * member=device_ll->head;
    for(int i=0;i<device_ll->member_num;i++){
        XLdevice * device=(XLdevice*)member->data;
        if(device->event_id==id){
            device->reset=DEVICE_NOT_RESET;
        }
        member=member->next;
    }
}

XLdevice * device_get_update(event_id_t id,char * only_name){
    XLevent * event=event_get_by_id(id);
    if(event==NULL)return NULL;
    extern XLll * device_ll;
    XLll_member * member=device_ll->head;
    for(int i=0;i<device_ll->member_num;i++){
        XLdevice * device=(XLdevice*)member->data;
        if(device->event_id==id&&strcmp(device->onlyname,only_name)==0){
            device->reset=DEVICE_ALREADY_RESET;
            return device;
        }
        member=member->next;
    }
    return NULL;
}

void device_del_not_update(event_id_t id){
    XLevent * event=event_get_by_id(id);
    if(event==NULL)return;
    extern XLll * device_ll;
    XLll_member * member=device_ll->head;
    for(int i=0;i<device_ll->member_num;i++){
        XLdevice * device=(XLdevice*)member->data;
        if(device->event_id==id&&device->reset==DEVICE_NOT_RESET){
            member=member->next;
            ll_del_member(device_ll,member);
        }
        else member=member->next;
    }
}

XLll * device_get_arg_ll(event_id_t id){
    XLevent * event=event_get_by_id(id);
    if(event==NULL)return NULL;
    XLll * arg_ll=ll_create(sizeof(arg_device));
    extern XLll * device_ll;
    XLll_member * member=device_ll->head;
    for(int i=0;i<device_ll->member_num;i++){
        XLdevice * device=(XLdevice*)member->data;
        if(device->event_id==id){
            arg_device arg;
            strcpy(arg.only_name,device->onlyname);
            strcpy(arg.show_name,device->showname);
            ll_add_member_tail(arg_ll,&arg,sizeof(arg_device));
        }
        member=member->next;
    }
    return arg_ll;
}

int device_test(XLdevice_source * device1,XLdevice_source * device2){

}

//发送设备连接指令
int device_op(unknow_id_t id,bool event0_dev1,
              XLdevice_source * device1,XLdevice_source * device2
              ,bool c0_dc1,SOOT soot){

    if(device1==NULL||device2==NULL)return 0;

    XLins * ins;
    XLcore * core1=core_get_by_net(&device1->core_net);
    XLcore * core2=core_get_by_net(&device2->core_net);

    if(core1==NULL||core2==NULL)return 0;

    XLdevice_source *device;
    XLsource receiver;
    receiver.mode=SOURCE_DEVICE;
    //检验是否为自己的设备
    int mydevice=0;
    if(event0_dev1==1&&core1->id==CORE_MYSELF_ID&&id==device1->id){
        receiver.net=core2->net;
        receiver.id=device2->id;
        device=device2;
        mydevice++;
    }
    if(event0_dev1==1&&core2->id==CORE_MYSELF_ID&&id==device2->id){
        receiver.net=core1->net;
        receiver.id=device1->id;
        device=device1;
        mydevice+=2;
    }

    if(mydevice==3)return 0;   //两个都是自己的设备无法操作

    if(mydevice!=0){
        if(c0_dc1==0)ins=ins_create(OPNAME_CONNECTWITH,"");
        if(c0_dc1==1)ins=ins_create(OPNAME_DISCONNECTWITH,"");
        ins_add_arg(ins,"device",ARG_TYPE_DEVICE_SOURCE,device);
        return send_inss(id,event0_dev1,&receiver,ins,soot,OUT_TIME);
    }
    //两个设备，哪个在本机就发往哪个，如果都不在本机就发往第一个
    if(c0_dc1==0)ins=ins_create(OPNAME_CONNECT,"");
    if(c0_dc1==1)ins=ins_create(OPNAME_DISCONNECT,"");

    if(core1->id==CORE_MYSELF_ID){
        receiver.net=core1->net;
        receiver.id=device1->id;
        device=device2;
    }
    else if(core2->id==CORE_MYSELF_ID){
        receiver.net=core2->net;
        receiver.id=device2->id;
        device=device1;
    }
    else{
        receiver.net=core1->net;
        receiver.id=device1->id;
        device=device2;
    }

    ins_add_arg(ins,"device",ARG_TYPE_DEVICE_SOURCE,device);

    return send_inss(id,event0_dev1,&receiver,ins,soot,OUT_TIME);
 }

//发送设备连接指令
int device_send_connect(dev_id_t dev_id,XLdevice_source * device1,XLdevice_source * device2,SOOT soot){
    return device_op(dev_id,1,device1,device2,0,soot);
}

int device_send_connectwith(dev_id_t dev_id,XLdevice_source * device,SOOT soot){
    XLdevice_source dev_local;
    dev_local.id=dev_id;
    dev_local.core_net=network_get_local_info();
    return device_op(dev_id,1,&dev_local,device,0,soot);
}

//发送设备断开连接指令
int device_send_disconnect(dev_id_t dev_id,XLdevice_source * device1,XLdevice_source * device2,SOOT soot){
    return device_op(dev_id,1,device1,device2,1,soot);
}

int device_send_disconnectwith(dev_id_t dev_id,XLdevice_source * device,SOOT soot){
    XLdevice_source dev_local;
    dev_local.id=dev_id;
    dev_local.core_net=network_get_local_info();
    return device_op(dev_id,1,&dev_local,device,1,soot);
}

int event_send_connect(event_id_t event_id,XLdevice_source * device1,XLdevice_source * device2,SOOT soot){
    return device_op(event_id,0,device1,device2,0,soot);
}

int event_send_disconnect(event_id_t event_id,XLdevice_source * device1,XLdevice_source * device2,SOOT soot){
    return device_op(event_id,0,device1,device2,1,soot);
}


/*
connect_info * get_device_op_ins(event_id_t event_id,XLpak_ins * ins){
    if(ins==NULL)return NULL;
    XLins_arg * arg1=INS_get_par(&ins->Ins,"device1");
    XLins_arg * arg2=INS_get_par(&ins->Ins,"device2");
    if(arg1==NULL||arg2==NULL)return NULL;
    XLdevice_source * devs1=(XLdevice_source*)arg1->data;
    XLdevice_source * devs2=(XLdevice_source*)arg2->data;
    XLcore * core1=core_get_by_net(&devs1->core_net);
    XLcore * core2=core_get_by_net(&devs2->core_net);
    if(core1==NULL||core2==NULL)return NULL;
    if(core1->id==CORE_MYSELF_ID&&core2->id==CORE_MYSELF_ID)return NULL;
    XLdevice * my_dev;
    XLdevice_info * out_dev;
    if(core1->id==CORE_MYSELF_ID){
        my_dev=device_get_local(devs1->id);
        out_dev=device_info_get(devs2->id,devs2->id);
    }
    if(core2->id==CORE_MYSELF_ID){
        my_dev=device_get_local(devs2->id);
        out_dev=device_info_get(core1->id,devs1->id);
    }

    if(my_dev==NULL||out_dev==NULL)return NULL;
    if(my_dev->event_id!=event_id)return NULL;  //?
    connect_info * info=(connect_info*)malloc(sizeof(connect_info));
    info->local=my_dev;
    info->out=out_dev;
    return info;
}

connect_info * get_device_connect_ins(event_id_t event_id,XLpak_ins * ins){
    if(ins==NULL)return NULL;
    if(strcpy(ins->Ins.op_name,DEVICE_CONNECT_OPNAME)!=0)return NULL;
    connect_info * info=get_device_op_ins(event_id,ins);
    XLll_member * member=ll_get_member_compare(&info->local->device_source_ll,0,sizeof(XLdevice_source),&info->out->source);
    if(member!=NULL)return NULL;
    return info;
}

connect_info * get_device_connectwith(event_id_t event_id,XLpak_ins * ins){
    if(ins==NULL)return NULL;
    if(strcpy(ins->Ins.op_name,DEVICE_CONNECT_OPNAME)!=0)return NULL;
    connect_info * info=get_device_op_ins(event_id,ins);
    XLll_member * member=ll_get_member_compare(&info->local->device_source_ll,0,sizeof(XLdevice_source),&info->out->source);
    if(member!=NULL)return NULL;
    return info;
}*/
/*
get_device_connect();
get_device_connect_require();
get_device_disconnect();
get_device_disconnect_require();
get_device_recv();
*/

/*
connect_info * get_device_disconnectwith(event_id_t event_id,XLpak_ins * ins){
    if(ins==NULL)return NULL;
    if(strcpy(ins->Ins.op_name,DEVICE_CONNECT_OPNAME)!=0)return NULL;
    connect_info * info=get_device_op_ins(event_id,ins);
    XLll_member * member=ll_get_member_compare(&info->local->device_source_ll,0,sizeof(XLdevice_source),&info->out->source);
    if(member!=NULL)return NULL;
    return info;
}

connect_info * get_device_disconnect_ins(event_id_t event_id,XLpak_ins * ins){
    if(ins==NULL)return NULL;
    if(strcpy(ins->Ins.op_name,DEVICE_DISCONNECT_OPNAME)!=0)return NULL;
    connect_info * info=get_device_op_ins(event_id,ins);
    XLll_member * member=ll_get_member_compare(&info->local->device_source_ll,0,sizeof(XLdevice_source),&info->out->source);
    if(member==NULL)return NULL;
    return info;
}

int accept_connect(){

}

int refuse_connect(){

}

int get_device_connect_recv_ins(){
 //get accept refuse
}

*/

/*----------------------设备信息传输--------------------------------------*/

int send_deviceinfo(core_id_t core_id){
    XLcore * core=core_get_by_id(core_id);
    if(core==NULL/*||core_id==1*/)return 0;
    XLins * ins;
    ins=ins_create("device_send","");
    XLll *ll=ll_create(sizeof(XLdevice_info));
    extern XLll * device_ll;
    XLll_member *member=device_ll->head;
    for(int i=0;i<device_ll->member_num;i++){
        XLdevice * device=(XLdevice *)member->data;
        XLdevice_info info;
        info.source.id=device->id;
        info.source.core_net=core->net;
        info.direction=device->direction;
        strcpy(info.protocol,device->protocol);
        strcpy(info.onlyname,device->onlyname);
        strcpy(info.showname,device->showname);

        ll_add_member_tail(ll,&info,sizeof(XLdevice_info));
        member=member->next;
    }
    ins_add_arg(ins,"device",ARG_TYPE_XLLL,ll);
    XLsource source_recv;
    XLsource source_send;
    source_recv.mode=SOURCE_SYSTEM;
    source_send.mode=SOURCE_SYSTEM;
    source_send.net=network_get_local_info();
    source_recv.net=core->net;
    network_send_ins(&source_send,&source_recv,ins);
    return 1;
}

void recv_deviceinfo(XLpak_ins * pak_ins){
    if(pak_ins==NULL)return;
    XLcore * core=core_get_by_net(&pak_ins->sender.net);
    if(core==NULL)return;
    XLll * ll=INS_get_par_ll(&pak_ins->Ins,"device");
    //ll_free(core->device_ll);
    //ll_del_member_all(&core->device_ll);
    core->device_ll=*ll;
}

int send_deviceinfo_req(core_id_t core_id){
    XLcore * core=core_get_by_id(core_id);
    if(core==NULL/*||core_id==1*/)return 0;
    XLins * ins;
    ins=ins_create("device_req","");
    XLsource source_recv;
    XLsource source_send;
    source_recv.mode=SOURCE_SYSTEM;
    source_send.mode=SOURCE_SYSTEM;
    source_send.net=network_get_local_info();
    source_recv.net=core->net;
    network_send_ins(&source_send,&source_recv,ins);
    return 1;
}

void recv_deviceinfo_req(XLpak_ins * pak_ins){
    if(pak_ins==NULL)return;
    XLcore * core=core_get_by_net(&pak_ins->sender.net);
    if(core==NULL)return;
    send_deviceinfo(core->id);
}

