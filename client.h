#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDataStream>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    Client(QTcpSocket* _socket, QObject* parent);
    void setSocket(QTcpSocket* _socket);
    QTcpSocket* getSocket() const;

public slots:
    bool connectToHost(QString host, int port);
    bool writeData(QByteArray data);

public: signals:
    void readyRead();

private:
    QTcpSocket *socket;
    QByteArray IntToArray(qint32 source);
};
#endif //CLIENT_H
