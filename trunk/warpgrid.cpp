/** Projekt: Warp
*   Autor:   Miroslav Schery
*            Zsolt Horváth
*
*   Posledna uprava: 19.5.2011
*/

#include <QtGui>

#include "warpgrid.h"
#include <math.h>

WarpGrid::WarpGrid(int rowNum, int colNum, QSize imgSize)
{
    rowsNum = rowNum;
    columnsNum = colNum;
    dimensionX = imgSize.width();
    dimensionY = imgSize.height();
    //splPrecision = precision;

    polynomDegree = DEF_POLY_DEGREE;
    approxPrecision = DEF_APPROX_PRECISION;


    rowCtrlPts = NULL;
    colCtrlPts = NULL;
    bSplRowLines = NULL;
    bSplColLines = NULL;

    pixelMap = NULL;

    verticalSplines = NULL;
    horizontalSplines = NULL;

    ctrlPtsAvailable = false;
    splinesAvailable = false;
    mappingAvailable = false;

    initGrid();
    initSplines();
    initPixelMap();
    initCtrlPts();
}

QSize WarpGrid::setGridSize(QSize newGridSize)
{
    std::string result2;
    char result[512] = {'\0'};
    sprintf(result, "Minimum width/height allowed for polynome of degree %d is %d!\n", polynomDegree, polynomDegree - 1);
    result2.append(result);

    if(newGridSize.width() + 2 <= polynomDegree){
        QMessageBox::information(0, "Warning", result2.c_str());
        newGridSize.setWidth(polynomDegree - 1);
    }

    if(newGridSize.height() + 2 <= polynomDegree){
        QMessageBox::information(0, "Warning", result2.c_str());
        newGridSize.setHeight(polynomDegree - 1);
    }

//    if(newGridSize.width() == rowsNum && newGridSize.height() == columnsNum){
//        // velkost mriezky nemenim, je rovnaka
//        return newGridSize;
//    }

    deleteGrid();
    deleteSplines();
    deletePixelMap();

    rowsNum = newGridSize.height();
    columnsNum = newGridSize.width();

    initGrid();
    initSplines();
    initPixelMap();
    initCtrlPts();

    return newGridSize;
}

int WarpGrid::getPolynomDegree(){
    return polynomDegree;
}

int WarpGrid::getApproxPrecision(){
    return approxPrecision;
}

//-------------------------------------------------------------------
void WarpGrid::initGrid()
{// dorob testy inicializovania

    // kontrolne body riadkov
    rowCtrlPts = new QVector3D *[rowsNum + 1];
    for(int i = 0; i <= rowsNum; i++){
        rowCtrlPts[i] = new QVector3D[columnsNum + 1];
    }

    // kontrolne body stlpcov
    colCtrlPts = new QVector3D *[columnsNum + 1];
    for(int i = 0; i <= columnsNum; i++){
        colCtrlPts[i] = new QVector3D[rowsNum + 1];
    }
}

//-------------------------------------------------------------------
void WarpGrid::initSplines()
{// dorob testy inicializovania
    verticalSplines = new BSpline *[columnsNum + 1];
    for(int i = 0; i < columnsNum + 1; i++){
        verticalSplines[i] = new BSpline(rowsNum + 1,
                                         dimensionY,
                                         polynomDegree,
                                         approxPrecision);
    }

    horizontalSplines = new BSpline *[rowsNum + 1];
    for(int i = 0; i < rowsNum + 1; i++){
        horizontalSplines[i] = new BSpline(columnsNum + 1,
                                           dimensionX,
                                           polynomDegree,
                                           approxPrecision);
    }

    bSplRowLines = new QPointF *[rowsNum + 1];

    bSplColLines = new QPointF *[columnsNum + 1];
}


void WarpGrid::initCtrlPts()
{
    float dRow = (float) dimensionY / rowsNum;
    float dCol = (float) dimensionX / columnsNum;
    for(int i = 0; i < rowsNum; i++){
        for(int j = 0; j < columnsNum; j++){
            QPoint ctrlPoint(j * dCol, i * dRow);
            setCommonCtrlPnt(QPoint(j, i), ctrlPoint);
        }
    }
    for(int i = 0; i < rowsNum; i++){
        QPoint ctrlPoint(dimensionX - 1, i * dRow);
        setCommonCtrlPnt(QPoint(columnsNum, i), ctrlPoint);
    }
    for(int i = 0; i < columnsNum; i++){
        QPoint ctrlPoint(i * dCol, dimensionY - 1);
        setCommonCtrlPnt(QPoint(i, rowsNum), ctrlPoint);
    }
    QPoint ctrlPoint(dimensionX - 1, dimensionY - 1);
    setCommonCtrlPnt(QPoint(columnsNum, rowsNum), ctrlPoint);

    resetSplines();

    ctrlPtsAvailable = true;
}

//-------------------------------------------------------------------
void WarpGrid::initPixelMap(bool defaultFill)
{
    pixelMap = new QPointF *[dimensionY];
    for(int i = 0; i < dimensionY; i++){
        pixelMap[i] = new QPointF[dimensionX];
        if(defaultFill){
            for(int j = 0; j < dimensionX; j++){
                pixelMap[i][j] = QPointF(i, j);
            }
        }
    }
}

void WarpGrid::setPolynomDegree(int degree)
{
    for(int i = 0; i <= rowsNum; i++){
        horizontalSplines[i]->setPolynomDegree(degree);
    }
    for(int i = 0; i <= columnsNum; i++){
        verticalSplines[i]->setPolynomDegree(degree);
    }
    polynomDegree = degree;
}

void WarpGrid::setApproxPrecision(int precision)
{
    for(int i = 0; i <= rowsNum; i++){
        horizontalSplines[i]->setApproxPrecision(precision);
    }
    for(int i = 0; i <= columnsNum; i++){
        verticalSplines[i]->setApproxPrecision(precision);
    }
    approxPrecision = precision;
}

//-------------------------------------------------------------------
int WarpGrid::resetGrid(int imgSizeX, int imgSizeY)
{// dorob testy inicializovania

    deleteGrid();
    deleteSplines();
    deletePixelMap();

    dimensionX = imgSizeX;
    dimensionY = imgSizeY;

    initGrid();
    initSplines();
    initPixelMap();


    for(int i = 0; i <= rowsNum; i++){
        horizontalSplines[i]->setApproxResolution(dimensionX);
    }
    for(int i = 0; i <= columnsNum; i++){
        verticalSplines[i]->setApproxResolution(dimensionY);
    }

    initCtrlPts();

    return(EXIT_SUCCESS);
}

// opravit krajne body
void WarpGrid::mapImgCol(int column)
{
    float colWidth = (float)(dimensionX-1) / columnsNum;// original obrazok
    float leftEdge = column * colWidth;// original obrazok

    for(int i = 0; i < dimensionY; i++){
        int leftBorder = round(getColSplineAt(i, column).x());
        int rightBorder = round(getColSplineAt(i, column + 1).x());
        float increment = colWidth / (rightBorder - leftBorder);
        float interval = 0;

        for(int j = leftBorder; j <= rightBorder; j++){
            if(i>=dimensionY || j >=dimensionX) QMessageBox::information(0, "Test", "mimo pola - nad");
            if(i<0 || j<0)QMessageBox::information(0, "Test", "mimo pola - pod");

            pixelMap[i][j].setX(leftEdge + interval);
            interval += increment;
        }
    }
}

void WarpGrid::mapImgRow(int row)
{
    float rowHeight = (float)(dimensionY-1) / rowsNum;// original obrazok
    float topEdge = row * rowHeight;// original obrazok

    for(int i = 0; i < dimensionX; i++){
        int topBorder = round(getRowSplineAt(i, row).y());
        int bottomBorder = round(getRowSplineAt(i, row + 1).y());
        float increment = rowHeight / (bottomBorder - topBorder);
        float interval = 0;
        for(int j = topBorder; j <= bottomBorder; j++){
            if(i>=dimensionX || j >=dimensionY)QMessageBox::information(0, "Test2", "mimo pola - nad");
            if(i<0 || j<0)QMessageBox::information(0, "Test2", "mimo pola - pod");

            pixelMap[j][i].setY(topEdge + interval);
            interval += increment;
        }
    }
}

void WarpGrid::mapWholeImg()
{
    if(!splinesAvailable){
        QMessageBox::information(0, "Warning!", "WarpGrid::mapWholeImg() pole pre splinesAvailable nebolo inicializovane, memozem pokracovat!");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < rowsNum; i++){
        mapImgRow(i);
    }

    for(int i = 0; i < columnsNum; i++){
        mapImgCol(i);
    }

    mappingAvailable = true;
}

QSize WarpGrid::getGridSize()
{
    return QSize(columnsNum, rowsNum);
}

QSize WarpGrid::getGridResolution()
{
    return QSize(dimensionX, dimensionY);
}

//-------------------------------------------------------------------
void WarpGrid::setRowCtrlPoint(QPoint ctrlPoint, QPoint newPosition)
{
    if(ctrlPoint.x() > columnsNum || ctrlPoint.y() > rowsNum){
        QMessageBox::information(0, "Warning!", "WarpGrid::setRowCtrlPoint() zapisovanie mimo pola!");
    }else{
        rowCtrlPts[ctrlPoint.y()][ctrlPoint.x()] = QVector3D(newPosition);
        horizontalSplines[ctrlPoint.y()]->setCtrlPoint(QVector3D(newPosition), ctrlPoint.x());
    }
}

//-------------------------------------------------------------------
void WarpGrid::setRowCtrlPoint(QPoint ctrlPoint, QVector3D newPosition)
{
    if(ctrlPoint.x() > columnsNum || ctrlPoint.y() > rowsNum){
        QMessageBox::information(0, "Warning!", "WarpGrid::setRowCtrlPoint() zapisovanie mimo pola!");
    }else{
        rowCtrlPts[ctrlPoint.y()][ctrlPoint.x()] = newPosition;
        horizontalSplines[ctrlPoint.y()]->setCtrlPoint(newPosition, ctrlPoint.x());
    }
}

//-------------------------------------------------------------------
void WarpGrid::setColCtrlPoint(QPoint ctrlPoint, QPoint newPosition)
{
    if(ctrlPoint.x() > columnsNum || ctrlPoint.y() > rowsNum){
        QMessageBox::information(0, "Warning!", "WarpGrid::setColCtrlPoint() zapisovanie mimo pola!");
    }else{
        colCtrlPts[ctrlPoint.x()][ctrlPoint.y()] = QVector3D(newPosition);
        verticalSplines[ctrlPoint.x()]->setCtrlPoint(QVector3D(newPosition), ctrlPoint.y());
    }
}

//-------------------------------------------------------------------
void WarpGrid::setColCtrlPoint(QPoint ctrlPoint, QVector3D newPosition)
{
    if(ctrlPoint.x() > columnsNum || ctrlPoint.y() > rowsNum){
        QMessageBox::information(0, "Warning!", "WarpGrid::setColCtrlPoint() zapisovanie mimo pola!");
    }else{
        colCtrlPts[ctrlPoint.x()][ctrlPoint.y()] = newPosition;
        verticalSplines[ctrlPoint.x()]->setCtrlPoint(newPosition, ctrlPoint.y());
    }
}

void WarpGrid::setCommonCtrlPnt(QPoint ctrlPoint, QPoint newPosition)
{
    setColCtrlPoint(ctrlPoint, newPosition);
    setRowCtrlPoint(ctrlPoint, newPosition);
}

void WarpGrid::setCommonCtrlPnt(QPoint ctrlPoint, QVector3D newPosition)
{
    setColCtrlPoint(ctrlPoint, newPosition);
    setRowCtrlPoint(ctrlPoint, newPosition);
}

// PREROBIT!!!!!!!!!!!!!!!!!!!!!!
QVector3D WarpGrid::getCommonCtrlPnt(QPoint position)
{
    return getRowCtrlPoint(position);
}

//-------------------------------------------------------------------
QVector3D WarpGrid::getRowCtrlPoint(QPoint ctrlPPosition)
{
    if(ctrlPPosition.x() > columnsNum || ctrlPPosition.y() > rowsNum){
        QMessageBox::information(0, "Warning!", "WarpGrid::getRowCtrlPoint() zapisovanie mimo pola!");
        return QVector3D(0.0, 0.0, 0.0);
    }
    if(!ctrlPtsAvailable){
        QMessageBox::information(0, "Warning!", "WarpGrid::getRowCtrlPoint() pole pre kontrolne body nebolo inicializovane!");
        return QVector3D(0.0, 0.0, 0.0);
    }
    return rowCtrlPts[ctrlPPosition.y()][ctrlPPosition.x()];
}

//-------------------------------------------------------------------
QVector3D WarpGrid::getColCtrlPoint(QPoint ctrlPPosition)
{

    if(ctrlPPosition.x() > columnsNum || ctrlPPosition.y() > rowsNum){
        QMessageBox::information(0, "Warning!", "WarpGrid::getColCtrlPoint() zapisovanie mimo pola!");
        return QVector3D(0.0, 0.0, 0.0);
    }
    if(!ctrlPtsAvailable){
        QMessageBox::information(0, "Warning!", "WarpGrid::getColCtrlPoint() pole pre kontrolne body nebolo inicializovane!");
        return QVector3D(0.0, 0.0, 0.0);
    }
    return colCtrlPts[ctrlPPosition.y()][ctrlPPosition.x()];
}

//-------------------------------------------------------------------
void WarpGrid::computeRowLines()
{
    for(int r = 0; r <= rowsNum; r++){
        bSplRowLines[r] = horizontalSplines[r]->calcHorizAprxArray();
    }
}

void WarpGrid::computeColLines()
{
    for(int c = 0; c <= columnsNum; c++){
        bSplColLines[c] = verticalSplines[c]->calcVertAprxArray();
    }
}

void WarpGrid::computeSplines(){
    computeRowLines();
    computeColLines();
    splinesAvailable = true;
}

void WarpGrid::initRowLines(int resolution, int precision){
    for(int r = 0; r <= rowsNum; r++){
        bSplRowLines[r] = horizontalSplines[r]->calcHorizAprxArray(resolution, precision);
    }
}

void WarpGrid::initColLines(int resolution, int precision){
    for(int c = 0; c <= columnsNum; c++){
        bSplColLines[c] = verticalSplines[c]->calcVertAprxArray(resolution, precision);
    }
}

void WarpGrid::initSplines(int resolution, int precision){
    initRowLines(resolution, precision);
    initColLines(resolution, precision);
    splinesAvailable = true;
}

void WarpGrid::resetRowLines(){
    for(int r = 0; r <= rowsNum; r++){
        int precision = horizontalSplines[r]->getApproxPrecision();
        int resolution = horizontalSplines[r]->getApproxResolution();
        bSplRowLines[r] = horizontalSplines[r]->calcHorizAprxArray(resolution, precision);
    }
}

void WarpGrid::resetColLines(){
    for(int c = 0; c <= columnsNum; c++){
        int precision = verticalSplines[c]->getApproxPrecision();
        int resolution = verticalSplines[c]->getApproxResolution();
        bSplColLines[c] = verticalSplines[c]->calcVertAprxArray(resolution, precision);
    }
}

void WarpGrid::resetSplines(){
    resetRowLines();
    resetColLines();
    splinesAvailable = true;
}

//-------------------------------------------------------------------
// kontrola!!!!!!!!
QPointF WarpGrid::getRowSplineAt(int x, int rowIndex)
{

    if(rowIndex > rowsNum || rowIndex < 0 || x >= dimensionX || x < 0){
        QMessageBox::information(0, "Warning!", "WarpGrid::getRowSplineAt() si mimo pola!");
        return QPointF(0.0, 0.0);
    }
    if(!splinesAvailable){
        QMessageBox::information(0, "Warning!", "WarpGrid::getRowSplineAt() pole pre spline-y nebolo inicializovane!");
        return QPointF(0.0, 0.0);
    }
    return horizontalSplines[rowIndex]->getApproxSplineAt(x);
}

//-------------------------------------------------------------------
// kontrola!!!!!!!!
QPointF WarpGrid::getColSplineAt(int y, int colIndex)
{
    if(colIndex > columnsNum || y >= dimensionY){
        QMessageBox::information(0, "Warning!", "WarpGrid::getColSplineAt() si mimo pola!");
        return QPointF(0.0, 0.0);
    }
    if(!splinesAvailable){
        QMessageBox::information(0, "Warning!", "WarpGrid::getColSplineAt() pole pre spline-y nebolo inicializovane!");
        return QPointF(0.0, 0.0);
    }
    return verticalSplines[colIndex]->getApproxSplineAt(y);
}


//-------------------------------------------------------------------
void WarpGrid::deletePixelMap()
{
    mappingAvailable = false;

    if(pixelMap){
        for(int i = 0; i < dimensionY; i++){
            delete[] pixelMap[i];
        }
        delete[] pixelMap;
        pixelMap = NULL;
    }
}

//-------------------------------------------------------------------
void WarpGrid::deleteGrid()
{
    ctrlPtsAvailable = false;

    if(rowCtrlPts){
        for(int i = 0; i <= rowsNum; i++){
            delete[] rowCtrlPts[i];

        }
        delete[] rowCtrlPts;
        rowCtrlPts = NULL;
    }
    if(colCtrlPts){
        for(int i = 0; i <= columnsNum; i++){
            delete[] colCtrlPts[i];
        }
        delete[] colCtrlPts;
        colCtrlPts = NULL;
    }
}

//-------------------------------------------------------------------
void WarpGrid::deleteSplines()
{
    splinesAvailable = false;

    // >> NEW ///////////////////////////////////////////////////
    if(verticalSplines){
        for(int i = 0; i < columnsNum + 1; i++){
            delete verticalSplines[i];
        }
        delete[] verticalSplines;
        verticalSplines = NULL;
    }

    if(horizontalSplines){
        for(int i = 0; i < rowsNum + 1; i++){
            delete horizontalSplines[i];
        }
        delete[] horizontalSplines;
        horizontalSplines = NULL;
    }
    // << NEW ///////////////////////////////////////////////////


    if(bSplRowLines){
        delete[] bSplRowLines;
        bSplRowLines = NULL;
    }

    if(bSplColLines){
        delete[] bSplColLines;
        bSplColLines = NULL;
    }
}

