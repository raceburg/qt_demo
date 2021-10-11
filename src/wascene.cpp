#include "wascene.h"
#include <QPoint>
#include <QStyleOptionGraphicsItem>
#include <math.h>


wascene::wascene(QObject *parent) :
    QGraphicsScene(parent),
    m_currentAction(DefaultType),
    m_previousAction(0),
    m_leftMouseButtonPressed(false)
{
    ruler = new wasceneRuler(this);
    grid  = new wasceneGrid(this);
    manualLayerItem = new QGraphicsPixmapItem();
    traceToolItem = new QGraphicsPathItem();
    polygonItem = new QGraphicsPolygonItem();
    addItem(manualLayerItem);
    addItem(traceToolItem);
    addItem(polygonItem);
    mmLayer = new wasceneManualLayer(
                    manualLayerItem,
                    traceToolItem,
                    polygonItem
                );
}

wascene::~wascene()
{
    removeContourOfStone();
    removeItem(manualLayerItem);
    removeItem(traceToolItem);
    removeItem(polygonItem);
    delete manualLayerItem;
    delete traceToolItem;
    delete polygonItem;
    delete ruler;
    if (image != nullptr)
    {
        removeItem(image);
        delete image;
    }
}


int wascene::currentAction() const
{
    return m_currentAction;
}

void wascene::changeDragMode(bool dm)
{
    moveMode = dm;
}

// получение m_previousPosition
// используется для размещения линейки
QPointF wascene::previousPosition() const
{
    return m_previousPosition;
}

// режим работы сцены
void wascene::setCurrentAction(const int type)
{
    m_currentAction = type;
    switch (m_currentAction) {
    case DefaultType:
        emit currentActionChanged(setDefaultType); break;
    case RullerType:
        emit currentActionChanged(setRullerType); break;
    case SelectionType:
        emit currentActionChanged(setSelectionType); break;
    case ModifyObjectUsedBrush:
        emit currentActionChanged(setModifyObjectUsedBrush); break;
    case ModifyObjectUsedPolygon:
        emit currentActionChanged(setModifyObjectUsedPolygon); break;
    }
}

QPixmap wascene::QPixmapFromItem(QGraphicsItem *item){
    QPixmap pixmap(image->boundingRect().size().toSize());
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    QStyleOptionGraphicsItem opt;
    item->paint(&painter, &opt);
    return pixmap;
}

void wascene::setPreviousPosition(const QPointF previousPosition)
{
    if (m_previousPosition == previousPosition)
        return;
    m_previousPosition = previousPosition;
    emit previousPositionChanged();
}

QGraphicsPixmapItem* wascene::getManuallyMarkedLayer()
{
    return mmLayer->getLayer();
}

void wascene::keyPressEvent(QKeyEvent *event)
{
    switch (m_currentAction){
        case ModifyObjectUsedPolygon:
        {
            if (event->key() == Qt::Key_Delete && !moveMode)
            {
                // удаление точек из полигона
                mmLayer->removePointFromPolygon();
            }
            break;
        }
    }
    QGraphicsScene::keyPressEvent(event);
}

void wascene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() & Qt::LeftButton)
    {
        m_leftMouseButtonPressed = true;
        setPreviousPosition(event->scenePos());
    }

    if (event->button() & Qt::RightButton)
    {
        m_rightMouseButtonPressed = true;
    }

    switch (m_currentAction)
    {
        // режим добавления линеек
        case RullerType:
        {

            if (m_leftMouseButtonPressed && !(event->button() & Qt::RightButton) && !(event->button() & Qt::MiddleButton))
            {
                ruler->add();
            }
            break;
        }
        // ручная разметка изображения с помощью кисти
        case ModifyObjectUsedBrush:
        {
            if (m_leftMouseButtonPressed)
                mmLayer->brushTool(event->scenePos(), event->scenePos());
            break;
        }
        // ручная разметка изображения с помощью полигона
        // фиксирование точек полигона
        case ModifyObjectUsedPolygon:
        {
            if (m_leftMouseButtonPressed && !(event->button() & Qt::RightButton) && !(event->button() & Qt::MiddleButton) && !moveMode)
            {
                if (!mmLayer->polygonUse())
                {
                    mmLayer->newPolygon(event->scenePos());
                }
                // добавление новой точки в полигону
                mmLayer->addPointToPolygon(event->scenePos());
            }
            if (m_rightMouseButtonPressed && !(event->button() & Qt::LeftButton) && !(event->button() & Qt::MiddleButton) && !moveMode)
            {
                // замыкание полигона
                mmLayer->polygonClosure();
                emit sendMessage(Info,"Добавлен "+mmLayer->getMessage());
            }
            break;
        }
        default:
        {
            QGraphicsScene::mousePressEvent(event);
            break;
        }
    }
}


// обработка события перемещения мыши
void wascene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    switch (m_currentAction)
    {
        // следовать концом линейки за курсором
        case RullerType:
        {
            if (m_leftMouseButtonPressed)
            {
                ruler->updateLine(m_previousPosition,event->scenePos());
            }
            break;
        }
        // переместить указатель кисти за курсором
        case ModifyObjectUsedBrush:
        {
            // рисовать след от кисти
            if (m_leftMouseButtonPressed && !moveMode)
            {
                mmLayer->brushTool(m_previousPosition, event->scenePos());
                m_previousPosition = event->scenePos();
            }
            break;
        }
        // изменение разметки объектов с использвоанием полигона
        case ModifyObjectUsedPolygon:
        {
            if (!moveMode && mmLayer->polygonUse())
            {
                mmLayer->movePolygonPoint(event->scenePos());
            }
            break;
        }
        // режим выделения контура
        // подсветка контура над которым находиться мышь
        case SelectionType:
        {
            QGraphicsPolygonItem* polygonItem = nullptr;
            // пробегаем в цикле все элементы для текущего положения курсора
            foreach (QGraphicsItem* item_a , items(event->scenePos()))
            {
                // поиск совпадения с элементом из списка
                for (int k=0; k < stones.size(); k++)
                {
                    QGraphicsItem* item_b = stones[k];
                    if (item_a == item_b)
                    {
                        // совпадение найдено
                        polygonItem = static_cast<QGraphicsPolygonItem*>(item_a);
                        if (selectedPolygon != nullptr)
                        {
                            if (selectedPolygon == polygonItem)
                                break;
                            else
                            {
                                selectedPolygon->setBrush(selectedPolygonBrush);
                                selectedPolygon->setOpacity(opacity);
                            }

                        }
                        // назначение новых свойств для эемента
                        selectedPolygonBrush = polygonItem->brush();
                        selectedPlygonOpacity = polygonItem->opacity();
                        polygonItem->setBrush(Qt::red);
                        polygonItem->setOpacity(0.9);
                        selectedPolygon = polygonItem;
                        emit areaSelected(k);
                        break;
                    }
                }
                if (polygonItem != nullptr) break;
            }
            if (polygonItem == nullptr && selectedPolygon != nullptr)
            {
                // восстаовление свойств при перемещении курсора с выделенного объекта
                selectedPolygon->setBrush(selectedPolygonBrush);
                selectedPolygon->setOpacity(selectedPlygonOpacity);
                selectedPolygon = nullptr;
                emit areaSelected(-1);
            }
            QGraphicsScene::mouseMoveEvent(event);
            break;
        }
        default:
        {
            QGraphicsScene::mouseMoveEvent(event);
            break;
        }
    }
}

void wascene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() & Qt::LeftButton) m_leftMouseButtonPressed = false;
    if (event->button() & Qt::RightButton) m_rightMouseButtonPressed = false;

    switch (m_currentAction)
    {
        case RullerType:
        {
            // выставить линейку
            bool gridstatus = grid->updateruler(image, m_previousPosition,event->scenePos(), ruler->rulerLen(), ruler->curentRuler());
            emit sendRulerPoints(QLineF(m_previousPosition,event->scenePos()), ruler->curentRuler(), grid->getPerspectiveD());
            emit gridReady(gridstatus);
            emit sendMessage(Info,grid->getMessage());
            m_currentAction = DefaultType;
            break;
        }
        case ModifyObjectUsedBrush:
        {
            // закончить след от кисти
            mmLayer->brushToolEnd();
            break;
        }
        default:
        {
            QGraphicsScene::mouseReleaseEvent(event);
            break;
        }
    }
}

// покидание курсором рабочей области
void wascene::leaveWA()
{
    if (m_currentAction == SelectionType && selectedPolygon != nullptr)
    {
        selectedPolygon->setBrush(selectedPolygonBrush);
        selectedPolygon->setOpacity(opacity);
        selectedPolygon = nullptr;
        emit areaSelected(-1);
    }
}

void  wascene::deleteruler(int lineNum)
{
      ruler->remove(lineNum);
      setCurrentAction(DefaultType);
      bool gridstatus = grid->deleteruler(image, lineNum);
      emit gridReady(gridstatus);
      emit removeRuler(lineNum);
      if (gridstatus == false) gridShow = false;
      if (gridShow) grid->show(zoom);
      else  grid->hide();
}

// показать/скрыть линейку
void wascene::showRuller(bool visible)
{
    ruler->show(visible);
}

// добавление контуров размеченных объектов
// при перемещении курсора над объектом будет отображаться информация об объекте
void wascene::addContoursOfStone(QList<PolygonItem> in)
{
    foreach (PolygonItem p, in)
        addContourOfStone(&p);
    mmLayer->drawStones(image->boundingRect().size().toSize(), &in);
    emit manualModeEnable(true);
}

// добавление контура объекта
void wascene::addContourOfStone(PolygonItem* plgnpoint)
{
    // добавление нового полигона для камня
    QGraphicsPolygonItem* stone = new QGraphicsPolygonItem();
    stone->setPolygon(*plgnpoint);
    EditAreaType type = plgnpoint->getType();
    stone->setBrush(areaPaintColor(type));
    stone->setOpacity(m_objectOpacity);
    stone->setVisible(m_objectVisible);
    stone->setZValue(10.0+(stones.size()+1)*0.00001);
    stones.push_back(stone);
    addItem(stone);
}

// удаление контуров со сцены размеченных объектов
void wascene::removeContourOfStone()
{
    foreach (QGraphicsPolygonItem* stone, stones)
    {
        removeItem(stone);
        delete stone;
    }
    stones.clear();
}

// возвращает количество отображаемых размеченных объектов
bool wascene::contoursExist()
{
    return stones.size() > 0;
}

void wascene::createUnallocatedLayers()
{
    // подготовка неразмеченной области
    if (unallocated == nullptr)
    {
        // создание слоя
        unallocated = new QGraphicsPixmapItem();
        QPixmap pixmap(image->boundingRect().size().toSize());
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        QPen pen(Qt::transparent);
        QBrush bruch(Qt::red);
        pen.setWidth(0);
        painter.setPen(pen);
        painter.fillRect(0,0,image->boundingRect().size().toSize().width(),image->boundingRect().size().toSize().height(),bruch);
        painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        // изображение автоматически найденых контуров на слой
        if (stones.size()>0)
        {
            foreach (QGraphicsPolygonItem* stone, stones)
            {
                QStyleOptionGraphicsItem opt;
                stone->paint(&painter, &opt);
            }
        }
        unallocated->setPixmap(pixmap);
        unallocated->setZValue(13.0);
        unallocated->setOpacity(opacity);
        addItem(unallocated);
    }
}

// загрузка фонового изображения на котором расположенны камни
// очистка данных ручной разметки разметки изображения
bool wascene::addImage(QString fileName, LoadImageType type)
{
    if (type == LoadImageType::baseImage)
    {
        if (image == nullptr)
        {
            image = new QGraphicsPixmapItem();
            addItem(image);
        }
        image->setTransformationMode(Qt::SmoothTransformation);
        QPixmap pixmap (fileName);
        if (pixmap.isNull()) return false;
        image->setPixmap(pixmap);
        image->setZValue(10.0);
        setSceneRect(0, 0, pixmap.width(), pixmap.height());
        // обновление слоев отображения пользовательской разметки
        mmLayer->createManuallyMarkedLayer(image->boundingRect().size().toSize());
        emit currentActionChanged(SceneAction::imageLoad);
        return true;
    }
    if (type == LoadImageType::maskImage)
    {
        return mmLayer->fromFile(this,fileName);
    }
    return false;
}

// обрабока команд управления внешним видом
void wascene::setLayersProperty(
        // команда
        InstructionName property,
        // параметры команды
        QVariant items,
        // передача цвета, параметр используется для задания цвета размеченной области
        QColor colour)
{
    // текущая линейка и ее длина
    if (property ==InstructionName::ruler_setMetricLength
        && items.userType() == QMetaType::QVariantList)
    {
        setCurrentAction(RullerType);
        ruler->setCurentRuler(items.toList()[0].toInt(),items.toList()[1].toInt());
    }

    // используется для пересчета сетки при условии сохрания позиции
    if (property ==InstructionName::ruler_updatePixelLen
        && items.userType() == QMetaType::QVariantList)
    {
        if (grid->lineInit(items.toList()[0].toInt()))
        {
            ruler->setCurentRuler(items.toList()[0].toInt(),items.toList()[1].toInt());
            bool gridstatus = grid->updaterulerlen(image, ruler->rulerLen(), ruler->curentRuler());
            emit sendRulerPoints(grid->getLine(ruler->curentRuler()), ruler->curentRuler(), grid->getPerspectiveD());
            emit gridReady(gridstatus);
            emit sendMessage(Info,grid->getMessage());
        }
    }

    // удаление линейки по номеру
    if (property ==InstructionName::ruler_delete
        && items.type() == QVariant::Int)
    {
        deleteruler(items.toInt());
    }


    if  (property == InstructionName::hideAllObjects)
    {
        m_objectVisible = false;
        if (selectedarea != nullptr)
            selectedarea->hide();
        else
            foreach (QGraphicsPolygonItem* stone, stones)
                    stone->hide();
    }

    if  (property == InstructionName::visibleObjects
         && items.type() == QVariant::Bool)
    {
        m_objectVisible = items.toBool();
        foreach (QGraphicsPolygonItem* stone, stones)
                stone->setVisible(items.toBool());
    }

    if  (property == InstructionName::showAllObjects)
    {
        m_objectVisible = true;
        if (selectedarea != nullptr)
            selectedarea->show();
        else
            foreach (QGraphicsPolygonItem* stone, stones)
                    stone->show();
    }

    if  (property == InstructionName::showFraction
         && items.userType() == QMetaType::QVariantList)
    {
        int maxIndex = stones.size()-1;
        foreach (QVariant id, items.toList())
        {
            int Iid = id.toInt();
            if (id>maxIndex)
                emit sendMessage(Warn,"Элемент вне диапазона: showFraction["+QString::number(Iid)+"("+QString::number(maxIndex)+")]");
            else
               stones.at(Iid)->show();
        }
    }

    if  (property == InstructionName::setColorFraction
         && items.userType() == QMetaType::QVariantList)
    {
        int maxIndex = stones.size()-1;
        foreach (QVariant id, items.toList())
        {
            int Iid = id.toInt();
            if (id>maxIndex)
               emit sendMessage(Warn,"Элемент вне диапазона: setColorFraction["+QString::number(Iid)+"("+QString::number(maxIndex)+")]");
            else
            {
               stones.at(Iid)->setBrush(colour);
            }
        }
        emit currentActionChanged(updateColorFraction);
    }

    if  (property == InstructionName::setColorAllFraction)
    {
        int maxIndex = stones.size()-1;
        for (int Iid=0; Iid < maxIndex; Iid++)
            stones.at(Iid)->setBrush(colour);
    }

    if  (property == InstructionName::setOpacityObjects
         && items.type() == QVariant::Double)
    {
        opacity = items.toDouble();
        if (selectedarea != nullptr)
            selectedarea->setOpacity(opacity);
        foreach (QGraphicsPolygonItem* stone, stones)
                stone->setOpacity(opacity);
        if (unallocated != nullptr)
            unallocated->setOpacity(opacity);
        m_objectOpacity = opacity;
    }

    if (property == InstructionName::unallocatedAreaShow)
    {
        createUnallocatedLayers();
        unallocated->setOpacity(opacity);
        unallocated->show();
    }

    if (property == InstructionName::unallocatedAreaHide)
    {
        unallocated->hide();
    }

    if (property == InstructionName::manualLayer_SetVisible
            && items.type() == QVariant::Bool)
    {
        mmLayer->visible(items.toBool());
    }

    if (property == InstructionName::manualLayer_SetOpacity
        && items.type() == QVariant::Double)
    {
        mmLayer->setOpacity(items.toDouble());
    }

    if (property == InstructionName::manualLayer_SetEditedAreaType
        && items.type() == QVariant::Int)
    {
        mmLayer->setAreaType((EditAreaType)items.toInt());
    }

    if (property == InstructionName::manualLayer_SetBrushSize
        && items.userType() == QVariant::Int)
    {
        mmLayer->setBrushSize(items.toInt());
        emit currentActionChanged(updateBrushSize);
    }


    if (property == InstructionName::manualLayer_AddFullSizeImagePolygon
            &&
            (
                (m_currentAction == ModifyObjectUsedBrush &&
                 !m_leftMouseButtonPressed && !moveMode)
                ||
                (m_currentAction == ModifyObjectUsedPolygon &&
                 !m_leftMouseButtonPressed && !moveMode &&
                 !mmLayer->polygonUse()
                 )
            )
        )
        mmLayer->fullSizeImagePolygon(image);

    if (property == InstructionName::showGrid
            && items.type() == QVariant::Double)
    {
        double gridsize = grid->show(items.toDouble());
        emit sendGridSize(gridsize);
        gridShow = true;
        zoom = items.toDouble();
    }

    if (property == InstructionName::hideGrid)
    {
        grid->hide();
        gridShow = false;
    }

    if (property == InstructionName::clearWorkLayers
            && items.userType() == QVariant::Int)
    {
        if (items.toInt()>0)
        {
            m_currentAction = DefaultType;
            selectedPolygon = nullptr;
        }
        if (items.toInt()&RulerLayer)
        {
            grid->removeAndHide(image);
            ruler->removeAll();
        }

        if (items.toInt()&PolygonLayer)
        {
            removeContourOfStone();
            emit manualModeEnable(false);
            if (unallocated != nullptr)
            {
                removeItem(unallocated);
                delete unallocated;
                unallocated = nullptr;
            }
        }
        if (image != nullptr && items.toInt()&ImageLayer)
        {
            mmLayer->remove();
            removeItem(image);
            delete image;
            image = nullptr;
        }
    }
}


void wascene::setLayersProperty(InstructionName property,
                                QVariant items)
{
    setLayersProperty(property,items,Qt::transparent);
}

void wascene::setLayersProperty(InstructionName property)
{
    QVariant items;
    setLayersProperty(property,items,Qt::transparent);
}


QGraphicsPixmapItem* wascene::getImageItem()
{
    return image;
}

void wascene::loadRulers(RulerLines rl)
{
    foreach (int key, rl.keys())
    {
        ruler->setCurentRuler(key,rl[key].mm_len);
        ruler->add();
        ruler->updateLine(rl[key].line.p1(),rl[key].line.p2());
        bool gridstatus = grid->updateruler(image,
                                            rl[key].line.p1(),
                                            rl[key].line.p2(),
                                            rl[key].mm_len,
                                            key);
//        emit sendRulerPoints(rl[key].position, key, grid->getPerspectiveD());
        emit gridReady(gridstatus);
        emit sendMessage(Info,grid->getMessage());
    }
}
