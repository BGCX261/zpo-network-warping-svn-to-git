/** Projekt: Warp
*   Autor:   Miroslav Schery
*            Zsolt Horváth
*
*   Posledna uprava: 6.5.2011
*/

#ifndef WARPGRID_H
#define WARPGRID_H

#include "bspline.h"
#include <QtGui>

class WarpGrid
{

public:
    WarpGrid(int rowNum, int colNum, QSize imgSize);

    ~WarpGrid();

public:
    int rowsNum;
    int columnsNum;
    int dimensionX;
    int dimensionY;

    int polynomDegree;
    int approxPrecision;

    QVector3D **rowCtrlPts;
    QVector3D **colCtrlPts;
    QPointF **bSplRowLines;
    QPointF **bSplColLines;

    QPointF **pixelMap;

    BSpline **verticalSplines;
    BSpline **horizontalSplines;

    bool ctrlPtsAvailable;
    bool splinesAvailable;
    bool mappingAvailable;

    // FUNKCIE >>>>> //////////////////////////////////////
    QSize setGridSize(QSize newGridSize);

    void initGrid();
    void initCtrlPts();
    void initSplines();
    void initPixelMap(bool defaultFill = true);

    void deletePixelMap();
    void deleteGrid();
    void deleteSplines();

    int resetGrid(int imgSizeX, int imgSizeY);

    void setRowCtrlPoint(QPoint ctrlPoint, QPoint newPosition);
    void setRowCtrlPoint(QPoint ctrlPoint, QVector3D newPosition);
    QVector3D getRowCtrlPoint(QPoint position);

    void setColCtrlPoint(QPoint ctrlPoint, QPoint newPosition);
    void setColCtrlPoint(QPoint ctrlPoint, QVector3D newPosition);
    QVector3D getColCtrlPoint(QPoint position);

    void setCommonCtrlPnt(QPoint ctrlPoint, QPoint newPosition);
    void setCommonCtrlPnt(QPoint ctrlPoint, QVector3D newPosition);

    QVector3D getCommonCtrlPnt(QPoint position);

    QSize getGridSize();
    QSize getGridResolution();

    void initRowLines(int resolution, int precision = DEF_APPROX_PRECISION);
    void initColLines(int resolution, int precision = DEF_APPROX_PRECISION);
    void initSplines(int resolution, int precision = DEF_APPROX_PRECISION);

    void computeRowLines();
    void computeColLines();
    void computeSplines();

    void resetRowLines();
    void resetColLines();
    void resetSplines();

    QPointF getRowSplineAt(int rowIndex, int x);
    QPointF getColSplineAt(int y, int colIndex);

    void mapImgCol(int column);
    void mapImgRow(int row);
    void mapWholeImg();

    void setPolynomDegree(int degree);
    void setApproxPrecision(int precision);
    int getPolynomDegree();
    int getApproxPrecision();
};

#endif // WARPGRID_H
