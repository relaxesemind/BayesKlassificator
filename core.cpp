#include "core.h"
#include "Managers/DataBaseManager/datamanager.h"
#include <QStack>
#include <QPoint>
#include <QDebug>
#include <QFloat16>
#include <QColor>
#include <cmath>
#include "Managers/MathManager/mathmanager.h"
#include "grapherservice.h"

const QRgb black = qRgb(0,0,0);
const qint32 BIN_BLACK = 0x0;
const quint32 BIN_WHITE = 0xFFFFFF;
//const quint32 ARGB_A = 0xFF000000;
//const qreal zoomMultiple = 1.05;
//const QRgb default_color = qRgb(0,145,218);

inline bool inner(qint32 x, qint32 y, Cell::Labels& V)
{//return true if d is inner point
 //dangerous function. May cause out of range!
    return (V[y][x + 1])&&(V[y][x - 1])&&(V[y + 1][x])&&(V[y - 1][x]);
}

inline bool isBlack(qint32 x, qint32 y, const QImage& im)
{
    return im.pixel(x,y) == black;
}

uint toGrayScale(const QImage& image,qint32 x ,qint32 y)
{//I = 0.2125R + 0.7154G + 0.0721B
 //return GrayScale pixel(x,y) from image with half precision
 //mostly fast;
    QRgb q = image.pixel(x,y);
    qfloat16 half_precision = qRed(q) * 0.212F + qGreen(q) * 0.715F + qBlue(q) * 0.0721F;
    return static_cast<uint> (half_precision);
}

QImage Bradley_Rot(const QImage& src)
{//main idea from this : https://habrahabr.ru/post/278435
 //my own implementation

    const qint32 w = src.width();
    const qint32 h = src.height();
    QImage ret_img(w,h,src.format());
    const qint32 S = w / 8;
    const float t = 0.15F;
    qint32 s2 = S / 2;
    //get integral_image
    //S(x, y) = I(x, y) + S(x-1, y) + S(x, y-1) – S(x-1, y-1);
    Cell::Labels integral_image
            (h,Cell::Labels_row(w,0));
    for (qint32 x = 0; x < w; ++x) integral_image[0][x] = toGrayScale(src,x,0);
    for (qint32 y = 0; y < h; ++y) integral_image[y][0] = toGrayScale(src,0,y);

    for (qint32 y = 1; y < h; ++y)
    {
        for (qint32 x = 1; x < w; ++x)
        {
            integral_image[y][x] = toGrayScale(src,x,y) + integral_image[y][x-1]
                    + integral_image[y-1][x] - integral_image[y-1][x-1];
        }
    }

    for (qint32 y = 0; y < h; ++y)
    {
        for (qint32 x = 0; x < w; ++x)
        {
            qint32 x1 = x - s2;
            qint32 x2 = x + s2;
            qint32 y1 = y - s2;
            qint32 y2 = y + s2;

            if (x1 < 0) x1 = 0;
            if (x2 >= w) x2 = w - 1;
            if (y1 < 0) y1 = 0;
            if (y2 >= h) y2 = h-1;

            qint32 count = (x2-x1)*(y2-y1);
            //S(x, y) = S(A) + S(D) – S(B) – S(C)
            qint32 sum = integral_image[y2][x2] - integral_image[y1][x2]
                    - integral_image[y2][x1] + integral_image[y1][x1];
            if ((toGrayScale(src,x,y) * count)
                    < (sum * (1.0F - t)))
            {
                ret_img.setPixel(x,y,BIN_BLACK);
            }else
            {
                ret_img.setPixel(x,y,BIN_WHITE);
            }

        }
    }
    return ret_img;
}

void fill(const QImage& img, Cell::Labels& V, qint32 _x, qint32 _y, quint32 L)
{
  QPoint t;
  QStack<QPoint> depth;
  depth.push(QPoint(_x,_y));
  const qint32 w = img.width();
  const qint32 h = img.height();

  while (!depth.empty())
  {
    t = depth.pop();
    qint32 x = t.rx();
    qint32 y = t.ry();
    V[y][x] = L; // filling.

    if((x + 1 < w)&&(isBlack(x+1,y,img))&&(V[y][x + 1] == 0))
    {
        depth.push(QPoint(x+1,y));
    }
    if((x - 1> -1)&&(isBlack(x-1,y,img))&&(V[y][x - 1] == 0))
    {
        depth.push(QPoint(x-1,y));
    }
    if((y + 1< h)&&(isBlack(x,y+1,img))&&(V[y + 1][x] == 0))
    {
        depth.push(QPoint(x,y+1));
    }
    if((y - 1> -1)&&(isBlack(x,y-1,img))&&(V[y - 1][x] == 0))
    {
        depth.push(QPoint(x,y-1));
    }
  }
}

void Core::getCellsFromDBprivate(QSqlTableModel *model, QVector<Cell> &vector, CellType type)
{
//                               id INTEGER PRIMARY KEY AUTOINCREMENT, "
//                                 TABLE_INERTIA " REAL NULL,"
//                                 TABLE_ENTROPY " REAL NULL,"
//                                 TABLE_HCOMP   " INTEGER NULL,"
//                                 TABLE_CELLTYPE " INTEGER NULL,"
//                                 TABLE_PATH " VARCHAR(255) NOT NULL "
   vector.clear();

   for (int row = 0; row < model->rowCount(); ++row) {
       Cell anyCell;
       anyCell.px = QPixmap();
       for (int column = 0; column < 6; ++column) {
           QModelIndex index = model->index(row,column,QModelIndex());
           QVariant var = model->data(index,Qt::DisplayRole);

           switch (column) {
           case 0:
                anyCell.id = var.toInt();
               break;
           case 1:
                anyCell.momentOfInertia = var.toDouble();
               break;
           case 2:
                anyCell.entropy = var.toDouble();
               break;
           case 3:
                anyCell.Hcomponent = var.toInt();
               break;
           case 4:
               anyCell.type = var.toInt();
               break;
           case 5:

               anyCell.px.load(var.toString());
               break;
           }
       }

       if (anyCell.type == type) {
           vector.push_back(anyCell);
       }

   }
   //  qDebug () << "size = " << cells.size();
}

void Core::getCellsFromDB(CellType type, QSqlTableModel *model)
{
    switch (type) {
    case CellType::Normal:
         getCellsFromDBprivate(model,cellNormal,type);
        break;
    case CellType::Limfoma:
        getCellsFromDBprivate(model,cellLim,type);
        break;
    case CellType::B_type:
        getCellsFromDBprivate(model,cellB,type);
        break;
    case CellType::T_type:
        getCellsFromDBprivate(model,cellT,type);
        break;
    }
}

void Core::getCellsFromDB(QSqlTableModel *model) {
    getCellsFromDBprivate(model,cellNormal,CellType::Normal);
    getCellsFromDBprivate(model,cellLim,CellType::Limfoma);
    getCellsFromDBprivate(model,cellB,CellType::B_type);
    getCellsFromDBprivate(model,cellT,CellType::T_type);


}

void Core::calculateParams()
{
    repeatCalc(cellNormal);
    emit progress(12);
    repeatCalc(cellT);
    emit progress(24);
    repeatCalc(cellB);
    emit progress(45);
    repeatCalc(cellLim);
    emit progress(60);

    MathManager::shared().calculatePoints(cellNormal,CellType::Normal);
    emit progress(75);
    MathManager::shared().calculatePoints(cellT,CellType::T_type);
    emit progress(84);
    MathManager::shared().calculatePoints(cellB,CellType::B_type);
    emit progress(91);
    MathManager::shared().calculatePoints(cellLim,CellType::Limfoma);
    emit progress(100);
}

void Core::drawGraphs(Attribute attr)
{
    MathManager::Points pointsN = MathManager::shared().getPoints(CellType::Normal,attr);
    MathManager::Points pointsL = MathManager::shared().getPoints(CellType::Limfoma,attr);
    MathManager::Points pointsB = MathManager::shared().getPoints(CellType::B_type,attr);
    MathManager::Points pointsT = MathManager::shared().getPoints(CellType::T_type,attr);

    GrapherService::shared().addGraphByCellType(CellType::Normal,pointsN);
    GrapherService::shared().addGraphByCellType(CellType::Limfoma,pointsL);
    GrapherService::shared().addGraphByCellType(CellType::T_type,pointsT);
    GrapherService::shared().addGraphByCellType(CellType::B_type,pointsB);
}

Cell Core::calculateParamsForOneCell(const QImage &image)
{
    Cell cell;
    cell.px = QPixmap::fromImage(image);
    cell.adjacencyMatrix = getAdjacencyMatrix(image);
    cell.entropy = getEntropy(cell.adjacencyMatrix);
    cell.momentOfInertia = getMomentInertia(cell.adjacencyMatrix);
    cell.Hcomponent = getHcomponent(image);

    return cell;
}

void Core::formObjects(Cell::Labels &Labels, Cell &cell, quint32 L)
{
    if (cell.binPX.isNull()) {
        return;
    }
    const quint32 size = --L; // size == num of objects
    Cell::Objects V(size);
    const int _h = cell.binPX.height();
    const int _w = cell.binPX.width();

    for(quint64 i = 0;i < size; ++i)
     {
       V[i] = S_area(i);
     }
//-----------------------------------------------------------------------------

    if ((size > 0)&&(_h > 2)&&(_w > 2))
    {
    for(int y = 1; y < _h - 1; ++y)//general case
      for(int x = 1; x < _w - 1; ++x)//general case
      {
          auto id = Labels[y][x];
          if ((id > 0)&&(id < size + 1))
          {
            QPoint t(x,y);
            V[id - 1].add_main(t);

            if (!inner(x,y,Labels))
            {
                V[id - 1].add_cont(t);
            }
          }
      }
    for(int x = 1; x < _w - 1; ++x)//top case
     {
        auto id = Labels[0][x];
        if ((id > 0)&&(id < size + 1))
        {
           QPoint t(x,0);
           V[id - 1].add_main(t);
           V[id - 1].add_cont(t);
        }
     }
    for(int x = 1; x < _w - 1; ++x)//bottom case
     {
        auto id = Labels[_h - 1][x];
        if ((id > 0)&&(id < size + 1))
                {
                   QPoint t(x,_h - 1);
                   V[id - 1].add_main(t);
                   V[id - 1].add_cont(t);
                }
     }
    for(int y = 0; y < _h; ++y)//left case
     {
         auto id = Labels[y][0];
         if ((id > 0)&&(id < size + 1))
         {
            QPoint t(0,y);
            V[id - 1].add_main(t);
            V[id - 1].add_cont(t);
         }
     }
    for(int y = 0; y < _h; ++y)//right case
     {
         auto id = Labels[y][_w - 1];
         if ((id > 0)&&(id < size + 1))
         {
            QPoint t(_w - 1,y);
            V[id - 1].add_main(t);
            V[id - 1].add_cont(t);
         }
     }
   }

    cell.objects = std::move(V);
}

void Core::repeatCalc(QVector<Cell> &vector)
{
    for (Cell &cell : vector) {

        if (cell.px.isNull()) {
            continue;
        }
//        QImage binImage = Bradley_Rot(cell.px.toImage());
//        cell.binPX = binImage;
//        Cell::Labels Labels (binImage.height(), Cell::Labels_row(binImage.width(),0));
//        quint32 L = 1; // starting id valu

//        for(qint32 y = 0; y < binImage.height(); ++y)
//          for(qint32 x = 0; x < binImage.width(); ++x)
//          {
//           if((!Labels[y][x])&&(isBlack(x,y,binImage)))
//           {
//             fill(binImage,Labels,x,y,L++);//very fast!
//           }
//          }
//        formObjects(Labels,cell,L);
        QImage temp = cell.px.toImage();

        cell.adjacencyMatrix = getAdjacencyMatrix(temp);
        cell.entropy = getEntropy(cell.adjacencyMatrix);
        cell.momentOfInertia = getMomentInertia(cell.adjacencyMatrix);
        cell.Hcomponent = getHcomponent(temp);
    }
}

void Core::calcParamsForOneVector(QVector<Cell> &vector)
{
//    for (Cell &cell : vector) {
//        if (cell.objects.isEmpty()) {
//            continue;
//        }

//    }
}

qint32 brignessInPos(qint32 xPos, qint32 yPos, const QImage& image)
{
    QRgb pixel = image.pixel(xPos,yPos);
    return (qRed(pixel) + qBlue(pixel) + qGreen(pixel)) / 3;
}

//#include <iostream>
Cell::AdjacencyMatrix Core::getAdjacencyMatrix(const QImage &image)
{///https://cyberleninka.ru/article/v/kontseptualnaya-model-raspoznavaniya-blastnyh-kletok-v-sisteme-kompyuternoy-mikroskopii
    Cell::Labels G(256,Cell::Labels_row(256,0));
    Cell::AdjacencyMatrix g(256,QVector<double>(256,0.0));
    const int h = image.height();
    const int w = image.width();
    for(qint32 y = 0; y < h; ++y) {
      for(qint32 x = 0; x < w - 1; ++x) {
          int i = brignessInPos(x, y,image);
          int j = brignessInPos(x + 1, y,image);
          ++G[i][j];

      //    std::cout<<G[i][j]<<" ";
        }
    //  std::cout<<std::endl;
    }

    for(qint32 i = 0; i < 256; ++i)
      for(qint32 j = 0; j < 256; ++j) {
          g[i][j] = static_cast<double> (G[i][j]) / (h*w);
      }

    return g;
}

double Core::getMomentInertia( Cell::AdjacencyMatrix &g)
{
    double res = 0;
    for(qint32 i = 0; i < 256; ++i)
      for(qint32 j = 0; j < 256; ++j) {
          res += (i - j)*(i - j)*g[i][j];
      }
    return res;
}

double Core::getEntropy( Cell::AdjacencyMatrix &g)
{
    double res = 0;
    for(qint32 i = 0; i < 256; ++i)
      for(qint32 j = 0; j < 256; ++j) {
          if (g[i][j] > 0) {
              res += g[i][j] * std::log(g[i][j]);
            //  qDebug() <<" res = " << res;
          }
      }

    return -res;
}

int Core::getHcomponent(const QImage &image)
{
    int64_t result = 0;
    for(qint32 y = 0; y < image.height(); ++y)
      for(qint32 x = 0; x < image.width(); ++x) {
          QColor color = image.pixelColor(x,y);
          int H = color.hsvHue();
          result += H;
      }
    result /= image.height() * image.width();
    return static_cast<int> (result);
}












