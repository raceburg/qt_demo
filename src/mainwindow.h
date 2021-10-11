#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gradingtask.h"
#include "view.h"
#include "wascene.h"
#include "contours_processing.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


// --------------------------------
// класс описания основного окна приложения
// --------------------------------

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    // сигал выбора изображения
    void imageSelected(QString);

private:
    Ui::MainWindow  *ui;
    // окружение для работы с потоком
    // QList<int> A;
    // QList<int> R;
    // GradingTask     *gradingTask;

    // Загружаемый файл
    QString         imageName;
    // виджет для управления  и отображения сценой с объектами
    view*                   waView;
    // графическая сцена
    wascene*                scene;
    // текущий номер линейки
    int                     currentRuler;
    // структура данных для линеек
    RulerInfo               rulerInfo;
    // информация о контурах
    QList<countour_inf>     areaInfo;
    // статистика для просмотра изображения
    statisticTable          viewStat;

    // вывод сообщений для пользователя, не реализованно
    void printMessage(mesType, QString);
    // применение цветовой палитры к размеченным объетам
    void colorStones();

public slots:
    // окружение для работы с потоком
    // void taskEnd(QObject*);
    // открытие изображения
    void imageSelect();
    // установка режима работы приложения
    void setActionMode(int m);
    // назначение поверхности для ручной разметки
    void setAreaType(int a = 0);
    // находжение контуров
    void findContours(void);
    // заполнение талицы фракций
    void fillViewFractionTbl();


};
#endif // MAINWINDOW_H
