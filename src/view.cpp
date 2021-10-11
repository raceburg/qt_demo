#include "view.h"
#include <QDebug>
#include <math.h>
#include <QScrollBar>


view::view(QWidget *parent) : QGraphicsView(parent)
{
}

void view::setZoomBtn(bool zoomBtn)
{
    zoomButtom = zoomBtn;
}

void view::enterEvent(QEvent *event)
{
    QGraphicsView::enterEvent(event);
//    viewport()->setCursor(Qt::CrossCursor);
}

void view::leaveEvent(QEvent *event)
{
    emit leaveWA();
    QGraphicsView::leaveEvent(event);
//    viewport()->setCursor(Qt::CrossCursor);
}

void view::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
//    viewport()->setCursor(Qt::CrossCursor);
}

void view::mousePressEvent(QMouseEvent *event)
{
    if (zoomButtom && (event->button() & Qt::LeftButton || event->button() & Qt::RightButton))
    {
        pos = event->pos();
        int l_zoom = 10;
        if (event->button() & Qt::RightButton) l_zoom = -10;
        SetZoomFactor(l_zoom, &pos);
        event->accept();
        emit changeZoom(_zoom);
    }
    else
    {
        QGraphicsView::mousePressEvent(event);
    }
//    viewport()->setCursor(Qt::CrossCursor);
}

void view::wheelEvent(QWheelEvent *event)
{
    bool deftlt = true;
//    if (QApplication::keyboardModifiers() & Qt::ShiftModifier)
//    {
//        horizontalScrollBar()->setValue(horizontalScrollBar()->value()+event->delta());
//        event->accept();
//        deftlt = false;
//    }
    if (QApplication::keyboardModifiers() & Qt::ShiftModifier)
    {
        pressSpaceAndwheelEvent(event->delta());
        event->accept();
        deftlt = false;
    }
    if (QApplication::keyboardModifiers() & Qt::ControlModifier)
    {
        pos = event->pos();
//        SetZoomFactor(_zoom*pow(1.2, event->delta() / 240.0), &pos);  // старое решение
        SetZoomFactor(event->delta(), &pos);
        event->accept();
        emit changeZoom(_zoom);
        updateCursor();
        deftlt = false;
    }
    if (deftlt)
    {
        QGraphicsView::wheelEvent(event);
    }
}

void view::fitInView(const QGraphicsItem *item,
               Qt::AspectRatioMode aspectRadioMode)
{
    QGraphicsView::fitInView(item, aspectRadioMode);
    _zoom = transform().m11();
    emit changeZoom(_zoom);
}


void view::calcZoom()
{
    emit changeZoom(transform().m11());
}

void view::SetZoomFactor(qreal delta, const QPoint * optMousePos)
{
    QPointF oldPos;
    if (optMousePos) oldPos = mapToScene(*optMousePos);
    qreal dxy = 1;
    qreal dzoom = 1.1;
    if ((delta>0)&&(_zoom <= 10.0)) dxy = dzoom;
    if ((delta<0)&&(_zoom >= 0.02)) dxy = 1/dzoom;
    scale(dxy, dxy);
    _zoom = transform().m11(); //transform().m22() - тоже
    if (optMousePos)
    {
        const QPointF newPos = mapFromScene(oldPos);
        const QPointF move   = newPos-*optMousePos;
        horizontalScrollBar()->setValue(static_cast<int>(move.x()) + horizontalScrollBar()->value());
        verticalScrollBar()->setValue(static_cast<int>(move.y()) + verticalScrollBar()->value());
    }
}

void view::resizeEvent(QResizeEvent *event)
{
    int h,w;
    w = event->size().width();
    h = event->size().height();
    translate(w1/w, h1/h);
    w1 = event->size().width();
    h1 = event->size().height();
}

void view::setDrag(bool drag)
{
    if (drag)
    {
        setDragMode(QGraphicsView::ScrollHandDrag);
        emit changeDragMode(true);
    }
    else
    {
        setDragMode(QGraphicsView::NoDrag);
        emit changeDragMode(false);
    }
}

void view::keyPressEvent(QKeyEvent *event)
{
//    не используется, управление перетаскиванием
//    if (event->modifiers() & Qt::ShiftModifier)
//    {
//        m_shiftKeyPressed = !m_shiftKeyPressed;
//        setDrag(m_shiftKeyPressed);
//    }
    pressKeyEvent(QApplication::keyboardModifiers(), event->key());
    QGraphicsView::keyPressEvent(event);
}

void view::keyReleaseEvent(QKeyEvent *event)
{
    QGraphicsView::keyReleaseEvent(event);
}

void view::setBrushSize(int brushSize, bool _updateCursor)
{
    this->brushSize = brushSize;
    if (_updateCursor)
        updateCursor();
}

void view::setWaMode(SceneAction mode)
{
    wamode = mode;
    updateCursor();
}

// обновление вида курсора
void view::updateCursor()
{
    switch (wamode)
    {
        case updateBrushSize:
        case setModifyObjectUsedBrush:
        {
            QPixmap pixmapTool(brushSize*_zoom,brushSize*_zoom);
            pixmapTool.fill(Qt::transparent);
            QPainter painterTool(&pixmapTool);
            QPen penTool;
            penTool.setColor(Qt::green);
            penTool.setCosmetic(false);
            penTool.setWidth(2);
            painterTool.setPen(penTool);
            painterTool.drawEllipse(0,0,brushSize*_zoom-penTool.width(),brushSize*_zoom-penTool.width());
            QCursor cursor(pixmapTool);
            this->setCursor(cursor);
            break;
        }
        case setRullerType:
        case setModifyObjectUsedPolygon:
        {
            this->setCursor(Qt::CrossCursor);
            break;
        }
        case setDefaultType:
        case setSelectionType:
        {
            this->setCursor(Qt::ArrowCursor);
            break;
        }
        default: break;
    }
}
