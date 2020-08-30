#include "card.h"

Card::Card(CardKind _cardKind, CardSize _cardSize)
    : cardKind(_cardKind), cardSize(_cardSize) {}

Assist::CardKind Card::getCardKind() const {
    return cardKind;
}

Assist::CardSize Card::getCardSize() const {
    return cardSize;
}

QPair<Assist::CardKind,Assist::CardSize> Card::getCard() const {
    return qMakePair(cardKind, cardSize);
}

uint qHash(const Card& card) {
    return int(card.getCardKind()) * 100 + int(card.getCardSize());
}
