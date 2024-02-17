#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>

#include <xuiconsole.h>
#include <xuiconsoleball.h>
#include <xuisetting.h>
#include <xuidsp.h>

#include <core.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    XuiConsole console_ui;
    void stack_widget_init(void);
private slots:

    void on_listWidget_itemClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
    XuiDsp dsp_ui;
    XuiSetting setting_ui;
    XuiConsoleBall consoleball_ui;
};
#endif // MAINWINDOW_H
