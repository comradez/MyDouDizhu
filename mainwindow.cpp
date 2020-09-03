 #include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , player(new Player()) {
    ui->setupUi(this);
    //ui->graphicsView->setStyleSheet("background: transparent;border:0px");
    ui->pushButtonDecline->setVisible(false);
    ui->pushButtonLandlord->setVisible(false);

    cardNum = cardNumNext = cardNumPrevious = 17;

    inputDialog = new InitDialog(this);
    inputDialog->exec();
    player->setType(inputDialog->getPlayerType());
    setWindowTitle(QString('A' + (int)player->getType()));
    getDataFromDialog();
    qDebug() << "B and C";
    if (player->getType() == PlayerType::A) {
        for (int i = CardSize::Three; i <= CardSize::Two; i++) {
            for (int j = (int)CardKind::Spade; j <= (int)CardKind::Club; j++) {
                globalDeck.push_back(Card(CardKind(j), CardSize(i)));
            }
        }
        globalDeck.push_back(Card(CardKind::Joker, CardSize::SmallJoker));
        globalDeck.push_back(Card(CardKind::Joker, CardSize::BigJoker));
        //放入54张牌

        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(globalDeck.begin(), globalDeck.end(), g);

        //洗牌
        for (int i = 51; i < 54; i++) {
            landlordCards.insert(globalDeck[i]);
        } //留出三张地主牌
        //画地主牌
        QString cardB = "card;", cardC = "card;";
        for (int i = 0; i < 51; i++) {
            if (i % 3 == 0)
                player->addCard(globalDeck[i]);
            else if (i % 3 == 1)
                cardB += globalDeck[i].toString();
            else if (i % 3 == 2)
                cardC += globalDeck[i].toString();
        } //发掉剩余51张牌
        repaint();
        sendTo(1, cardB.toUtf8());
        sendTo(2, cardC.toUtf8());
        //画自己的17张牌
        int randPlayer = QRandomGenerator::global()->bounded(0, 3);
        if (randPlayer == 0) { //A最先叫地主
            paticipate(1, -1);
            qDebug() << "A";
        } else if (randPlayer == 1) {
            qDebug() << "B";
            sendTo(1, QString("paticipate").toUtf8());
        } else if (randPlayer == 2) {
            qDebug() << "C";
            sendTo(2, QString("paticipate").toUtf8());
        }
        repaint();
        sendToRest("repaint");
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
            QString cardLandlord = QString("card;");
            for (const auto& each : landlordCards) {
                cardLandlord += each.toString();
            }
            if (current == 1 || current == -1) {
                sendToRest(QString("chosen;%1").arg(int(nextOne())).toUtf8());
                landlord = nextOne();
                sendToNext(cardLandlord.toUtf8());
                cardNumNext += 3;
                sendToNext("repaint");
            } else {
                sendToRest(QString("chosen;%1").arg(int(previousOne())).toUtf8());
                landlord = previousOne();
                sendToPrevious(cardLandlord.toUtf8());
                cardNumPrevious += 3;
                sendToPrevious("repaint");
            }
            qDebug() << "landlord is " << int(landlord);
            ui->pushButtonDecline->setVisible(false);
            ui->pushButtonLandlord->setVisible(false);
        });
        connect(ui->pushButtonLandlord, &QPushButton::clicked, [=](){
            sendToRest(QString("chosen;%1").arg(int(player->getType())).toUtf8());
            landlord = player->getType();
            qDebug() << "landlord is " << int(landlord);
            for (auto&& each : landlordCards) {
                player->addCard(each);
            } repaint();
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
            //QByteArray* buffer1 = new QByteArray();
            //qint32 *size1 = new qint32(0);
            buffers.insert(connects[1], new QByteArray());
            sizes.insert(connects[1], new qint32(0));
            buffers.insert(connects[2], new QByteArray());
            sizes.insert(connects[2], new qint32(0));
            break;
        }
        case (PlayerType::B): {
            connects[0] = inputDialog->getSocketOne();
            connects[1] = nullptr;
            connects[2] = inputDialog->getConnectionBC();
            buffers.insert(connects[0], new QByteArray());
            sizes.insert(connects[0], new qint32(0));
            buffers.insert(connects[2], new QByteArray());
            sizes.insert(connects[2], new qint32(0));
            break;
        }
        case (PlayerType::C): {
            connects[0] = inputDialog->getSocketOne();
            connects[1] = inputDialog->getSocketTwo();
            connects[2] = nullptr;
            buffers.insert(connects[0], new QByteArray());
            sizes.insert(connects[0], new qint32(0));
            buffers.insert(connects[1], new QByteArray());
            sizes.insert(connects[1], new qint32(0));
            break;
        }
    }
    for (auto&& each : connects) {
        if (each != nullptr) {
            disconnect(each, 0, 0, 0);
            connect(each, &QTcpSocket::readyRead, this, &MainWindow::readyRead);
        }
    }
    connect(this, &MainWindow::dataReceived, this, &MainWindow::handleRead);
}

QByteArray MainWindow::IntToArray(qint32 source) {
    QByteArray temp;
    QDataStream data(&temp, QIODevice::ReadWrite);
    data << source;
    return temp;
}

bool MainWindow::sendTo(int dst, QByteArray data) {
    QTcpSocket* socket = connects[dst];
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
            return sendTo(1, data);
        case (PlayerType::B):
            return sendTo(2, data);
        case (PlayerType::C):
            return sendTo(0, data);
    } return false;
}

bool MainWindow::sendToPrevious(QByteArray data) {
    switch (player->getType()) {
        case (PlayerType::A):
            return sendTo(2, data);
        case (PlayerType::B):
            return sendTo(0, data);
        case (PlayerType::C):
            return sendTo(1, data);
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

void MainWindow::paintEvent(QPaintEvent *ev) {
    Q_UNUSED(ev);

    QPainter painter(this);
    QPixmap pixmap;
    painter.translate(xpos, ypos);
    int cnt = 0;
    qDebug() << player->expose().size();
    for (const auto& each : player->expose()) {
        QPixmap pixmap;
        QString fileName;
        fileName += cardKinds[int(each.getCardKind())];
        fileName += cardSizes[int(each.getCardSize())];
        qDebug() << fileName;
        pixmap.load("../MyDouDizhu/cards/" + fileName + ".png");
        if (each.getChosen() == true) {
            painter.drawPixmap(cnt, -40, pixmap);
        } else {
            painter.drawPixmap(cnt, 0, pixmap);
        }
        cnt += 40;
    }
}

void MainWindow::mousePressEvent(QMouseEvent *ev) {
    int x = ev->x() - xpos;
    int y = ev->y() - ypos;
    int cardHeight = 300;
    if (y > 0 && y < cardHeight) {
        int cx = x / 40;
        player->toggleChosen(cx);
        repaint();
    }
}

void MainWindow::readyRead() {

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
                qDebug() << data;
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

/*
 * paticipate 从我开始抢地主
 *
 * landlord;num1;num2 我是第num1个抢地主的人,最后一个抢过的人是num2
 *
 * chosen;num 地主是num
 */

void MainWindow::handleRead(QByteArray data) {
    QString message = QString(data);
    QStringList attrList = message.split(";");
    if (attrList[0] == "paticipate") {
        paticipate(1, 1);
    } else if (attrList[0] == "repaint") {
        repaint();
    } else if (attrList[0] == "landlord") {
        paticipate(attrList[1].toInt(), attrList[2].toInt());
    } else if (attrList[0] == "chosen") {
        landlord = PlayerType(attrList[1].toInt());
        qDebug() << "landlord is " << int(landlord);
    } else if (attrList[0] == "card") {
        int cnt = 0;
        for (const auto& each : attrList) {
            cnt++;
            if (each != "" && each != "card") {
                player->addCard(Card(each));
            }
        } qDebug() << cnt;
        repaint();
    }
}
