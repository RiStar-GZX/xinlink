#ifndef XUIDSP_H
#define XUIDSP_H

#include <QWidget>

namespace Ui {
class XuiDsp;
}

class XuiDsp : public QWidget
{
    Q_OBJECT

public:
    explicit XuiDsp(QWidget *parent = nullptr);
    ~XuiDsp();

private:
    Ui::XuiDsp *ui;
};

#endif // XUIDSP_H
