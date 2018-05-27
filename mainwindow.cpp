#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QStringList>
#include <QDebug>

#include "core.h"
#include "grapherservice.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    DataManager::sharedManager().connectToDataBase();
    this->setupModel(TABLE,
                     QStringList() << tr("id")
                                   << tr("Имя")
                                   << tr("Путь")
                     );
    configueUI();
//    QMessageBox::information(nullptr,
//     QObject::tr("Классификация"),tr("Клетка нормального типа"));
    Core::shared().getCellsFromDB(model);
    ui->scrollArea->setWidget(GrapherService::shared().view);
    connect(&Core::shared(),&Core::progress,this,&MainWindow::progressBar);
}

void MainWindow::configueUI()
{
//        ui->tableViewNo
//        ui->tableView->setColumnHidden(0, true);    // Hide the column id Records
//        ui->tableView->setColumnHidden(2, true);    // Hide the column with image
//        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
//        ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
//        ui->tableView->resizeColumnsToContents();
//        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);  // editing is not allowed
//        ui->tableView->horizontalHeader()->setStretchLastSection(true);     // Stretch the last
//    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
//     TABLE_INERTIA " REAL NULL,"
//     TABLE_ENTROPY " REAL NULL,"
//     TABLE_HCOMP   " INTEGER NULL,"
//     TABLE_CELLTYPE " INTEGER NULL,"
//     TABLE_PATH " VARCHAR(255) NOT NULL "
    ui->tableViewNormal->setModel(model);
    ui->tableViewLim->setModel(model);
    ui->tableViewBcells->setModel(model);
    ui->tableViewTcells->setModel(model);

    for (int i = 0; i < 5; ++i) {
      ui->tableViewNormal->setColumnHidden(i,true);
      ui->tableViewLim->setColumnHidden(i,true);
      ui->tableViewBcells->setColumnHidden(i,true);
      ui->tableViewTcells->setColumnHidden(i,true);
    }
    ui->tableViewNormal->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewNormal->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewNormal->resizeColumnsToContents();
    ui->tableViewNormal->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewNormal->horizontalHeader()->setStretchLastSection(true);

    ui->tableViewLim->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewLim->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewLim->resizeColumnsToContents();
    ui->tableViewLim->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewLim->horizontalHeader()->setStretchLastSection(true);

    ui->tableViewBcells->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewBcells->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewBcells->resizeColumnsToContents();
    ui->tableViewBcells->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewBcells->horizontalHeader()->setStretchLastSection(true);

    ui->tableViewTcells->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewTcells->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewTcells->resizeColumnsToContents();
    ui->tableViewTcells->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewTcells->horizontalHeader()->setStretchLastSection(true);

    model->select();
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupModel(const QString &tableName, const QStringList &headers)
{
    model = new QSqlTableModel(this);
    model->setTable(tableName);

//    for(int i = 0, j = 0; i < model->columnCount(); i++, j++){
//            model->setHeaderData(i,Qt::Horizontal,headers[j]);
//    }
}


void MainWindow::on_action_triggered()//load normal cells to DB from files
{
    QStringList filenames = QFileDialog::getOpenFileNames(this,tr("BMP files"),
                                                          QDir::currentPath(),tr("Bitmap files (*.bmp);;All files (*.*)") );
    qDebug() <<  filenames.count();
    if (!filenames.empty()) {
        for (int i = 0; i < filenames.count(); ++i) {

          qDebug() << i;
          DataManager::sharedManager().insertIntoTable(0,0,0,0,filenames.at(i));
        }
    }
    model->select();
    Core::shared().getCellsFromDB(CellType::Normal, model);
}

void MainWindow::on_action_2_triggered()//load limfoma cells to DB from files
{
    QStringList filenames = QFileDialog::getOpenFileNames(this,tr("BMP files"),
                                                          QDir::currentPath(),tr("Bitmap files (*.bmp);;All files (*.*)") );
    if (!filenames.empty()) {
        for (int i = 0; i < filenames.count(); ++i) {
            DataManager::sharedManager().insertIntoTable(0,0,0,3,filenames.at(i));
        }
    }
    model->select();
    Core::shared().getCellsFromDB(CellType::Limfoma, model);
}

void MainWindow::on_action_3_triggered()//load B cells
{
    QStringList filenames = QFileDialog::getOpenFileNames(this,tr("BMP files"),
                                                          QDir::currentPath(),tr("Bitmap files (*.bmp);;All files (*.*)") );
    if (!filenames.empty()) {
        for (int i = 0; i < filenames.count(); i++) {
            DataManager::sharedManager().insertIntoTable(0,0,0,2,filenames.at(i));
        }
    }
    model->select();
    Core::shared().getCellsFromDB(CellType::B_type, model);
}

void MainWindow::on_action_4_triggered()//load T cells
{
    QStringList filenames = QFileDialog::getOpenFileNames(this,tr("BMP files"),
                                                          QDir::currentPath(),tr("Bitmap files (*.bmp);;All files (*.*)") );
    if (!filenames.empty()) {
        for (int i = 0; i < filenames.count(); i++) {
            DataManager::sharedManager().insertIntoTable(0,0,0,1,filenames.at(i));
        }
    }
    model->select();
    Core::shared().getCellsFromDB(CellType::T_type, model);
}

void MainWindow::on_pushButton_clicked()//learning
{
    Core::shared().calculateParams();
}

void MainWindow::on_pushButton_2_clicked()
{
    bool inertia = ui->radioButton->isChecked();
    bool H = ui->radioButton_2->isChecked();

    if (inertia) {
        Core::shared().drawGraphs(Attribute::INERTIA);
    } else if (H) {
        Core::shared().drawGraphs(Attribute::HCOMP);
    } else {
        Core::shared().drawGraphs(Attribute::ENTROPY);
    }
}


void MainWindow::on_pushButton_3_clicked()
{
    GrapherService::shared().clearView();
}

void MainWindow::on_action_5_triggered()//load unknown cell
{
    QString filename = QFileDialog::getOpenFileName(this,tr("BMP files"),
                                                    QDir::currentPath(),tr("Bitmap files (*.bmp);;All files (*.*)"));
    if (!filename.isEmpty()) {
        QPixmap cellpx;
        cellpx.load(filename);
        bool inertia = ui->radioButton->isChecked();
        bool H = ui->radioButton_2->isChecked();
        cell = Core::shared().calculateParamsForOneCell(cellpx.toImage());

        if (inertia) {
            MathManager::Points line = MathManager::shared().getPointsForOneCell(cell,Attribute::INERTIA);
            GrapherService::shared().addVerticalLine(line);
        } else if (H) {
            MathManager::Points line = MathManager::shared().getPointsForOneCell(cell,Attribute::HCOMP);
            GrapherService::shared().addVerticalLine(line);
        } else {
            MathManager::Points line = MathManager::shared().getPointsForOneCell(cell,Attribute::ENTROPY);
            GrapherService::shared().addVerticalLine(line);
        }

    }
}

void MainWindow::progressBar(int value)
{
   ui->progressBar->setValue(value);
}
