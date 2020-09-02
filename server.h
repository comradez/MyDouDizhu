#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);

signals:
    void dataReceived(QByteArray);

private slots:
    void newConnection();
    void disconnected();
    void readyRead();

private:
    QTcpServer *server;
    QHash<QTcpSocket*, QByteArray*> buffers;
    QHash<QTcpSocket*, qint32*> sizes;
    qint32 ArrayToInt(QByteArray source);
};
#endif // SERVER_H
