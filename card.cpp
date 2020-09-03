#include "card.h"

Card::Card(CardKind _cardKind, CardSize _cardSize)
    : cardKind(_cardKind), cardSize(_cardSize) {}

Card::Card(QString _message) {
    QStringList stringList = _message.split(",");
    cardKind = CardKind(stringList[0].toInt());
    cardSize = CardSize(stringList[1].toInt());
    qDebug() << (int)cardKind << (int)cardSize;
}

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

QString Card::toString() const {
    return QString("%1,%2;").arg((int)cardKind).arg((int)cardSize);
}

void Card::setChosen(bool _chosen) {
    chosen = _chosen;
}

bool Card::getChosen() const {
    return chosen;
}

void Card::show() const {
    qDebug() << "This is a card:" << (int)cardKind << (int)cardSize;
}
