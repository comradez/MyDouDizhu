#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "assist.h"
#include "player.h"
#include "initdialog.h"
#include "controller.h"
#include <QMainWindow>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    typedef Assist::PlayerType PlayerType;
    typedef Assist::CardCombo CardCombo;
    typedef Assist::CardKind CardKind;
    typedef Assist::CardSize CardSize;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    InitDialog* inputDialog;
};
#endif // MAINWINDOW_H
