#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "assist.h"
#include "player.h"
#include "initdialog.h"
#include "controller.h"
#include "comparer.h"
#include <random>
#include <QMainWindow>
#include <QRandomGenerator>
#include <QDebug>
#include <QPainter>
#include <QPixmap>
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
    PlayerType landlord;
    int cardNumPrevious, cardNumNext;
    const int xpos = 70, ypos = 420, xcenter = 210, ycenter = 10;
    void paintEvent(QPaintEvent* ev) override;
    void mousePressEvent(QMouseEvent* ev) override;
    PlayerType lastPlayer;
    QList<Card> previousCombo;
    CardCombo previousComboType;

    std::vector<Card> globalDeck;
    std::vector<Card> landlordCards;

    QTcpSocket* connects[3];

    QString cardKinds[5] = {"S", "H", "D", "C", "J"};
    QString cardSizes[18] = {"-1", "-1", "-1", "3", "4", "5", "6", "7", "8",
                           "9", "10", "J", "Q", "K", "A", "2", "small", "big"};

    void getDataFromDialog();
    bool sendTo(int dst, QByteArray data);
    bool sendToNext(QByteArray data);
    bool sendToPrevious(QByteArray data);
    bool sendToRest(QByteArray data);
    void paticipate(int number, int current);
    QByteArray IntToArray(qint32 source);
    qint32 ArrayToInt(QByteArray source);
    void handleRead(QByteArray data);
    void readyRead();
    void handleSkip();
    void handlePlay();
    void handleQuit();
    void start();
    void init();
    void reset();
    void gameInit();

    PlayerType nextOne() const;
    PlayerType previousOne() const;

    QHash<QTcpSocket*, QByteArray*> buffers;
    QHash<QTcpSocket*, qint32*> sizes;
    bool replay[3]{false};
    bool readyB = false, readyC = false;
public: signals:
    void dataReceived(QByteArray data);
};
#endif // MAINWINDOW_H
