#include "grapherservice.h"
#include <QColor>
#include <QtCharts/QLegend>
#include <QMessageBox>

GrapherService::GrapherService()
{

    view = new QChartView();
    chart = new QChart();

    view->setChart(chart);
    view->setRenderHint(QPainter::Antialiasing);

}

void GrapherService::addGraphByCellType(CellType type, MathManager::Points &points)
{
     QSplineSeries *series = new QSplineSeries();
     for (QPointF &p : points) {
         series->append(p);
     }
     QColor color;
     switch (type) {
     case CellType::Normal:
          color.setNamedColor("green");
          series->setName("Норма");
         break;
     case CellType::Limfoma :
         color.setNamedColor("orange");
         series->setName("Лимфома");
         break;
     case CellType::B_type:
        color.setNamedColor("lightsteelblue");
        series->setName("В тип");
         break;
     case CellType::T_type:
        color.setNamedColor("indigo");
        series->setName("Т тип");
         break;
     }

     series->setColor(color);
     chart->addSeries(series);
     chart->createDefaultAxes();
}

void GrapherService::addVerticalLine(MathManager::Points &points)
{
    QLineSeries *series = new QLineSeries();
    for (QPointF &p : points) {
        series->append(p);
    }
    QColor color;
    color.setNamedColor("red");
    series->setColor(color);
    series->setName("Неизвестная клетка");
    chart->addSeries(series);
    chart->createDefaultAxes();


}

void GrapherService::clearView()
{
    chart->removeAllSeries();
}


