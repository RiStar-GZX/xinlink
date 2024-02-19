#include "xuiconsoleball.h"
#include "ui_xuiconsoleball.h"
#include <xuipixmapitem.h>
#include <monitor.h>

XuiConsoleBall::XuiConsoleBall(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::XuiConsoleBall)
{
    ui->setupUi(this);
    QPixmap pixmap;
    pixmap.load("/home/gzx/下载/tela-circle-icon-theme/src/scalable/devices/computer.svg");
    QGraphicsScene * sence=new QGraphicsScene;
    //ui->graphicsView->setScene(sence);
    XuiPixmapItem * item=new XuiPixmapItem(pixmap);
    XuiPixmapItem * item2=new XuiPixmapItem(pixmap);
    item2->setPos(100,100);
    sence->addItem(item);
    sence->addItem(item2);    
}

XuiConsoleBall::~XuiConsoleBall()
{
    delete ui;
}

typedef struct console_dev_index{
    int index;
    core_id_t core_id;
    dev_id_t dev_id;
}console_dev_index;

XLll local_dev_index_ll=*ll_create(sizeof(console_dev_index));
XLll out_dev_index_ll=*ll_create(sizeof(console_dev_index));

void XuiConsoleBall::on_pushButton_pressed()
{
    ui->listWidget->clear();
    ll_del_member_all(&local_dev_index_ll);
    extern XLll * device_ll;
    XLll_member * member=device_ll->head;
    for(int i=0;i<device_ll->member_num;i++){
        XLdevice * device=(XLdevice*)member->data;
        console_dev_index index;
        index.index=i;
        index.dev_id=device->id;
        index.core_id=CORE_MYSELF_ID;
        ll_add_member_tail(&local_dev_index_ll,&index,sizeof(console_dev_index));
        ui->listWidget->addItem(device->showname);
        member=member->next;
    }
}


void XuiConsoleBall::on_pushButton_2_clicked()
{
    emit core_update();
    emit out_device_update();
}

struct {
    console_dev_index local_dev;
    console_dev_index out_dev;
}console_info;


typedef struct console_core_index{
    int index;
    core_id_t core_id;
}console_core_index;

XLll core_index_ll=*ll_create(sizeof(console_core_index));

void XuiConsoleBall::core_update(void){
    extern XLll * core_ll;
    network_core_find_send();
    sleep(1);
    XLll_member * member=core_ll->head;
    ui->comboBox->clear();
    //ui->listWidget_2->clear();
    ll_del_member_all(&core_index_ll);
    for(int i=0;i<core_ll->member_num;i++){
        XLcore * core=(XLcore*)member->data;
        if(core->id==1){
            char str[NAME_LENGTH];
            strcpy(str,core->name);
            strcat(str,"(本机)");
            ui->comboBox->addItem(str);
        }
        else {
            ui->comboBox->addItem(core->name);
            char * ip_str;
            in_addr in;
            in.s_addr=core->net.ip;
            ip_str=inet_ntoa(in);
            //ui->listWidget_2->addItem(ip_str);
        }
        console_core_index index;
        index.index=i;
        index.core_id=core->id;
        ll_add_member_tail(&core_index_ll,&index,sizeof(console_core_index));
        member=member->next;
    }
}

void XuiConsoleBall::out_device_update(void){
    int index=ui->comboBox->currentIndex();
    ui->listWidget_2->clear();
    XLll_member * mem=ll_get_member_compare(&core_index_ll,0,sizeof(int),&index);
    if(mem==NULL)return;
    core_id_t core_id=((console_core_index*)mem->data)->core_id;
    XLcore * core=core_get_by_id(core_id);
    if(core==NULL)return;

    send_deviceinfo_req(core_id);
    sleep(2);
    XLll_member * member=core->device_ll.head;
    ll_del_member_all(&out_dev_index_ll);
    for(int i=0;i<core->device_ll.member_num;i++){
        XLdevice_info * device=(XLdevice_info*)member->data;
        console_dev_index index;
        index.dev_id=device->source.id;
        index.core_id=core->id;
        index.index=i;
        ll_add_member_tail(&out_dev_index_ll,&index,sizeof(console_dev_index));
        ui->listWidget_2->addItem(device->showname);
        member=member->next;
    }
}

void XuiConsoleBall::on_comboBox_currentIndexChanged(int index)
{
    emit out_device_update();
}


void XuiConsoleBall::on_listWidget_2_currentRowChanged(int currentRow)
{
    printf("row:%d\n",currentRow);
    XLll_member * member=ll_get_member_compare(&out_dev_index_ll,0,sizeof(int),&currentRow);
    if(member==NULL)return;
    console_dev_index * index=(console_dev_index *)member->data;
    console_info.out_dev=*index;
    if(index->core_id==CORE_MYSELF_ID){
        XLdevice *device=device_get_local(index->dev_id);
        if(device==NULL)return;
        printf("local device:%s %s\n",device->onlyname,device->showname);
        return;
    }
    XLdevice_info * device=device_info_get(index->core_id,index->dev_id);
    if(device==NULL)return;
    printf("out device:%s %s\n",device->onlyname,device->showname);
}


void XuiConsoleBall::on_listWidget_currentRowChanged(int currentRow)
{
    printf("row:%d\n",currentRow);
    XLll_member * member=ll_get_member_compare(&local_dev_index_ll,0,sizeof(int),&currentRow);
    if(member==NULL)return;
    console_dev_index * index=(console_dev_index *)member->data;
    console_info.local_dev=*index;
    XLdevice* device=device_get_local(index->dev_id);
    printf("local device:%s %s\n",device->onlyname,device->showname);
}



void XuiConsoleBall::on_pushButton_connect_clicked()
{
    extern monitor_contain contain;
    contain.operate.id[1]=console_info.local_dev.dev_id;
    contain.operate.id[2]=console_info.local_dev.core_id;
    contain.operate.id[3]=console_info.out_dev.dev_id;
    contain.operate.id[4]=console_info.out_dev.core_id;
    printf("\nss:%d %d %d %d\n",contain.operate.id[1],contain.operate.id[2],
           contain.operate.id[3],contain.operate.id[4]);
    contain.operate.mode=MONITOR_OPMODE_CONNECT;
    contain.operate.state=OPSTATE_DOING;

}


void XuiConsoleBall::on_pushButton_disconnect_clicked()
{
    extern monitor_contain contain;
    contain.operate.id[1]=console_info.local_dev.dev_id;
    contain.operate.id[2]=console_info.local_dev.core_id;
    contain.operate.id[3]=console_info.out_dev.dev_id;
    contain.operate.id[4]=console_info.out_dev.core_id;
    printf("\nss:%d %d %d %d\n",contain.operate.id[1],contain.operate.id[2],
           contain.operate.id[3],contain.operate.id[4]);
    contain.operate.mode=MONITOR_OPMODE_DISCONNECT;
    contain.operate.state=OPSTATE_DOING;
}

