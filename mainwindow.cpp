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
        QString cardB, cardC;
        for (int i = 0; i < 17; i++) {
            player->addCard(*globalDeck.begin());
            globalDeck.erase(globalDeck.begin());
            cardB += globalDeck.begin()->toString();
            //sendToB(globalDeck.begin()->toString().toUtf8());
            globalDeck.erase(globalDeck.begin());
            cardC += globalDeck.begin()->toString();
            //sendToC(globalDeck.begin()->toString().toUtf8());
            globalDeck.erase(globalDeck.begin());
        } //发掉剩余51张牌
        sendToB(cardB.toUtf8());
        sendToC(cardC.toUtf8());
        qDebug() << "checkpoint";

        int randPlayer = QRandomGenerator::global()->bounded(0, 3);
        if (randPlayer == 0) { //A最先叫地主
            paticipate(1, -1);
        } else if (randPlayer == 1) {
            sendToB(QString("paticipate").toUtf8());
        } else if (randPlayer == 2) {
            sendToC(QString("paticipate").toUtf8());
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
            sendToNext(QString("landlord;3;%1").arg(current).toUtf8());
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
                sendToRest(QString("chosen;%1").arg(int(nextOne())).toUtf8());
                landlord = nextOne();
            } else {
                sendToRest(QString("chosen;%1").arg(int(previousOne())).toUtf8());
                landlord = previousOne();
            }
            ui->pushButtonDecline->setVisible(false);
            ui->pushButtonLandlord->setVisible(false);
        });
        connect(ui->pushButtonLandlord, &QPushButton::clicked, [=](){
            sendToRest(QString("chosen;%1").arg(int(player->getType())).toUtf8());
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
    switch (player->getType()) {
        case (PlayerType::A): {
            connects[0] = nullptr;
            connects[1] = inputDialog->getConnectionAB();
            connects[2] = inputDialog->getConnectionAC();
            break;
        }
        case (PlayerType::B): {
            connects[0] = inputDialog->getSocketOne();
            connects[1] = nullptr;
            connects[2] = inputDialog->getConnectionBC();
            break;
        }
        case (PlayerType::C): {
            connects[0] = inputDialog->getSocketOne();
            connects[1] = inputDialog->getSocketTwo();
            connects[2] = nullptr;
            break;
        }
    }
    for (auto&& each : connects) {
        if (each != nullptr) {
            disconnect(each, 0, 0, 0);
            connect(each, &QTcpSocket::readyRead, this, &MainWindow::readyRead);
        }
    }
    connect(this, &MainWindow::readyRead, this, &MainWindow::handleRead);
}

QByteArray MainWindow::IntToArray(qint32 source) {
    QByteArray temp;
    QDataStream data(&temp, QIODevice::ReadWrite);
    data << source;
    return temp;
}

bool MainWindow::sendToA(QByteArray data) {
    QTcpSocket* socket = connects[0];
    if(socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(IntToArray(data.size()));
        socket->write(data);
        return socket->waitForBytesWritten();
    } else {
        return false;
    }
}

bool MainWindow::sendToB(QByteArray data) {
    QTcpSocket* socket = connects[1];
    if(socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(IntToArray(data.size()));
        socket->write(data);
        return socket->waitForBytesWritten();
    } else {
        return false;
    }
}

bool MainWindow::sendToC(QByteArray data) {
    QTcpSocket* socket = connects[2];
    if(socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(IntToArray(data.size()));
        socket->write(data);
        return socket->waitForBytesWritten();
    } else {
        return false;
    }
}

bool MainWindow::sendToNext(QByteArray data) {
    switch (player->getType()) {
        case (PlayerType::A):
            return sendToB(data);
        case (PlayerType::B):
            return sendToC(data);
        case (PlayerType::C):
            return sendToA(data);
    } return false;
}

bool MainWindow::sendToPrevious(QByteArray data) {
    switch (player->getType()) {
        case (PlayerType::A):
            return sendToC(data);
        case (PlayerType::B):
            return sendToA(data);
        case (PlayerType::C):
            return sendToB(data);
    } return false;
}

bool MainWindow::sendToRest(QByteArray data) {
    return sendToNext(data) && sendToPrevious(data);
}

Assist::PlayerType MainWindow::nextOne() const {
    return PlayerType((int(player->getType()) + 1) % 3);
}

Assist::PlayerType MainWindow::previousOne() const {
    return PlayerType((int(player->getType()) + 2) % 3);
}

void MainWindow::readyRead()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    QByteArray *buffer = buffers.value(socket);
    qint32 *s = sizes.value(socket);
    qint32 size = *s;
    while (socket->bytesAvailable() > 0)
    {
        buffer->append(socket->readAll());
        while ((size == 0 && buffer->size() >= 4) || (size > 0 && buffer->size() >= size))
        {
            if (size == 0 && buffer->size() >= 4)
            {
                size = ArrayToInt(buffer->mid(0, 4));
                *s = size;
                buffer->remove(0, 4);
            }
            if (size > 0 && buffer->size() >= size)
            {
                QByteArray data = buffer->mid(0, size);
                buffer->remove(0, size);
                size = 0;
                *s = size;
                emit dataReceived(data);
            }
        }
    }
}

qint32 MainWindow::ArrayToInt(QByteArray source)
{
    qint32 temp;
    QDataStream data(&source, QIODevice::ReadWrite);
    data >> temp;
    return temp;
}

