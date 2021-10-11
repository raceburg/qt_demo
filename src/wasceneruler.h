#ifndef WASCENERULLER_H
#define WASCENERULLER_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsPathItem>
#include <QPainter>


struct SceneRuler
{
    // объект QGraphicsScene
    QGraphicsPathItem*    line = nullptr;
    // длина линейки
    double                rulerlen = 0;
    // настройки линии линейки
    QPen                  pen = QPen();
};


class wasceneRuler
{
public:
    wasceneRuler(QGraphicsScene* parent);
    ~wasceneRuler();
    // добавление новой линейки
    void    add();
    // назначение линии на QGraphicsScene для текущей линейки
    void    updateLine(QPointF begin, QPointF end);
    // удалить линейку с номером
    void    remove(int);
    // удаилить все линейки
    void    removeAll();
    // удалить текущую линейку
    void    removeCurrentRuler();
    // показать/скрыть линейки
    void    show(bool);
    // назначить номер текущей линейки и ее длину
    void    setCurentRuler(int curentRuler, int rulerLen);
    // возвращает номер текущей линейки
    int     curentRuler();
    // возвращает длину текущей линейки
    int     rulerLen();

private:
    // линейки
    QList <SceneRuler>             rulers;
    // указатель QGraphicsScene
    QGraphicsScene*                scene;
    // выбранный номер линейки
    int                            currentRulerNum = 0;
    // длина текущей линейки
    int                            curentRulerLen = 0;
};

#endif // WASCENERULLER_H
