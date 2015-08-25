/** Projekt: Warp
*   Autor:   Miroslav Schery
*            Zsolt Horváth
*
*   Posledna uprava: 6.5.2011
*/

#ifndef WARPWINDOW_H
#define WARPWINDOW_H

#include "bspline.h"
#include "warpgrid.h"
#include "imginterpolator.h"

#include <QBrush>
#include <QPen>

#include <QWidget>


class WarpWindow : public QWidget
{
    Q_OBJECT

public:
    WarpWindow(QSize gridSize, QSize size, QWidget *parent = 0);

    //QSize minimumSizeHint() const;
    QSize sizeHint() const;

    QSize getGridSize();
    QSize getGridResolution();

    QVector3D getCommonCtrlPnt(QPoint position);
    void setCommonCtrlPnt(QPoint ctrlPoint, QPoint newPosition);
    void setCommonCtrlPnt(QPoint ctrlPoint, QVector3D newPosition);

    void transformSrcImg();

    void save(const QString filename);

    void loadSrcImage(const QString filename);

    QSize setGridSize(QSize newSize);

    int getGridPolyDegree();
    int getGridApproxPrecision();

protected:
    void paintEvent(QPaintEvent *event);

public slots:
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setAntialiased(bool antialiased);
    void setTransformed(bool transformed);

    void showCPGrid(bool show);
    void showBSplines(bool show);
    void setBSApproxPrecision(int precision);
    void setBSPolynomDegree(int degree);
    void setSplinesVisible(bool visible);

    void setInterpMethod(int method);

private:
    ImgInterpolator::iMethod intMethod;

    WarpGrid *grid;

    QPen *pen;
    QBrush *brush;
    bool antialiased;
    bool transformed;

    bool showSplines;
    bool showCtrlPtsGrid;

    bool recalcAll;

    ImgInterpolator *srcImage;
    ImgInterpolator *warpedImage;

    QSize widgetSize;

    BSpline *spline;

    bool draw;
};

#endif // WARPWINDOW_H
