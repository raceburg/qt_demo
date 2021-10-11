#include "wascenegrid.h"
#include <math.h>
#include <algorithm>

wasceneGrid::wasceneGrid(QGraphicsScene* parent)
{
    gridList.push_back({200,2,Qt::white,0.5});
    gridList.push_back({100,2,Qt::white,1.0});
    gridList.push_back({50,1,Qt::white,5});
    gridList.push_back({10,1,Qt::gray,100});
    opacity = 0.7;
    scene = parent;
    for (int k = 0; k < gridList.size(); k++)
    {
        standard.push_back(nullptr);
        perspective.push_back(nullptr);
    }
}

bool wasceneGrid::deleteruler(QGraphicsPixmapItem* image, int numLine)
{
    if (numLine < 0 || numLine > 1) return useGrid;
    linePoint[numLine].update_count = 0;
    drawgrid(image);
    return useGrid;
}

bool wasceneGrid::updateruler(QGraphicsPixmapItem* image, QPointF beg, QPointF end, int mm, int numLine)
{
    if (numLine < 0 || numLine > 1) return useGrid;
    linePoint[numLine].update_count++;
    linePoint[numLine].beg = beg;
    linePoint[numLine].end = end;
    linePoint[numLine].mm = mm;
    drawgrid(image);
    return useGrid;
}

bool wasceneGrid::updaterulerlen(QGraphicsPixmapItem* image, int mm, int numLine)
{
    if (numLine < 0 || numLine > 1) return useGrid;
    linePoint[numLine].update_count++;
    linePoint[numLine].mm = mm;
    drawgrid(image);
    return useGrid;
}

void wasceneGrid::removeAndHide(QGraphicsPixmapItem* image)
{
    deleteruler(image, 0);
    deleteruler(image, 1);
    hide();
}

void wasceneGrid::drawgrid(QGraphicsPixmapItem* image)
{
    useGrid = false;
    perspectiveUse = false;

    foreach (auto item, standard_line)
        for (int k=0; k<item.size(); k++){
            if (item[k] != nullptr)
                scene->removeItem(item[k]);
        }
    standard_line.clear();

    if ((linePoint[0].init() || linePoint[1].init()) &&
            linePoint[0].init() != linePoint[1].init())
    {
        int p = 0;
        if (linePoint[1].init()) p = 1;
        QLineF rl(linePoint[p].beg,linePoint[p].end);
        QLineF limg(QPointF(image->boundingRect().top(),image->boundingRect().left()),
                    QPointF(image->boundingRect().bottom(),image->boundingRect().right()));
        if (rl.length() > 0.05*limg.length())
        {
            useGrid = true;
            QPointF point = linePoint[p].beg-linePoint[p].end;
            double pix = point.manhattanLength();
            for (int k = 0; k < gridList.size(); k++)
            {
                QList <QGraphicsLineItem *> QGraphicsLineItem_list;
                if (standard[k] == nullptr)
                {
                    standard[k] = new QGraphicsPixmapItem();
                    scene->addItem(standard[k]);
                    standard[k]->setZValue(15-k*0.1);
                }
                QSizeF imageSize = image->boundingRect().size();
                QPixmap pixmap(imageSize.toSize());
                pixmap.fill(Qt::transparent);
                QPainter painter(&pixmap);
                GridSrc* gs = &gridList[k];
                QPen pen(gs->color);
                pen.setWidth(gs->width);
                pen.setCosmetic(true);
                painter.setPen(pen);
                QPainterPath path;
                int mm = linePoint[p].mm;
                for (double x = imageSize.width()/2; x > 0; x -= pix/mm*gs->step)
                {
                    QPointF top1(x,image->boundingRect().top());
                    QPointF bot1(x,image->boundingRect().bottom());
                    QPointF top2(imageSize.width()-x,image->boundingRect().top());
                    QPointF bot2(imageSize.width()-x,image->boundingRect().bottom());
//                    painter.drawLine(top1,bot1);
//                    painter.drawLine(top2,bot2);
                    QLineF line1(top1,bot1);
                    QLineF line2(top2,bot2);
                    QGraphicsLineItem * lineItem1 = new QGraphicsLineItem(line1);
                    QGraphicsLineItem * lineItem2 = new QGraphicsLineItem(line2);
                    lineItem1->setPen(pen);
                    lineItem2->setPen(pen);
                    lineItem1->setZValue(20);
                    lineItem2->setZValue(20);
                    lineItem1->hide();
                    lineItem2->hide();
                    QGraphicsLineItem_list.push_back(lineItem1);
                    QGraphicsLineItem_list.push_back(lineItem2);
                    scene->addItem(lineItem1);
                    scene->addItem(lineItem2);
                }
                for (double y = imageSize.height()/2; y > 0; y -= pix/mm*gs->step)
                {
                    QPointF top1(image->boundingRect().left(),y);
                    QPointF bot1(image->boundingRect().right(),y);
                    QPointF top2(image->boundingRect().left(),imageSize.height()-y);
                    QPointF bot2(image->boundingRect().right(),imageSize.height()-y);
//                    painter.drawLine(top1,bot1);
//                    painter.drawLine(top2,bot2);
                    QLineF line1(top1,bot1);
                    QLineF line2(top2,bot2);
                    QGraphicsLineItem * lineItem1 = new QGraphicsLineItem(line1);
                    QGraphicsLineItem * lineItem2 = new QGraphicsLineItem(line2);
                    lineItem1->setPen(pen);
                    lineItem2->setPen(pen);
                    lineItem1->setZValue(20);
                    lineItem2->setZValue(20);
                    lineItem1->hide();
                    lineItem2->hide();
                    QGraphicsLineItem_list.push_back(lineItem1);
                    QGraphicsLineItem_list.push_back(lineItem2);
                    scene->addItem(lineItem1);
                    scene->addItem(lineItem2);
                }
                standard[k]->setPixmap(pixmap);
                standard[k]->hide();
                standard[k]->setOpacity(opacity);
                standard_line.push_back(QGraphicsLineItem_list);
             }
             statusMessage = "Построена сетка без учёта перспективы по " + QString::number(p+1) + "-й линейке";
        }
        else
            statusMessage = "Невозможно построить сетку. Короткая линейка.";
    }
    if (linePoint[0].init() && linePoint[1].init())
    {
//                               A
//                              /|
//                             / |
//           |----------------D--E--------------------|
//           |               /   |                    |
//           |              M++++N                    |
//           |             /     |                    |
//           |            /  x   |                    |
//           |           F+++++++P                    |
//           |          /        |                    |
//           |         /         |                    |
//           |--------B----------C--------------------|
//
//            BC = x + A*PC
//            DE = x - A*(NP+EN)
//
        QRectF up(std::min(linePoint[0].beg.x(), linePoint[0].end.x()),
                std::min(linePoint[0].beg.y(), linePoint[0].end.y()),
                std::abs(linePoint[0].beg.x()-linePoint[0].end.x()),
                std::abs(linePoint[0].beg.y()-linePoint[0].end.y()));
        QRectF down(std::min(linePoint[1].beg.x(), linePoint[1].end.x()),
                std::min(linePoint[1].beg.y(), linePoint[1].end.y()),
                std::abs(linePoint[1].beg.x()-linePoint[1].end.x()),
                std::abs(linePoint[1].beg.y()-linePoint[1].end.y()));
        if (down.center().y() < up.center().y())
        {
            QRectF t(down);
            down = up;
            up = t;
        }
        double MN = up.width();
        double FP = down.width();
        double NP = down.center().y()-up.center().y();
        double A  = (FP-MN)/(2*NP);
        double AP = FP/A;
        double EN = up.center().y();
        double PC = image->boundingRect().bottom()-NP-EN;
        double AC = AP+PC;
        double AE = AP-NP-EN;
//      длина линеек
        QLineF lr0(linePoint[0].beg,linePoint[0].end);
        QLineF lr1(linePoint[1].beg,linePoint[1].end);
        QLineF limg(QPointF(image->boundingRect().top(),image->boundingRect().left()),
                    QPointF(image->boundingRect().bottom(),image->boundingRect().right()));
        double Lr0 = lr0.length();
        double Lr1 = lr1.length();
        double Limg = limg.length();
        if (down.width()>up.width() && AE > 0 &&
//          проверка минимальной длины
            Lr0 > 0.05*Limg && Lr1 > 0.05*Limg)
        {
            perspectiveUse = true;
            perspectiveD = AE/AC;
            useGrid = true;

            foreach (auto item, perspective_line)
                for (int k=0; k<item.size(); k++){
                    if (item[k] != nullptr)
                        scene->removeItem(item[k]);
                }
            perspective_line.clear();

            for (int k = 0; k < gridList.size(); k++)
            {
                if (perspective[k] == nullptr)
                {
                    perspective[k] = new QGraphicsPixmapItem();
                    scene->addItem(perspective[k]);
                    perspective[k]->setZValue(15-k*0.1);
                }

                QList <QGraphicsLineItem *> QGraphicsLineItem_list;

                QSizeF imageSize = image->boundingRect().size();
                QPixmap pixmap(imageSize.toSize());
                pixmap.fill(Qt::transparent);
                QPainter painter(&pixmap);
                GridSrc* gs = &gridList[k];
                QPen pen(gs->color);
                pen.setWidth(gs->width);
                pen.setCosmetic(true);
                painter.setPen(pen);
                QPainterPath path;
                {
                    QPointF top1(imageSize.width()/2,image->boundingRect().top());
                    QPointF bot1(imageSize.width()/2,image->boundingRect().bottom());
                    QLineF line1(top1,bot1);
                    QGraphicsLineItem * lineItem1 = new QGraphicsLineItem(line1);
                    lineItem1->setPen(pen);
                    lineItem1->setZValue(20);
                    lineItem1->hide();
                    QGraphicsLineItem_list.push_back(lineItem1);
                    scene->addItem(lineItem1);

                }
                int mm = linePoint[1].mm;
                for (double x = FP/mm*gs->step; imageSize.width()/2-x > -1*imageSize.width(); x += FP/mm*gs->step)
                {
                    double A = AP/x;
                    double xtop = AE/A;
                    double xbot = AC/A;
                    QPointF top1(imageSize.width()/2-xtop,image->boundingRect().top());
                    QPointF bot1(imageSize.width()/2-xbot,image->boundingRect().bottom());
                    QPointF top2(imageSize.width()/2+xtop,image->boundingRect().top());
                    QPointF bot2(imageSize.width()/2+xbot,image->boundingRect().bottom());
//                    painter.drawLine(top1,bot1);
//                    painter.drawLine(top2,bot2);
                    QLineF line1(top1,bot1);
                    QLineF line2(top2,bot2);
                    QGraphicsLineItem * lineItem1 = new QGraphicsLineItem(line1);
                    QGraphicsLineItem * lineItem2 = new QGraphicsLineItem(line2);
                    lineItem1->setPen(pen);
                    lineItem2->setPen(pen);
                    lineItem1->setZValue(20);
                    lineItem2->setZValue(20);
                    lineItem1->hide();
                    lineItem2->hide();
                    QGraphicsLineItem_list.push_back(lineItem1);
                    QGraphicsLineItem_list.push_back(lineItem2);
                    scene->addItem(lineItem1);
                    scene->addItem(lineItem2);
                }
                for (double y = imageSize.height()/2; y > 0; y -= FP/mm*gs->step)
                {
                    QPointF top1(image->boundingRect().left(),y);
                    QPointF bot1(image->boundingRect().right(),y);
                    QPointF top2(image->boundingRect().left(),imageSize.height()-y);
                    QPointF bot2(image->boundingRect().right(),imageSize.height()-y);
//                    painter.drawLine(top1,bot1);
//                    painter.drawLine(top2,bot2);
                    QLineF line1(top1,bot1);
                    QLineF line2(top2,bot2);
                    QGraphicsLineItem * lineItem1 = new QGraphicsLineItem(line1);
                    QGraphicsLineItem * lineItem2 = new QGraphicsLineItem(line2);
                    lineItem1->setPen(pen);
                    lineItem2->setPen(pen);
                    lineItem1->setZValue(20);
                    lineItem2->setZValue(20);
                    lineItem1->hide();
                    lineItem2->hide();
                    QGraphicsLineItem_list.push_back(lineItem1);
                    QGraphicsLineItem_list.push_back(lineItem2);
                    scene->addItem(lineItem1);
                    scene->addItem(lineItem2);
                }
                perspective[k]->setPixmap(pixmap);
                perspective[k]->hide();
                perspective[k]->setOpacity(opacity);
                perspective_line.push_back(QGraphicsLineItem_list);
             }
            statusMessage = "Построена сетка с учетом перспективы";
        }
        statusMessage = "Невозможно построить сетку с перспективой. Некорректное расположение линеек.";
    }
}

void wasceneGrid::hide()
{
    QList<QList<QGraphicsLineItem*>> *gilist = &standard_line;
    foreach (auto item, *gilist)
        for (int k=0; k<item.size(); k++)
            if (item[k] != nullptr)
                item[k]->hide();
    gilist = &perspective_line;
    foreach (auto item, *gilist)
        for (int k=0; k<item.size(); k++)
            if (item[k] != nullptr)
                item[k]->hide();
}

double wasceneGrid::show(double mash)
{
    if (linePoint[0].need_update() || linePoint[1].need_update()) hide();
    if (useGrid)
    {
        QList<QList<QGraphicsLineItem*>> *gilist = &standard_line;
        if (perspectiveUse)
            gilist = &perspective_line;
        foreach (auto item, *gilist)
            for (int k=0; k<item.size(); k++)
                item[k]->hide();
        for (int k = 0; k < gilist->size(); k++)
            if (mash < gridList[k].zoom)
            {
                for (int m=0; m<(*gilist)[k].size(); m++)
                    (*gilist)[k][m]->show();
                return gridList[k].step;
            }
        return -1;
    }
    return -1;
}

QString wasceneGrid::getMessage()
{
    return statusMessage;
}

double wasceneGrid::getPerspectiveD()
{
    if (perspectiveUse)
        return perspectiveD;
    return -1;
}


QLineF wasceneGrid::getLine(int n)
{
    QLineF l (linePoint[n].beg,linePoint[n].end);
    return l;
}

bool wasceneGrid::lineInit(int n)
{
    return linePoint[n].init();
}
