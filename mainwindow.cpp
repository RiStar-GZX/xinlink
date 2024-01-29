#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <core.h>
#include <instruction.h>
#include <network.h>
#include <dsp.h>

#include <qdebug.h>
#include <qlistview.h>
#include <core.h>
#include <QStandardItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    emit this->core_update();
    emit this->device_update();

}

MainWindow::~MainWindow()
{
    delete ui;
}




void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    if(item->text()=="操作台"){
        qDebug()<<"OK"<<item->text()<<endl;
    }
}

void MainWindow::core_update(void){
    extern XLll * core_ll;
    network_core_find_send();
    sleep(1);
    XLll_member * member=core_ll->head;
    ui->comboBox_core->clear();
    for(int i=0;i<core_ll->member_num;i++){
        XLcore * core=(XLcore*)member->data;
        if(core->id==1){
            char str[NAME_LENGTH];
            strcpy(str,core->name);
            strcat(str,"(本机)");
            ui->comboBox_core->addItem(str);
        }
        else ui->comboBox_core->addItem(core->name);
        member=member->next;
    }
}

void MainWindow::device_update(void){
    extern XLll * event_ll;
    ui->listWidget_2->clear();
    XLll_member * member=event_ll->head;
    for(int i=0;i<event_ll->member_num;i++){
        XLevent * event=(XLevent*)member->data;

        ui->listWidget_2->addItem(event->info.name);
        member=member->next;
    }
}



void MainWindow::on_pushButton_clicked()
{
    emit this->core_update();
    emit this->device_update();
}


void MainWindow::on_listWidget_2_itemClicked(QListWidgetItem *item)
{
    ui->label->setText(item->text());
}

