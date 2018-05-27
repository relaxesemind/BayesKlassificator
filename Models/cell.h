#ifndef CELL_H
#define CELL_H

#include <QObject>

#include "Common/types.h"
#include <QPixmap>
#include <QImage>
#include <QVector>

struct S_area
{
    quint64 id;
    QVector<QPoint> Points;
    QVector<QPoint> CPoints;
    S_area() = default;

    explicit S_area(quint64 number): id(number), Points(0), CPoints(0){}

    S_area& operator=(const S_area& obj)
    {
        if (this == &obj)
            return *this;
        id = obj.id;
        Points = obj.Points;
        CPoints = obj.CPoints;
        return *this;
    }

    void add_main(QPoint p)
    {
        Points.push_back(p);
    }

    void add_cont(QPoint p)
    {
        CPoints.push_back(p);
    }
};

class Cell {
public:

     Cell() = default;

    using Labels = QVector<QVector<quint32>>;
    using Labels_row = QVector<quint32>;
    using Objects = QVector<S_area>;
    using AdjacencyMatrix = QVector<QVector<double>>;
    ///attributes

    int id;
    double momentOfInertia;
    int Hcomponent;
    double entropy;
    int type;
    QPixmap px;
    QImage binPX;
    AdjacencyMatrix adjacencyMatrix;
    Objects objects;
};

#endif // CELL_H
