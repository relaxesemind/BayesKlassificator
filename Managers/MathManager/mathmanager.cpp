#include "mathmanager.h"

#include <algorithm>
#include <QDebug>

///p(C| F1, F2...Fn) = (1/Z)* p(C) * П (p(Fi | C));
/// Вероятность появления класса С клетки, зависящей от F1...Fn признаков
/// Где 1/Z - маштабный множитель, зависящий только от признаков (он равен p(П Fi); в нашем случае в среднем 0.5
/// p(С) сделат параметром из UI (обычно 1/4)
/// p(C|F1) = (1/Z)*p(C) * p(F1|C);

void MathManager::calculatePoints(QVector<Cell> &cells, CellType type)
{

     calcPrivateInertia(cells, frequency[type][Attribute::INERTIA]);
     calcPrivateEntropy(cells, frequency[type][Attribute::ENTROPY]);
     calcPrivateHComp(cells, frequency[type][Attribute::HCOMP]);

}

void MathManager::setHorizontalStep(Attribute attr, qreal value)
{
    horizonalStep[attr] = value;
}

MathManager::Points MathManager::getPoints(CellType type, Attribute attribute)
{
    return frequency[type][attribute];
}

MathManager::Points MathManager::getPointsForOneCell(const Cell &cell, Attribute attribute)
{
    Points line;

    switch (attribute) {
    case Attribute::INERTIA:
        for (double y = 0; y < 0.5; y += 0.02) {
            QPointF p = QPointF(cell.momentOfInertia,y);
            line.push_back(p);

            PforCell[CellType::Normal] = std::max_element(frequency[CellType::Normal][Attribute::INERTIA].begin(),
                    frequency[CellType::Normal][Attribute::INERTIA].end(),[&](QPointF a, QPointF b){
                    return (std::abs(a.x() - cell.momentOfInertia) < std::abs(b.x() - cell.momentOfInertia));
            })->y();
            qDebug() << PforCell[CellType::Normal];
            PforCell[CellType::Limfoma] = std::max_element(frequency[CellType::Limfoma][Attribute::INERTIA].begin(),
                    frequency[CellType::Limfoma][Attribute::INERTIA].end(),[&](QPointF a, QPointF b){
                    return (std::abs(a.x() - cell.momentOfInertia) < std::abs(b.x() - cell.momentOfInertia));
            })->y();
            qDebug() << PforCell[CellType::Limfoma];
            PforCell[CellType::B_type] = std::min_element(frequency[CellType::B_type][Attribute::INERTIA].begin(),
                    frequency[CellType::B_type][Attribute::INERTIA].end(),[&](QPointF a, QPointF b){
                    return (std::abs(a.x() - cell.momentOfInertia) < std::abs(b.x() - cell.momentOfInertia));
            })->y();
            PforCell[CellType::T_type] = std::min_element(frequency[CellType::T_type][Attribute::INERTIA].begin(),
                    frequency[CellType::T_type][Attribute::INERTIA].end(),[&](QPointF a, QPointF b){
                    return (std::abs(a.x() - cell.momentOfInertia) < std::abs(b.x() - cell.momentOfInertia));
            })->y();

        }

        break;
    case Attribute::ENTROPY:
        for (double y = 0; y < 1.0; y += 0.2) {
            line.push_back(QPointF(cell.entropy,y));
        }
        break;
    case Attribute::HCOMP:
        for (double y = 0; y < 1.0; y += 0.2) {
            line.push_back(QPointF(cell.Hcomponent,y));
        }
        break;
    }

   return line;
}



void MathManager::calcPrivateInertia(QVector<Cell> &cells, Points &freq)
{
    double max = std::max_element(cells.begin(),cells.end(),[](const Cell &a,const Cell &b) {
        return (a.momentOfInertia < b.momentOfInertia);})->momentOfInertia;

    double min = std::min_element(cells.begin(),cells.end(),[](const Cell &a,const Cell &b) {
        return (a.momentOfInertia < b.momentOfInertia);})->momentOfInertia;

    double summa = std::accumulate(cells.begin(),cells.end(),0.0,[](double a,const Cell &b) {
                        return a + b.momentOfInertia;});
    qreal step = (std::abs(max - min)/50);
    int n = 0;
    for (double x = min; x < max; x += step) {
        for (Cell &cell : cells) {
            if (std::abs(cell.momentOfInertia - x) < step) {
                ++n;
                //freq.push_back(QPointF((x + step)*0.5,cell.momentOfInertia / summa));
            }
        }

        freq.push_back(QPointF(x,static_cast<double>(n)/cells.count()));
        n = 0;
    }


}

void MathManager::calcPrivateEntropy(QVector<Cell> &cells, Points &freq)
{

    double max = std::max_element(cells.begin(),cells.end(),[](const Cell &a,const Cell &b) {
        return a.entropy < b.entropy;})->entropy;

    double min = std::min_element(cells.begin(),cells.end(),[](const Cell &a,const Cell &b) {
        return a.entropy < b.entropy;})->entropy;

    double summa = std::accumulate(cells.begin(),cells.end(),0.0,[](double a,const Cell &b) {
                        return a + b.entropy;});
    qreal step = (max - min)/50;
    int n = 0;
    for (double x = min; x < max; x += step) {
        for (Cell &cell : cells) {
            if (std::abs(cell.entropy - x) < step) {
                ++n;
                //freq.push_back(QPointF((x + step)*0.5,cell.momentOfInertia / summa));
            }
        }
        freq.push_back(QPointF(x,static_cast<double>(n)/cells.count()));
         n = 0;
    }
}

void MathManager::calcPrivateHComp(QVector<Cell> &cells, Points &freq)
{


    double max = std::max_element(cells.begin(),cells.end(),[](const Cell &a,const Cell &b) {
        return a.Hcomponent < b.Hcomponent;})->Hcomponent;

    double min = std::min_element(cells.begin(),cells.end(),[](const Cell &a,const Cell &b) {
        return a.Hcomponent < b.Hcomponent;})->Hcomponent;

    double summa = std::accumulate(cells.begin(),cells.end(),0.0,[](double a,const Cell &b) {
                        return a + b.Hcomponent;});
    qreal step = (max - min)/50; // horizonalStep[Attribute::HCOMP];
    int n = 0;
    for (double x = min; x < max; x += step) {
        for (Cell &cell : cells) {
            if (std::abs(cell.Hcomponent - x) < step) {
                ++n;
                //freq.push_back(QPointF((x + step)*0.5,cell.momentOfInertia / summa));
            }
        }
        freq.push_back(QPointF(x,static_cast<double>(n)/cells.count()));
         n = 0;
    }
}
