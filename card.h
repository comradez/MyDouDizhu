#ifndef CARD_H
#define CARD_H

#include "assist.h"
#include <QWidget>
#include <QString>
#include <QStringList>
#include <QPair>

class Card {
private:
    typedef Assist::CardKind CardKind;
    typedef Assist::CardSize CardSize;
    CardKind cardKind;
    CardSize cardSize;

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
        return int(cardSize) < int(b.cardSize);
    }
    QString toString() const;
};

uint qHash(const Card& card);

#endif // CARD_H
