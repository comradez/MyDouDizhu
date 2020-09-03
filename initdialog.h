#ifndef INITDIALOG_H
#define INITDIALOG_H

#include "assist.h"
#include "client.h"
#include "server.h"
#include <chrono>
#include <thread>
#include <tuple>
#include <QDebug>
#include <QDialog>
#include <QTcpServer>
#include <QTcpSocket>
using std::tuple;

namespace Ui {
class InitDialog;
}

class InitDialog : public QDialog
{
    Q_OBJECT

public:
    typedef Assist::PlayerType PlayerType;
    const int portAB = 60001, portAC = 60002, portBC = 60003;
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
    bool readyA = false, readyB = false, readyC = false;

    void startMatching();
    void startGame();
    void acceptConnection();
    void handleA();
    void handleB();
    void handleC();

    void startMatch();

public:
    PlayerType getPlayerType() const;
    QTcpSocket* getConnectionAB() const;
    QTcpSocket* getConnectionAC() const;
    QTcpSocket* getConnectionBC() const;
    QTcpSocket* getSocketOne() const;
    QTcpSocket* getSocketTwo() const;
};

#endif // INITDIALOG_H
