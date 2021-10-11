#ifndef STATISTICTABLE_H
#define STATISTICTABLE_H
#include <QObject>
#include <QMap>
#include <QColor>
#include "contour_inf.h"


struct Fraction
{
    double min = 0;
    double Max = 0;
    QString str(){
        if (Max == std::numeric_limits<double>::max())
            return QString::number(min) + "+";
        else
            return QString::number(min) + "-" + QString::number(Max);
    }
    bool inRange(double size)
    {
        return min <= size && size < Max;
    }
};


struct StatisticRow
{
    Fraction fraction;
    // цвет фракции
    QColor color = QColor();
    // процент от общего количества
    double area = 0;
    double median = 0;
    // количество размеченных объектов
    int count = 0;
    // минимальный размер
    double min = 0;
    // максимальный размер
    double Max = 0;
    // список номеров контуров
    QList<int> contours;
    // размер фракции
};


struct statisticTable
{
    statisticTable()
    {
        hsvMin.setHsvF(0.496, 1, 1, 1);
        hsvMax.setHsvF(0.712, 1, 1, 1);
    }
    // ---------------------------------------------------------------------
    QList<StatisticRow> statRow;
    // ---------------------------------------------------------------------
    // статистические данные, общие данные
    QMap<QString,QString>   common;
    // ---------------------------------------------------------------------
    void getAreaStat(QList<countour_inf> area,
                     QStringList fractination);
private:
    QColor hsvMin = QColor();
    QColor hsvMax = QColor();
};

#endif // STATISTICTABLE_H
