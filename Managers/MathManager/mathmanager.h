#ifndef MATHMANAGER_H
#define MATHMANAGER_H

#include "Models/cell.h"

#include <QObject>
#include <QVector>
#include <QPointF>

///p(C| F1, F2...Fn) = (1/Z)* p(C) * П (p(Fi | C));
///
/// p(C| F1, F2...Fn) = (1/Z)* p(C) * П
/// Вероятность появления класса С клетки, зависящей от F1...Fn признаков
/// Где 1/Z - маштабный множитель, зависящий только от признаков (он равен p(П Fi); в нашем случае в среднем 0.5
/// p(С) сделат параметром из UI (обычно 1/4)

class MathManager : public QObject
{
    Q_OBJECT
private:
    MathManager() = default;
    MathManager(const MathManager&);
    MathManager operator=(MathManager &);
public:
    static MathManager& shared() {
        static MathManager manager;
        return manager;
    }
    using Points = QVector<QPointF>;
    void calculatePoints(QVector <Cell> &cells, CellType type);
    void setHorizontalStep(Attribute attr, qreal);
    Points getPoints(CellType type, Attribute attribute);
    Points getPointsForOneCell(const Cell &cell, Attribute attribute);

private:
    Points frequency[NUM_OF_TYPES][NUM_OF_ATTRIBUTES];
    double PforCell[NUM_OF_TYPES];
    qreal horizonalStep[NUM_OF_ATTRIBUTES];
    void calcPrivateInertia(QVector<Cell> &cells, Points &);
    void calcPrivateEntropy(QVector<Cell> &cells, Points &);
    void calcPrivateHComp(QVector<Cell> &cells, Points &);
};

#endif // MATHMANAGER_H
