#include "polygonitem.h"

// цвет области в зависимости от типа разметки
QColor areaPaintColor(EditAreaType type)
{
    if (type == freeArea) return Qt::blue;
    if (type == stoneArea) return Qt::red;
    if (type == sandArea) return Qt::yellow;
    return Qt::white;
}
