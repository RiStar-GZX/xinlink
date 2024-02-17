#include "xuiconsole.h"
#include "ui_xuiconsole.h"
#include <mainwindow.h>
#include <monitor.h>

XuiConsole::XuiConsole(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::XuiConsole)
{
    ui->setupUi(this);
    emit event_update();
    emit core_update();
}

XuiConsole::~XuiConsole()
{
    delete ui;
}

void XuiConsole::core_update(void){
    extern XLll * core_ll;
    network_core_find_send();
    sleep(1);
    XLll_member * member=core_ll->head;
    ui->comboBox_core_2->clear();
    ui->listWidget_out->clear();
    for(int i=0;i<core_ll->member_num;i++){
        XLcore * core=(XLcore*)member->data;
        if(core->id==1){
            //char str[NAME_LENGTH];
            //strcpy(str,core->name);
            //strcat(str,"(本机)");
            //ui->comboBox_core_2->addItem(str);
        }
        else {
            ui->comboBox_core_2->addItem(core->name);
            char * ip_str;
            in_addr in;
            in.s_addr=core->net.ip;
            ip_str=inet_ntoa(in);
            ui->listWidget_out->addItem(ip_str);
        }
        member=member->next;
    }
}

void XuiConsole::on_pushButton_update_clicked()
{
    emit this->core_update();
    emit this->event_update();
}

void XuiConsole::device_update(void){
    extern monitor_contain contain;
    XLll_member * event=ll_get_member_compare(&contain.local_device_ll,
                          sizeof(uint32_t)*2+sizeof(XLnet),strlen(labelname)+1,labelname);
    ui->listWidget_localdev->clear();
    if(event==NULL)return;
    monitor_device * device=(monitor_device*)event->data;
    XLll_member * member=device->arg_ll.head;
    for(int i=0;i<device->arg_ll.member_num;i++){
        arg_device * arg=(arg_device*)member->data;
        ui->listWidget_localdev->addItem(arg->show_name);
        member=member->next;
    }
}

void XuiConsole::event_update(void){
    extern XLll * event_ll;
    ui->listWidget_local->clear();
    XLll_member * member=event_ll->head;
    for(int i=0;i<event_ll->member_num;i++){
        XLevent * event=(XLevent*)member->data;
        ui->listWidget_local->addItem(event->info.name);
        member=member->next;
    }
}

void XuiConsole::on_listWidget_local_itemClicked(QListWidgetItem *item)
{
    ui->label_name->text()=item->text();
    QByteArray ba=item->text().toUtf8();
    XLevent * event=event_get_by_name(ba.data());
    event_now=event->id;
    strcpy(labelname,ba.data());
    ui->label_name->setText(item->text());
    emit device_update();
    //extern monitor_contain contain;
    //contain.operate.mode=MONITOR_OPMODE_LSDEV;
    emit update_auto();
}


void XuiConsole::on_pushButton_3_clicked()
{
    //extern monitor_contain contain;
    //contain.operate.mode=MONITOR_OPMODE_LSDEV;
    emit update_auto();
}

void XuiConsole::update_auto(void){
    extern monitor_contain contain;
    contain.operate.mode=MONITOR_OPMODE_LSDEV;
    contain.operate.state=OPSTATE_DOING;
    contain.operate.event_id=event_now;
    int mode=contain.operate.mode;
   // for(int i=0;i<30;i++){
        while(contain.operate.state!=OPSTATE_FINISH){
            usleep(10000);
        }
    //    else return;
    //}
    printf("FINISH\n");
    if(mode==MONITOR_OPMODE_LSDEV)emit device_update();
}

void XuiConsole::on_listWidget_localdev_itemClicked(QListWidgetItem *item)
{
    int row=ui->listWidget_localdev->currentRow();
    extern monitor_contain contain;
    XLll_member * event=ll_get_member_compare(&contain.local_device_ll,
                                               sizeof(uint32_t)*2+sizeof(XLnet),strlen(labelname)+1,labelname);
    if(event==NULL)return;
    monitor_device * device=(monitor_device*)event->data;
    XLll_member * member=device->arg_ll.head;
    for(int i=0;i<row;i++){
        member=member->next;
    }
    device_now=(arg_device*)member->data;
    printf("row:%d name:%s onlyname:%s\n",row,device_now->show_name,device_now->only_name);
    //out_update(arg);
}

void XuiConsole::out_update(arg_device * arg){
    ui->listWidget_out->clear();

}

void XuiConsole::on_pushButton_clicked()
{
    if(ui->listWidget_out->currentItem()==NULL)return;
    extern monitor_contain contain;
    contain.operate.event_id=event_now;
    QByteArray ba=ui->listWidget_out->currentItem()->text().toUtf8();
    strcpy(contain.operate.ip,ba.data());
    strcpy(contain.operate.device_name,device_now->only_name);
    contain.operate.mode=MONITOR_OPMODE_CONNECTWITH;
    contain.operate.state=OPSTATE_DOING;
}

void XuiConsole::on_pushButton_2_clicked()
{

}

