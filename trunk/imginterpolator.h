/** Projekt: Warp
*   Autor:   Miroslav Schery
*            Zsolt Horváth
*
*   Posledna uprava: 6.5.2011
*/

#ifndef IMGINTERPOLATOR_H
#define IMGINTERPOLATOR_H

#include <QImage>
#include <QPixmap>
#include <QtGui>

class ImgInterpolator
{


public:
    ImgInterpolator();
    ImgInterpolator(QSize size);
    ImgInterpolator(QPixmap *pixmap);
    ~ImgInterpolator();

    enum iMethod{
        NEAREST = 0,
        BILINEAR,
        BICUBIC
    };

    void createNewImg(QSize size);

    void loadNewImg(const QString filename);

    QRgb iClamp(QPointF position);
    QRgb iBilinear(QPointF position);
    QRgb iBicubic(QPointF position);

    QImage *img;
    QSize size;
};

#endif // IMGINTERPOLATOR_H
