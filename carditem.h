#ifndef CARDITEM_H
#define CARDITEM_H

#include <QObject>
#include <QPixmap>
#include <QGraphicsItem>

class CardItem : public QGraphicsItem
{
    Q_OBJECT
public:
    CardItem(QGraphicsItem* parent = nullptr);
private:
    QPixmap* picture;
};

#endif // CARDITEM_H
