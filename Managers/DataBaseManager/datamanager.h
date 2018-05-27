#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QSql>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QFile>
#include <QDate>
#include <QDebug>
#include <QString>
#include <QCoreApplication>

#define DATABASE_HOSTNAME   "MBCDataBase"
#define DATABASE_NAME       "MBCDATA.db"

#define TABLE                   "Cells"
#define TABLE_HCOMP             "hcopmonent"
#define TABLE_INERTIA           "momentOfInertia"
#define TABLE_ENTROPY           "entropy"
#define TABLE_CELLTYPE          "type"
#define TABLE_PATH              "path"

class DataManager : public QObject
{
    Q_OBJECT
private:
    DataManager(QObject *parent = nullptr);
    DataManager(const DataManager&);
    DataManager& operator=(DataManager&);

public:
    static DataManager& sharedManager() {
        static DataManager manager;
        return manager;
    }
    ~DataManager();

    void connectToDataBase();

signals:

public slots:
   // bool getAllData();
    bool insertIntoTable(const QVariantList &data);
    bool insertIntoTable(double inertia = 0, int Hcomp = 0, double entropy = 0, int type = 0,const QString &path = QString() );

private:
    QSqlDatabase db;

private:
    bool openDataBase();
    bool restoreDataBase();
    void closeDataBase();
    bool createTable();
};

#endif // DATAMANAGER_H
