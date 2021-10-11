#ifndef RULERSTRUCT_H
#define RULERSTRUCT_H
#include <QLineF>
#include <QMap>

// настройки линейки
struct RulerLineInfo
{
    RulerLineInfo() {mm_len = -1;}
    // линия на изображении
    RulerLineInfo(QLineF l, int lm) {line = l; mm_len = lm;}
    QLineF line;
    // длина линейки
    int    mm_len;
};

typedef QMap<int,RulerLineInfo> RulerLines;

// контейнер для двух линеек
struct RulerInfo
{
    double Perspective;
    RulerLines item;
    bool useRuler() { return item.size() > 0; }
    bool setRuler(int Lnum, QLineF line, int mm_len, double perspective)
    {
        Perspective = perspective;
        RulerLineInfo ri(line,mm_len);
        item[Lnum] = ri;
        return true;
    }
};

#endif // RULERSTRUCT_H
