#ifndef WASCENEGRID_H
#define WASCENEGRID_H

#include <QObject>
#include <QGraphicsPolygonItem>
#include <QGraphicsScene>
#include <QPainter>

struct GridSrc {
    double step;
    int width;
    QColor color;
    double zoom;
};

struct slinePoint
{
    QPointF beg;
    QPointF end;
    int update_count = 0;
    int  mm;
    bool need_update()
    {
        if (update_count == last_update)
            return false;
        else {
            last_update = update_count;
            return true;
        }
    }
    bool init(){
        return update_count > 0;
    }

private:
    int last_update = -1;
};

class wasceneGrid : public QObject
{
    Q_OBJECT

public:
            wasceneGrid(QGraphicsScene* parent);
            // обновление информации о линейках, возвращается флаг построенной сетки
    bool    updateruler(QGraphicsPixmapItem* image, QPointF beg, QPointF end, int mm, int numLine);
    // обновление длины, возвращается флаг построенной сетки
    bool    updaterulerlen(QGraphicsPixmapItem* image, int mm, int numLine);
            // удаление информации о линейках, возвращается флаг построенной сетки
    bool    deleteruler(QGraphicsPixmapItem* image, int numLine);
    double  show(double mash);
    void    hide();
    void    removeAndHide(QGraphicsPixmapItem* image);
    QString getMessage();
    double  getPerspectiveD();
    QLineF  getLine(int);
    bool    lineInit(int);

private:
    void    drawgrid(QGraphicsPixmapItem* image);
    // сетка с перспективой
    QList <QGraphicsPixmapItem*>    perspective;
    QList <QList<QGraphicsLineItem*>>    perspective_line;
    // линейная сетка
    QList <QGraphicsPixmapItem*>    standard;
    QList <QList<QGraphicsLineItem*>>    standard_line;
    // список парамеров сетки (интервал, толщина)
    QList <GridSrc>                 gridList;
    // использовать перспективу, иначе стандартную сетку
    bool                            perspectiveUse = false;
    // сетка формированна
    bool                            useGrid = false;
    // номер сетки
    int                             num = 0;
    QGraphicsScene*                 scene;
    // линейки
    slinePoint                      linePoint[2];
    // прозрачность
    double                          opacity;
    // текстовые сообщения о состоянии
    QString                         statusMessage;
    //
    double                          perspectiveD = -1;
};

#endif // WASCENEGRID_H
