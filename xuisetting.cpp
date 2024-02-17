#include "xuisetting.h"
#include "ui_xuisetting.h"
#include <xuiabout.h>

XuiSetting::XuiSetting(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::setting)
{
    ui->setupUi(this);
}

XuiSetting::~XuiSetting()
{
    delete ui;
}

void XuiSetting::on_listWidget_setting_itemClicked(QListWidgetItem *item)
{
    if(item->text()=="关于xinlink"){
        emit about_show();
    }
}

void XuiSetting::about_show(void){
    Xuiabout about;
    about.show();

    //while(1);
}


