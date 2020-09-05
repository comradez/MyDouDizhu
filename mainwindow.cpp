 #include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , player(new Player())
    , playList(new QMediaPlaylist(this))
    , globalPlayer(new QMediaPlayer(this))
    , winPlayer(new QMediaPlayer(this)) {
    ui->setupUi(this);
    ui->pushButtonDecline->setEnabled(false);
    ui->pushButtonLandlord->setEnabled(false);
    cardNumNext = cardNumPrevious = 17;
    ui->lcdNext->display(cardNumNext);
    ui->lcdPrevious->display(cardNumPrevious);
    inputDialog = new InitDialog(this);
    inputDialog->exec();
    player->setType(inputDialog->getPlayerType());
    setWindowTitle(QString('A' + (int)player->getType()));
    ui->labelThis->setText("Player " + QString('A' + (int)player->getType()));
    ui->labelPrevious->setText("Player " + QString('A' + ((int)player->getType() + 2) % 3));
    ui->labelNext->setText("Player " + QString('A' + ((int)player->getType() + 1) % 3));
    getDataFromDialog();
    if (player->getType() == PlayerType::A) {
        playList->addMedia(QMediaContent(QUrl("qrc:/audios/ BGM1.mp3")));
        playList->addMedia(QMediaContent(QUrl("qrc:/audios/ BGM2.mp3")));
        playList->addMedia(QMediaContent(QUrl("qrc:/audios/ BGM3.mp3")));
        playList->setPlaybackMode(QMediaPlaylist::Random);
        globalPlayer->setPlaylist(playList);
        globalPlayer->play();
        connect(winPlayer, &QMediaPlayer::stateChanged, this, &MainWindow::handleStateChanged);
        gameInit();
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

//-----重载的eventHandler-----

void MainWindow::mousePressEvent(QMouseEvent *ev) {
    int x = ev->x() - xpos;
    int y = ev->y() - ypos;

    int cardHeight = 300;
    if (y > 0 && y < cardHeight && x > 0) {
        unsigned int cx = x / 40;
        if (cx >= player->expose().size() - 1 && cx <= player->expose().size() + 3) {
            cx = player->expose().size() - 1;
        } else if (cx > player->expose().size() + 3) {
            return;
        }
        player->toggleChosen(cx);
        qDebug() << "clicked" << x << y << "and card number" << cx << " is toggled";
        repaint();
    }
}

void MainWindow::paintEvent(QPaintEvent *ev) {
    Q_UNUSED(ev);

    QPainter painter(this);
    painter.save();
    QPixmap pixmap;
    painter.translate(xpos, ypos);
    int cnt = 0;
    for (const auto& each : player->expose()) {
        QPixmap pixmap;
        QString fileName;
        fileName += cardKinds[int(each.getCardKind())];
        fileName += cardSizes[int(each.getCardSize())];
        pixmap.load("../MyDouDizhu/cards/" + fileName + ".png");
        QPixmap scaledPixmap = pixmap.scaled(pixmap.size() * 0.6, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        if (each.getChosen() == true) {
            painter.drawPixmap(cnt, -40, pixmap);
        } else {
            painter.drawPixmap(cnt, 0, pixmap);
        }
        cnt += 40;
    }
    cnt = 0;
    painter.restore();
    painter.save();
    painter.translate(xcenter, ycenter);
    for (const auto& each : previousCombo) {
        QPixmap pixmap;
        QString fileName;
        fileName += cardKinds[int(each.getCardKind())];
        fileName += cardSizes[int(each.getCardSize())];
        pixmap.load("../MyDouDizhu/cards/" + fileName + ".png");
        QPixmap scaledPixmap = pixmap.scaled(pixmap.size() * 0.6, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(cnt, 0, scaledPixmap);
        cnt += 40;
    }
    cnt = 0;
    painter.restore();
    painter.save();
    painter.translate(xlandlord, ylandlord);
    for (const auto& each : landlordCards) {
        QPixmap pixmap;
        if (landlordDecided) {
            QString fileName;
            fileName += cardKinds[int(each.getCardKind())];
            fileName += cardSizes[int(each.getCardSize())];
            pixmap.load("../MyDouDizhu/cards/" + fileName + ".png");
        } else {
            pixmap.load("../MyDouDizhu/cards/PADDING.png");
        }
        QPixmap scaledPixmap = pixmap.scaled(pixmap.size() * 0.3, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        painter.drawPixmap(cnt, 0, scaledPixmap);
        cnt += 60;
    }
    cnt = 0;
    painter.restore();
    painter.save();
    painter.translate(xprevious, yprevious);
    painter.rotate(90);
    QPixmap padPixmap("../MyDouDizhu/cards/PADDING.png");
    QPixmap scaledPadPixmap = padPixmap.scaled(padPixmap.size() * 0.5, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    for (int i = 0; i < cardNumPrevious; i++) {
        painter.drawPixmap(cnt, 0, scaledPadPixmap);
        cnt += 10;
    }
    cnt = 0;
    painter.restore();
    painter.translate(xnext, ynext);
    painter.rotate(90);
    for (int i = 0; i < cardNumNext; i++) {
        painter.drawPixmap(cnt, 0, scaledPadPixmap);
        cnt += 10;
    }
}

//-----准备函数和重置函数-----

void MainWindow::getDataFromDialog() {
    switch (player->getType()) {
        case (PlayerType::A): {
            connects[0] = nullptr;
            connects[1] = inputDialog->getConnectionAB();
            connects[2] = inputDialog->getConnectionAC();
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
            connect(each, &QTcpSocket::disconnected, this, &MainWindow::handleQuit);
        }
    }
    connect(this, &MainWindow::dataReceived, this, &MainWindow::handleRead);
}

void MainWindow::gameInit() {
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
        landlordCards.push_back(globalDeck[i]);
        sendToRest(("landlordcard;" + globalDeck[i].toString()).toUtf8());
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
    } else if (randPlayer == 1) {
        sendTo(1, QString("paticipate").toUtf8());
    } else if (randPlayer == 2) {
        sendTo(2, QString("paticipate").toUtf8());
    }
    repaint();
    sendToRest("repaint");
}

void MainWindow::init() {
    disconnect(ui->pushButtonDecline, 0, 0, 0);
    disconnect(ui->pushButtonLandlord, 0, 0, 0);
    connect(ui->pushButtonDecline, &QPushButton::clicked, this, &MainWindow::handleSkip);
    connect(ui->pushButtonLandlord, &QPushButton::clicked, this, &MainWindow::handlePlay);
    ui->pushButtonDecline->setText("Skip");
    ui->pushButtonLandlord->setText("Play");
    ui->pushButtonDecline->setVisible(true);
    ui->pushButtonLandlord->setVisible(true);
    ui->pushButtonDecline->setEnabled(false);
    ui->pushButtonLandlord->setEnabled(false);
    previousComboType = CardCombo::Illegal;
    lastPlayer = landlord;
}

void MainWindow::start() {
    ui->pushButtonDecline->setEnabled(true);
    ui->pushButtonLandlord->setEnabled(true);
}

void MainWindow::reset() {
    player->clearCardsChosen();
    player->reset();
    lastPlayer = PlayerType::A;
    previousCombo.clear();
    landlordCards.clear();
    globalDeck.clear();
    previousComboType = CardCombo::Illegal;
    cardNumPrevious = cardNumNext = 17;
    ui->lcdNext->display(cardNumNext);
    ui->lcdPrevious->display(cardNumPrevious);
    ui->pushButtonDecline->setEnabled(false);
    ui->pushButtonLandlord->setEnabled(false);
    ui->pushButtonDecline->setText("Decline");
    ui->pushButtonLandlord->setText("Try");
    disconnect(ui->pushButtonDecline, 0, 0, 0);
    disconnect(ui->pushButtonLandlord, 0, 0, 0);
    qDebug() << (int)player->getType() << "reset";
    replay[0] = replay[1] = replay[2] = false;
    readyB = readyC = false;
    if (player->getType() != PlayerType::A) {
        sendTo(0, ("complete;" + QString::number((int)player->getType())).toUtf8());
    } repaint();
}

//-----抢地主-----

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
            ui->identityThis->setText("Peasant");
            if (current == 1 || current == -1) {
                ui->identityNext->setText("Landlord");
                ui->identityPrevious->setText("Peasant");
                sendToRest(QString("chosen;%1").arg(int(nextOne())).toUtf8());
                landlord = nextOne();
                sendToNext(cardLandlord.toUtf8());
                cardNumNext += 3;
                ui->lcdNext->display(cardNumNext);
                sendToNext("repaint");
                sendToRest("init");
                sendToNext("start");
            } else {
                ui->identityNext->setText("Peasant");
                ui->identityPrevious->setText("Landlord");
                sendToRest(QString("chosen;%1").arg(int(previousOne())).toUtf8());
                landlord = previousOne();
                sendToPrevious(cardLandlord.toUtf8());
                cardNumPrevious += 3;
                ui->lcdPrevious->display(cardNumPrevious);
                sendToPrevious("repaint");
                sendToRest("init");
                sendToPrevious("start");
            } landlordDecided = true;
            disconnect(ui->pushButtonDecline, 0, 0, 0);
            disconnect(ui->pushButtonLandlord, 0, 0, 0);
            init();
        });
        connect(ui->pushButtonLandlord, &QPushButton::clicked, [=](){
            sendToRest(QString("chosen;%1").arg(int(player->getType())).toUtf8());
            sendToRest("init");
            landlord = player->getType();
            for (auto each : landlordCards) {
                player->addCard(each);
            } landlordDecided = true;
            ui->identityThis->setText("Landlord");
            ui->identityNext->setText("Peasant");
            ui->identityPrevious->setText("Peasant");
            disconnect(ui->pushButtonDecline, 0, 0, 0);
            disconnect(ui->pushButtonLandlord, 0, 0, 0);
            init();
            start();
            repaint();
        });
    }
}

//-----信息发送和接受函数-----

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

QByteArray MainWindow::IntToArray(qint32 source) {
    QByteArray temp;
    QDataStream data(&temp, QIODevice::ReadWrite);
    data << source;
    return temp;
}

qint32 MainWindow::ArrayToInt(QByteArray source) {
    qint32 temp;
    QDataStream data(&source, QIODevice::ReadWrite);
    data >> temp;
    return temp;
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

//-----自定义的eventHandler-----

void MainWindow::handleRead(QByteArray data) {
    QString message = QString(data);
    QStringList attrList = message.split(";");
    if (attrList.size() == 0) {
        qDebug() << "Empty message got!";
        return;
    }
    if (attrList[0] == "paticipate") {
        paticipate(1, 1);
    } else if (attrList[0] == "repaint") {
        repaint();
    } else if (attrList[0] == "landlord") {
        paticipate(attrList[1].toInt(), attrList[2].toInt());
    } else if (attrList[0] == "chosen") {
        landlord = PlayerType(attrList[1].toInt());
        lastPlayer = landlord;
        if (landlord == nextOne()) {
            cardNumNext += 3;
            ui->lcdNext->display(cardNumNext);
            ui->identityThis->setText("Peasant");
            ui->identityNext->setText("Landlord");
            ui->identityPrevious->setText("Peasant");
        } else if (landlord == previousOne()) {
            cardNumPrevious += 3;
            ui->lcdPrevious->display(cardNumPrevious);
            ui->identityThis->setText("Peasant");
            ui->identityNext->setText("Peasant");
            ui->identityPrevious->setText("Landlord");
        } else {
            ui->identityThis->setText("Landlord");
            ui->identityNext->setText("Peasant");
            ui->identityPrevious->setText("Peasant");
        }
        landlordDecided = true;
        repaint();
        qDebug() << (int)landlord;
    } else if (attrList[0] == "card") {
        int cnt = 0;
        for (const auto& each : attrList) {
            cnt++;
            if (each != "" && each != "card") {
                player->addCard(Card(each));
            }
        }
        repaint();
    } else if (attrList[0] == "init") {
        init();
    } else if (attrList[0] == "start") {
        start();
    } else if (attrList[0] == "previous") { //上家出牌
        previousCombo.clear();
        player->clearCardsChosen();
        int pos = attrList.indexOf("type");
        for (int i = 1; i < pos; i++) {
            previousCombo.push_back(Card(attrList[i]));
            //qDebug() << attrList[i];
        }
        previousComboType = (CardCombo)attrList[pos + 1].toInt();
        lastPlayer = (PlayerType)attrList[pos + 3].toInt();
        if (attrList[pos + 4] == "played") {
            cardNumPrevious -= previousCombo.size();
            ui->lcdPrevious->display(cardNumPrevious);
        }
        qDebug() << (int)previousComboType << (int)lastPlayer;
        repaint();
        if (lastPlayer == player->getType() && attrList.indexOf("skipped") != -1)
            ui->pushButtonDecline->setEnabled(false);
        else
            ui->pushButtonDecline->setEnabled(true);
        ui->pushButtonLandlord->setEnabled(true);
    } else if (attrList[0] == "doubleprevious") { //下家出牌,此时我只关心他出了什么牌
        //可以直接放进previousCombo用来显示
        previousCombo.clear();
        player->clearCardsChosen();
        for (const auto& each : attrList) {
            if (each != "doubleprevious" && each != "" && each != "played" && each != "skipped") {
                qDebug() << each;
                previousCombo.push_back(Card(each));
            }
        }
        int pos = attrList.indexOf("skipped");
        if (pos == -1) { //等于-1表示上家真的出牌了
            cardNumNext -= previousCombo.size();
            ui->lcdNext->display(cardNumNext);
        }
        repaint();
    } else if (attrList[0] == "winner") {
        PlayerType winner = (PlayerType)attrList[1].toInt();
        QMessageBox::StandardButton ret;
        QString text = QString(landlord == winner ? "The landlord wins!" : "The peasants win!") + " Play again?";
        if (player->getType() == PlayerType::A) {
            globalPlayer->pause();
        }
        if (winner == player->getType()) {
            winPlayer->setMedia(QUrl("qrc:/audios/victory.mp3"));
        } else {
            winPlayer->setMedia(QUrl("qrc:/audios/failure.mp3"));
        }
        winPlayer->play();
        ret = QMessageBox::information(this, "Game Finished!", text, QMessageBox::Ok | QMessageBox::No);
        if (ret == QMessageBox::Ok) { //重新开始
            replay[(int)player->getType()] = true;
            sendToRest(("replay;" + QString::number((int)player->getType())).toUtf8());
        } else { //退出
            handleQuit();
        }
    } else if (attrList[0] == "replay") {
        replay[attrList[1].toInt()] = true;
        if (replay[0] && replay[1] && replay[2]) { //三人都决定重新开始
            reset();
        }
    } else if (attrList[0] == "complete") { //要发送给A的"我准备好重新开始了"
        if (attrList[1].toInt() == 1) {
            readyB = true;
        } else if (attrList[1].toInt() == 2) {
            readyC = true;
        }
        if (readyB && readyC && player->getType() == PlayerType::A) {
            gameInit();
        }
    } else if (attrList[0] == "landlordcard") {
        landlordCards.push_back(Card(attrList[1]));
    }
}



void MainWindow::handleSkip() {
    //直接把上家出的牌、对应的类型和最后一个出过牌的人发给下家
    QString message = "";
    for (const auto& each : previousCombo) {
        message += each.toString();
    } message += ("type;" + QString::number((int)previousComboType) + ";");
    message += ("last;" + QString::number((int)lastPlayer));
    sendToNext(QString("previous;" + message + ";skipped").toUtf8());
    //因为我这次没有出牌而上家的previousCombo还是他自己出的那些牌,所以其实不用sendToPrevious
    ui->pushButtonDecline->setEnabled(false);
    ui->pushButtonLandlord->setEnabled(false);
    player->clearCardsChosen();
} //

void MainWindow::handlePlay() { //
   /* 检测所选的牌是否合法
    * 检测所选的牌是否大于上家出的牌组合
    * 如果都是,那么从手牌中删去这些牌
    * 并且把这个牌组合、它对应的类型和最后一个出过牌的人(就是自己)
    * 发给下家
    */
    QString message = "";
    CardCombo chosenCardCombo = player->checkCards();
    Comparer checkGreater;
    auto chosenCard = player->getChosenCard();
    QString debugMessage = "Cards chosen:";
    for (const auto& each : chosenCard) {
        debugMessage += each.toString();
    } qDebug() << debugMessage;
    if (chosenCardCombo != CardCombo::Illegal) { //合法
        if (previousComboType == CardCombo::Illegal || lastPlayer == player->getType() ||
            chosenCardCombo == CardCombo::SuperBomb ||
            //第一次出牌 ||　上次是我出的牌（另两人都没出）|| 王炸是无条件满足的
            (chosenCardCombo == previousComboType && checkGreater(chosenCard, previousCombo, chosenCardCombo)) ||
            //我和上次出牌类型一致，而且更大（炸弹接炸弹算在这里面，炸弹的大小判断这里有了）
            (chosenCardCombo == CardCombo::Bomb && previousComboType != CardCombo::SuperBomb)) {
            //因为炸弹接炸弹会被判定为前一种,所以这里处理的是炸弹接除王炸以外的其他牌型
            previousCombo.clear();
            player->clearCardsChosen();
            for (const auto& each : chosenCard) {
                message += each.toString();
                player->removeCard(each);
                previousCombo.push_back(each);
            }
            sendToPrevious(QString("doubleprevious;" + message + ";played").toUtf8());
            message += ("type;" + QString::number((int)chosenCardCombo) + ";");
            message += ("last;" + QString::number((int)player->getType()));
            sendToNext(QString("previous;" + message + ";played").toUtf8());
            ui->pushButtonDecline->setEnabled(false);
            ui->pushButtonLandlord->setEnabled(false);
            if (player->expose().size() == 0) { //哈哈 俺赢啦
                sendToRest(("winner;" + QString::number((int)player->getType())).toUtf8());
                if (player->getType() == PlayerType::A) {
                    globalPlayer->pause();
                }
                winPlayer->setMedia(QUrl("qrc:/audios/victory.mp3"));
                winPlayer->play();
                QString text = QString(player->getType() == landlord ? "The landlord wins!" : "The peasants win!") + " Play again?";
                auto ret = QMessageBox::information(this, "Game Finished!", text, QMessageBox::Ok | QMessageBox::No);
                if (ret == QMessageBox::Ok) { //重新开始
                    replay[(int)player->getType()] = true;
                    sendToRest(("replay;" + QString::number((int)player->getType())).toUtf8());
                } else { //退出
                    handleQuit();
                }
            }
            repaint();
        } else {
            QMessageBox::warning(this, "Invalid", "This combo is yet also invalid");
        }
    } else {
        QMessageBox::warning(this, "Invalid", "This combo is invalid");
    }
}

void MainWindow::handleQuit() {
    for (auto&& each : connects) {
        if (each != nullptr) {
            each->disconnectFromHost();
        }
    } close();
}

void MainWindow::handleStateChanged(QMediaPlayer::State state) {
    if (state == QMediaPlayer::StoppedState) {
        globalPlayer->play();
    }
}
