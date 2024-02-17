#include "xuiabout.h"
#include "ui_xuiabout.h"

Xuiabout::Xuiabout(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Xuiabout)
{
    ui->setupUi(this);
}

Xuiabout::~Xuiabout()
{
    delete ui;
}
