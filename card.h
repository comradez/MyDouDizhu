#ifndef CARD_H
#define CARD_H

#include "assist.h"
#include <QWidget>
#include <QString>
#include <QStringList>
#include <QPair>
#include <QDebug>

class Card {
private:
    typedef Assist::CardKind CardKind;
    typedef Assist::CardSize CardSize;
    CardKind cardKind;
    CardSize cardSize;
    bool chosen;
public:
    Card(CardKind _cardKind, CardSize _cardSize);
    Card(QString _message);
    CardKind getCardKind() const;
    CardSize getCardSize() const;
    QPair<CardKind,CardSize> getCard() const;
    bool operator==(const Card& b) const {
        return cardKind == b.cardKind && cardSize == b.cardSize;
    }
    bool operator<(const Card& b) const {
        if(int(cardSize) < int(b.cardSize)) {
            return true;
        } else if (int(cardSize) > int(b.cardSize)) {
            return false;
        } else return int(cardKind) < int(cardKind);
    }
    void setChosen(bool _chosen);
    bool getChosen() const;
    void show() const;
    QString toString() const;
};

uint qHash(const Card& card);

#endif // CARD_H
