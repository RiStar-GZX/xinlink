#include<instruction.h>
#include <network.h>
#include <event.h>
#include <core.h>

XLll * monitor_ll;
//----------------------------------------------//
//              INS的编码与解码                   //
//----------------------------------------------//

//创建一个指令
XLins * ins_create3(int num,...){
    va_list ap;
    va_start(ap,num);
    char * arg_name=NULL;
    uint arg_type=0;
    void * arg_data=NULL;
    XLins * ins=(XLins*)malloc(sizeof(XLins));
    ins->arg_ll=*ll_create(sizeof(XLins_arg));
    for(int i=0;i<num;i++){
        if((arg_name=va_arg(ap,char*))==NULL){
            break;
        }
        if((arg_type=va_arg(ap,uint))==0){
            break;
        }
        if((arg_data=va_arg(ap,void*))==NULL){
            break;
        }
        XLins_arg arg;
        arg.type=arg_type;
        strcpy(arg.name,arg_name);
        if(arg_type==ARG_TYPE_INT)arg.data_size=sizeof(int);
        if(arg_type==ARG_TYPE_STR)arg.data_size=strlen((char*)arg_data)+1;

        arg.data=malloc(arg.data_size);
        memcpy(arg.data,arg_data,arg.data_size);

        ll_add_member_tail(&ins->arg_ll,&arg,sizeof(XLins_arg));
    }
    //ins->num=&ins->arg_ll.member_num;
    va_end(ap);
    return ins;
}


XLins * ins_create(const char * op_name,const char * format,...){
    if(op_name==NULL||format==NULL)return NULL;
    va_list ap;
    va_start(ap,format);
    char * arg_name=NULL;
    void * arg_data=NULL;
    XLins * ins=(XLins*)malloc(sizeof(XLins));
    strcpy(ins->op_name,op_name);
    ins->arg_ll=*ll_create(sizeof(XLins_arg));
    char * p=(char*)format;
    for(int i=0;i<strlen(format);i++){
        if(*p==' '){
            p++;
            continue;
        }
        if((arg_name=va_arg(ap,char*))==NULL){
            break;
        }
        if((arg_data=va_arg(ap,void*))==NULL){
            break;
        }

        int type;

        switch (*p) {
        case 'i':
            type=ARG_TYPE_INT;
            break;
        case 's':
            type=ARG_TYPE_STR;
            break;
        case 'f':
            type=ARG_TYPE_FLOAT;
        case 'l':
            type=ARG_TYPE_XLLL;
            break;
        default:
            continue;
            break;
        }
        ins_add_arg(ins,arg_name,type,arg_data);
        p++;
    }
    va_end(ap);
    return ins;
}

//name|type|data
int message(XLpak_ins *pak_ins,const char * op_name,uint num,...){
    if(pak_ins==NULL||op_name==NULL)return 0;
    if(strcmp(op_name,pak_ins->Ins.op_name)!=0)return 0;
    va_list ap;
    va_start(ap,num);
    for(int i=0;i<num;i++){
        char * name=va_arg(ap,char*);
        if(INS_get_par(&pak_ins->Ins,name)==NULL)return 0;
    }
    return 1;
}

uint8_t * arg_type_xlll_to_data(XLll * ll,uint * data_size){
    if(ll==NULL||data_size==NULL)return NULL;
    uint size=sizeof(ll->member_num)+sizeof(ll->member_size);
    size+=ll->member_num*(ll->member_size+sizeof(uint));
    //|member_num|member_size|(data_size|data)...
    uint8_t * data=(uint8_t*)malloc(size);
    int p=0;
    data_add(data,&p,&ll->member_num,sizeof(uint));
    data_add(data,&p,&ll->member_size,sizeof(uint));
    XLll_member * member=ll->head;
    for(int i=0;i<ll->member_num;i++){
        data_add(data,&p,member->data,ll->member_size);

        member=member->next;
    }
    *data_size=size;
    return data;
}

XLll * ll_to_arg_type_xlll(uint8_t* data){
    if(data==NULL)return NULL;
    int p=0;
    uint member_num,member_size;
    data_get(data,&p,&member_num,sizeof(uint));
    data_get(data,&p,&member_size,sizeof(uint));

    XLll * ll=ll_create(member_size);
    for(int i=0;i<member_num;i++){
        uint8_t * member_data=(uint8_t*)malloc(member_size);
        data_get(data,&p,member_data,member_size);
        ll_add_member_tail(ll,member_data,member_size);
    }
    return ll;
}

int ins_add_arg(XLins *ins,const char * name,uint type,void * data){
    if(ins==NULL||name==0)return 0;
    XLins_arg arg;
    strcpy(arg.name,name);
    arg.type=type;
    switch (type) {
    case ARG_TYPE_INT:
        arg.data_size=sizeof(int);
        break;
    case ARG_TYPE_FLOAT:
        arg.data_size=sizeof(float);
        break;
    case ARG_TYPE_STR:
        arg.data_size=strlen((char*)data)+1;
        break;
    case ARG_TYPE_DEVICE_SOURCE:
        arg.data_size=sizeof(XLdevice_source);
        break;
    case ARG_TYPE_XLLL:
        uint data_size;
        arg.data=arg_type_xlll_to_data((XLll*)data,&data_size);
        if(arg.data==NULL)return 0;
        arg.data_size=data_size;
        ll_add_member_tail(&ins->arg_ll,&arg,sizeof(XLins_arg));
        return 1;
        break;

    default:
        return 0;
        break;
    }
    arg.data=malloc(arg.data_size);
    memcpy(arg.data,data,arg.data_size);
    ll_add_member_tail(&ins->arg_ll,&arg,sizeof(XLins_arg));
    return 1;
}

int ins_add_arg_data(XLins *ins,const char * name,void * data,uint datasize){
    if(ins==NULL||name==NULL||data==NULL)return 0;
    XLins_arg arg;
    strcpy(arg.name,name);
    arg.type=ARG_TYPE_DATA;
    arg.data_size=datasize;
    arg.data=malloc(datasize);
    memcpy(arg.data,data,datasize);
    ll_add_member_tail(&ins->arg_ll,&arg,sizeof(XLins_arg));
    return 1;
}

int ins_del_arg(XLins * ins,const char * name){
    if(ins==NULL||name==NULL)return 0;
    XLll_member * member=ins->arg_ll.head;
    for(int i=0;i<ins->arg_ll.member_num;i++){
        XLins_arg * arg=(XLins_arg*)member->data;
        if(strcmp(name,arg->name)==0){
            member=member->next;
            ll_del_member_num(&ins->arg_ll,i);
        }
        else member=member->next;
    }
    return 1;
}

void ins_printf(XLins * ins){
    if(ins==NULL)return;
    XLll_member * member=ins->arg_ll.head;
    for(int i=0;i<ins->arg_ll.member_num;i++){
        XLins_arg * arg=(XLins_arg*)member->data;
        printf("arg%d %s %d ",i+1,arg->name,arg->type);
        if(arg->type==ARG_TYPE_INT)printf("%d\n",*(int*)arg->data);
        member=member->next;
    }
}

int INS_cpy(XLins * dect,XLins * src){
    if(dect==NULL||src==NULL)return 0;
    if(ll_cpy(&dect->arg_ll,&src->arg_ll)<=0)return 0;
    strcpy(dect->op_name,src->op_name);
    return 1;
}

//----------------------------------------------//
//               指令的处理转发                   //
//----------------------------------------------//
int ins_send_to_event(XLpak_ins * ins){
    //printf("INS_SEND_TO_EVENT!:");
    //ins_printf(&ins->Ins);
    if(ins->receiver.mode==SOURCE_SYSTEM){
        if(message(ins,"device_send",0))
        {
            recv_deviceinfo(ins);
        }
        if(message(ins,"device_req",0))
        {
            recv_deviceinfo_req(ins);
        }
        return 1;
    }
    //添加指令到各个监视器中
    extern XLll * monitor_ll;
    if(monitor_ll==NULL)return -1;
    XLll_member * member_now=monitor_ll->head;

    for(int i=0;i<monitor_ll->member_num;i++)
    {
        XLmonitor * monitor_now=(XLmonitor*)member_now->data;
        int num=0;
        XLsource *source;
        source=&monitor_now->receiver;
        //if(monitor_now->receiver.mode==ins->receiver.mode){
        if(source->mode==SOURCE_EVENTID&&source->id==ins->receiver.id)num++;

            /*if(source->mode==SOURCE_APPNAME&&strcmp(source->name,ins->receiver.name)==0){
                num++;
            }*/

        //}
        if(ins->receiver.mode==SOURCE_DEVICE){
            XLdevice * device=device_get_local(source->id);
            if(device!=NULL&&source->id==device->event_id)
            {
                num++;
            }
        }
        if(num==0){
            member_now=member_now->next;
            continue;
        }
        queue_add_ins(&monitor_now->queue_head,ins,0);

        member_now=member_now->next;
    }

    return 1;
}
//----------------------------------------------//
//            指令监视器的基本操作                 //
//----------------------------------------------//
mon_id_t monitor_create(XLsource * receiver)
{
    if(receiver==NULL)return -1;

    extern XLll * monitor_ll;
    if(monitor_ll==NULL)monitor_ll=ll_create(sizeof(XLmonitor));
    XLmonitor monitor_new;
    monitor_new.id=1;
    monitor_new.source_ll=*ll_create(sizeof(XLsource));
    monitor_new.queue_head.queue=NULL;
    monitor_new.receiver=*receiver;
    //添加首个成员
    if(monitor_ll->head==NULL){
        ll_add_member_head(monitor_ll,&monitor_new,sizeof(XLmonitor));
        return 1;
    }
    //添加成员
    XLll_member * member_now=monitor_ll->head;
    int mode=0,mem_front_id=-1;//-1代表位于首位
    //获得新的成员的ID和要插入的位置
    for(int i=0;i<monitor_ll->member_num;i++){
        XLmonitor * monitor_now=(XLmonitor*)member_now->data;
        if(mode==0&&monitor_new.id==monitor_now->id){
            monitor_new.id++;
            mem_front_id=i;
        }
        else if(mode==1&&monitor_new.id==monitor_now->id){
            monitor_new.id++;
            mem_front_id=i;
            mode=0;
        }
        else if(mode==0)mode=1;

        member_now=member_now->next;
    }

    if(mem_front_id==-1)ll_insert_member_front(monitor_ll,&monitor_new,sizeof(XLmonitor),0);
    else ll_insert_member_next(monitor_ll,&monitor_new,sizeof(XLmonitor),mem_front_id);
    return monitor_new.id;

}

void monitor_show(void)
{
    printf("monitor show:\n");
    extern XLll * monitor_ll;
    if(monitor_ll==NULL)return;
    XLll_member * member_now=monitor_ll->head;
    if(member_now==NULL)return;
    for(int i=0;i<monitor_ll->member_num;i++)
    {
        XLmonitor * monitor_now=(XLmonitor*)member_now->data;
        printf("%d ",monitor_now->id);
        member_now=member_now->next;
    }
    printf("\n");
}

int monitor_remove(mon_id_t id)
{
    extern XLll * monitor_ll;
    if(monitor_ll==NULL)return -1;
    XLll_member * member_now=monitor_ll->head;
    for(int i=0;i<monitor_ll->member_num;i++){
        XLmonitor * monitor_now=(XLmonitor*)member_now->data;
        if(monitor_now->id==id){
            monitor_remove_all_member(monitor_now->id);
            member_now=member_now->next;
            ll_del_member_num(monitor_ll,i);
            return 1;
        }
        else member_now=member_now->next;
    }
    return -1;
}

XLmonitor * monitor_get_by_id(mon_id_t id){
    extern XLll * monitor_ll;
    XLll_member * member=ll_get_member_compare(monitor_ll,0,sizeof(mon_id_t),&id);
    if(member==NULL)return NULL;
    return (XLmonitor*)member->data;
}

XLsource *monitor_get_source(mon_id_t id){
    XLmonitor * monitor=monitor_get_by_id(id);
    if(monitor!=NULL)return &monitor->receiver;
    return NULL;
}

XLqueue * monitor_get_queue(mon_id_t monitor_id){
    XLmonitor * monitor=monitor_get_by_id(monitor_id);
    if(monitor==NULL)return NULL;
    if(monitor->queue_head.queue==NULL)return NULL;
    return monitor->queue_head.queue;
}

XLqueue * monitor_get_queue_delay(mon_id_t monitor_id,uint time){
    XLmonitor * monitor=monitor_get_by_id(monitor_id);
    if(monitor==NULL)return NULL;
    for(int i=0;i<time;i++){
        if(monitor->queue_head.queue!=NULL)return monitor->queue_head.queue;
        usleep(1000);
    }
    return NULL;
}

XLpak_ins * monitor_get_pak_ins(mon_id_t monitor_id){
    XLqueue * queue=monitor_get_queue(monitor_id);
    if(queue==NULL)return NULL;
    return &queue->in.pak_ins;
}

XLpak_ins * monitor_get_pak_ins_delay(mon_id_t monitor_id,uint time){
    XLqueue * queue=monitor_get_queue_delay(monitor_id,time);
    if(queue==NULL)return NULL;
    return &queue->in.pak_ins;
}

XLins * monitor_get_ins(mon_id_t monitor_id){
    XLmonitor * monitor=monitor_get_by_id(monitor_id);
    if(monitor==NULL)return NULL;
    if(monitor->queue_head.queue==NULL)return NULL;
    return &monitor->queue_head.queue->in.pak_ins.Ins;
}

int monitor_remove_member(mon_id_t monitor_id){
    XLmonitor * monitor=monitor_get_by_id(monitor_id);
    if(monitor==NULL)return -1;
    if(monitor->queue_head.queue==NULL)return -1;
    return queue_del_head(&monitor->queue_head);
}

int monitor_remove_all_member(mon_id_t monitor_id){
    XLmonitor * monitor=monitor_get_by_id(monitor_id);
    if(monitor==NULL)return -1;
    queue_remove_all(&monitor->queue_head);
    return 1;
}

//----------------------------------------------//
//            对指令的参数进行提取                 //
//----------------------------------------------//

XLins_arg * INS_get_par(XLins * ins,const char * name){
    if(ins==NULL||name==NULL)return NULL;
    XLll_member * member=ins->arg_ll.head;
    for(int i=0;i<ins->arg_ll.member_num;i++){
        XLins_arg * ins_arg=(XLins_arg*)member->data;
        if(strcmp(ins_arg->name,name)==0){
            return ins_arg;
        }
        member=member->next;
    }
    return NULL;
}


int INS_get_par_int(XLins * ins,const char * name){
    XLins_arg * arg=INS_get_par(ins,name);
    if(arg==NULL)return 0;
    return *(int*)arg->data;
}

char * INS_get_par_str(XLins * ins,const char * name){
    XLins_arg * arg=INS_get_par(ins,name);
    if(arg==NULL)return NULL;
    if(arg->type!=ARG_TYPE_STR)return NULL;
    return (char*)arg->data;
}

XLll * INS_get_par_ll(XLins * ins,const char * name){
    XLins_arg * arg=INS_get_par(ins,name);
    if(arg==NULL)return NULL;
    if(arg->type!=ARG_TYPE_XLLL)return NULL;
    return ll_to_arg_type_xlll((uint8_t*)arg->data);
}

XLdevice_source * INS_get_par_device_source(XLins * ins,const char * name){
    XLins_arg * arg=INS_get_par(ins,name);
    if(arg==NULL)return NULL;
    if(arg->type!=ARG_TYPE_DEVICE_SOURCE)return NULL;
    return (XLdevice_source*)arg->data;
}

void * INS_get_par_data(XLins * ins,const char * name,uint * datasize){
    XLins_arg * arg=INS_get_par(ins,name);
    if(arg==NULL)return NULL;
    return ll_to_arg_type_xlll((uint8_t*)arg->data);
}

int source_cmp(XLsource *source1,XLsource *source2){
    int i=0;
    if(source1->mode==source2->mode)i++;
    if(source1->mode==SOURCE_EVENTID&&source1->id==source2->id)i+=1;
    if(source1->mode==SOURCE_APPNAME&&strcmp(source1->name,source2->name)==0)i+=1;
    if(source1->net.ip==source2->net.ip)i+=1;
    if(i==3)return 1;
    return 0;
}

int source_cpy(XLsource * source1,XLsource * source2){
    if(source1==NULL||source2==NULL)return -1;
    source1->id=source2->id;
    source1->mode=source2->mode;
    source1->net.ip=source2->net.ip;
    source1->net.port=source2->net.port;
    if(source2->mode==SOURCE_APPNAME&&strlen(source2->name)<NAME_LENGTH){
        strcpy(source1->name,source2->name);
    }
    return 1;
}

/*-----------------------------------------------------------------------*/
int file_wait_end(FILE * file,int num,int time){
    for(int i=0;i<num;i++){
        getc(file);
    }
    for(int i=0;i<1000;i++){
        char c=fgetc(file);
        if(c==EOF)return 1;
        usleep(time);
    }
    return 0;
}

int file_get_str(FILE * file,const char * str){
    if(str==NULL||file==NULL)return 0;
    int str_len=strlen(str);
    for(int i=0;i<str_len;i++){
        char c=fgetc(file);
        if(c==EOF)return 0;
        if(c!=str[i])i=0;
    }
    return 1;
}

int catch_keyword(const char * str,const char * compare){
    if(str==NULL||compare==NULL)return 0;
    int str_len=strlen(compare),j=0;
    for(int i=0;i<str_len;i++){
        char c=str[j];
        if(c=='\0')return 0;
        if(c>=65&&c<=90&&c!=compare[i]&&c!=compare[i]+32)i=0;
        else if(c>=90&&c<=122&&c!=compare[i]&&c!=compare[i]-32)i=0;
        else if(c!=compare[i])i=0;
        j++;
    }
    return 1;
}

char * file_get_str_front_stop(FILE * file,char front,char stop){
    if(file==NULL)return NULL;
    int mode=0;
    char * str=(char*)malloc(sizeof(char)*64);
    for(int i=0;i<128;i++){
        char c=fgetc(file);
        str[i]=c;
        if(c=='\0'){
            free(str);
            return NULL;
        }
        else if(c!=front&&mode==0)i=0;
        else if(c==front&&mode==0)mode=1;
        else if(c==stop&&mode==1){
            str[i+1]='\0';
            return str;
        }
    }
    return NULL;
}

void str_del_char(const char * str,int p){
    if(str==NULL)return;
    int len=strlen(str)+1;
    char s[len+1],j=0;
    for(int i=0;i<len;){
        if(i!=p){
            s[j]=str[i];
            j++;
        }
        i++;
    }
    strcpy((char*)str,s);
}
/*-------------------------------------------------------------------------*/

