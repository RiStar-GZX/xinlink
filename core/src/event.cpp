#include <event.h>
#include <network.h>

XLll * app_ll;
XLll * event_ll;

int app_add(const char* name,EVENT event)
{
    if(name==NULL||event==NULL)return -1;
    XLapp_contain app_new;
    strcpy(app_new.info.name,name);
    app_new.id=1;
    app_new.event=event;

    extern XLll * app_ll;
    if(app_ll==NULL)app_ll=ll_create(sizeof(XLapp_contain));
    //添加首个成员
    if(app_ll->head==NULL){
        ll_add_member_head(app_ll,&app_new,sizeof(XLapp_contain));
        return 1;
    }
    //添加成员
    XLll_member * member_now=app_ll->head;
    int mode=0,mem_front_id=-1;//-1代表位于首位
    //获得新的成员的ID和要插入的位置
    for(int i=0;i<app_ll->member_num;i++){
        XLapp_contain * app_now=(XLapp_contain*)member_now->data;
        if(mode==0&&app_new.id==app_now->id){
            app_new.id++;
            mem_front_id=i;
        }
        else if(mode==1&&app_new.id==app_now->id){
            app_new.id++;
            mem_front_id=i;
            mode=0;
        }
        else if(mode==0)mode=1;

        member_now=member_now->next;
    }

    if(mem_front_id==-1)ll_insert_member_front(app_ll,&app_new,sizeof(XLapp_contain),0);
    else ll_insert_member_next(app_ll,&app_new,sizeof(XLapp_contain),mem_front_id);
    return app_new.id;
}

int app_remove(app_contain_id_t id)
{
    extern XLll * app_ll;
    if(app_ll==NULL)return -1;
    XLll_member * member_now=app_ll->head;
    for(int i=0;i<app_ll->member_num;i++){
        XLapp_contain * app_now=(XLapp_contain*)member_now->data;
        if(app_now->id==id){
            ll_del_member_num(app_ll,i);
            return 1;
        }
        member_now=member_now->next;
    }
    return -1;
}

XLapp_contain * app_get_by_id(app_contain_id_t id)
{
    extern XLll * app_ll;
    XLll_member * member=ll_get_member_compare(app_ll,FRONTSIZE_APPCONTAIN_ID,sizeof(app_contain_id_t),&id);
    if(member==NULL)return NULL;
    return (XLapp_contain*)member->data;
}

XLapp_contain * app_get_by_name(const char* name)
{
    extern XLll * app_ll;
    int name_len=strlen(name)+1;
    if(name_len>NAME_LENGTH)return NULL;
    XLll_member * member=ll_get_member_compare(app_ll,FRONTSIZE_APPCONTAIN_NAME,name_len,(void*)name);
    if(member==NULL)return NULL;
    return (XLapp_contain*)member->data;
}


void app_show(void)
{
    printf("app show:\n");
    extern XLll * app_ll;
    if(app_ll==NULL)return;
    XLll_member * member_now=app_ll->head;
    if(member_now==NULL)return;
    for(int i=0;i<app_ll->member_num;i++)
    {
        XLapp_contain * app_now=(XLapp_contain*)member_now->data;
        printf("%d ",app_now->id);
        member_now=member_now->next;
    }
    printf("\n");
}

int app_set_event(const char* name,EVENT event)
{
    XLapp_contain * app_get;
    app_get=app_get_by_name(name);
    if(app_get==NULL)return -1;
    app_get->event=event;
    return 1;
}

/*********************ins_event***********************/

event_id_t event_create(XLapp_info * info,EVENT event){
    if(info==NULL)return 0;
    XLsource source;
    source.mode=SOURCE_EVENTID;
    XLevent event_new;
    event_new.id=1;
    event_new.event=event;
    event_new.info=*info;
    extern XLll * event_ll;
    if(event_ll==NULL)event_ll=ll_create(sizeof(XLevent));
    //添加首个成员
    if(event_ll->head==NULL){
        source.id=event_new.id;
        event_new.mon_id=monitor_create(&source);
        ll_add_member_head(event_ll,&event_new,sizeof(XLevent));
        return 1;
    }
    //添加成员
    XLll_member * member_now=event_ll->head;
    int mode=0,mem_front_id=-1;//-1代表位于首位
    //获得新的成员的ID和要插入的位置
    for(int i=0;i<event_ll->member_num;i++){
        XLevent * event_now=(XLevent*)member_now->data;
        if(mode==0&&event_new.id==event_now->id){
            event_new.id++;
            mem_front_id=i;
        }
        else if(mode==1&&event_new.id==event_now->id){
            event_new.id++;
            mem_front_id=i;
            mode=0;
        }
        else if(mode==0)mode=1;

        member_now=member_now->next;
    }

    source.id=event_new.id;
    event_new.mon_id=monitor_create(&source);
    if(mem_front_id==-1)ll_insert_member_front(event_ll,&event_new,sizeof(XLevent),0);
    else ll_insert_member_next(event_ll,&event_new,sizeof(XLevent),mem_front_id);
    return source.id;
}

int event_remove(event_id_t id)
{
    extern XLll * event_ll;
    if(event_ll==NULL)return -1;
    XLll_member * member_now=event_ll->head;
    for(int i=0;i<event_ll->member_num;i++){
        XLevent * event_now=(XLevent*)member_now->data;
        if(event_now->id==id){
            ll_del_member_num(event_ll,i);
            return 1;
        }
        member_now=member_now->next;
    }
    return -1;
}

XLevent * event_get_by_id(event_id_t id)
{
    extern XLll * event_ll;
    XLll_member *member=ll_get_member_compare(event_ll,FRONTSIZE_EVENT_ID,sizeof(event_id_t),&id);
    if(member==NULL)return NULL;
    return (XLevent*)member->data;
}

int event_set_name(event_id_t id,const char* name){
    XLevent * event=event_get_by_id(id);
    if(event==NULL||name==NULL)return 0;
    strcpy(event->info.name,name);
    return 1;
}

void event_show(void){
    extern XLll * event_ll;
    if(event_ll==NULL)return;
    XLll_member * member_now=event_ll->head;
    if(member_now==NULL)return;
    printf("event show:\n");
    for(int i=0;i<event_ll->member_num;i++)
    {
        XLevent * event_now=(XLevent*)member_now->data;
        printf("|%s|%d\n",event_now->info.name,event_now->id);
        member_now=member_now->next;
    }
}

event_id_t event_create_and_run(const char* app_name){
    XLapp_contain * app=app_get_by_name(app_name);
    if(app==NULL)return -1;
    event_id_t event_id=event_create(&app->info,app->event);
    if(event_id<=0)return -1;
    //运行事件(会在另一个线程中运行事件）
    event_run(event_id);
    return event_id;
}


#ifdef PLATFORM_ESP32
void event_thread(void * arg)
#endif
#ifdef PLATFORM_LINUX
    void * event_thread(void * arg)
#endif
{
    XLevent *event=(XLevent*)arg;
    XLsource * source=monitor_get_source(event->mon_id);
    /*while(1){
        XLqueue *member=monitor_get_member_in(event->mon_id);
        if(member==NULL){
            usleep(1000);
            continue;
        }*/
        XLevent_par par;
        par.source=source;
        //par.mode=member->mode;
        //par.pak_in=member->in;
        par.mon_id=event->mon_id;

        event->event(&par);

        monitor_remove_all_member(event->mon_id);
    //}
    event_remove(event->id);
#ifdef PLATFORM_LINUX
    return NULL;
#endif
}

#ifdef PLATFORM_LINUX
int event_run(event_id_t id)
{
    pthread_t thread;
    XLevent * event=event_get_by_id(id);
    if(event==NULL)return -1;
    pthread_create(&thread,NULL,event_thread,event);
    if(!thread)perror("thread");
    return 1;
}
#endif

#ifdef PLATFORM_ESP32
int event_run(event_id_t id)
{
    //pthread_t thread;
    XLevent * event=event_get_by_id(id);
    xTaskCreate(event_thread,"broadcast_recv",4096,event,0,NULL);
    return 1;
}
#endif

/*
int event_set_device_mark(event_id_t id,const char * name,XLsource * source,uint state){
    XLevent * event=event_get_by_id(id);
    if(event==NULL||name==NULL)return 0;
    XLdevice_mark mark_new;
    strcpy(mark_new.name,name);
    if(source!=NULL){
        mark_new.source=*source;
        mark_new.use_source=ENABLE;
    }
    else mark_new.use_source=DISABLE;

    mark_new.state=state;
    XLll_member * member=event->device_mark.head;

    for(int i=0;i<event->device_mark.member_num;i++){
        XLdevice_mark * mark=(XLdevice_mark*)member->data;
        if(strcmp(mark->name,name)==0){
            return 0;
        }
        member=member->next;
    }
    ll_add_member_tail(&event->device_mark,&mark_new,sizeof(XLdevice_mark));
    return 1;
}

int event_remove_device_mark(event_id_t id,const char * name){
    XLevent * event=event_get_by_id(id);
    if(event==NULL||name==NULL)return 0;

    XLll_member * member=event->device_mark.head;

    for(int i=0;i<event->device_mark.member_num;i++){
        XLdevice_mark * mark=(XLdevice_mark*)member->data;
        if(strcmp(mark->name,name)==0){
            ll_del_member(&event->device_mark,i);
            return 1;
        }
        member=member->next;
    }
    return 1;
}*/

/*
int event_send_info(){

}*/
/*
XLpak_info * event_get_info(event_id_t id){
    XLevent * event=event_get_by_id(id);
    if(event==NULL)return 0;
    XLpak_info info;
    ll_cpy(&info.device_mark,&event->device_mark);
    strcpy(info.op_name,event->);
}


int send_to_link_event(XLpak_info * info){

    if(info==NULL)return 0;
    printf("SEND TO LINK EVENT!\n");
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
        if(monitor_now->receiver.mode==info->receiver.mode){
            if((source->mode==EVENT_ID||source->mode==ACCESS)
                &&source->id==info->receiver.id)num++;
            if(source->mode==EVENT_NAME&&strcmp(source->name,info->receiver.name)==0){
                num++;
            }
        }
        if(num!=1){
            member_now=member_now->next;
            continue;
        }

        XLqueue_in in;
        in.pak_info=*info;
        queue_add(QUEUE_TYPE_LINK_INFO,&monitor_now->queue_head,&in,0);
        member_now=member_now->next;
        printf("YES!\n");
    }
    return 1;
}
*/
