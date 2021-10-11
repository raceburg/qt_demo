#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <qdebug.h>

#define BIT(X) (1<<X)

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->toolFrame->hide();
    // скрыть статистику распределения
    ui->fractionLabel->hide();
    ui->fractionTable->hide();
    ui->fractionsSize->hide();
    // настройка кнопок
    ui->arrow->hide();
    ui->brush->hide();
    ui->polygon->hide();
    ui->L1->hide();
    ui->L2->hide();
    ui->opacityFrame->hide();
    ui->allocFrame->hide();
    ui->brushFrame->hide();
    ui->rulerFrame->hide();
    // даобвление элемента просмотра рабочего пространства
    QLayout* layout = ui->imageWorkarea->layout();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    waView = new view(this);
    waView->hide();
    layout->addWidget(waView);
    // добавление сцены для рабочего пространства
    scene = new wascene();
    waView->setScene(scene);
    scene->setLayersProperty(InstructionName::setOpacityObjects, ui->stoneOpacity->value()*0.01);
    scene->setLayersProperty(InstructionName::manualLayer_SetOpacity, ui->manualOpacity->value()*0.01);
    // события от кнопок
    // кнопка открыть изображение
    connect(ui->openImage, &QToolButton::clicked, this, &MainWindow::imageSelect);
    // изменение размера кисти
    connect(ui->brushSize, &QSlider::valueChanged, [=](int a){
        scene->setLayersProperty(InstructionName::manualLayer_SetBrushSize, a);
        waView->setBrushSize(ui->brushSize->value());
    });
    // изменение прозрачности камней
    connect(ui->stoneOpacity, &QSlider::valueChanged, [=](int a){
        scene->setLayersProperty(setOpacityObjects, a*0.01);
    });
    // изменение прозрачности слоя ручной разметки
    connect(ui->manualOpacity, &QSlider::valueChanged, [=](int a){
        scene->setLayersProperty(InstructionName::manualLayer_SetOpacity, a*0.01);
        waView->setBrushSize(ui->brushSize->value());
    });
    // нажатие кнопки из группы, изменение режима работы
    connect(ui->action, SIGNAL(buttonClicked(int)), this, SLOT(setActionMode(int)));
    // нажатие кнопки из группы, изменение размечаемой области
    connect(ui->areaType, SIGNAL(buttonClicked(int)), this, SLOT(setAreaType(int)));
    // изменение размера кисти скроллером
    connect(waView, &view::pressSpaceAndwheelEvent, [=](int w){
        if (scene->currentAction() == ActionTypes::ModifyObjectUsedBrush)
            ui->brushSize->setValue(ui->brushSize->value()+(w>>5));
    });
    //  передача данных для линейки:
    //  сигнал с кнопки добавления линейки
    connect(ui->addRuler, &QToolButton::clicked, [=](){
        bool ok;
        int rl = ui->rulerLen->text().toInt(&ok, 10);
        // преобразование выполнено корректно
        if (ok) {
            QVariantList vl;
            vl.push_back(currentRuler);
            vl.push_back(rl*10);
            scene->setLayersProperty(InstructionName::ruler_setMetricLength, vl);
        }
    });
    //  команда на пересчет
    connect(scene->mmLayer, &wasceneManualLayer::updateImage, this, &MainWindow::findContours);
    //  передача координат линейки
    connect(scene, &wascene::sendRulerPoints, [=](QLineF line,int lnum, double perspective){
        rulerInfo.setRuler(lnum, line, ui->rulerLen->text().toInt()*10, perspective);
        if (scene->contoursExist())
            findContours();
    });
}

// обработка нажатия кнопки выбора базового изображения из файла
void MainWindow::imageSelect()
{
    imageName = QFileDialog::getOpenFileName(this,
                                            tr("Select image"),
                                            "",
                                            tr("Image Files (*.png *.jpg *.JPG *.jpeg *.JPEG)"));
    // загрузка изображения из выбранного файла
    if (!imageName.isEmpty())
    {
        scene->setLayersProperty(InstructionName::clearWorkLayers, AllLayer);
        rulerInfo.clear();
        waView->setWaMode(SceneAction::setDefaultType);
        waView->show();
        ui->noImage->hide();
        if (!scene->addImage(imageName, LoadImageType::baseImage))
        {
            printMessage(Warn,"Неподдерживаемый формат изображения в файле "+imageName);
            return;
        }
        waView->fitInView(scene->getImageItem(), Qt::KeepAspectRatio);
        printMessage(Info,"Открыто изображение " + imageName);
        // активация кнопок
        ui->arrow->show();
        ui->brush->show();
        ui->polygon->show();
        ui->L1->show();
        ui->L2->show();
        ui->toolFrame->show();
        // скрыть элементы интерфейса
        ui->fractionLabel->hide();
        ui->fractionTable->hide();
        ui->fractionsSize->hide();
        scene->setCurrentAction(ActionTypes::ModifyObjectUsedBrush);
    }
}

// изменение режима работы
void MainWindow::setActionMode(int m)
{
    Q_UNUSED(m)
    int mode = 0;
    if (ui->arrow->isChecked())          mode |= BIT(0);
    if (ui->brush->isChecked())          mode |= BIT(1);
    if (ui->polygon->isChecked())        mode |= BIT(2);
    if (ui->L1->isChecked())             mode |= BIT(3);
    if (ui->L2->isChecked())             mode |= BIT(4);
    // изменение внешнего вида панели в зависимости от выбранного функционала
    ui->allocFrame->setVisible(mode&BIT(2)||mode&BIT(1));
    ui->brushFrame->setVisible(mode&BIT(1));
    ui->opacityFrame->setVisible(mode&BIT(0)||mode&BIT(2)||mode&BIT(1));
    ui->stoneOpacity->setVisible(mode&BIT(0));
    ui->manualOpacity->setVisible(mode&BIT(2)||mode&BIT(1));
    // задание режима работы view
    if (mode&BIT(0)) waView->setWaMode(SceneAction::setSelectionType);
    if (mode&BIT(1)) waView->setBrushSize(ui->brushSize->value(), false);
    if (mode&BIT(1)) waView->setWaMode(SceneAction::setModifyObjectUsedBrush);
    if (mode&BIT(2)) waView->setWaMode(SceneAction::setModifyObjectUsedPolygon);
    if (mode&BIT(3)||mode&BIT(2)) waView->setWaMode(SceneAction::setRullerType);
    // задание режима работы сцены
    if (mode&BIT(0)) scene->setCurrentAction(ActionTypes::SelectionType);
    if (mode&BIT(1)) scene->setLayersProperty(InstructionName::manualLayer_SetBrushSize, ui->brushSize->value());
    if (mode&BIT(1)) scene->setCurrentAction(ActionTypes::ModifyObjectUsedBrush);
    if (mode&BIT(2)) scene->setCurrentAction(ActionTypes::ModifyObjectUsedPolygon);
    scene->setLayersProperty(InstructionName::manualLayer_SetVisible, mode&BIT(1)||mode&BIT(2));
    scene->setLayersProperty(InstructionName::visibleObjects, (bool)(mode&BIT(0)));
    // линейки
    if (mode&BIT(3)) currentRuler = 0;
    if (mode&BIT(4)) currentRuler = 1;
    ui->rulerFrame->setVisible(mode&BIT(3)||mode&BIT(4));
    if (mode&BIT(3)||mode&BIT(4))
    {
        scene->setCurrentAction(ActionTypes::DefaultType);
        if (mode&BIT(3) && rulerInfo.item.contains(0))
            ui->rulerLen->setText(QString::number(int(rulerInfo.item[0].mm_len/10)));
        else
        {
            if (mode&BIT(4) && rulerInfo.item.contains(1))
                ui->rulerLen->setText(QString::number(int(rulerInfo.item[1].mm_len/10)));
            else
                ui->rulerLen->setText("100");
        }
    }
    if (mode&BIT(0)||mode&BIT(1)||mode&BIT(2)) setAreaType();
    // генерировать событие во время перемещения указателя мыши
    waView->setMouseTracking(true);
}

void MainWindow::findContours(void)
{
    // добавление информации о контурах в рабочее пространство
    contours_processing cp;
    QList<PolygonItem> polygonList = cp.getContours(scene->getManuallyMarkedLayer()->pixmap());

    // текущий режим работы
    int mode = scene->currentAction();
    // удаление слоев из рабочего пространства, удаляются все слои кроме изображения
    scene->setLayersProperty(InstructionName::clearWorkLayers, PolygonLayer);
    scene->addContoursOfStone(polygonList);
    scene->setCurrentAction(mode);
    // генерировать событие во время перемещения указателя мыши
    waView->setMouseTracking(true);
    if (rulerInfo.useRuler())
    {
        // получение информации о контурах
        areaInfo = cp.getArea(rulerInfo);
        // формирование стат информации в виде структуры
        viewStat.getAreaStat(areaInfo,
                             ui->fractionsSize->text().split(QRegExp(",\\s*")));
        // заполнение таблицы и информации на основе статистических данных
        fillViewFractionTbl();
        colorStones();
    }
    scene->setCurrentAction(mode);
}

// назначение поверхности для ручной разметки
void MainWindow::setAreaType(int a)
{
    Q_UNUSED(a)
    int mode = 0;
    if (ui->allocArea->isChecked())      mode |= BIT(0);
    if (ui->unallocArea->isChecked())    mode |= BIT(1);
    if (ui->unusedArea->isChecked())     mode |= BIT(2);
    if (mode&BIT(0)) scene->setLayersProperty(InstructionName::manualLayer_SetEditedAreaType, stoneArea);
    if (mode&BIT(1)) scene->setLayersProperty(InstructionName::manualLayer_SetEditedAreaType, freeArea);
    if (mode&BIT(2)) scene->setLayersProperty(InstructionName::manualLayer_SetEditedAreaType, sandArea);
}

void MainWindow::fillViewFractionTbl()
{
    ui->fractionTable->clearContents();
    ui->fractionTable->setShowGrid(true); // Включаем сетку
    // Разрешаем выделение только одного элемента
    ui->fractionTable->setSelectionMode(QAbstractItemView::SingleSelection);
    // Разрешаем выделение построчно
    ui->fractionTable-> setSelectionBehavior(QAbstractItemView::SelectRows);
    // Растягиваем последнюю колонку на всё доступное пространство
    ui->fractionTable->horizontalHeader()->setStretchLastSection(true);
    ui->fractionTable->horizontalHeader()->setMinimumSectionSize(5);
    ui->fractionTable->setRowCount(viewStat.statRow.size());
    ui->fractionTable->setColumnCount(4);
    ui->fractionTable->setHorizontalHeaderLabels(
                QStringList()<<""<<"d"<<"%"<<"N");
    int k = 0;
    foreach (StatisticRow row, viewStat.statRow)
    {
        QTableWidgetItem* item = new QTableWidgetItem("");
        item->setBackground(row.color);
        ui->fractionTable->setItem(k, 0, item);
        item = new QTableWidgetItem(row.fraction.str());
        ui->fractionTable->setItem(k, 1, item);
        item = new QTableWidgetItem(QString::number(row.area));
        ui->fractionTable->setItem(k, 2, item);
        item = new QTableWidgetItem(QString::number(row.count));
        ui->fractionTable->setItem(k, 3, item);
        k++;
    }
    // ширина колонок
    ui->fractionTable->horizontalHeader()->resizeSection(0,10);
    ui->fractionTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->fractionTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    // показать элементы интерфейса
    ui->fractionLabel->show();
    ui->fractionTable->show();
    ui->fractionsSize->show();
}

void MainWindow::colorStones()
{
    foreach (StatisticRow row, viewStat.statRow)
    {
        QVariantList qvl;
        foreach (int n, row.contours)
            qvl.push_back(n);
        scene->setLayersProperty(setColorFraction, qvl, row.color);
    }
}

MainWindow::~MainWindow()
{
    delete scene;
    delete waView;
    delete ui;
}


void MainWindow::printMessage(mesType type, QString message)
{
//    QTime currTime = QTime::currentTime();
//    QString Message = currTime.toString("hh:mm:ss");
//    Message = Message + ":   " + message;
//    ui->lastMessage->setText(Message);
//    ui->messages->moveCursor (QTextCursor::End);
//    ui->messages->insertPlainText ("\n");
//    ui->messages->insertPlainText (Message);
//    ui->messages->moveCursor (QTextCursor::End);
//    if (type==mesType::Err)
//        ui->lastMessage->setStyleSheet("QLineEdit { background-color : red; color : white; }");
//    if (type==mesType::Warn)
//        ui->lastMessage->setStyleSheet("QLineEdit { background-color : yellow; color : black; }");
//    if (type==mesType::Info)
//        ui->lastMessage->setStyleSheet("QLineEdit { background-color : white; color : black; }");
}

// пример запуска потока
// {
//     A << 1 << 10 << 100;
//     QObject* qi = (QObject*) &A;
//     QObject* qo = (QObject*) &R;
//     gradingTask = new GradingTask(test, qi, qo);
//     connect(gradingTask, &GradingTask::result, this, &MainWindow::taskEnd);
//     QThreadPool::globalInstance()->start(gradingTask);
//     emit imageSelected(imageName);
// }
//
// void MainWindow::taskEnd(QObject* r)
// {
//     disconnect(gradingTask, &GradingTask::result, this, &MainWindow::taskEnd);
//     QList<int>* in_data = (QList<int>*)(r);
//     qDebug() << (*in_data)[0];
// }

// функция выполняемая в потоке, для проверки
//void test(QObject* in, QObject* out)
//{
//    QList<int>* in_data = (QList<int>*)(in);
//    int r = 0;
//    foreach (int d, *in_data) {
//        r += d;
//    }
//    QList<int>* out_data = (QList<int>*)(out);
//    (*out_data) << r;
//}
