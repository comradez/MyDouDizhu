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
    void sendToA(QString message);
    void sendToB(QString message);
    void sendToC(QString message);
    void sendToNext(QString message);
    void sendToPrevious(QString message);
    void sendToRest(QString message);
    void paticipate(int number, int current);
    void fromA();
    void fromB();
    void fromC();
    void from();

    PlayerType nextOne() const;
    PlayerType previousOne() const;

    PlayerType playerType;
    PlayerType landlord;
    QTcpServer* serverOne;
    QTcpServer* serverTwo;
    QTcpSocket* socketOne;
    QTcpSocket* socketTwo;
    QTcpSocket* connectionAB;
    QTcpSocket* connectionBC;
    QTcpSocket* connectionAC;
};
#endif // MAINWINDOW_H
