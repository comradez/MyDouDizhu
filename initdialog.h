#ifndef INITDIALOG_H
#define INITDIALOG_H

#include "assist.h"
#include <chrono>
#include <thread>
#include <QDebug>
#include <QDialog>
#include <QTcpServer>
#include <QTcpSocket>

namespace Ui {
class InitDialog;
}

class InitDialog : public QDialog
{
    Q_OBJECT

public:
    typedef Assist::PlayerType PlayerType;
    const int portAB = 60001, portAC = 60002, portBC = 60003;
    bool connectAB, connectBC, connectAC;
    PlayerType currentType;
    explicit InitDialog(QWidget *parent = nullptr);
    ~InitDialog();

private:
    Ui::InitDialog *ui;
    QTcpServer* serverOne;
    QTcpServer* serverTwo;
    QTcpSocket* socketOne;
    QTcpSocket* socketTwo;
    QTcpSocket* connectionAB;
    QTcpSocket* connectionBC;
    QTcpSocket* connectionAC;
    void startMatching();
    void acceptConnection();
    void handleAB();
    void handleAC();
    void handleBC();
};

#endif // INITDIALOG_H
