#ifndef PLAYER_H
#define PLAYER_H

#include "card.h"
#include <QSet>

class Player {
private:
    typedef Assist::CardCombo CardCombo;
    bool landLord;
    QSet<Card> hand, cardsChosen;
public:
    Player();
    void addCard(const Card& card);
    void setLandLord();
    bool isLandLord() const;
    CardCombo checkCards() const;
};



#endif // PLAYER_H
