#ifndef PLAYER_H
#define PLAYER_H

#include "card.h"
#include <QObject>
#include <set>
#include <algorithm>
#include <functional>
#include <utility>
#include <QSet>
#include <QDebug>
#include <QVector>

class Player : public QObject {
    Q_OBJECT
private:
    bool sorted = true;
    typedef Assist::CardCombo CardCombo;
    typedef Assist::CardSize CardSize;
    typedef Assist::PlayerType PlayerType;
    typedef bool(*fbool)();
    PlayerType type;
    bool landLord;
    std::vector<Card> hand;
    //std::vector<bool> handChosen;
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
    void addCard(Card card);
    void setLandLord();
    bool isLandLord() const;
    const std::vector<Card>& expose();
    void toggleChosen(int pos);

    void setType(PlayerType _type);
    PlayerType getType() const;
    const QList<Card>& getChosenCard() const;

    CardCombo checkCards();

    //QList<Card>& expose();
};



#endif // PLAYER_H
