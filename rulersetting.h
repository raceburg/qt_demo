#ifndef RULERSETTING_H
#define RULERSETTING_H

#include <QWidget>
#include <QDoubleValidator>
#include <QDebug>
#include "rulerstruct.h"


class ruler;

namespace Ui {
class rulerSetting;
}

struct RulerStruct
{
    RulerLines   line;
};

class rulerSetting : public QWidget
{
    Q_OBJECT

public:
    explicit rulerSetting(QWidget *parent = nullptr);
    ~rulerSetting();
    double  getScale(int);
    RulerLines  getRuller();
    void    redColor();
    void    setMM(RulerLines rl);

private slots:
    void on_delete1_clicked();
    void on_delete2_clicked();
    void on_add1_clicked();
    void on_add2_clicked();

signals:
    void deleteruler(int);
    void addruler(int, int);


private:
    Ui::rulerSetting    *ui;
    QDoubleValidator    m_doubleValidator;
    RulerStruct         scaleInstr;
//    QPointF             p1, p2;
//    bool                init1 = false;
//    bool                init2 = false;
    QString             g_qss;
//    double              perspectiveD;
};

#endif // RULERSETTING_H
