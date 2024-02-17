#ifndef XUICONSOLE_H
#define XUICONSOLE_H

#include <QWidget>
#include <QListWidgetItem>
#include <core.h>

namespace Ui {
class XuiConsole;
}

class XuiConsole : public QWidget
{
    Q_OBJECT

public:
    explicit XuiConsole(QWidget *parent = nullptr);
    ~XuiConsole();
    void event_update(void);
    void core_update(void);
    void device_update(void);
    void out_update(arg_device *arg);
    void update_auto(void);
private slots:

    void on_pushButton_update_clicked();


    void on_listWidget_local_itemClicked(QListWidgetItem *item);

    void on_pushButton_3_clicked();


    void on_listWidget_localdev_itemClicked(QListWidgetItem *item);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::XuiConsole *ui;
    char labelname[64];
    event_id_t event_now;
    arg_device * device_now;
};

#endif // XUICONSOLE_H
