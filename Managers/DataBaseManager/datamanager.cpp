#include "datamanager.h"

#include <QtSql/QSqlDatabase>
#include <QCoreApplication>
#include <QVariantList>

DataManager::DataManager(QObject *parent) : QObject(parent)
{

}

DataManager::~DataManager()
{

}

void DataManager::connectToDataBase()
{
    if(!QFile(QCoreApplication::applicationDirPath()+ "/" + DATABASE_NAME).exists()){
            this->restoreDataBase();
        } else {
            this->openDataBase();
    }
}

bool DataManager::insertIntoTable(const QVariantList &data)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO " TABLE " ( "
                  TABLE_INERTIA ", "
                  TABLE_ENTROPY ", "
                  TABLE_HCOMP ", "
                  TABLE_CELLTYPE ", "
                  TABLE_PATH " "
                  " ) "
                  "VALUES (:inertia, :entropy, :Hcomp, :type, :path) ");
    query.bindValue(":inertia",     data[0].toDouble());
    query.bindValue(":entropy",     data[1].toDouble());
    query.bindValue(":Hcomp",       data[2].toInt());
    query.bindValue(":type",        data[3].toInt());
    query.bindValue(":path",        data[4].toString());
   // qDebug() << query.value();
    if(!query.exec()){
        qDebug() << "error insert into " << TABLE;
        qDebug() << query.lastError().text();
        return false;
    } else {
        return true;
    }
}
bool DataManager::insertIntoTable(double inertia, int Hcomp, double entropy, int type,
                                  const QString &path) {
    QVariantList data;
    data.append(inertia);
    data.append(entropy);
    data.append(Hcomp);
    data.append(type);
    data.append(path);
    return insertIntoTable(data);
}

bool DataManager::openDataBase()
{
      db = QSqlDatabase::addDatabase("QSQLITE");
      db.setHostName(DATABASE_HOSTNAME);
      db.setDatabaseName(QCoreApplication::applicationDirPath()+ "/" DATABASE_NAME);
      if(!db.open()){
          return false;
          qFatal( "Failed to open database" );
      } else {
          return true;
      }
}

bool DataManager::restoreDataBase()
{
    if(this->openDataBase()){
            return (this->createTable()) ? true : false;
        } else {
            qDebug() << "Не удалось восстановить базу данных";
            return false;
        }
}

void DataManager::closeDataBase()
{
    db.close();
}

bool DataManager::createTable()
{
    QSqlQuery query(db);
    if(!query.exec( "CREATE TABLE " TABLE " ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                             TABLE_INERTIA " REAL NULL,"
                             TABLE_ENTROPY " REAL NULL,"
                             TABLE_HCOMP   " INTEGER NULL,"
                             TABLE_CELLTYPE " INTEGER NULL,"
                             TABLE_PATH " VARCHAR(255) NOT NULL "
                        " )"
                    )){
        qDebug() << "DataBase: error of create " << TABLE;
        qDebug() << query.lastError().text();
        return false;
    } else {
        return true;
    }
}
