#ifndef GRAPHERSERVICE_H
#define GRAPHERSERVICE_H

#include <QObject>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QLineSeries>

#include"Managers/MathManager/mathmanager.h"

using namespace QtCharts;

class GrapherService : public QObject
{
    Q_OBJECT
private:
    GrapherService();
    GrapherService(const GrapherService&);
    GrapherService operator=(GrapherService&);
public:
    static GrapherService& shared() {
        static GrapherService service;
        return service;
    }

    void addGraphByCellType(CellType type, MathManager::Points& points);
    void addVerticalLine(MathManager::Points& points);
    void clearView();

public:
    QChartView *view;
    QChart *chart;

private:

};

#endif // GRAPHERSERVICE_H
