#ifndef CONTOURS_PROCESSING_H
#define CONTOURS_PROCESSING_H
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QtGlobal>
#include "opencv2/opencv.hpp"
#include "contour_inf.h"
#include "rulerstruct.h"
#include "statisticobjects.h"
#include "polygonitem.h"
using namespace std;

//https://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap/

// NOTE: This does not cover all cases - it should be easy to add new ones as required.
inline QImage  cvMatToQImage( const cv::Mat &inMat )
{
   switch ( inMat.type() )
   {
      // 8-bit, 4 channel
      case CV_8UC4:
      {
         QImage image( inMat.data,
                       inMat.cols, inMat.rows,
                       static_cast<int>(inMat.step),
                       QImage::Format_ARGB32 );

         return image;
      }

      // 8-bit, 3 channel
      case CV_8UC3:
      {
         QImage image( inMat.data,
                       inMat.cols, inMat.rows,
                       static_cast<int>(inMat.step),
                       QImage::Format_RGB888 );

         return image.rgbSwapped();
      }

      // 8-bit, 1 channel
      case CV_8UC1:
      {
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
         QImage image( inMat.data,
                       inMat.cols, inMat.rows,
                       static_cast<int>(inMat.step),
                       QImage::Format_Grayscale8 );
#else
         static QVector<QRgb>  sColorTable;

         // only create our color table the first time
         if ( sColorTable.isEmpty() )
         {
            sColorTable.resize( 256 );

            for ( int i = 0; i < 256; ++i )
            {
               sColorTable[i] = qRgb( i, i, i );
            }
         }

         QImage image( inMat.data,
                       inMat.cols, inMat.rows,
                       static_cast<int>(inMat.step),
                       QImage::Format_Indexed8 );

         image.setColorTable( sColorTable );
#endif

         return image;
      }

      default:
         qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
         break;
   }

   return QImage();
}

// If inImage exists for the lifetime of the resulting cv::Mat, pass false to inCloneImageData to share inImage's
// data with the cv::Mat directly
//    NOTE: Format_RGB888 is an exception since we need to use a local QImage and thus must clone the data regardless
//    NOTE: This does not cover all cases - it should be easy to add new ones as required.
inline cv::Mat QImageToCvMat( const QImage &inImage, bool inCloneImageData = true )
{
   switch ( inImage.format() )
   {
      // 8-bit, 4 channel
      case QImage::Format_ARGB32:
      case QImage::Format_ARGB32_Premultiplied:
      {
         cv::Mat  mat( inImage.height(), inImage.width(),
                       CV_8UC4,
                       const_cast<uchar*>(inImage.bits()),
                       static_cast<size_t>(inImage.bytesPerLine())
                       );

         return (inCloneImageData ? mat.clone() : mat);
      }

      // 8-bit, 3 channel
      case QImage::Format_RGB32:
      {
         if ( !inCloneImageData )
         {
            qWarning() << "ASM::QImageToCvMat() - Conversion requires cloning so we don't modify the original QImage data";
         }

         cv::Mat  mat( inImage.height(), inImage.width(),
                       CV_8UC4,
                       const_cast<uchar*>(inImage.bits()),
                       static_cast<size_t>(inImage.bytesPerLine())
                       );

         cv::Mat  matNoAlpha;

         cv::cvtColor( mat, matNoAlpha, cv::COLOR_BGRA2BGR );   // drop the all-white alpha channel

         return matNoAlpha;
      }

      // 8-bit, 3 channel
      case QImage::Format_RGB888:
      {
         if ( !inCloneImageData )
         {
            qWarning() << "ASM::QImageToCvMat() - Conversion requires cloning so we don't modify the original QImage data";
         }

         QImage   swapped = inImage.rgbSwapped();

         return cv::Mat( swapped.height(), swapped.width(),
                         CV_8UC3,
                         const_cast<uchar*>(swapped.bits()),
                         static_cast<size_t>(swapped.bytesPerLine())
                         ).clone();
      }

      // 8-bit, 1 channel
      case QImage::Format_Indexed8:
      {
         cv::Mat  mat( inImage.height(), inImage.width(),
                       CV_8UC1,
                       const_cast<uchar*>(inImage.bits()),
                       static_cast<size_t>(inImage.bytesPerLine())
                       );

         return (inCloneImageData ? mat.clone() : mat);
      }

      default:
         qWarning() << "ASM::QImageToCvMat() - QImage format not handled in switch:" << inImage.format();
         break;
   }

   return cv::Mat();
}

typedef std::string String;
inline void showPixmap(const String& winname, const QPixmap &inPixmap)
{
    cv::Mat  cvMat = QImageToCvMat( inPixmap.toImage(), false );
    cv::imshow(winname, cvMat);
}


void  getAreaStat(QList<countour_inf> area, QStringList fractination, QList<QColor> colors, statisticTable *stbl);
class contours_processing
{
public:
    contours_processing();
    contours_processing(const QPixmap &inPixmap);
    QList<PolygonItem>      getContours(const QPixmap &inPixmap);
    QList<countour_inf>     getArea(RulerInfo ri);
    QList<QRect>            getRectContours();
    QSize                   getSize();
    int                     getDepth();


private:
    vector<vector<cv::Point> >  contours;
    QList<countour_inf>         area;
    int                         contousrID = 0;
    QSize                       isz;
    int                         channels;

private:

    inline QPixmap cvMatToQPixmap( const cv::Mat &inMat )
    {
       return QPixmap::fromImage( cvMatToQImage( inMat ) );
    }

    // If inPixmap exists for the lifetime of the resulting cv::Mat, pass false to inCloneImageData to share inPixmap's data
    // with the cv::Mat directly
    //    NOTE: Format_RGB888 is an exception since we need to use a local QImage and thus must clone the data regardless
    inline cv::Mat QPixmapToCvMat( const QPixmap &inPixmap, bool inCloneImageData = true )
    {
       return QImageToCvMat( inPixmap.toImage(), inCloneImageData );
    }
};


#endif // CONTOURS_PROCESSING_H
