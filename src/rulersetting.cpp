#include "rulersetting.h"
#include "ui_rulersetting.h"
#include <QDoubleValidator>

// конструктор класса
rulerSetting::rulerSetting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::rulerSetting), m_doubleValidator(0.01, 5, 2, this)
{
    ui->setupUi(this);
    // настройка контроля ввода значений
    m_doubleValidator.setNotation(QDoubleValidator::StandardNotation);
    // раскладка по умолчанию
    QLocale locale(QLocale::English);
    m_doubleValidator.setLocale(locale);
    ui->m1->setValidator(&m_doubleValidator);
    // значения по умолчанию для полей масштаба
    ui->m1->setText("1,0");
    ui->m2->setText("1,0");
    // сохранение стиля кнопки
    g_qss = ui->add1->styleSheet();
}

rulerSetting::~rulerSetting()
{
    delete ui;
}

// удаление первой линейки
void rulerSetting::on_delete1_clicked()
{
    // сигнал удаления линейки на сцену
    scaleInstr.line.remove(0);
    emit deleteruler(0);
}

// уделение второй линейки
void rulerSetting::on_delete2_clicked()
{
    // сигнал удаления линейки на сцену
    scaleInstr.line.remove(1);
    emit deleteruler(1);
}

// добавление/изменение 1 линейки
void rulerSetting::on_add1_clicked()
{
    // возвращение первоначального вида кнопки
    ui->add1->setStyleSheet(g_qss);
    // сигнал добавления/изменения на сцену
    emit addruler(0, getScale(0)*1000);
}

// добавление/изменение 1 линейки
void rulerSetting::on_add2_clicked()
{
    // сигнал добавления/изменения на сцену
    emit addruler(1, getScale(1)*1000);
}

double rulerSetting::getScale(int n)
{
    QString str = ui->m->text();
    if (n == 1) str = ui->m2->text();
    str.replace(",", ".");
    double metr = str.toDouble();
    return metr;
}

RulerLines rulerSetting::getRuller()
{
    return scaleInstr.line;
}

void rulerSetting::setMM(RulerLines rl)
{
    foreach (int key, rl.keys())
    {
        if (key==0) ui->m->setText(QString::number(rl[key].mm_len/1000.0));
        if (key==1) ui->m2->setText(QString::number(rl[key].mm_len/1000.0));
    }
}

// кнопка добавления 1 красным отображается цветом
void rulerSetting::redColor()
{
    QColor colr = QColor(Qt::red);
    QString qss = QString("background-color: %1").arg(colr.name());
    ui->add1->setStyleSheet(qss);
}
