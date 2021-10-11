#ifndef WASCENEMANUALLAYER_H
#define WASCENEMANUALLAYER_H

#include <QObject>
#include <QGraphicsPolygonItem>
#include <QGraphicsScene>
#include <QPainter>
#include "polygonitem.h"
#include "wavariables.h"

struct changeLogItem
{
    QPixmap pixmap;
    QString discr;
};

class wasceneManualLayer: public QObject
{
    Q_OBJECT

public:
    wasceneManualLayer(QGraphicsPixmapItem* manualLayerItem,
                       QGraphicsPathItem* traceToolItem,
                       QGraphicsPolygonItem* polygonItem);
    virtual ~wasceneManualLayer(){}
    // создание слоя
    void                    createManuallyMarkedLayer(QSize imageSize);
    // заполнение слоя размеченными объектами
    void                    drawStones(QSize imageSize,
                                       QList <PolygonItem>* stones);
    // возвращает область
    QGraphicsPixmapItem*    getLayer();
    // замыкание полигона
    void                    polygonClosure();
    // создание полигона
    void                    newPolygon(QPointF point);
    // добавление точки к полигону
    void                    addPointToPolygon(QPointF point);
    // удаление точки из полигона
    void                    removePointFromPolygon();
    // отслеживание перемешения точки полигона
    void                    movePolygonPoint(QPointF point);
    // используется полигон для выделениения или нет
    bool                    polygonUse();
    // установить прозрачность
    void                    setOpacity(double opacity);
    // удалить слой
    void                    remove(QGraphicsScene* scene);
    // кисть
    // создание линии инструментом brush
    void                    brushTool(QPointF beg,
                                      QPointF end);
    // установить размер кисти
    void                    setBrushSize(int brushSize);
    // завершение линии инструментом brush
    void                    brushToolEnd();
    // квадратный полигон на весь снимок
    void                    fullSizeImagePolygon(QGraphicsPixmapItem* image);

    // показать/скрыть слой
    void                    visible(bool);
    // загрузить из файла
    bool                    fromFile(QGraphicsScene* scene,
                                     QString filename);
    // виден ли текущий слой
    bool                    isVisible();
    // сообщение
    QString                 getMessage();
    // показать изменения
    bool                    viewChangeLog(int num);
    // тип изменяемой области камень, неразмеченная область или область для удаления
    void                    setAreaType(EditAreaType areaType);

signals:
    void                    changedLogItemsAdded(QStringList);
    void                    updateImage(void);

private:
    void                    addChangedLogItem(changeLogItem);


private:
    // Item для показа инструментов редактирования
    QGraphicsPathItem*              manuallyToolLayer = nullptr;
    bool                            toolUse = false;
    // слой содержащий растровое изображение разметки
    // используется для определения контуров
    // неразмеченная область обозначается синим цветом
    // размеченная: камни красным красным, песок также красным но другого отенка
    QGraphicsPixmapItem*            manuallyMarkedLayer = nullptr;
    // режим ручного редактирования с помощью полигона
    QGraphicsPolygonItem*           polygonTools = nullptr;
    // сообщение
    QString                         stateMessage;
    // список изменённых состояний
    QList<changeLogItem>            changeLogSheet;
    // шаг с которого нужно изменить историю
    int                             selectedLog = 0;
    // флаг отката
    // возвращения к одному из предыдущих размеченных состояний
    bool                            undo =false;
    // счетчик полигонов, для обозначения изменённых состояний
    int                             polygonCounter = 1;
    // счетчик линий, для обозначения изменённых состояний
    int                             bruchCounter = 1;
    // счетчик авторазметки, для обозначения изменённых состояний
    int                             amCounter = 0;
    // тип
    EditAreaType                    areaType = freeArea;

    // окружение для brush
    int brushSize                   = 1;
    QPen                            brushToolPen;
    QPixmap                         brushToolPm;
    QPainter                        brushToolPainter;
    // для формирования следа
    QPen                            trackBrushToolPen;
    QPixmap                         trackBrushToolPm;
    QPainter                        trackBrushToolPainter;
    QGraphicsPathItem*              traceToolLayer = nullptr;
    // прозрачность следа инструментов
    double                          toolOpacity = 0.5;
    QColor                          toolColor = Qt::green;
};

#endif // WASCENEMANUALLAYER_H
