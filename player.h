#ifndef PLAYER_H
#define PLAYER_H

#include "card.h"
#include <algorithm>
#include <QSet>
#include <QVector>

class Player {
private:
    typedef Assist::CardCombo CardCombo;
    typedef Assist::CardSize CardSize;
    bool landLord;
    QSet<Card> hand;
    QList<Card> cardsChosen;
public:
    Player();
    void addCard(const Card& card);
    void setLandLord();
    bool isLandLord() const;
    CardCombo checkCards() const;
};



#endif // PLAYER_H
