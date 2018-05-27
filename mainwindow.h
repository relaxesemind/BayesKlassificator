#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QModelIndex>
#include <QString>

#include "Managers/DataBaseManager/datamanager.h"
#include "Common/types.h"
#include "Models/cell.h"

//#include <QtCharts/QtCharts>
//QT_CHARTS_USE_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setupModel(const QString& name, const QStringList &headers);
    QSqlTableModel *model;
private:
    void configueUI();

private slots:

    void on_action_triggered();

    void on_action_2_triggered();

    void on_action_3_triggered();

    void on_action_4_triggered();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_action_5_triggered();

    void progressBar(int value);

private:
    Ui::MainWindow *ui;
    Cell cell;
};

#endif // MAINWINDOW_H
