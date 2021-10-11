#ifndef POLYGONITEM_H
#define POLYGONITEM_H
#include <QPolygonF>
#include <QGraphicsPolygonItem>
#include "wavariables.h"

// типы поверхностей
enum EditAreaType
{
    freeArea=1,
    stoneArea,
    sandArea
};

// класс описывающй выделенную поверхность
class PolygonItem : public QPolygonF
{
public:
    PolygonItem(): QPolygonF() {}
    PolygonItem(const QPolygonF &polygon): QPolygonF(polygon) {}
    // задать тип размеченной области
    void setType(EditAreaType type) const { areaType = type; }
    // вернуть тип размеченной области
    EditAreaType getType(void) const { return areaType; }

private:
    // тип размеченной области
    mutable EditAreaType areaType;
};


// цвет области в зависимости от типа
QColor areaPaintColor(EditAreaType type);


#endif // POLYGONITEM_H
