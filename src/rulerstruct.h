#ifndef RULERSTRUCT_H
#define RULERSTRUCT_H
#include <QLineF>
#include <QMap>

struct RulerLinesInfo
{
    QLineF  position;
    int     mm_len;
};

struct RulerLineInfo
{
    RulerLineInfo() {mm_len = -1;}
    RulerLineInfo(QLineF l, int lm) {line = l; mm_len = lm;}
    QLineF line;
    int    mm_len;
};

typedef QMap<int,RulerLineInfo> RulerLines;

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
    void clear()
    {
        Perspective = 0;
        item.clear();
    }
};

#endif // RULERSTRUCT_H
