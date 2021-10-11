#include "wasceneruler.h"

// создание элементов для отображения линейки
wasceneRuler::wasceneRuler(QGraphicsScene* parent)
{
    scene = parent;
    SceneRuler ruler;
    rulers.push_back(ruler);
    rulers.push_back(ruler);
    // стиль линеек
    QPen pen = rulers[0].pen;
    pen.setColor(Qt::green);
    pen.setWidth(20);
//    pen.setCosmetic(true);
    pen.setCapStyle(Qt::FlatCap);
    rulers[0].pen = pen;
    pen.setColor(Qt::blue);
    rulers[1].pen = pen;
}

// очистка
wasceneRuler::~wasceneRuler()
{
    removeAll();
}

// удаление всех линеек со сцены
void wasceneRuler::removeAll()
{
    remove(0);
    remove(1);
}

// добавление линейки, если ее нет
// предварительно нужно установить номер линейки lineNum
void wasceneRuler::add()
{
    if (rulers[currentRulerNum].line == nullptr)
    {
        rulers[currentRulerNum].line = new QGraphicsPathItem();
        scene->addItem(rulers[currentRulerNum].line);
    }
}

// перемещение линейки
// предварительно нужно установить номер линейки lineNum
void wasceneRuler::updateLine(QPointF begin, QPointF end)
{
    rulers[currentRulerNum].line->setPen(rulers[currentRulerNum].pen);
    QPainterPath path;
    path.moveTo(begin);
    path.lineTo(end);
    rulers[currentRulerNum].line->setPath(path);
    rulers[currentRulerNum].line->setOpacity(0.4);
    rulers[currentRulerNum].line->setZValue(20.0);
}

// удаление линейки с номером lineNum
void wasceneRuler::remove(int lineNum)
{
    if (rulers[lineNum].line != nullptr)
    {
        scene->removeItem(rulers[lineNum].line);
        delete rulers[lineNum].line;
        rulers[lineNum].line = nullptr;
    }
}

// удалить текущую линейку
void wasceneRuler::removeCurrentRuler()
{
    remove(curentRuler());
}

// скрыть показать линейки
void wasceneRuler::show(bool visible)
{
    int lineNum = 0;
    if (rulers[lineNum].line != nullptr)
        rulers[lineNum].line->setVisible(visible);
    lineNum = 1;
    if (rulers[lineNum].line != nullptr)
        rulers[lineNum].line->setVisible(visible);
}

// установить номер линейки для работы
// и задать ее длину
void wasceneRuler::setCurentRuler(int n, int rulerLen)
{
    currentRulerNum = n;
    this->curentRulerLen = rulerLen;
}

int wasceneRuler::curentRuler()
{
    return currentRulerNum;
}

int wasceneRuler::rulerLen()
{
    return curentRulerLen;
}
