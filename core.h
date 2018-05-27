#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QVector>

#include "Models/cell.h"
#include "Common/types.h"
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv/cv.hpp>
#include <QSqlTableModel>

//using namespace cv;

class Core : public QObject
{
    Q_OBJECT
private:
    Core() = default;
    Core(const Core&);
    Core& operator=(Core&);
public:
    static Core& shared() {
        static Core instance;
        return instance;
    }


    void getCellsFromDB(CellType type,QSqlTableModel *model);
    void getCellsFromDB(QSqlTableModel *model);
    void calculateParams();
    void drawGraphs(Attribute attr);
    Cell calculateParamsForOneCell(const QImage &image);

private:

 void formObjects(Cell::Labels &Labels, Cell &cell, quint32 L);
 void getCellsFromDBprivate(QSqlTableModel *model, QVector<Cell> &vector, CellType type);
 void repeatCalc(QVector<Cell> &);
 void calcParamsForOneVector(QVector<Cell> &);
 Cell::AdjacencyMatrix getAdjacencyMatrix(const QImage &image);
 double getMomentInertia( Cell::AdjacencyMatrix&);
 double getEntropy( Cell::AdjacencyMatrix&);
 int getHcomponent(const QImage &);
signals:
    void progress(int value);
public slots:

public:

private:
   QVector<Cell> cellNormal;
   QVector<Cell> cellLim;
   QVector<Cell> cellB;
   QVector<Cell> cellT;
};

#endif // CORE_H
