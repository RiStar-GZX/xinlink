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
    emit this->stack_widget_init();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::stack_widget_init(){
    ui->stackedWidget->insertWidget(1,&console_ui);
    ui->stackedWidget->insertWidget(3,&dsp_ui);
    ui->stackedWidget->insertWidget(2,&setting_ui);
    ui->stackedWidget->insertWidget(0,&consoleball_ui);
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    //qDebug()<<"OK"<<item->text()<<endl;
    if(item->text()=="操作台"){
        ui->stackedWidget->setCurrentIndex(0);
    }
    else if(item->text()=="支持程序"){
        ui->stackedWidget->setCurrentIndex(1);
    }
    else if(item->text()=="设置"){
        ui->stackedWidget->setCurrentIndex(2);
    }
    this->update();
}
