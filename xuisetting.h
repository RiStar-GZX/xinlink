#ifndef XUISETTING_H
#define XUISETTING_H

#include <QWidget>
#include <QListWidgetItem>

namespace Ui {
class setting;
}

class XuiSetting : public QWidget
{
    Q_OBJECT

public:
    explicit XuiSetting(QWidget *parent = nullptr);
    ~XuiSetting();
    void about_show(void);

private slots:
    void on_listWidget_setting_itemClicked(QListWidgetItem *item);

private:
    Ui::setting *ui;
};

#endif // XUISETTING_H
