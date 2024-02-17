#ifndef XUIABOUT_H
#define XUIABOUT_H

#include <QWidget>

namespace Ui {
class Xuiabout;
}

class Xuiabout : public QWidget
{
    Q_OBJECT

public:
    explicit Xuiabout(QWidget *parent = nullptr);
    ~Xuiabout();

private:
    Ui::Xuiabout *ui;
};

#endif // XUIABOUT_H
