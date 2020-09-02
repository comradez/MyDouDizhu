#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "assist.h"
#include "player.h"
#include "initdialog.h"
#include "controller.h"
#include <QMainWindow>
#include <QRandomGenerator>
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
    Player* player;

    std::vector<Card> globalDeck;
    std::set<Card> landlordCards;

    Controller* controller;
    GlobalController* globalController;
    QTcpSocket* connects[3];

    void getDataFromDialog();
    bool sendToA(QByteArray data);
    bool sendToB(QByteArray data);
    bool sendToC(QByteArray data);
    bool sendToNext(QByteArray data);
    bool sendToPrevious(QByteArray data);
    bool sendToRest(QByteArray data);
    void paticipate(int number, int current);
    QByteArray IntToArray(qint32 source);
    qint32 ArrayToInt(QByteArray source);
    void handleRead(QByteArray data);
    void readyRead();


    PlayerType nextOne() const;
    PlayerType previousOne() const;

    QHash<QTcpSocket*, QByteArray*> buffers;
    QHash<QTcpSocket*, qint32*> sizes;

    PlayerType playerType;
    PlayerType landlord;
    QTcpServer* serverOne;
    QTcpServer* serverTwo;
    QTcpSocket* socketOne;
    QTcpSocket* socketTwo;
    QTcpSocket* connectionAB;
    QTcpSocket* connectionBC;
    QTcpSocket* connectionAC;
public: signals:
    void dataReceived(QByteArray data);
};
#endif // MAINWINDOW_H
