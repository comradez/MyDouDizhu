#ifndef CARD_H
#define CARD_H

#include "assist.h"
#include <QString>
#include <QPair>

class Card {
private:
    typedef Assist::CardKind CardKind;
    typedef Assist::CardSize CardSize;
    CardKind cardKind;
    CardSize cardSize;

public:
    Card(CardKind _cardKind, CardSize _cardSize);
    CardKind getCardKind() const;
    CardSize getCardSize() const;
    QPair<CardKind,CardSize> getCard() const;
    bool operator<(const Card& b) {
        return int(cardSize) < int(b.cardSize);
    }
};

uint qHash(const Card& card);

#endif // CARD_H
