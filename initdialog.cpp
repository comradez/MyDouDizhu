#include "initdialog.h"
#include "ui_initdialog.h"

InitDialog::InitDialog(QWidget *parent) :
    QDialog(parent),
    currentType(PlayerType::None),
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
        serverOne->listen(QHostAddress(ui->lineEditB->text()), portAB);
        connect(serverOne, &QTcpServer::newConnection, this, &InitDialog::acceptConnection);

        serverTwo = new QTcpServer(nullptr);
        serverTwo->listen(QHostAddress(ui->lineEditC->text()), portAC);
        connect(serverTwo, &QTcpServer::newConnection, this, &InitDialog::acceptConnection);
    } else if (currentType == PlayerType::B) {
        socketOne = new QTcpSocket(nullptr);
        socketOne->connectToHost(QHostAddress(ui->lineEditA->text()), portAB);
        while (socketOne->waitForConnected(500) == false) {
            qDebug() << "Connection Failed";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            socketOne->connectToHost(QHostAddress(ui->lineEditA->text()), portAB);
        } socketOne->write(QByteArray("B"));

        serverOne = new QTcpServer(nullptr);
        serverOne->listen(QHostAddress(ui->lineEditC->text()), portBC);
        connect(serverOne, &QTcpServer::newConnection, this, &InitDialog::acceptConnection);
    } else if (currentType == PlayerType::C) {
        socketOne = new QTcpSocket(nullptr);
        socketOne->connectToHost(QHostAddress(ui->lineEditA->text()), portAC);
        while (socketOne->waitForConnected(500) == false) {
            qDebug() << "Connection Failed";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            socketOne->connectToHost(QHostAddress(ui->lineEditA->text()), portAC);
        } socketOne->write(QByteArray("C"));

        socketTwo = new QTcpSocket(nullptr);
        socketTwo->connectToHost(QHostAddress(ui->lineEditA->text()), portBC);
        while (socketTwo->waitForConnected(500) == false) {
            qDebug() << "Connection Failed";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            socketTwo->connectToHost(QHostAddress(ui->lineEditB->text()), portBC);
        } socketTwo->write(QByteArray("C"));
    }
}

void InitDialog::acceptConnection() {
    if (currentType == PlayerType::A) {
        if (serverOne->hasPendingConnections()) {
            connectionAB = serverOne->nextPendingConnection();
            connect(connectionAB, &QTcpSocket::readyRead, this, &InitDialog::handleAB);
        }
        if (serverTwo->hasPendingConnections()) {
            connectionAC = serverTwo->nextPendingConnection();
            connect(connectionAC, &QTcpSocket::readyRead, this, &InitDialog::handleAC);
        }
    } else if (currentType == PlayerType::B) {
        if (serverOne->hasPendingConnections()) {
            connectionBC = serverOne->nextPendingConnection();
            connect(connectionBC, &QTcpSocket::readyRead, this, &InitDialog::handleBC);
        }
    }
}

void InitDialog::handleAB() {
    if (QString(connectionAB->readAll()) == tr("B")) {
        connectAB = true;
        qDebug() << "AB OK!";
    }
}

void InitDialog::handleAC() {
    if (QString(connectionAC->readAll()) == tr("C")) {
        connectAC = true;
        qDebug() << "AC OK!";
    }
}

void InitDialog::handleBC() {
    if (QString(connectionBC->readAll()) == tr("C")) {
        connectBC = true;
        qDebug() << "BC OK!";
    }
}
