#include "player.h"

Player::Player() : landLord(false) {}

void Player::addCard(const Card &card) {
    hand.insert(card);
}

Assist::CardCombo Player::checkCards() const {
    std::sort(cardsChosen.begin(), cardsChosen.end());
    switch (cardsChosen.size()) {
        case 1: {
            return CardCombo::Single;
            break;
        }
        case 2: {
            if (cardsChosen.begin()->getCardSize() == cardsChosen.rbegin()->getCardSize()) {
                return CardCombo::Pair;
            } else {
                return CardCombo::Illegal;
            } break;
        }
        case 3: {
            auto middleIter = cardsChosen.begin()++;
            if (cardsChosen.begin()->getCardSize() == cardsChosen.rbegin()->getCardSize() &&
                cardsChosen.begin()->getCardSize() == middleIter->getCardSize()) {
                return CardCombo::Three;
            } else {
                return CardCombo::Illegal;
            } break;
        }
        case 4: {
            if (cardsChosen[0].getCardSize() == cardsChosen[1].getCardSize() &&
                cardsChosen[1].getCardSize() == cardsChosen[2].getCardSize() &&
                cardsChosen[2].getCardSize() == cardsChosen[3].getCardSize()) {
                return CardCombo::Bomb;
            } else if (cardsChosen[0].getCardSize() == cardsChosen[1].getCardSize() &&
                       cardsChosen[1].getCardSize() == cardsChosen[2].getCardSize() &&
                       cardsChosen[2].getCardSize() != cardsChosen[3].getCardSize()) {
                return CardCombo::ThreeAndOne;
            } else if (cardsChosen[0].getCardSize() != cardsChosen[1].getCardSize() &&
                       cardsChosen[1].getCardSize() == cardsChosen[2].getCardSize() &&
                       cardsChosen[2].getCardSize() == cardsChosen[3].getCardSize()) {
                return CardCombo::ThreeAndOne;
            }
        }
    }
}
