#include "xuidsp.h"
#include "ui_xuidsp.h"

XuiDsp::XuiDsp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::XuiDsp)
{
    ui->setupUi(this);
}

XuiDsp::~XuiDsp()
{
    delete ui;
}
