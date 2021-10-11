#ifndef VIEW_H
#define VIEW_H

#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QApplication>
#include <QWheelEvent>
#include <QCursor>
#include "wavariables.h"

class view : public QGraphicsView
{
    Q_OBJECT
public:
    explicit view           (QWidget *parent = nullptr);
    void fitInView          (const QGraphicsItem *item,
                             Qt::AspectRatioMode aspectRadioMode = Qt::IgnoreAspectRatio);
    void setBrushSize       (int brushSize, bool _updateCursor = true);

protected:
    void enterEvent        (QEvent *event)                     override;
    void mouseReleaseEvent (QMouseEvent *event)                override;
    void mousePressEvent   (QMouseEvent *event)                override;
    void wheelEvent        (QWheelEvent *event)                override;
    void keyPressEvent     (QKeyEvent *event)                  override;
    void keyReleaseEvent   (QKeyEvent *event)                  override;
    void resizeEvent       (QResizeEvent *event)               override;
    void leaveEvent        (QEvent *event)                     override;

public slots:
    void setWaMode          (SceneAction property);
    void setDrag            (bool drag);
    void setZoomBtn         (bool zoomBtn);
    void calcZoom           ();

private:
//     void resizeEvent(QResizeEvent *event) override;
    void            SetZoomFactor(qreal zoom, const QPoint * optMousePos);
    void            updateCursor();
    qreal           _zoom = 0.2;
    int             brushSize = 0;
    bool            m_shiftKeyPressed = false;
    QPoint          pos;
    int             w1;
    int             h1;
    bool            zoomButtom = false;
    SceneAction     wamode = noAction;

signals:
    void changeDragMode(bool dm);
    void changeZoom(double zoom);
    void leaveWA();
    void pressKeyEvent(Qt::KeyboardModifiers, int);
    void pressSpaceAndwheelEvent(int);

public slots:
};

#endif // VIEW_H
