#include "wascenemanuallayer.h"
#include <QStyleOptionGraphicsItem>
#include <QCursor>
#include "contours_processing.h"

inline double area (QPointF a, QPointF b, QPointF c) {
    return (b.x() - a.x()) * (c.y() - a.y()) - (b.y() - a.y()) * (c.x() - a.x());
}

inline bool intersect_1 (double a, double b, double c, double d) {
    if (a > b)  std::swap (a, b);
    if (c > d)  std::swap (c, d);
    return std::max(a,c) <= std::min(b,d);
}

// опеределение взаимопересекающегося многоугольника
bool intersect (QPointF a, QPointF b, QPointF c, QPointF d) {
    return intersect_1 (a.x(), b.x(), c.x(), d.x())
        && intersect_1 (a.y(), b.y(), c.y(), d.y())
        && area(a,b,c) * area(a,b,d) <= 0
        && area(c,d,a) * area(c,d,b) <= 0;
}


// класс управления объектами отображения ручной разметки - слой ручной разметки
// описание объектов в заголовчном файле
wasceneManualLayer::wasceneManualLayer(QGraphicsPixmapItem* manualLayerItem,
                                       QGraphicsPathItem* traceToolItem,
                                       QGraphicsPolygonItem* polygonItem)
{
    manuallyMarkedLayer = manualLayerItem;
    manuallyMarkedLayer->setOpacity(0.5);
    traceToolLayer = traceToolItem;
    polygonTools = polygonItem;
    traceToolLayer->hide();
    polygonTools->hide();
}

// возвращает указатель на объект ручной разметки
QGraphicsPixmapItem* wasceneManualLayer::getLayer()
{
    return manuallyMarkedLayer;
}

// добавление существующей разметки камней на слой ручной разметки
void wasceneManualLayer::drawStones(QSize imageSize,
                                    QList <PolygonItem>* stones)
{
    QPixmap pixmap(imageSize);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    QPen pen(areaPaintColor(areaType));
    pen.setWidth(1);
    painter.setPen(pen);
    if (stones->size() > 0)
    {
        foreach (PolygonItem pol_stone, *stones)
        {
            QGraphicsPolygonItem stone;
            stone.setPolygon(pol_stone);
            QStyleOptionGraphicsItem opt;
            stone.setBrush(areaPaintColor(pol_stone.getType())); // цвет области в зависимости от типа
            stone.paint(&painter, &opt);
        }
    }
    painter.end();
    manuallyMarkedLayer->setPixmap(pixmap);
}

// очистка слоя ручной разметки
void wasceneManualLayer::createManuallyMarkedLayer(QSize imageSize)
{
    QPixmap pixmap (imageSize);
    pixmap.fill(Qt::transparent);
    manuallyMarkedLayer->setPixmap(pixmap);
    manuallyMarkedLayer->setZValue(11.0);
    manuallyMarkedLayer->hide();
}

// установка типа разметки
void wasceneManualLayer::setAreaType(EditAreaType areaType)
{
    this->areaType = areaType;
}

// установка размера кисти
void wasceneManualLayer::setBrushSize(int brushSize)
{
    this->brushSize = brushSize;
}

// создание следа при перемещении кисти
void wasceneManualLayer::brushTool(QPointF beg,
                                   QPointF end)
{
    // слой с инструментами
    if (!toolUse)
    {
        QPen pen;
        pen.setColor(toolColor);
        pen.setJoinStyle(Qt::RoundJoin);
        pen.setCapStyle(Qt::RoundCap);
        pen.setWidthF(brushSize);
        traceToolLayer->setPath(QPainterPath());
        QPainterPath p = traceToolLayer->path();
        p.moveTo(beg);
        traceToolLayer->setPath(p);
        traceToolLayer->setPen(pen);
        traceToolLayer->setZValue(11.1);
        traceToolLayer->setOpacity(toolOpacity);
        traceToolLayer->show();
        toolUse = true;
    }
    // след на рабочем изображении (маске)
    QPainterPath p = traceToolLayer->path();
    if (beg == end)
        p.lineTo(end+QPointF(0.1, 0.1));
    else
        p.lineTo(end);
    traceToolLayer->setPath(p);
    traceToolLayer->show();
}

// окончание работы с инструментом кисть
// перемещение нарисованного изображения на слой ручной разметки manuallyMarkedLayer
void wasceneManualLayer::brushToolEnd()
{
    qDebug() << areaType;
    if (toolUse)
    {
        QPixmap pm = manuallyMarkedLayer->pixmap();
        QPainter painter(&pm);
        QPen pen;
        pen.setColor(areaPaintColor(areaType));
        pen.setJoinStyle(Qt::RoundJoin);
        pen.setCapStyle(Qt::RoundCap);
        pen.setWidthF(brushSize);
        QStyleOptionGraphicsItem opt;
        traceToolLayer->setPen(pen);
        traceToolLayer->paint(&painter, &opt);
        painter.end();
        manuallyMarkedLayer->setPixmap(pm);
        traceToolLayer->hide();
        toolUse = false;
        addChangedLogItem({pm,"Разметка кистью #"+QString::number(bruchCounter)});
        bruchCounter++;
        emit updateImage();
    }
}

// создание  объекта для разметки с помощью полигона
void wasceneManualLayer::newPolygon(QPointF point)
{
    QPolygonF ppoint;
    ppoint.push_back(point);
    polygonTools->setBrush(toolColor);
    polygonTools->setOpacity(toolOpacity);
    polygonTools->setZValue(14.0);
    polygonTools->setPolygon(ppoint);
    polygonTools->show();
}

// перемещение последней точки полигона в точку point
// в случае обнаружение самопересечения многоугольника,
// фигура меняет цвет
void wasceneManualLayer::movePolygonPoint(QPointF point)
{
    QPolygonF ppoint = polygonTools->polygon();
    ppoint.back() = point;
    polygonTools->setPolygon(ppoint);
    // проверка самопересечения многоугольника
    // проверка пересечения линий полигона.
    // если линии пересекаются то меняется цвет заливки
    bool b_intersect = false;
    if (ppoint.size() > 3)
    {
        QPointF a = ppoint.first();
        QPointF b = ppoint.back();
        QPointF c = ppoint[ppoint.size()-2];
        for (int k=1; k < ppoint.size()-2; k++)
            if (intersect(a,b,ppoint[k],ppoint[k+1]))
            {
                b_intersect = true;
                break;
            }
        if (!b_intersect)
            for (int k=0; k < ppoint.size()-3; k++)
            if (intersect(b,c,ppoint[k],ppoint[k+1]))
            {
                b_intersect = true;
                break;
            }
    }
    else
    {
        QPen pen;
        if (ppoint.size()<3)
        {
            pen.setColor(Qt::green);
            pen.setWidth(2);
            pen.setCosmetic(false);
        }
        else
        {
//                pen.setColor(Qt::transparent);
//                pen.setWidth(0);
            pen.setStyle(Qt::NoPen);
        }
        polygonTools->setPen(pen);
    }
    if (b_intersect)
    {
        polygonTools->setBrush(Qt::red);
        stateMessage = "полигон неправильной формы";
    }
    else
    {
        polygonTools->setBrush(Qt::green);
    }
}

// удаление последней точки из полигона
void wasceneManualLayer::removePointFromPolygon()
{
    if (polygonTools != nullptr)
    {
        // удаление точки из полигона
        QPolygonF ppoint = polygonTools->polygon();
        if (ppoint.size()>2)
        {
            ppoint.remove(ppoint.size()-2);
            polygonTools->setPolygon(ppoint);
        }
    }
}

// добавление точки к полигону
void wasceneManualLayer::addPointToPolygon(QPointF point)
{
    // добавление новой точки в полигону
    QPolygonF ppoint = polygonTools->polygon();
    ppoint.push_back(point);
    polygonTools->setPolygon(ppoint);
}

// замыкание полигона
void wasceneManualLayer::polygonClosure()
{
    QPixmap pm = manuallyMarkedLayer->pixmap();
    QPainter painter(&pm);
    QStyleOptionGraphicsItem opt;
    polygonTools->setBrush(areaPaintColor(areaType));
    polygonTools->paint(&painter, &opt);
    painter.end();
    manuallyMarkedLayer->setPixmap(pm);
    addChangedLogItem({pm,"Добавление полигона #"+QString::number(polygonCounter)});
    polygonCounter++;
    polygonTools->hide();
    emit updateImage();
}

// создание полигона размером с исходное изображение
void wasceneManualLayer::fullSizeImagePolygon(QGraphicsPixmapItem* image)
{
    QRectF rect = image->boundingRect();
    newPolygon(rect.topLeft());
    addPointToPolygon(rect.topRight());
    addPointToPolygon(rect.bottomRight());
    addPointToPolygon(rect.bottomLeft());
    polygonClosure();
}

// флаг использования инструмента полиго
bool wasceneManualLayer::polygonUse()
{
    return polygonTools->isVisible();
}

// назначение прозрачности слоя ручной разметки
void wasceneManualLayer::setOpacity(double opacity)
{
    if (manuallyMarkedLayer != nullptr)
        manuallyMarkedLayer->setOpacity(opacity);
}

// функция не используется
void wasceneManualLayer::remove(QGraphicsScene *scene)
{
    polygonCounter = 1;
    bruchCounter   = 1;
    amCounter      = 1;
    changeLogSheet.clear();
    if (manuallyMarkedLayer != nullptr)
    {
        scene->removeItem(manuallyMarkedLayer);
        delete manuallyMarkedLayer;
        manuallyMarkedLayer = nullptr;
    }
    if (polygonTools != nullptr)
    {
        scene->removeItem(polygonTools);
        delete polygonTools;
        polygonTools = nullptr;
    }
}

// назначение видимости слоя ручной разметки
void wasceneManualLayer::visible(bool in)
{
    manuallyMarkedLayer->setVisible(in);
}

// возвращает видимость слоя ручной разметки
bool wasceneManualLayer::isVisible()
{
    return manuallyMarkedLayer->isVisible();
}

// загрузка слоя ручной разметки из файла
bool wasceneManualLayer::fromFile(QGraphicsScene *scene, QString fileName)
{
    if (manuallyMarkedLayer == nullptr)
    {
        // создание слоя
        manuallyMarkedLayer = new QGraphicsPixmapItem();
        scene->addItem(manuallyMarkedLayer);
    }
    QPixmap pixmap (fileName);
    if (pixmap.isNull()) return false;
    manuallyMarkedLayer->setPixmap(pixmap);
    addChangedLogItem({pixmap,"Импорт из файла"});
    manuallyMarkedLayer->setZValue(11.0);
    manuallyMarkedLayer->show();
    return true;
}

// возвращение pixmap из журнала изменений
bool wasceneManualLayer::viewChangeLog(int num)
{
    if (manuallyMarkedLayer == nullptr)
        return false;
    if (num < 0 && changeLogSheet.size()>1)
        num = selectedLog - 1;
    if (num < 0 || num > changeLogSheet.size()-1)
        return false;
    selectedLog = num;
    undo = true;
    QPixmap pm = manuallyMarkedLayer->pixmap();
    QPixmap pixmap = changeLogSheet[num].pixmap;
    if (pixmap.isNull()) return false;
    manuallyMarkedLayer->setPixmap(pixmap);
    manuallyMarkedLayer->setZValue(11.0);
    manuallyMarkedLayer->show();
    emit updateImage();
    return true;
}

// добавление pixmap в журнал изменений
void wasceneManualLayer::addChangedLogItem(changeLogItem item)
{
    if (undo)
    {
        auto sz = changeLogSheet.size();
        for (auto k = selectedLog+1; k < sz; k++)
            changeLogSheet.removeLast();
    }
    undo = false;
    changeLogSheet.push_back(item);
    QStringList sl;
    for (auto k = 0; k < changeLogSheet.size(); k++)
        sl.push_back(changeLogSheet[k].discr);
    selectedLog = changeLogSheet.size() - 1;
    emit changedLogItemsAdded(sl);
}

// возращение последнего сообщения
QString wasceneManualLayer::getMessage()
{
    return stateMessage;
}
