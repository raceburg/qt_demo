#ifndef MYSCENE_H
#define MYSCENE_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QResizeEvent>
#include <QFileDialog>
#include <QDebug>
#include <QGraphicsItem>
#include <QMessageBox>
#include <QPainter>
#include <qmath.h>
#include <QApplication>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QDebug>

#include "wavariables.h"
#include "wasceneruler.h"
#include "wascenegrid.h"
#include "wascenemanuallayer.h"
#include "messagetype.h"
#include "polygonitem.h"
#include "rulerstruct.h"

class QGraphicsSceneMouseEvent;
class QKeyEvent;

// флаги для очистки
#define ImageLayer (1<<0)
#define RulerLayer (1<<1)
#define PolygonLayer (1<<2)
#define AllLayer (0xFF)

struct PolygonTools
{
    QGraphicsPolygonItem*  item = nullptr;
    bool                   intersect= false;
};

class wascene : public QGraphicsScene
{
    Q_OBJECT

    Q_PROPERTY(int currentAction READ currentAction WRITE setCurrentAction NOTIFY currentActionChanged)
    Q_PROPERTY(QPointF previousPosition READ previousPosition WRITE setPreviousPosition NOTIFY previousPositionChanged)

public:
    explicit wascene(QObject *parent = nullptr);
    ~wascene() override;

    int currentAction() const;
    QPointF previousPosition() const;

    void setCurrentAction(const int type);
    void setPreviousPosition(const QPointF previousPosition);
    bool addImage(QString fileName, LoadImageType type);
    QGraphicsPixmapItem* getImageItem();
    void addContourOfStone(PolygonItem*);
    void addContoursOfStone(QList<PolygonItem>);
    bool contoursExist();
    void showRuller(bool);
    void createUnallocatedLayers();
    QGraphicsPixmapItem* getManuallyMarkedLayer();
    QPixmap QPixmapFromItem(QGraphicsItem *);
    void loadRulers(RulerLines ruler);


 signals:
     void previousPositionChanged();
     // изменение текущего состояния
     void currentActionChanged(SceneAction i);
     // передача координат для линейки
     void sendRulerPoints(QLineF line, int LNum, double perspective);
     void removeRuler(int LNum);
     // номер выделенной области
     void areaSelected(int);
     // возвращение размера сетки, если -1 то линейки не установлены или установлены неправильно
     void sendGridSize(double);
     // наличие сетки
     void gridReady(bool);
     // сообщения
     void sendMessage(mesType, QString);
     // разрешение использование ручного режима
     void manualModeEnable(bool ens);

 protected:
     void mousePressEvent   (QGraphicsSceneMouseEvent *event)   override;
     void mouseMoveEvent    (QGraphicsSceneMouseEvent *event)   override;
     void mouseReleaseEvent (QGraphicsSceneMouseEvent *event)   override;
     void keyPressEvent     (QKeyEvent *event)                  override;


 public slots:
    void setLayersProperty (InstructionName property, QVariant items,  QColor colourItems);
    void setLayersProperty (InstructionName property, QVariant items);
    void setLayersProperty (InstructionName property);
    void changeDragMode    (bool drag);
    void leaveWA           ();

 private:
    void deleteruler(int);
    void removeContourOfStone();

public:
    // слой для ручной разметки
     wasceneManualLayer*    mmLayer;

 private:
     int                    m_currentAction;
     int                    m_previousAction;
     QPointF                m_previousPosition;
     bool                   m_leftMouseButtonPressed;
     bool                   m_rightMouseButtonPressed;
     bool                   m_shiftKeyPressed = false;
     bool                   m_objectVisible = true;
     double                 m_objectOpacity = 0.4;
     // линейки
     wasceneRuler*          ruler;
     // сетка
     wasceneGrid*           grid;
     // указатель на изображение
     QGraphicsPixmapItem*   image = nullptr;
     // список указателей на полигоны выделенных камней
     QList <QGraphicsPolygonItem *> stones;
     // полигон над которым находится курсор
     QGraphicsPolygonItem*  selectedPolygon = nullptr;
     QBrush                 selectedPolygonBrush;
     double                 selectedPlygonOpacity;
     // выделенная пользователем область, для подсветки
     QGraphicsPixmapItem*   selectedarea = nullptr;
     // область размеченных пользователем данных
     QGraphicsPixmapItem*   manualLayerItem;
     // область для отображения следа инструмента
     QGraphicsPathItem*     traceToolItem;
     // область для отображения разметки с помощью полигона
     QGraphicsPolygonItem*  polygonItem;
     // прозрачность, заданнная пользователем
//     double                 opacityManuallayer;
     double                 opacity;
     // неразмеченная область
     QGraphicsPixmapItem*   unallocated = nullptr;
     // режим перемещения изображения вкл/выкл
     bool                   moveMode = false;
     // флаг показа сетки
     bool                   gridShow = false;
     // zoom
     double                 zoom;
     // сообщения состояния
     QString                stateMessage;
 };

#endif // MYSCENE_H

