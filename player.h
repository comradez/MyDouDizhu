#ifndef PLAYER_H
#define PLAYER_H

#include "card.h"
#include <QObject>
#include <algorithm>
#include <functional>
#include <utility>
#include <QSet>
#include <QDebug>
#include <QVector>

class Player : public QObject {
    Q_OBJECT
private:
    typedef Assist::CardCombo CardCombo;
    typedef Assist::CardSize CardSize;
    typedef Assist::PlayerType PlayerType;
    typedef bool(*fbool)();
    PlayerType type;
    bool landLord;
    QSet<Card> hand;
    QList<Card> cardsChosen;
    QVector<int> assistVector;
    bool (Player::*checkers[15])() const = {
        &Player::checkSingle,
        &Player::checkPair,
        &Player::checkThree,
        &Player::checkThreeAndOne,
        &Player::checkThreeAndPair,
        &Player::checkFourAndTwo,
        &Player::checkFourAndPair,
        &Player::checkFourAndTwoPairs,
        &Player::checkAero,
        &Player::checkAeroAndOne,
        &Player::checkAeroAndPair,
        &Player::checkContinous,
        &Player::checkContinousPair,
        &Player::checkBomb,
        &Player::checkSuperBomb
    };
    CardCombo combos[15] = {
        CardCombo::Single,
        CardCombo::Pair,
        CardCombo::Three,
        CardCombo::ThreeAndOne,
        CardCombo::ThreeAndPair,
        CardCombo::FourAndTwo,
        CardCombo::FourAndPair,
        CardCombo::FourAndTwoPairs,
        CardCombo::Aero,
        CardCombo::AeroAndOne,
        CardCombo::AeroAndPair,
        CardCombo::Continous,
        CardCombo::ContinousPair,
        CardCombo::Bomb,
        CardCombo::SuperBomb
    };

    bool continous(int value, int times, const QVector<int>& paraVector, bool twoException) const;
    bool checkSingle() const;
    bool checkPair() const;
    bool checkSuperBomb() const;
    bool checkThree() const;
    bool checkThreeAndOne() const;
    bool checkThreeAndPair() const;
    bool checkBomb() const;
    bool checkFourAndTwo() const;
    bool checkFourAndPair() const;
    bool checkFourAndTwoPairs() const;
    bool checkAero() const;
    bool checkAeroAndOne() const;
    bool checkAeroAndPair() const;
    bool checkContinous() const;
    bool checkContinousPair() const;
public:
    Player();
    void addCard(const Card& card);
    void setLandLord();
    bool isLandLord() const;

    void setType(PlayerType _type);
    PlayerType getType() const;

    CardCombo checkCards();

    //QList<Card>& expose();
};



#endif // PLAYER_H
