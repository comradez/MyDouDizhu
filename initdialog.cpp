#include "initdialog.h"
#include "ui_initdialog.h"

InitDialog::InitDialog(QWidget *parent) :
    QDialog(parent),
    currentType(PlayerType::A),
    ui(new Ui::InitDialog)
{
    ui->setupUi(this);
    connect(ui->radioButtonA, &QRadioButton::toggled, [=](bool flag) {
        if (flag) {
            currentType = PlayerType::A;
            ui->lineEditA->setText("LocalHost");
            ui->lineEditA->setDisabled(true);
            ui->lineEditB->setText("127.0.0.1");
            ui->lineEditB->setDisabled(false);
            ui->lineEditC->setText("127.0.0.1");
            ui->lineEditC->setDisabled(false);
        }
    });
    connect(ui->radioButtonB, &QRadioButton::toggled, [=](bool flag) {
        if (flag) {
            currentType = PlayerType::B;
            ui->lineEditB->setText("LocalHost");
            ui->lineEditB->setDisabled(true);
            ui->lineEditC->setText("127.0.0.1");
            ui->lineEditC->setDisabled(false);
            ui->lineEditA->setText("127.0.0.1");
            ui->lineEditA->setDisabled(false);
        }
    });
    connect(ui->radioButtonC, &QRadioButton::toggled, [=]() {
        currentType = PlayerType::C;
        ui->lineEditC->setText("LocalHost");
        ui->lineEditC->setDisabled(true);
        ui->lineEditA->setText("127.0.0.1");
        ui->lineEditA->setDisabled(false);
        ui->lineEditB->setText("127.0.0.1");
        ui->lineEditB->setDisabled(false);
    });
    connect(ui->pushButton, &QPushButton::clicked, this, &InitDialog::startMatching);
}


InitDialog::~InitDialog()
{
    delete ui;
}

void InitDialog::startMatching() {
    if (currentType == PlayerType::A) {
        serverOne = new QTcpServer(nullptr);
        serverTwo = new QTcpServer(nullptr);
        serverOne->listen(QHostAddress::LocalHost, portAB);
        serverTwo->listen(QHostAddress::LocalHost, portAC);
        connect(serverOne, &QTcpServer::newConnection, this, &InitDialog::handleA);
        connect(serverTwo, &QTcpServer::newConnection, this, &InitDialog::handleA);
    }
    if (currentType == PlayerType::B) {
        serverOne = new QTcpServer(nullptr);
        serverOne->listen(QHostAddress::LocalHost, portBC);
        connect(serverOne, &QTcpServer::newConnection, this,&InitDialog::handleB);
        socketOne = new QTcpSocket(nullptr);
        socketOne->connectToHost(QHostAddress(ui->labelA->text()), portAB);
        while (socketOne->waitForConnected(500) == false) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            socketOne->connectToHost(QHostAddress::LocalHost, portAB);
        }
    }
    if (currentType == PlayerType::C) {
        socketOne = new QTcpSocket(nullptr);
        socketTwo = new QTcpSocket(nullptr);
        socketOne->connectToHost(QHostAddress::LocalHost, portAC);
        socketTwo->connectToHost(QHostAddress::LocalHost, portBC);
        while (socketTwo->waitForConnected(500) == false) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            socketTwo->connectToHost(QHostAddress::LocalHost, portBC);
        }
        while (socketOne->waitForConnected(500) == false) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            socketOne->connectToHost(QHostAddress::LocalHost, portAC);
        }
        connect(socketOne, &QTcpSocket::readyRead, this, &InitDialog::handleC);
    }
}

void InitDialog::handleA() {
    if (serverOne->hasPendingConnections()) {
        connectionAB = serverOne->nextPendingConnection();
        readyB = true;
    }
    if (serverTwo->hasPendingConnections()) {
        connectionAC = serverTwo->nextPendingConnection();
        readyC = true;
    }
    if (readyB && readyC) {
        if (connectionAC->state() == QTcpSocket::ConnectedState) {
            connectionAC->write("A is ready!");
        }
        connect(connectionAB, &QTcpSocket::readyRead, [=]() {
            if (connectionAB->readAll() == "B is ready!") {
                startGame();
                connectionAC->write("Startgame");
                //disconnect(connectionAB, 0, 0, 0);
                //disconnect(connectionAC, 0, 0, 0);
            }
        });
    }
}

void InitDialog::handleB() {
    if (serverOne->hasPendingConnections()) {
        connectionBC = serverOne->nextPendingConnection();
        connect(connectionBC, &QTcpSocket::readyRead, [=]() {
            QByteArray byteArray = connectionBC->readAll();
            if (byteArray == "C is ready!") {
                socketOne->write("B is ready!");
            } else if (byteArray == "Startgame") {
                startGame();
                //disconnect(connectionBC, 0, 0, 0);
                //disconnect(socketOne, 0, 0, 0);
            }
        });
    }
}

void InitDialog::handleC() {
    QByteArray byteArray = socketOne->readAll();
    if (byteArray == "A is ready!") {
        socketTwo->write("C is ready!");
    } else if (byteArray == "Startgame") {
        socketTwo->write("Startgame");
        startGame();
        //disconnect(socketOne, 0, 0, 0);
        //disconnect(socketTwo, 0, 0, 0);
    }
}

void InitDialog::startGame() {
    accept();
    qDebug() << (int)currentType << "starts game!";
}

Assist::PlayerType InitDialog::getPlayerType() const {
    return currentType;
}

QTcpSocket* InitDialog::getConnectionAB() const {
    return connectionAB;
}

QTcpSocket* InitDialog::getConnectionAC() const {
    return connectionAC;
}

QTcpSocket* InitDialog::getConnectionBC() const {
    return connectionBC;
}

QTcpSocket* InitDialog::getSocketOne() const {
    return socketOne;
}

QTcpSocket* InitDialog::getSocketTwo() const {
    return socketTwo;
}
