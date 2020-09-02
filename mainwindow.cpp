#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , player(new Player()) {
    ui->setupUi(this);
    ui->graphicsView->setStyleSheet("background: transparent;border:0px");
    ui->pushButtonDecline->setVisible(false);
    ui->pushButtonLandlord->setVisible(false);

    inputDialog = new InitDialog(this);
    inputDialog->exec();
    player->setType(inputDialog->getPlayerType());
    getDataFromDialog();
    if (player->getType() == PlayerType::A) {
        qDebug() << "checkpoint";

        for (int i = CardSize::Three; i <= CardSize::Two; i++) {
            for (int j = (int)CardKind::Spade; j <= (int)CardKind::Club; j++) {
                globalDeck.push_back(Card(CardKind(j), CardSize(i)));
            }
        }
        globalDeck.push_back(Card(CardKind::Joker, CardSize::SmallJoker));
        globalDeck.push_back(Card(CardKind::Joker, CardSize::BigJoker));
        //放入54张牌
        std::random_shuffle(globalDeck.begin(), globalDeck.end());
        //洗牌
        qDebug() << "checkpoint";
        for (int i = 0; i < 3; i++) {
            landlordCards.insert(*globalDeck.begin());
            globalDeck.erase(globalDeck.begin());
        } //留出三张地主牌
        for (int i = 0; i < 17; i++) {
            player->addCard(*globalDeck.begin());
            globalDeck.erase(globalDeck.begin());
            sendToB(globalDeck.begin()->toString());
            globalDeck.erase(globalDeck.begin());
            sendToC(globalDeck.begin()->toString());
            globalDeck.erase(globalDeck.begin());
        } //发掉剩余51张牌
        qDebug() << "checkpoint";

        int randPlayer = QRandomGenerator::global()->bounded(0, 3);
        if (randPlayer == 0) { //A最先叫地主
            paticipate(1, -1);
        } else if (randPlayer == 1) {
            sendToB("paticipate");
        } else if (randPlayer == 2) {
            sendToC("paticipate");
        }
    }
}

void MainWindow::paticipate(int number, int current) {
    if (number == 1) {
        ui->pushButtonDecline->setVisible(true);
        ui->pushButtonLandlord->setVisible(true);
        ui->pushButtonDecline->setEnabled(true);
        ui->pushButtonLandlord->setEnabled(true);
        connect(ui->pushButtonDecline, &QPushButton::clicked, [=](){
            sendToNext("landlord;2;-1");
            ui->pushButtonDecline->setVisible(false);
            ui->pushButtonLandlord->setVisible(false);
        });
        connect(ui->pushButtonLandlord, &QPushButton::clicked, [=](){
            sendToNext("landlord;2;1");
            ui->pushButtonDecline->setVisible(false);
            ui->pushButtonLandlord->setVisible(false);
        });
    } else if (number == 2) {
        ui->pushButtonDecline->setVisible(true);
        ui->pushButtonLandlord->setVisible(true);
        ui->pushButtonDecline->setEnabled(true);
        ui->pushButtonLandlord->setEnabled(true);
        connect(ui->pushButtonDecline, &QPushButton::clicked, [=](){
            sendToNext(QString("landlord;3;%1").arg(current));
            ui->pushButtonDecline->setVisible(false);
            ui->pushButtonLandlord->setVisible(false);
        });
        connect(ui->pushButtonLandlord, &QPushButton::clicked, [=](){
            sendToNext("landlord;3;2");
            ui->pushButtonDecline->setVisible(false);
            ui->pushButtonLandlord->setVisible(false);
        });
    } else if (number == 3) {
        ui->pushButtonDecline->setVisible(true);
        ui->pushButtonLandlord->setVisible(true);
        ui->pushButtonDecline->setEnabled(true);
        ui->pushButtonLandlord->setEnabled(true);
        connect(ui->pushButtonDecline, &QPushButton::clicked, [=]() {
            if (current == 1 || current == -1) {
                sendToRest(QString("chosen;%1").arg(int(nextOne())));
                landlord = nextOne();
            } else {
                sendToRest(QString("chosen;%1").arg(int(previousOne())));
                landlord = previousOne();
            }
            ui->pushButtonDecline->setVisible(false);
            ui->pushButtonLandlord->setVisible(false);
        });
        connect(ui->pushButtonLandlord, &QPushButton::clicked, [=](){
            sendToRest(QString("chosen;%1").arg(int(player->getType())));
            landlord = player->getType();
            ui->pushButtonDecline->setVisible(false);
            ui->pushButtonLandlord->setVisible(false);
        });
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::getDataFromDialog() {
    if (player->getType() == PlayerType::A) {
        connects[0] = nullptr;
        connects[1] = inputDialog->getConnectionAB();
        connects[2] = inputDialog->getConnectionAC();
        disconnect(connects[1], 0, 0, 0);
        disconnect(connects[2], 0, 0, 0);
        connect(connects[1], &QTcpSocket::readyRead, this, &MainWindow::from);
        connect(connects[2], &QTcpSocket::readyRead, this, &MainWindow::from);
    } else if (player->getType() == PlayerType::B) {
        connects[0] = inputDialog->getSocketOne();
        connects[1] = nullptr;
        connects[2] = inputDialog->getConnectionBC();
        disconnect(connects[0], 0, 0, 0);
        disconnect(connects[2], 0, 0, 0);
        connect(connects[0], &QTcpSocket::readyRead, this, &MainWindow::from);
        connect(connects[2], &QTcpSocket::readyRead, this, &MainWindow::from);
    } else if (player->getType() == PlayerType::C) {
        connects[0] = inputDialog->getSocketOne();
        connects[1] = inputDialog->getSocketTwo();
        connects[2] = nullptr;
        disconnect(connects[0], 0, 0, 0);
        disconnect(connects[1], 0, 0, 0);
        connect(connects[0], &QTcpSocket::readyRead, this, &MainWindow::from);
        connect(connects[1], &QTcpSocket::readyRead, this, &MainWindow::from);
    }
}

void MainWindow::sendToA(QString message) {
    connects[0]->write(message.toUtf8());
}

void MainWindow::sendToB(QString message) {
    connects[1]->write(message.toUtf8());
}

void MainWindow::sendToC(QString message) {
    connects[2]->write(message.toUtf8());
}

void MainWindow::sendToNext(QString message) {
    if (player->getType() == PlayerType::A) {
        sendToB(message);
    } else if (player->getType() == PlayerType::B) {
        sendToC(message);
    } else {
        sendToA(message);
    }
}

void MainWindow::sendToPrevious(QString message) {
    if (player->getType() == PlayerType::A) {
        sendToC(message);
    } else if (player->getType() == PlayerType::B) {
        sendToA(message);
    } else {
        sendToB(message);
    }
}

void MainWindow::sendToRest(QString message) {
    sendToPrevious(message);
    sendToNext(message);
}

Assist::PlayerType MainWindow::nextOne() const {
    return PlayerType((int(player->getType()) + 1) % 3);
}

Assist::PlayerType MainWindow::previousOne() const {
    return PlayerType((int(player->getType()) + 2) % 3);
}

void MainWindow::from() {
    QString message = connects[0]->readAll(); //我猜缓冲区被清空了
    qDebug() << message;
    //connects[0]->flush();
    /*
    可能接收到的信息:
    paticipate 表示从我开始叫地主
    landlord;num1;num2 表示我是第num1个叫地主的,最后一个叫过地主的人是num2
    chosen;num 表示地主是num
    */
    QStringList attrList = message.split(";");
    if (attrList[0] == "paticipate") {
        paticipate(1, 1);
    } else if (attrList[0] == "landlord") {
        paticipate(attrList[1].toInt(), attrList[2].toInt());
    } else if (attrList[0] == "chosen") {
        landlord = PlayerType(attrList[1].toInt());
    }
}
