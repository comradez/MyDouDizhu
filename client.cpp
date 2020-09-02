#include "client.h"

Client::Client(QObject *parent) : QObject(parent) {
    socket = new QTcpSocket(nullptr);
    connect(socket, &QTcpSocket::readyRead, this, [=](){ emit readyRead(); });
}

Client::Client(QTcpSocket* _socket, QObject *parent) : QObject(parent) {
    socket = _socket;
    connect(socket, &QTcpSocket::readyRead, this, [=](){ emit readyRead(); });
}

void Client::setSocket(QTcpSocket *_socket) {
    socket = _socket;
    connect(socket, &QTcpSocket::readyRead, this, [=](){ emit readyRead(); });
}

QTcpSocket* Client::getSocket() const {
    return socket;
}

bool Client::connectToHost(QString host, int port) {
    socket->connectToHost(QHostAddress(host), port);
    return socket->waitForConnected();
}

bool Client::writeData(QByteArray data) {
    if (socket->state() == QTcpSocket::ConnectedState) {
        socket->write(IntToArray(data.size()));
        socket->write(data);
        return socket->waitForBytesWritten();
    } else {
        return false;
    }
}

QByteArray Client::IntToArray(qint32 source) {
    QByteArray temp;
    QDataStream data(&temp, QIODevice::ReadWrite);
    data << source;
    return temp;
}
