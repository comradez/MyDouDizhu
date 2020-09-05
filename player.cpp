#include "player.h"

Player::Player() {
    assistVector.resize(18);
}

void Player::addCard(Card card) {
    hand.push_back(card);
    sorted = false;
}

bool Player::checkSingle() const {
    return assistVector.count(1) == 1 &&
           assistVector.count(0) == 17;
}

bool Player::checkPair() const {
    return assistVector.count(2) == 1 && //有一种花色出现了两张牌
           assistVector.count(0) == 17; //而其他花色啥也没有
}

bool Player::checkSuperBomb() const {
    return assistVector[CardSize::SmallJoker] == 1 && //出现了小王
           assistVector[CardSize::BigJoker] == 1 && //出现了大王
           assistVector.count(0) == 16; //其他花色没出现
}

bool Player::checkThree() const {
    return assistVector.count(3) == 1 && //有一种花色出现了三张牌
           assistVector.count(0) == 17; //而其他花色啥也没有
}

bool Player::checkBomb() const {
    return assistVector.count(4) == 1 && //有一种花色出现了四张牌
           assistVector.count(0) == 17; //而其他花色啥也没有
}

bool Player::checkThreeAndOne() const {
    return assistVector.count(3) == 1 &&
           assistVector.count(1) == 1 &&
           assistVector.count(0) == 16;
}

bool Player::checkThreeAndPair() const {
    return assistVector.count(3) == 1 &&
           assistVector.count(2) == 1 &&
           assistVector.count(0) == 16;
}

bool Player::checkFourAndTwo() const {
    return (assistVector.count(4) == 1 &&
            assistVector.count(1) == 2 &&
            assistVector.count(0) == 15) ||
           (assistVector.count(4) == 1 &&
            assistVector.count(2) == 1 &&
            assistVector.count(0) == 16);
}

bool Player::checkFourAndTwoPairs() const {
    return assistVector.count(4) == 1 &&
           assistVector.count(2) == 2 &&
           assistVector.count(0) == 15;
}

bool Player::checkAero() const {
    int threeCount = assistVector.count(3);
    if (threeCount >= 2 && assistVector.count(0) == 18 - threeCount) {
        return continous(3, threeCount, assistVector, true);
    } return false;
}

bool Player::checkAeroAndOne() const {
    int threeCount = assistVector.count(3);
    if (threeCount >= 2 &&
        assistVector.count(1) == threeCount &&
        assistVector.count(0) == 18 - 2 * threeCount) {
        return continous(3, threeCount, assistVector, true);
    } return false;
}

bool Player::checkAeroAndMulti() const {
    int threeCount = assistVector.count(3);
    if (threeCount % 2 != 0) {
        return false;
    }
    if (threeCount >= 2 &&
        assistVector.count(2) == threeCount / 2 &&
        assistVector.count(0) == 18 - threeCount - threeCount / 2) {
        return continous(3, threeCount, assistVector, true);
    } return false;
}

bool Player::checkAeroAndPair() const {
    int threeCount = assistVector.count(3);
    if (threeCount >= 2 &&
        assistVector.count(2) == threeCount &&
        assistVector.count(0) == 18 - 2 * threeCount) {
        return continous(3, threeCount, assistVector, true);
    } return false;
}

bool Player::continous(int value, int times, const QVector<int>& paraVector, bool twoException) const {
    int *tempArray = new int[times];
    tempArray[0] = paraVector.indexOf(value);
    for (int i = 1; i < times; i++) {
        tempArray[i] = paraVector.indexOf(value, tempArray[i - 1] + 1);
        if (tempArray[i] != tempArray[i - 1] + 1 || (twoException && tempArray[i] == CardSize::Two)) {
            delete[] tempArray;
            return false;
        }
    }
    delete[] tempArray;
    return true;
}

bool Player::checkContinous() const {
    int oneCount = assistVector.count(1);
    if (oneCount >= 5 && assistVector.count(0) == 18 - oneCount) {
        return continous(1, oneCount, assistVector, true);
    } return false;
}

bool Player::checkContinousPair() const {
    int twoCount = assistVector.count(2);
    if (twoCount >= 3 && assistVector.count(0) == 18 - twoCount) {
        return continous(2, twoCount, assistVector, true);
    } return false;
}

void Player::toggleChosen(int pos) {
    if (hand[pos].getChosen() == true) {
        hand[pos].setChosen(false);
        for (auto it = cardsChosen.begin(); it != cardsChosen.end(); it++) {
            if (*it == hand[pos]) {
                cardsChosen.erase(it);
                break; //在erase之后直接break是安全的
            }
        }
    } else {
        hand[pos].setChosen(true);
        cardsChosen.push_back(hand[pos]);
    }
}

Assist::CardCombo Player::checkCards() {
    assistVector.clear();
    assistVector.resize(18);
    for (const auto& each : cardsChosen) {
        assistVector[each.getCardSize()]++;
    }
    for (int i = 0; i < 15; i++) {
        auto each = checkers[i];
        if ((this->*each)()) {
            qDebug() << (int)combos[i];
            return combos[i];
        }
    }
    return CardCombo::Illegal;
}

void Player::setType(PlayerType _type) {
    type = _type;
}

const std::vector<Card>& Player::expose() {
    if (sorted == false) {
        std::stable_sort(hand.begin(), hand.end());
        sorted = true;
    }
    return hand;
}

Assist::PlayerType Player::getType() const {
    return type;
}

const QList<Card>& Player::getChosenCard() const {
    return cardsChosen;
}

void Player::removeCard(const Card &card) {
    for (auto it = cardsChosen.begin(); it != cardsChosen.end(); it++) {
        if (*it == card) {
            it->setChosen(false);
            cardsChosen.erase(it);
            break;
        }
    }
    //cardsChosen.clear();
    for (auto it = hand.begin(); it != hand.end(); it++) {
        if (*it == card) {
            hand.erase(it);
            break;
        }
    }
}

void Player::clearCardsChosen() {
    cardsChosen.clear();
}

void Player::reset() {
    cardsChosen.clear();
    assistVector.clear();
    assistVector.resize(18);
    hand.clear();
}
