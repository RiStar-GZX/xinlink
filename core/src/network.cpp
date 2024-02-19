#include<network.h>
#include <core.h>
#include <event.h>
#include <core.h>

XLqueue_head send_queue_head;
XLqueue_head recv_queue_head;
XLqueue_head broadcast_queue_head;
XLqueue_head total_queue_head;


uint16_t pak_id=0;


//----------------------------------------------//
//               网络数据包制作工具                //
//----------------------------------------------//

//将数据包中的数据（*data）用16进制打印到屏幕上
void data_show(uint8_t *data,uint size){
    uint8_t *data_p=data;
    for(int i=0;i<size;i++){
        if(*data_p=='\0')printf("!");
        else printf("%c",*data_p);
        data_p++;
    }
    printf("\n");
}

//从一个结构体（*_struct）中复制（size）个大小的数据到另一个地址（* data），并将指针（*p）大小加上size
void data_add(uint8_t *data,int * p,void * _struct,uint size){
    if(data==NULL||_struct==NULL)return;
    uint8_t * struct_p=(uint8_t*)_struct;
    uint8_t * data_p=data;
    if(p!=NULL)data_p+=*p;
    int i;
    for(i=0;i<size;i++){
        *data_p=*(uint8_t*)struct_p;
        data_p++;
        struct_p++;
    }
    if(p!=NULL)*p+=i;
}

//将字符串（*str）复制到另一个地址（* data），并将指针（*p）大小加上字符串的大小
void data_add_str(uint8_t *data,int * p,char* str){
    data_add(data,p,str,strlen(str)+1);
}

//从一个地址数据（*data）中复制（size）个位到（* _struct），并将指针（*p）大小加上size
void data_get(uint8_t *data,int * p,void * _struct,uint size){
    if(data==NULL||_struct==NULL)return;
    uint8_t * data_p=data,*struct_p=(uint8_t*)_struct;
    if(p!=NULL)data_p+=*p;
    int i;
    for(i=0;i<size;i++){
        *struct_p=*data_p;
        data_p++;
        struct_p++;
    }
    if(p!=NULL)*p+=i;
}

//从一个地址数据（*data）中自动发现字符串并复制到（* str），并将指针（*p）大小加上size
int data_get_str(uint8_t *data,int *p,char * str){
    int len=strlen((char*)data+*p)+1;
    if(len>NAME_LENGTH)return 0;
    data_get(data,p,str,len);
    return 1;
}

char * data_get_str_p(uint8_t *data,int *p){
    int len=strlen((char*)data+*p)+1;
    if(len>NAME_LENGTH)return NULL;
    char *str=(char*)malloc(len);
    data_get(data,p,str,len);
    return str;
}

//----------------------------------------------//
//                   本地信息                    //
//----------------------------------------------//

//获得本机的网络信息，返回以XLnet网络结构体
XLnet network_get_local_info(void)
{
    XLnet net;
    net.port=NETWORK_PORT;

    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;
    char ip_address[INET_ADDRSTRLEN];

    if (getifaddrs(&ifap) == -1) {
        perror("getifaddrs error");
        exit(1);
    }

    for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr != NULL && ifa->ifa_addr->sa_family == AF_INET) {
            sa = (struct sockaddr_in *) ifa->ifa_addr;
            inet_ntop(AF_INET, &sa->sin_addr, ip_address, sizeof(ip_address));
            if(strcmp(ifa->ifa_name,"lo")!=0){
                freeifaddrs(ifap);
                net.ip=inet_addr(ip_address);
                return net;
            }
        }
    }

    freeifaddrs(ifap);
    exit(1);
}

//----------------------------------------------//
//                  网络包发送                   //
//----------------------------------------------//

//发送TCP数据包（现在是UDP没改），接收者为XLnet网络结构体对应的网络，发送的内容是(*data),大小为（datasize）
int TCP_send(XLnet * net,uint8_t * data,int datasize)
{
    //ERROR
    if(net==NULL)return 0;

    //create socket
    int sockfd;
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("fail to socket");
        return 0;
    }

    //printf("sockfd = %d\n", sockfd);

    //第二步：填充服务器网络信息结构体 sockaddr_in
    struct sockaddr_in serveraddr;
    socklen_t addrlen = sizeof(serveraddr);

    serveraddr.sin_family = AF_INET; //协议族，AF_INET：ipv4网络协议

    serveraddr.sin_addr.s_addr = net->ip;//ip地址
    serveraddr.sin_port = htons(net->port);

    if(sendto(sockfd, data, datasize, 0, (struct sockaddr *)&serveraddr, addrlen) == -1)
    {
        perror("fail to sendto");
        return 0;
    }
    //printf("TCP send\n");
    //第四步：关闭套接字文件描述符
    close(sockfd);
    return 1;
}

//发送广播数据包，发送的内容是(*buf),大小为（bufsize）
int Broadcast_send (void *buf,int bufsize)
{
     int sockfd;
     struct sockaddr_in broadcataddr; //服务器网络信息结构体
     socklen_t addrlen = sizeof(broadcataddr);
     //创建套接字
     if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
     {
         perror("fail to socket");
         return 0;
     }
     //设置为允许发送广播权限
     int on = 1;
     if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) < 0)
     {
         perror("fail to setsockopt");
         return 0;
     }
     //填充广播信息结构体
     broadcataddr.sin_family = AF_INET;
     broadcataddr.sin_addr.s_addr =inet_addr("255.255.255.255"); //192.168.3.255 255.255.255.255
     broadcataddr.sin_port = htons(NETWORK_BROADCAST_PORT);
     //第四步:进行通信

     if(sendto(sockfd, buf,bufsize, 0, (struct sockaddr *)&broadcataddr, addrlen) < 0)
     {
         perror("fail to sendto");
         return 0;
     }
     //关闭套接字文件描述符
     close(sockfd);
     return 0;
}
//----------------------------------------------//
//                 网络包服务线程                 //
//----------------------------------------------//

//指令发送线程，读取发送队列中的数据包结构体(XLpak_xxx)，将它们转换为buf然后通过TCP发送
void * ins_send_thread(void * arg){
     while (1) {
             int delay=DISABLE;
        while (1) {
            extern XLqueue_head send_queue_head;
            if(send_queue_head.queue==NULL){delay=ENABLE;break;}
            XLqueue * queue_now=send_queue_head.queue;
            //发送的包为指令
            if(queue_now->mode==QUEUE_TYPE_INS){
                XLcore *core=core_get_by_net(&queue_now->in.pak_ins.base.net_receiver);
                if(core==NULL)break;
                int size=0;

                uint8_t * data=pak_ins_to_buf(&queue_now->in.pak_ins,&size);
                if(size==0||data==NULL)break;
                TCP_send(&core->net,data,size+1000);
                free(data);
                break;
            }
            //发送的包为对接同意
            else if(queue_now->mode==QUEUE_TYPE_CONNECT){
                int size=0;
                uint8_t * data=pak_connect_to_buf(&queue_now->in.pak_connect,&size);
                TCP_send(&queue_now->in.pak_connect.base.net_receiver,data,size);

                free(data);
                break;
            }
            /*else if(queue_now->mode==QUEUE_TYPE_LINK_INFO){
                uint size=0;
                uint8_t *data=pak_info_to_buf(&queue_now->in.pak_info,&size);
                TCP_send(&queue_now->in.pak_info.base.net_receiver,data,size);
                free(data);
                break;
            }*/
            else {
                break;
            }
        }
        queue_del_head(queue_send());

        if(delay)usleep(10000);//没有这个延时程序会崩
    }
}

int decode_ins_receive_packet(XLnet *net, uint8_t *data)
{
    uint16_t mode = *(uint16_t *)data;
    uint16_t type = mode >> 12;
    // 数据包的模式是发送
    if (type == NETWORK_MODE_CONNECT)
    {
        XLpak_connect *pak_connect;
        pak_connect = buf_to_pak_connect(data);
        uint16_t connect_mode = mode & SEE_CONNECT_ONLY;
        // 对方发送核心信息
        if (connect_mode == PAK_CONNECT_SEND_CORE)
        {
            if (core_add(&pak_connect->base.net_sender, pak_connect->core_name) != -2)
                printf("发现新核心:%s\n", pak_connect->core_name);
            else
                printf("发现重复核心:%s\n", pak_connect->core_name);
        }
        // 对方发送核心对接请求
        if (connect_mode == PAK_CONNECT_REQUEST)
        {
            XLcore *core = core_get_by_net(&pak_connect->base.net_sender);
            if (core == NULL)
                return -1;
            core->safety = CORE_STATE_VERIFIED;
            printf("已设置核心%s为安全核心\n", core->name);
            network_core_connect_require_receive(pak_connect);
        }
        // 对方同意核心对接
        if (connect_mode == PAK_CONNECT_ACCEPT)
        {
            network_core_connect(pak_connect);
            printf("核心已通过验证\n");
        }
        // 对方取消核心对接
        if (connect_mode == PAK_DISCONNECT)
        {
            XLcore *core = core_get_by_net(&pak_connect->base.net_sender);
            if (core == NULL)
                return -1;
            core->safety = CORE_STATE_UNVERIFIED;
            printf("与核心%s的连接断开\n", core->name);
        }
        free(pak_connect);
    }
    // 数据包的模式是接收
    else if (type == NETWORK_MODE_INS/*&& network_safe(net) > 0*/)
    {
        printf("接收到了一条指令\n");
        XLpak_ins *pak_ins;
        if ((pak_ins = buf_to_pak_ins(data)) != NULL)
            queue_add_ins(queue_total(), pak_ins, 0);
        free(pak_ins);
    }
    return 1;
}

//指令接收线程，将网络buf转化为数据包结构体（XLpak_xxx）,对它进行处理或转发
void * ins_recv_thread(void * arg)
{
    int sockfd; //文件描述符
    struct sockaddr_in serveraddr; //服务器网络信息结构体
    socklen_t addrlen = sizeof(serveraddr);
    //创建套接字
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("fail to socket");
        exit(1);
    }
    //填充服务器网络信息结构体
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = network_get_local_info().ip;     //后面要传参
    serveraddr.sin_port = htons(network_get_local_info().port);
    //将套接字与服务器网络信息结构体绑定
    if(bind(sockfd, (struct sockaddr *)&serveraddr, addrlen) < 0)
    {
        perror("fail to bind");
        exit(1);
    }
    uint8_t * data=(uint8_t *)malloc(sizeof(uint8_t)*PAK_MAX_SIZE);
    while(1)
    {
        //进行通信
        memset(data,0,sizeof(uint8_t)*PAK_MAX_SIZE);
        struct sockaddr_in clientaddr;
        if(recvfrom(sockfd, data, PAK_MAX_SIZE, 0, (struct sockaddr *)&clientaddr, &addrlen) < 0)
        {
            perror("fail to recvfrom");
        }

        IP ip;
        ip=(IP)clientaddr.sin_addr.s_addr;
        XLcore *core =core_get_by_ip(ip);
        if(core!=NULL)
            printf("来自核心:%s:\n",core->name);
        else
            printf("来自核心:未知:\n");

        //printf("[%s - %d]:", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));    //调试

        XLnet net;
        net.port=clientaddr.sin_port;
        net.ip=clientaddr.sin_addr.s_addr;
        //if(((uint8_t *)&net.ip)[3]!=255)printf("!255:%d\n",((uint8_t *)&net.ip)[3]);
        decode_ins_receive_packet(&net,data);
    }

    //关闭文件描述符
    free(data);
    close(sockfd);
}

//广播发送线程，读取广播发送队列中的数据包结构体(XLpak_xxx)，将它们转换为buf然后通过广播发送
void * broadcast_send_thread(void * arg){
    while (1) {
        while (1) {
            extern XLqueue_head broadcast_queue_head;
            if(broadcast_queue_head.queue==NULL)break;
            XLqueue * queue_now=broadcast_queue_head.queue;
            //发送的包为对接
            if(queue_now->mode==NETWORK_MODE_CONNECT) {
                int size=0;
                uint8_t * data=pak_connect_to_buf(&queue_now->in.pak_connect,&size);
                //printf("BC send!\n");
                Broadcast_send(data,size);
                free(data);
                break;
            }
            else {
                break;
            }
        }
        queue_del_head(queue_broadcast());
        usleep(1000000);
    }
}

int decode_broadcast_receive_packet(uint8_t *data){
    uint16_t mode=*(uint16_t*)data;
    uint16_t type=mode>>12;
    uint16_t connect=mode&SEE_CONNECT_ONLY;
    if(type==NETWORK_MODE_CONNECT&&connect==PAK_CONNECT_SEND_CORE_REQUIRE){
        XLpak_connect *pak_connect;
        if((pak_connect=buf_to_pak_connect(data))==NULL)return -1;
        if(pak_connect->base.net_sender.ip==network_get_local_info().ip)return -1;
        //核心不重复
        if(network_core_find_receive(pak_connect)!=-2)printf("发现新核心:%s\n",pak_connect->core_name);
        else printf("发现重复核心:%s\n",pak_connect->core_name);
        free(pak_connect);
    }
    return 1;
}

//广播接收线程，将网络buf转化为数据包结构体（XLpak_connect），对它进行处理
void * broadcast_receive_thread(void * arg)
{
    int sockfd;
    struct sockaddr_in broadcataddr;
    socklen_t addrlen = sizeof(broadcataddr);
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("fail to socket");
        exit(1);
    }
    broadcataddr.sin_family = AF_INET;
    broadcataddr.sin_addr.s_addr = inet_addr("255.255.255.255");
    broadcataddr.sin_port = htons(NETWORK_BROADCAST_PORT);
    if(bind(sockfd, (struct sockaddr *)&broadcataddr, addrlen) < 0)
    {
        perror("fail to bind");
        exit(1);
    }
    uint8_t * data=(uint8_t*)malloc(sizeof(uint8_t)*PAK_MAX_SIZE);
    struct sockaddr_in sendaddr;
    while(1){
        memset(data,0,sizeof(uint8_t)*PAK_MAX_SIZE);
        if(recvfrom(sockfd, data,PAK_MAX_SIZE, 0, (struct sockaddr *)&sendaddr, &addrlen) < 0)
        {
            perror("fail to recvfrom");
        }
        printf("%s\n",data);

        XLnet net;
        net.port=sendaddr.sin_port;
        net.ip=sendaddr.sin_addr.s_addr;
        //if(((uint8_t *)&net.ip)[3]!=255)printf("=255:%d\n",((uint8_t *)&net.ip)[3]);
        decode_broadcast_receive_packet(data);
    }
    return NULL;
}

//总线程，用于处理收到的指令
void *total_thread(void * arg)
{
    while (1) {
        XLqueue_head * queue_head=queue_total();
        if(queue_head!=NULL&&queue_head->queue!=NULL){
            if(queue_head->queue->mode==QUEUE_TYPE_INS){
                ins_send_to_event(&queue_head->queue->in.pak_ins);
            }
        }
        queue_del_head(queue_total());
        usleep(10000);
    }
}

//线程初始化
int network_init(void)
{
    pthread_t out;
    pthread_create(&out,NULL,out_times,NULL);
    pthread_t send,receive,total,broad_send,broad_recv;
    pthread_create(&send,NULL,ins_send_thread,NULL);
    if(!send)perror("send thread");
    pthread_create(&receive,NULL,ins_recv_thread,NULL);
    if(!receive)perror("receive thread");
    pthread_create(&total,NULL,total_thread,NULL);
    if(!total)perror("total thread");
    pthread_create(&broad_send,NULL,broadcast_send_thread,NULL);
    if(!broad_send)perror("total thread");
    pthread_create(&broad_recv,NULL,broadcast_receive_thread,NULL);
    if(!broad_recv)perror("total thread");

    return 1;
}
//----------------------------------------------//
//                      队列                     //
//----------------------------------------------//

//删除队列中的队头成员，输入的是队头的指针
int queue_del_head(XLqueue_head *head)
{
    if(head==NULL)return -1;
    XLqueue * queue_head=head->queue;
    if(queue_head==NULL)return -1;

    if(queue_head->next==queue_head){
        free(queue_head);
        head->queue=NULL;
        return 1;
    }
    queue_head->next->front=queue_head->front;
    queue_head->front->next=queue_head->next;
    XLqueue * tmp=queue_head->next;
    free(queue_head);
    head->queue=tmp;
    return 1;
}

int queue_del_queue(XLqueue_head *head,XLqueue * queue)
{
    if(head==NULL)return -1;
    XLqueue * queue_now=head->queue;
    if(queue_now==NULL)return -1;

    if(queue_now==queue&&queue_now->next==queue_now){
        free(queue_now);
        head->queue=NULL;
        return 1;
    }
    while(1)
    {
        if(queue_now==queue){
            queue_now->next->front=queue_now->front;
            queue_now->front->next=queue_now->next;
            free(queue_now);
            return 1;
        }
        if(queue_now->next==head->queue)return -1;
        queue_now=queue_now->next;
    }
    return -1;
}

int queue_remove_all(XLqueue_head * head){
    if(head==NULL)return -1;
    while(1)
    {
        if(queue_del_head(head)<=0)return 1;
    }
}

//添加成员到队列，*head为队列的队头的指针，in为成员，mode设置成员类型，level设置优先级
int queue_add(int mode,XLqueue_head * head,XLqueue_in * in,LEVEL level)
{
    if(in==NULL||head==NULL)return -1;
    //if(mode!=QUEUE_TYPE_CONNECT&&mode!=QUEUE_TYPE_INS&&mode!=QUEUE_TYPE_SIGN)return -1;

    XLqueue * queue_new=(XLqueue*)malloc(sizeof (XLqueue));
    memset(queue_new,0,sizeof(XLqueue));
    queue_new->mode=mode;
    queue_new->level=level;
    queue_new->in=*in;
    if(mode==QUEUE_TYPE_CONNECT){
        strcpy(queue_new->in.pak_connect.core_name,in->pak_connect.core_name);
    }
    if(mode==QUEUE_TYPE_INS){
        //ll_cpy(&queue_new->in.pak_ins.Ins.arg_ll,&in->pak_ins.Ins.arg_ll);
    }

    if(head->queue==NULL){
        head->queue=queue_new;
        head->queue->front=head->queue;
        head->queue->next=head->queue;
        return 1;
    }

    XLqueue * queue_head=head->queue;

    XLqueue * queue_now=queue_head;
    while (1) {
        if(level>queue_now->level){
            queue_new->next=queue_now;
            queue_new->front=queue_now->front;

            queue_now->front->next=queue_new;
            queue_now->front=queue_new;

            if(queue_now==queue_head)head->queue=queue_new;
            return 1;
        }
        if(queue_now->next==queue_head)
        {
            queue_new->next=queue_now->next;
            queue_new->front=queue_now;

            queue_now->next->front=queue_new;
            queue_now->next=queue_new;
            return 1;
        }
        queue_now=queue_now->next;
    }

    return 0;
}

//添加INS成员到队列，*head为队列的队头的指针，*ins为成员，level设置优先级
int queue_add_ins(XLqueue_head * head,XLpak_ins * ins,LEVEL level){
    XLqueue_in in;
    in.pak_ins=*ins;
    return queue_add(QUEUE_TYPE_INS,head,&in,level);
}

//添加CONNECT成员到队列，*head为队列的队头的指针，*ins为成员，level设置优先级
int queue_add_connect(XLqueue_head * head,XLpak_connect * connect,LEVEL level){
    XLqueue_in in;
    in.pak_connect=*connect;
    return queue_add(QUEUE_TYPE_CONNECT,head,&in,level);
}

//展示队列中的成员（仅作调试用）
void queue_show(XLqueue_head * head){
    if(head==NULL)return;
    XLqueue * queue_head=head->queue;
    if(queue_head==NULL)
    {
        printf("no!\n");
        return;
    }
    printf("list:");
    XLqueue * ins_now=queue_head;
    while (1) {
        if(ins_now->next==queue_head)break;

        ins_now=ins_now->next;
    }
    printf("\n");
}

//总队列的快捷调用
XLqueue_head * queue_total(void)
{
    extern XLqueue_head total_queue_head;
    return &total_queue_head;
}

//发送队列的快捷调用
XLqueue_head * queue_send(void)
{
    extern XLqueue_head send_queue_head;
    return &send_queue_head;
}

//广播发送队列的快捷调用
XLqueue_head * queue_broadcast(void)
{
    extern XLqueue_head broadcast_queue_head;
    return &broadcast_queue_head;
}

//接收队列的快捷调用
XLqueue_head * queue_recv(void)
{
    extern XLqueue_head recv_queue_head;
    return &recv_queue_head;
}

//队列初始化
int queue_init(void){
    queue_total()->queue=NULL;
    queue_recv()->queue=NULL;
    queue_send()->queue=NULL;
    queue_broadcast()->queue=NULL;
    return 1;
}

//----------------------------------------------//
//            数据包结构体和buf的转换              //
//----------------------------------------------//

void data_add_source(uint8_t * data,int * p,XLsource * source){
    if(source==NULL||data==NULL||p==NULL)return;
    data_add(data,p,&source->mode,sizeof(uint32_t));
    data_add(data,p,&source->id,sizeof(uint32_t));
    data_add(data,p,&source->net,sizeof(XLnet));
    //if(source->mode==SOURCE_APPNAME)
        data_add_str(data,p,source->name);
}

void data_get_source(uint8_t * data,int * p,XLsource * source){
    data_get(data,p,&source->mode,sizeof(uint32_t));
    data_get(data,p,&source->id,sizeof(uint32_t));
    data_get(data,p,&source->net,sizeof(XLnet));
    if(/*source->mode==SOURCE_APPNAME&&*/data_get_str(data,p,source->name)==0){
        free(source);
        return;
    }
}

uint8_t * pak_ins_to_buf(XLpak_ins *pak_ins,int * size){
    if(pak_ins==NULL||size==NULL)return NULL;
    int pak_size;
//度量数据包大小
    //度量数据包头部大小
    pak_size=sizeof(XLpak_base)+2*SIZE_SOURCE_WITHOUT_NAME+100;
    //加上发送者接收者名字的大小
    /*if(pak_ins->sender.mode==SOURCE_APPNAME)
    {*/
        //if(pak_ins->sender.name!=NULL)pak_size+=strlen(pak_ins->sender.name)+1;
        //else return NULL;
    /*}
    if(pak_ins->receiver.mode==SOURCE_APPNAME)
    {*/
        //if(pak_ins->receiver.name!=NULL)pak_size+=strlen(pak_ins->receiver.name)+1;
        //else return NULL;
    //}
    //加上参数个数的标记
    pak_size+=sizeof(uint);
    XLll_member * member=pak_ins->Ins.arg_ll.head;
    for(int i=0;i<pak_ins->Ins.arg_ll.member_num;i++){
        XLins_arg * arg=(XLins_arg*)member->data;
        pak_size+=sizeof(uint32_t)*2;
        pak_size+=strlen(arg->name)+1;
        pak_size+=arg->data_size;
        member=member->next;
    }

//申请内存空间，写入数据
    uint8_t *data=(uint8_t*)malloc(pak_size);
    int p=0; //设置写入位置从零开始

    //添加头部
    data_add(data,&p,&pak_ins->base,sizeof(XLpak_base));
    //添加发送者接收者信息
    data_add_source(data,&p,&pak_ins->sender);
    data_add_source(data,&p,&pak_ins->receiver);

    data_add(data,&p,&pak_ins->mode,sizeof(uint));
    data_add(data,&p,&pak_ins->mark,sizeof(uint));
    //添加操作名
    data_add_str(data,&p,pak_ins->Ins.op_name);
    //添加参数个数
    data_add(data,&p,&pak_ins->Ins.arg_ll.member_num,sizeof(uint));
    //添加参数
    member=pak_ins->Ins.arg_ll.head;
    for(int i=0;i<pak_ins->Ins.arg_ll.member_num;i++){
        XLins_arg * arg=(XLins_arg*)member->data;
        uint name_len=strlen(arg->name)+1;
        data_add(data,&p,&name_len,sizeof(uint));
        data_add(data,&p,&arg->data_size,sizeof(uint));
        data_add(data,&p,&arg->type,sizeof(uint8_t));
        data_add_str(data,&p,arg->name);
        data_add(data,&p,arg->data,arg->data_size);
        member=member->next;
    }

    *size=p;    //返回包大小

    return data;
}

XLpak_ins * buf_to_pak_ins(uint8_t * buf){
    if(buf==NULL)return NULL;
    XLpak_ins * pak_ins=(XLpak_ins*)malloc(sizeof(XLpak_ins));
    int p=0;    //读取位置设为0
    //读出头部

    data_get(buf,&p,pak_ins,sizeof(XLpak_base));

    //读出发送者接受者信息
    data_get_source(buf,&p,&pak_ins->sender);
    data_get_source(buf,&p,&pak_ins->receiver);

    data_get(buf,&p,&pak_ins->mode,sizeof(uint));
    data_get(buf,&p,&pak_ins->mark,sizeof(uint));

    //读出操作名
    data_get_str(buf,&p,pak_ins->Ins.op_name);
    //读出参数

    //printf("读出参数\n");
    int argc=0;
    data_get(buf,&p,&argc,sizeof(uint));
    pak_ins->Ins.arg_ll=*ll_create(sizeof(XLins_arg));
    for(int i=0;i<argc;i++){
        XLins_arg arg;
        uint name_len=0;
        data_get(buf,&p,&name_len,sizeof(uint));
        data_get(buf,&p,&arg.data_size,sizeof(uint));
        data_get(buf,&p,&arg.type,sizeof(uint8_t));
        data_get_str(buf,&p,arg.name);
        arg.data=malloc(arg.data_size);
        data_get(buf,&p,arg.data,arg.data_size);
        ll_add_member_tail(&pak_ins->Ins.arg_ll,&arg,sizeof(XLins_arg));
    }


    return pak_ins;
}

//将buf转换为网络数据结构体（XLpak_connect）
XLpak_connect * buf_to_pak_connect(uint8_t* buf){
    if(buf==NULL)return NULL;
    int p=0;
    XLpak_connect * pak_connect=(XLpak_connect*)malloc(sizeof(XLpak_connect));

    data_get(buf,&p,pak_connect,sizeof(XLpak_base));
    if((data_get_str(buf,&p,pak_connect->core_name))==0){
        free(pak_connect);
        return NULL;
    }
    return pak_connect;
}

//将网络数据结构体（XLpak_connect）转换为buf
uint8_t * pak_connect_to_buf(XLpak_connect * pak_connect,int * size){
    if(pak_connect==NULL||size==NULL||pak_connect->core_name==NULL)return NULL;
    int p=0,pak_size;
    pak_size=sizeof(XLpak_base);
    pak_size+=strlen(pak_connect->core_name)+1;
    uint8_t * data=(uint8_t*)malloc(pak_size);
    data_add(data,&p,pak_connect,sizeof(XLpak_base));
    data_add_str(data,&p,pak_connect->core_name);
    *size=pak_size;
    return data;
}

//----------------------------------------------//
//                  网络接口                     //
//----------------------------------------------//

//网络检查，看看传入数据包标记的ip是否和传入的主机ip一致
int network_check(uint8_t * data,XLnet * net){
    if(data==NULL||net==NULL)return -2;
    XLpak_base base;
    data_get(data,NULL,&base,sizeof(XLpak_base));
    if(base.net_sender.ip!=net->ip)return -1;
    return 1;
}

int network_safe(XLnet *net){
    if(net==NULL)return -1;
    XLcore * core=core_get_by_net(net);
    if(core==NULL||core->safety!=CORE_STATE_VERIFIED)return -1;
    return 1;
}

//广播请求其他核心发送核信息（A1） 广播
int network_core_find_send(void){
    XLpak_connect pak;
    pak.base.net_sender=network_get_local_info();
    pak.base.mode=PAK_CONNECT_SEND_CORE_REQUIRE+(NETWORK_MODE_CONNECT<<12);

    XLcore * mycore=core_get_by_id(CORE_MYSELF_ID);
    if(mycore==NULL)return -1;
    strcpy(pak.core_name,mycore->name);
    pak.base.pak_size=sizeof(XLpak_connect)-sizeof(char*)+strlen(mycore->name)+1;
    queue_add_connect(queue_broadcast(),&pak,0);
    return 1;
}

//传入对方广播申请的数据包结构体，回复自己的核信息（B1）TCP
int network_core_find_receive(XLpak_connect * pak_connect){
   if(pak_connect==NULL)return -1;
   if(pak_connect->core_name==NULL)return -1;
    int ret;
    ret=core_add(&pak_connect->base.net_sender,pak_connect->core_name);

    XLpak_connect pak;
    pak.base.net_sender=network_get_local_info();
    pak.base.net_receiver=pak_connect->base.net_sender;

    pak.base.mode=PAK_CONNECT_SEND_CORE+(NETWORK_MODE_CONNECT<<12);

    XLcore * mycore=core_get_by_id(CORE_MYSELF_ID);
    if(mycore==NULL)return -1;
    strcpy(pak.core_name,mycore->name);
    pak.base.pak_size=sizeof(XLpak_connect)-sizeof(char*)+strlen(mycore->name)+1;
    queue_add_connect(queue_send(),&pak,0);
    return ret;
}

//发送核心对接请求（A1）TCP，传入要对接的核心的id
int network_core_connect_require_send(core_id_t core_id){
    if(core_id==1)return -1;
    XLcore * core=core_get_by_id(core_id);
    if(core==NULL)return -1;
    XLpak_connect pak;
    pak.base.net_sender=network_get_local_info();
    pak.base.net_receiver=core->net;
    pak.base.mode=PAK_CONNECT_REQUEST+(NETWORK_MODE_CONNECT<<12);

    XLcore * mycore=core_get_by_id(CORE_MYSELF_ID);
    strcpy(pak.core_name,mycore->name);
    pak.base.pak_size=sizeof(XLpak_connect)-sizeof(char*)+strlen(mycore->name)+1;
    core->safety=CORE_STATE_WAITTING;
    queue_add_connect(queue_send(),&pak,0);
    return 1;
}

//传入对方连接申请的数据包结构体，回复同意或不同意（B1）TCP
int network_core_connect_require_receive(XLpak_connect * pak_connect){
    if(pak_connect==NULL)return -1;
    XLcore *core=core_get_by_net(&pak_connect->base.net_sender);
    if(core==NULL)return -1;
    //缺少安全性验证
    core->safety=CORE_STATE_VERIFIED;

    XLpak_connect pak;
    pak.base.net_sender=network_get_local_info();
    pak.base.mode=PAK_CONNECT_ACCEPT+(NETWORK_MODE_CONNECT<<12);
    pak.base.net_receiver=pak_connect->base.net_sender;

    XLcore * mycore=core_get_by_id(CORE_MYSELF_ID);
    strcpy(pak.core_name,mycore->name);
    pak.base.pak_size=sizeof(XLpak_connect)-sizeof(char*)+strlen(mycore->name)+1;
    queue_add_connect(queue_send(),&pak,0);
    return 1;
}

//获得对方对接许可，设置对方的核心为安全核心(A2)
int network_core_connect(XLpak_connect * pak_connect){
    if(pak_connect==NULL)return -1;
    XLcore *core=core_get_by_net(&pak_connect->base.net_sender);
    if(core==NULL)return -1;
    //缺少网络验证
    if(core->safety!=CORE_STATE_WAITTING)return -1;
    core->safety=CORE_STATE_VERIFIED;
    return 1;
}

//断开与核心的连接，传入要断开的核心的ID
int network_core_disconnect_send(core_id_t core_id){
    if(core_id==1)return -1;
    XLcore * core=core_get_by_id(core_id);
    if(core==NULL)return -1;
    XLpak_connect pak;
    pak.base.net_sender=network_get_local_info();
    pak.base.net_receiver=core->net;
    pak.base.mode=PAK_DISCONNECT+(NETWORK_MODE_CONNECT<<12);

    XLcore * mycore=core_get_by_id(CORE_MYSELF_ID);
    strcpy(pak.core_name,mycore->name);
    pak.base.pak_size=sizeof(XLpak_connect)-sizeof(char*)+strlen(mycore->name)+1;
    core->safety=CORE_STATE_UNVERIFIED;
    queue_add_connect(queue_send(),&pak,0);
    return 1;
}

//向指定的核心发送命令
int network_send_ins(XLsource * sender,XLsource *receiver,XLins * ins){
    if(ins==NULL)return 0;
    XLcore * core=core_get_by_net(&receiver->net);
    if(core==NULL)return 0;

    XLpak_ins * pak_ins=(XLpak_ins*)malloc(sizeof(XLpak_ins));
    pak_ins->base.mode=SOURCE_EVENTID+(SOURCE_EVENTID<<4)+(NETWORK_MODE_INS<<12);
    pak_ins->base.net_sender=core_get_by_id(CORE_MYSELF_ID)->net;
    pak_ins->base.net_receiver=core->net;

    pak_ins->sender=*sender;
    pak_ins->sender.mode=SOURCE_EVENTID;
    pak_ins->sender.net=pak_ins->base.net_sender;
    pak_ins->receiver=*receiver;

    pak_ins->Ins=*ins;
    queue_add_ins(queue_send(),pak_ins,0);
    return 1;
}

int network_send_ins2(unknow_id_t id,bool event0_dev1,XLsource *receiver,uint mode,uint mark,XLins * ins){
    if(ins==NULL||receiver==NULL)return 0;
    XLcore * core=core_get_by_net(&receiver->net);
    if(core==NULL)return 0;
    XLevent * event;
    XLsource sender;
    sender.net=network_get_local_info();
    sender.id=id;
    if(event0_dev1==1){
        XLdevice * device=device_get_local(id);
        if(device==NULL)return 0;
        event=event_get_by_id(device->event_id);
        sender.mode=SOURCE_DEVICE;
    }
    else {
        event=event_get_by_id(id);
        sender.mode=SOURCE_EVENTID;
    }
    if(event==NULL)return 0;

    XLpak_ins * pak_ins=(XLpak_ins*)malloc(sizeof(XLpak_ins));
    pak_ins->base.mode=SOURCE_EVENTID+(SOURCE_EVENTID<<4)+(NETWORK_MODE_INS<<12);
    pak_ins->base.net_sender=core_get_by_id(CORE_MYSELF_ID)->net;
    pak_ins->base.net_receiver=core->net;

    pak_ins->sender=sender;
    pak_ins->receiver=*receiver;
    pak_ins->mode=mode;
    pak_ins->mark=mark;

    pak_ins->Ins=*ins;
    queue_add_ins(queue_send(),pak_ins,0);
    return 1;
}
/*--------------------------------------------------------*/

uint ins_mark_create(XLll *mark_ll,SOOT soot,uint out_time){
    if(mark_ll==NULL)return 0;
    srand(clock());
    ret_ins mark;
    mark.ins=NULL;
    if(soot==NULL)mark.mode=RET_INS_MARK_MODE_INS;
    else mark.mode=RET_INS_MARK_MODE_SOOT;
    mark.soot=soot;
    mark.time=clock();
    mark.out_time=out_time;
    do{
        mark.mark=(uint)rand();
    }while(ll_get_member_compare(mark_ll,0,sizeof(uint),&mark.mark)!=NULL||mark.mark==0);
    ll_add_member_tail(mark_ll,&mark,sizeof(ret_ins));
    return mark.mark;
}

int send_inss(unknow_id_t id,bool event0_dev1,XLsource *receiver,XLins * ins,SOOT soot,uint out_time){
    if(ins==NULL||receiver==NULL)return 0;
    XLcore * core=core_get_by_net(&receiver->net);
    if(core==NULL)return 0;
    XLevent * event;
    XLsource sender;
    sender.net=network_get_local_info();
    sender.id=id;
    if(event0_dev1==1){
        XLdevice * device=device_get_local(id);
        if(device==NULL)return 0;
        event=event_get_by_id(device->event_id);
        sender.mode=SOURCE_DEVICE;
    }
    else {
        event=event_get_by_id(id);
        sender.mode=SOURCE_EVENTID;
    }

    if(event==NULL)return 0;

    XLpak_ins * pak_ins=(XLpak_ins*)malloc(sizeof(XLpak_ins));
    pak_ins->base.mode=SOURCE_EVENTID+(SOURCE_EVENTID<<4)+(NETWORK_MODE_INS<<12);
    pak_ins->base.net_sender=core_get_by_id(CORE_MYSELF_ID)->net;
    pak_ins->base.net_receiver=core->net;

    pak_ins->sender=sender;
    pak_ins->receiver=*receiver;
    pak_ins->mode=INS_SEND;
    pak_ins->mark=ins_mark_create(&event->ret_ins_ll,soot,out_time);

    pak_ins->Ins=*ins;
    queue_add_ins(queue_send(),pak_ins,0);
    return pak_ins->mark;
}

int event_send_ins(event_id_t id,XLsource *receiver,XLins * ins){
    return send_inss(id,0,receiver,ins,NULL,OUT_TIME);
}

int event_send_ins_soot(event_id_t id,XLsource *receiver,XLins * ins,SOOT soot){
    if(soot==NULL)return 0;
    return send_inss(id,0,receiver,ins,soot,OUT_TIME);
}

int dev_send_ins(dev_id_t id,XLsource *receiver,XLins * ins){
    return send_inss(id,1,receiver,ins,NULL,OUT_TIME);
}

int dev_send_ins_soot(dev_id_t id,XLsource *receiver,XLins * ins,SOOT soot){
    if(soot==NULL)return 0;
    return send_inss(id,1,receiver,ins,soot,OUT_TIME);
}

XLpak_ins * wait_ins_return(event_id_t event_id,uint mark,uint time){
    XLevent * event=event_get_by_id(event_id);
    if(event==NULL)return NULL;
    for(int i=0;i<time;i++){
        XLll_member * member=ll_get_member_compare(&event->ret_ins_ll,0,sizeof(uint),&mark);
        if(member!=NULL){
            ret_ins * mark=(ret_ins*)member->data;
            if(mark->ins!=NULL){
                XLpak_ins * ins=mark->ins;
                ll_del_member(&event->ret_ins_ll,member);
                return ins;
            }
        }
        else return NULL;
        usleep(1000);
    }
    return NULL;
}

int event_return_ins(event_id_t id,XLpak_ins * recv_pak,XLins * ins){
    return network_send_ins2(id,0,&recv_pak->sender,INS_RECV,recv_pak->mark,ins);
}

int device_return_ins(dev_id_t id,XLpak_ins * recv_pak,XLins * ins){
    return network_send_ins2(id,1,&recv_pak->sender,INS_RECV,recv_pak->mark,ins);
}

void * out_times(void * arg){
    extern XLll * event_ll;
    while(1){
        XLll_member * member=event_ll->head;
        for(int i=0;i<event_ll->member_num;i++){
            XLevent * event=(XLevent*)member->data;
            XLll_member * mem=event->ret_ins_ll.head;
            for(int j=0;j<event->ret_ins_ll.member_num;j++){
                ret_ins * ret=(ret_ins*)mem->data;

                if((ret->time+ret->out_time)<clock()){
                    XLll_member * temp=mem->next;
                    if(ret->mode==RET_INS_MARK_MODE_SOOT){
                        XLsoot_par soot_par;
                        XLsource * source=monitor_get_source(event->mon_id);
                        soot_par.event_id=event->id;
                        soot_par.source=source;
                        soot_par.ins=NULL;
                        soot_par.error_code=SOOT_ERROR_CODE_TIMEOUT;
                        soot_run(ret->soot,&soot_par);
                        ll_del_member(&event->ret_ins_ll,mem);
                    }
                    else ll_del_member(&event->ret_ins_ll,mem);
                    mem=temp;
                }
                else mem=mem->next;
            }
            member=member->next;
        }
        usleep(10000);
    }
}

