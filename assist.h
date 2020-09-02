#ifndef ASSIST_H
#define ASSIST_H
class Assist {
    enum class PlayerType {
        A, B, C
    };
    enum class CardKind {
        Spade, Heart, Diamond, Club, Joker //黑桃，红桃，方块，梅花
    };
    enum CardSize {
        Three = 3, Four, Five, Six, Seven, Eight, Nine, Ten, Jack, Queen, King, One, Two, SmallJoker, BigJoker
    };
    enum class CardCombo {
        Single, Pair, Three, ThreeAndOne, ThreeAndPair, Bomb, FourAndTwo, FourAndPair, FourAndTwoPairs, //单张，对子，三不带, 三带一，三带二，炸弹，四带二
        Continous, ContinousPair, SuperBomb, Aero, AeroAndOne, AeroAndPair, Illegal //顺子，连对，王炸，飞机（不带翼），飞机（带小翼），飞机（带大翼）
    };
    friend class Card;
    friend class Player;
    friend class MainWindow;
    friend class InitDialog;
    friend class Controller;
    friend class GlobalController;
};

#endif // ASSIST_H
