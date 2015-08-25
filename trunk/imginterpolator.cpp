/** Projekt: Warp
*   Autor:   Miroslav Schery
*            Zsolt Horváth
*
*   Posledna uprava: 6.5.2011
*/

#include "imginterpolator.h"

ImgInterpolator::ImgInterpolator()
{
    img = NULL;
    size.setWidth(0);
    size.setHeight(0);
}

ImgInterpolator::ImgInterpolator(QSize imgSize)
{
    img = new QImage(imgSize, QImage::Format_RGB32);
    img->fill(0xffffff);
    size = imgSize;
}

ImgInterpolator::ImgInterpolator(QPixmap *pixmap)
{
    img = new QImage(pixmap->toImage());
    size = img->size();
}

ImgInterpolator::~ImgInterpolator()
{
    delete img;
}

void ImgInterpolator::createNewImg(QSize imgSize)
{
    delete img;
    img = new QImage(imgSize, QImage::Format_RGB32);
    img->fill(0xffffff);
    size = imgSize;
}

QRgb ImgInterpolator::iClamp(QPointF position)
{
    if(img == NULL){
        QMessageBox::information(0, "Bacha!", "Pouzivanie neinicializovaneho obrazku ImgInterpolator!");
        exit(EXIT_FAILURE);
    }
    return img->pixel(position.toPoint());
}

QRgb ImgInterpolator::iBilinear(QPointF position)
{
    if(img == NULL){
        QMessageBox::information(0, "Bacha!", "Pouzivanie neinicializovaneho obrazku ImgInterpolator!");
        exit(EXIT_FAILURE);
    }
    int p1x, p1y, p2x, p2y;

    float red, green, blue;
    float fx = floor(position.x());
    float fy = floor(position.y());

    p1x = (int)fx;
    p1y = (int)fy;
    p2x = (int)fx+1;
    p2y = (int)fy+1;
    float lx = position.x() - fx;
    float ly = position.y() - fy;
    float rx = 1 - lx;
    float ry = 1 - ly;

    if(p1x < 0) p1x = 0;
    if(p1y < 0) p1y = 0;
    if(p2x >= size.width()) p2x = size.width()-1;
    if(p2y >= size.height()) p2y = size.height()-1;

    float tmpF = rx*ry;
    QColor tmp = img->pixel(QPoint(p1x,p1y));

    red = tmpF * tmp.redF();
    green = tmpF * tmp.greenF();
    blue = tmpF * tmp.blueF();
    tmp = img->pixel(QPoint(p2x,p1y));

    tmpF = lx*ry;
    red += tmpF * tmp.redF();
    green += tmpF * tmp.greenF();
    blue += tmpF * tmp.blueF();
    tmp = img->pixel(QPoint(p1x,p2y));

    tmpF = rx*ly;
    red += tmpF * tmp.redF();
    green += tmpF * tmp.greenF();
    blue += tmpF * tmp.blueF();
    tmp = img->pixel(QPoint(p2x,p2y));

    tmpF = lx*ly;
    red += tmpF * tmp.redF();
    green += tmpF * tmp.greenF();
    blue += tmpF * tmp.blueF();
    //qDebug() << red << green << blue;

    return (QColor(red*255, green*255, blue*255).rgb());
}

inline double iCubic(double y0,double y1,
                     double y2,double y3,
                     double dx)
{
    double dx2 = dx * dx;
    double a0 = y3 - y2 - y0 + y1;
    double a1 = y0 - y1 - a0;
    double a2 = y2 - y0;
    double a3 = y1;

    double result = (a0*dx*dx2+a1*dx2+a2*dx+a3);
    if(result < 0) return 0.0;
    if(result > 1) return 1.0;
    return result;
}

QRgb ImgInterpolator::iBicubic(QPointF position)
{
    if(img == NULL){
        QMessageBox::information(0, "Bacha!", "Pouzivanie neinicializovaneho obrazku ImgInterpolator!");
        exit(EXIT_FAILURE);
    }

    QColor c1, c2, c3, c4;
    QColor t1, t2, t3, t4;

    float fx = floor(position.x());
    float fy = floor(position.y());
    float lx = position.x() - fx;
    float ly = position.y() - fy;

    int indexX[4] = {fx-1,fx,fx+1,fx+2};
    int indexY[4] = {fy-1,fy,fy+1,fy+2};

    for(int i = 0; i < 4; i++){
        if(indexX[i] < 0) indexX[i] = 0;
        if(indexY[i] < 0) indexY[i] = 0;
        if(indexX[i] >= size.width()) indexX[i] = size.width()-1;
        if(indexY[i] >= size.height()) indexY[i] = size.height()-1;
    }

    c1 = img->pixel(QPoint(indexX[0],indexY[0]));
    c2 = img->pixel(QPoint(indexX[1],indexY[0]));
    c3 = img->pixel(QPoint(indexX[2],indexY[0]));
    c4 = img->pixel(QPoint(indexX[3],indexY[0]));
    t1.setRedF(iCubic(c1.redF(),c2.redF(),c3.redF(),c4.redF(),lx));
    t1.setGreenF(iCubic(c1.greenF(),c2.greenF(),c3.greenF(),c4.greenF(),lx));
    t1.setBlueF(iCubic(c1.blueF(),c2.blueF(),c3.blueF(),c4.blueF(),lx));

    c1 = img->pixel(QPoint(indexX[0],indexY[1]));
    c2 = img->pixel(QPoint(indexX[1],indexY[1]));
    c3 = img->pixel(QPoint(indexX[2],indexY[1]));
    c4 = img->pixel(QPoint(indexX[3],indexY[1]));
    t2.setRedF(iCubic(c1.redF(),c2.redF(),c3.redF(),c4.redF(),lx));
    t2.setGreenF(iCubic(c1.greenF(),c2.greenF(),c3.greenF(),c4.greenF(),lx));
    t2.setBlueF(iCubic(c1.blueF(),c2.blueF(),c3.blueF(),c4.blueF(),lx));

    c1 = img->pixel(QPoint(indexX[0],indexY[2]));
    c2 = img->pixel(QPoint(indexX[1],indexY[2]));
    c3 = img->pixel(QPoint(indexX[2],indexY[2]));
    c4 = img->pixel(QPoint(indexX[3],indexY[2]));
    t3.setRedF(iCubic(c1.redF(),c2.redF(),c3.redF(),c4.redF(),lx));
    t3.setGreenF(iCubic(c1.greenF(),c2.greenF(),c3.greenF(),c4.greenF(),lx));
    t3.setBlueF(iCubic(c1.blueF(),c2.blueF(),c3.blueF(),c4.blueF(),lx));

    c1 = img->pixel(QPoint(indexX[0],indexY[3]));
    c2 = img->pixel(QPoint(indexX[1],indexY[3]));
    c3 = img->pixel(QPoint(indexX[2],indexY[3]));
    c4 = img->pixel(QPoint(indexX[3],indexY[3]));
    t4.setRedF(iCubic(c1.redF(),c2.redF(),c3.redF(),c4.redF(),lx));
    t4.setGreenF(iCubic(c1.greenF(),c2.greenF(),c3.greenF(),c4.greenF(),lx));
    t4.setBlueF(iCubic(c1.blueF(),c2.blueF(),c3.blueF(),c4.blueF(),lx));

    QColor result;
    result.setRedF(iCubic(t1.redF(),t2.redF(),t3.redF(),t4.redF(),ly));
    result.setGreenF(iCubic(t1.greenF(),t2.greenF(),t3.greenF(),t4.greenF(),ly));
    result.setBlueF(iCubic(t1.blueF(),t2.blueF(),t3.blueF(),t4.blueF(),ly));
    return result.rgb();
}

void ImgInterpolator::loadNewImg(const QString filename)
{
    delete img;
    img = new QImage();
    if(!img->load(filename)){
        QMessageBox::information(0, "Bacha!", "Unsupported image format!");
        exit(EXIT_FAILURE);
    }
    size = img->size();
}
