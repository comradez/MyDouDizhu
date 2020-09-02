#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QSet>
#include <QList>
#include <set>
#include "assist.h"
#include "card.h"
#include "player.h"

class Controller
{
public:
    Controller();
};

class GlobalController : public QObject {
    Q_OBJECT
private:
    typedef Assist::CardSize CardSize;
    typedef Assist::CardKind CardKind;
    QSet<Card> globalDeck;
    QSet<Card> landlordCards;
public:
    GlobalController(QObject* parent = nullptr);
    void addCard();
    void shuffleCard();
    void drawCard();
    void sendToB(QByteArray byteArray);
    void sendToC(QByteArray byteArray);
public: signals:
    void giveCardToA(Card card);
    void giveCardToB(Card card);
    void giveCardToC(Card card);
};

#endif // CONTROLLER_H
