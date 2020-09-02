#include "controller.h"
#include "mainwindow.h"

Controller::Controller()
{

}

GlobalController::GlobalController(QObject* parent)
    : QObject(parent) {
    addCard();
    shuffleCard();
    drawCard();
}

void GlobalController::addCard() {
    for (int i = CardSize::Three; i <= CardSize::Two; i++) {
        for (int j = (int)CardKind::Spade; j <= (int)CardKind::Club; j++) {
            globalDeck.insert(Card(CardKind(j), CardSize(i)));
        }
    }
    globalDeck.insert(Card(CardKind::Joker,  CardSize::SmallJoker));
    globalDeck.insert(Card(CardKind::Joker,  CardSize::BigJoker));
}

void GlobalController::shuffleCard() {
    std::random_shuffle(globalDeck.begin(), globalDeck.end());
}

void GlobalController::drawCard() {
    for (int i = 0; i < 3; i++) {
        landlordCards.insert(*globalDeck.begin());
        globalDeck.remove(*globalDeck.begin());
    }
    for (int i = 0; i < 17; i++) {
        emit giveCardToA(*globalDeck.begin());
        globalDeck.erase(globalDeck.begin());
        emit giveCardToB(*globalDeck.begin());
        globalDeck.erase(globalDeck.begin());
        emit giveCardToC(*globalDeck.begin());
        globalDeck.erase(globalDeck.begin());
    }
}
