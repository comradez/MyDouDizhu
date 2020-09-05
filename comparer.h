#ifndef COMPARER_H
#define COMPARER_H

#include <QVector>
#include <set>
#include <QList>
#include <QMessageBox>
#include "assist.h"
#include "card.h"

class Comparer {
public:
    typedef Assist::CardCombo CardCombo;
    Comparer(); //大于号
    bool operator()(const QList<Card>& a,
                    const QList<Card>& b,
                    const CardCombo type) {
        for (const auto& each : a) {
            bucketA[each.getCardSize()]++;
        }
        for (const auto& each : b) {
            bucketB[each.getCardSize()]++;
        }
        switch (type) {
            case (CardCombo::Single):
            case (CardCombo::Pair):
            case (CardCombo::Three):
            case (CardCombo::Bomb): {
                return a[0].getCardSize() > b[0].getCardSize();
                break;
            } //全都一样的牌形,直接比较第一张即可(炸弹可以比大小)
            case (CardCombo::ThreeAndOne):
            case (CardCombo::ThreeAndPair):
            //case (CardCombo::FourAndPair):
            case (CardCombo::FourAndTwo):
            case (CardCombo::FourAndTwoPairs): {
                int moreA = bucketA.indexOf(3) == -1 ?
                            bucketA.indexOf(4) : bucketA.indexOf(3);
                int moreB = bucketB.indexOf(3) == -1 ?
                            bucketB.indexOf(4) : bucketB.indexOf(3);
                return moreA > moreB;
            } // 3/31/32/411/42/422,比较多的那个即可(因为不可能重复)
            case (CardCombo::Aero):
            case (CardCombo::AeroAndOne):
            case (CardCombo::AeroAndMulti):
            case (CardCombo::AeroAndPair): {
                int moreA = -1, moreB = -1, cntA = 0, cntB = 0;
                while (bucketA.indexOf(3, moreA + 1) != -1) {
                    moreA = bucketA.indexOf(3, moreA + 1);
                    cntA += 1;
                }
                while (bucketB.indexOf(3, moreB + 1 != -1)) {
                    moreB = bucketB.indexOf(3, moreB + 1);
                    cntB += 1;
                }
                if (cntA == cntB) {
                    return moreA > moreB;
                } return false;
                break;
            } //飞机要额外判断是不是同样数目的3张,不同直接为false
            case (CardCombo::ContinousPair):
            case (CardCombo::Continous): {
                int val = bucketA.indexOf(1) == -1 ? 2 : 1;
                int maxA = -1, maxB = -1, cntA = 0, cntB = 0;
                while (bucketA.indexOf(1, maxA + 1) != -1) {
                    maxA = bucketA.indexOf(val, maxA + 1);
                    cntA += 1;
                }
                while (bucketB.indexOf(1, maxB + 1) != -1) {
                    maxB = bucketB.indexOf(val, maxB + 1);
                    cntB += 1;
                }
                if (cntA == cntB) {
                    return maxA > maxB;
                } return false;
            } //顺子和连对也要判断对应的长度
            default: //王炸最多就一个, 非法类型也没有比的必要
                return false; //do nothing
        }
    }
private:
    QVector<int> bucketA, bucketB;
};

#endif // COMPARER_H
