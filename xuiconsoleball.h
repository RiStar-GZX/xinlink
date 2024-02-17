#ifndef XUICONSOLEBALL_H
#define XUICONSOLEBALL_H

#include <QGraphicsView>
#include <QGraphicsItem>
#include <QListWidgetItem>

namespace Ui {
class XuiConsoleBall;
}

class XuiConsoleBall : public QWidget
{
    Q_OBJECT

public:
    explicit XuiConsoleBall(QWidget *parent = nullptr);
    ~XuiConsoleBall();
    void set_name(QString string);
    void core_update(void);
    void out_device_update(void);
private slots:
    void on_pushButton_pressed();

    void on_pushButton_2_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_listWidget_2_currentRowChanged(int currentRow);

    void on_listWidget_currentRowChanged(int currentRow);

    void on_pushButton_connect_clicked();

    void on_pushButton_disconnect_clicked();

private:
    Ui::XuiConsoleBall *ui;
};

#endif // XUICONSOLEBALL_H
