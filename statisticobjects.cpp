#include "statisticobjects.h"

void statisticTable::getAreaStat(QList<countour_inf> area,
                                 QStringList fractination)
{
    statRow.clear();
    // создание строк в соответсвии с набором фракций
    for (int k=0; k<fractination.size();k++)
    {
        QString str;
        QStringList item = fractination[k].split(QRegExp("(\\+|-)"));
        StatisticRow row;
        if (item[0]=="")
            row.fraction.min = 0;
        else
            row.fraction.min = item[0].toDouble();
        if (item[1]=="")
            row.fraction.Max = std::numeric_limits<double>::max();
        else
            row.fraction.Max = item[1].toDouble();
        statRow.push_back(row);
    }

    // создание цветовго обозначения фракций
    if (statRow.size() > 1)
    {
        double d = (hsvMax.hueF() - hsvMin.hueF()) / (statRow.size()-1);
        for (int k=0; k<statRow.size(); k++)
        {
            statRow[k].color.setHsvF(hsvMin.hueF() + k*d, 1, 1, 1);
        }
    }
    else
    {
        statRow[0].color = hsvMin;
    }

    // заполнение данными
    double summaryArea = 0;
    for (int n=0; n<area.size(); n++)
    {
        for (int k=0; k<statRow.size(); k++)
        {
            if (statRow[k].fraction.inRange(area[n].median))
            {
                statRow[k].contours.push_back(n);
                statRow[k].area += area[n].area;
                statRow[k].median += area[n].median;
                statRow[k].min += qMin(statRow[k].min, area[n].median);
                statRow[k].Max += qMax(statRow[k].Max, area[n].median);
                statRow[k].count++;
                break;
            }
        }
        summaryArea += area[n].area;
    }

    for (int k=0; k<statRow.size(); k++)
    {
        statRow[k].area = statRow[k].area/summaryArea;
    }
}
