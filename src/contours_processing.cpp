#include "contours_processing.h"

#define xTrans(M,x,y) (int)(M.at<double>(0,0)*x+M.at<double>(0,1)*y+M.at<double>(0,2))/(M.at<double>(2,0)*x+M.at<double>(2,1)*y+M.at<double>(2,2))
#define yTrans(M,x,y) (int)(M.at<double>(1,0)*x+M.at<double>(1,1)*y+M.at<double>(1,2))/(M.at<double>(2,0)*x+M.at<double>(2,1)*y+M.at<double>(2,2))
#define updPoint(P,M) P.
#define RGB2GRAY(a) 0.299*a.red()+0.587*a.green()+0.114*a.blue()

contours_processing::contours_processing()
{
}

contours_processing::contours_processing(const QPixmap &inPixmap)
{
    getContours(inPixmap);
}

// нахождение контуров при ручной размеке объектов
QList<PolygonItem>
contours_processing::getContours(const QPixmap &inPixmap)
{
    cv::Mat cvGray;
    cv::Mat cvBin;
    vector<double> area;
    vector<cv::Vec4i> hierarchy;

    isz = inPixmap.size();
    cv::Mat  cvMat = QPixmapToCvMat( inPixmap, false );
    channels = cvMat.channels();
    cv::cvtColor( cvMat, cvGray, cv::COLOR_BGR2GRAY );
    QList<PolygonItem> polygonList;
    // две итерации одна для разных диапазонов яркостей
    // QList<EditAreaType> eats = {stoneArea, sandArea};
    QList<EditAreaType> eats = {stoneArea};
    foreach (EditAreaType eat, eats)
    {
        QColor a = areaPaintColor(eat);
        int thres = RGB2GRAY(a);
        cv::inRange(cvGray, cv::Scalar(thres-10), cv::Scalar(thres+10), cvBin);
        int dilation_size = 1;
        cv::Mat element = getStructuringElement( cv::MORPH_ELLIPSE,
                                             cv::Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                             cv::Point( dilation_size, dilation_size ) );
        // контуp строится внутри объекта и с каждой итерацией возможно уменьшение размера объекта,
        // для предотвращения уменьшения объекта необходимо использовать dilate
        cv::dilate( cvBin, cvBin, element );
        vector<vector<cv::Point> >  contours;
        cv::findContours(cvBin, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
        this->contours.insert(this->contours.end(), contours.begin(), contours.end());
         foreach (auto contour, contours)
        {
            PolygonItem polygon;
            // добавление точек полигона
            foreach (auto item, contour)
                polygon.push_back(QPointF(item.x,item.y));
            polygon.setType(eat);
            // полигон в список
            polygonList.push_back(polygon);

        }
    }
    return polygonList;
}

QList<QRect> contours_processing::getRectContours()
{
    QList<QRect> rectList;
    foreach (auto contour, contours)
    {
        QRect qrect;
        cv::Rect rect = cv::boundingRect(contour);
        qrect.setLeft(rect.x);
        qrect.setTop(rect.y);
        qrect.setRight(rect.x+rect.width);
        qrect.setBottom(rect.y+rect.height);
        rectList.push_back(qrect);
    }
    return rectList;
}

QList<countour_inf> contours_processing::getArea(RulerInfo ri)
{
    double line_scale = 0;
    if (ri.item.size()==2 && ri.Perspective>0)
    {
        int line = (ri.item[0].line.length()>ri.item[1].line.length()) ? 0 : 1;
        // Input Quadilateral or Image plane coordinates
        cv::Point2f inputQuad[4];
        // Output Quadilateral or World plane coordinates
        cv::Point2f outputQuad[4];
        float D[2][2];
        // формирование точек для выполнения преобразования перспективы
        D[0][0] = 0;
        D[0][1] = (float)1+ri.Perspective;
        D[1][0] = (float)ri.Perspective/2;
        D[1][1] = (float)1+ri.Perspective/2;

        float w = isz.width();
        float h = isz.height();
        // точки для вычисления матрицы
        inputQuad[0] = cv::Point2f( 0,0 );
        inputQuad[1] = cv::Point2f( w-1, 0 );
        inputQuad[2] = cv::Point2f( w-1, h-1 );
        inputQuad[3] = cv::Point2f( 0, h-1 );
        //
        outputQuad[0] = cv::Point2f( w*D[0][0],0  );
        outputQuad[1] = cv::Point2f( w*D[0][1]-1,0 );
        outputQuad[2] = cv::Point2f( w*D[1][1]-1, h-1 );
        outputQuad[3] = cv::Point2f( w*D[1][0], h-1  );

        // матрица для преобразования перспективы
        cv::Mat M = cv::getPerspectiveTransform( inputQuad, outputQuad );
        cv::Point tp;
        foreach (auto contour, contours)
        {
            foreach (cv::Point p, contour)
            {
                tp.x = xTrans(M,p.x,p.y);
                tp.y = yTrans(M,p.x,p.y);
                p = tp;
            }
        }
        QLineF* l = &ri.item[line].line;
        QLineF  rl;
        rl.setP1(QPoint(xTrans(M,l->p1().x(),l->p1().y()),yTrans(M,l->p1().x(),l->p1().y())));
        rl.setP2(QPoint(xTrans(M,l->p2().x(),l->p2().y()),yTrans(M,l->p2().x(),l->p2().y())));
        line_scale = 0.1*ri.item[line].mm_len/rl.length();
        // Apply the Perspective Transform just found to the src image
    }
    if (ri.item.size()==1)
    {
        line_scale = (0.1*ri.item.begin()->mm_len)/ri.item.begin()->line.length();
    }
    area.clear();
    countour_inf ci;
    foreach (auto item, contours)
    {
        cv::Rect rect = cv::boundingRect(item);
        ci.area = cv::contourArea(item)*pow(line_scale,2);
        ci.median = (rect.size().width > rect.size().height) ? rect.size().width : rect.size().height;
        ci.median *= line_scale;
        area.push_back(ci);
    }
    return area;
}

int contours_processing::getDepth()
{
    return  channels;
}

QSize contours_processing::getSize()
{
    return isz;
}
