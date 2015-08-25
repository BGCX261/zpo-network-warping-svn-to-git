/** Projekt: Warp
*   Autor:   Miroslav Schery
*            Zsolt Horváth
*
*   Posledna uprava: 6.5.2011
*/

#include <QtGui>

#include "warpWindow.h"

WarpWindow::WarpWindow(QSize gridSize, QSize size, QWidget *parent)
    : QWidget(parent)
{
    antialiased = false;
    transformed = false;

    draw = false;
    recalcAll = true;

    showSplines = true;
    showCtrlPtsGrid = false;

    widgetSize = size;
    // setMaximumSize(widgetSize);

    QPixmap *pixmap = new QPixmap(size.width(), size.height());
    pixmap->fill(Qt::white); // default fill = Qt::White
    warpedImage = new ImgInterpolator(pixmap);
    delete pixmap;

    pixmap = new QPixmap(size.width(), size.height());
    pixmap->fill(Qt::white); // default fill = Qt::White
    srcImage = new ImgInterpolator(pixmap);
    delete pixmap;

    grid = new WarpGrid(gridSize.width(), gridSize.height(), size);

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    this->brush = new QBrush(Qt::blue);
    this->pen = new QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
}

/*QSize WarpWidget::minimumSizeHint() const
{
    return widgetSize;
}*/

void WarpWindow::setSplinesVisible(bool visible)
{
    showSplines = visible;
}

QSize WarpWindow::sizeHint() const
{
    return widgetSize;
}

// PUBLIC SLOTS >>>>>--------------------------------------
void WarpWindow::setPen(const QPen &pen){
    *this->pen = pen;
    update();
}

void WarpWindow::setBrush(const QBrush &brush){
    *this->brush = brush;
    update();
}

void WarpWindow::setAntialiased(bool antialiased){
    this->antialiased = antialiased;
    update();
}

void WarpWindow::setTransformed(bool transformed){
    this->transformed = transformed;
    update();
}

void WarpWindow::setBSApproxPrecision(int precision){
    grid->setApproxPrecision(precision);
    recalcAll = true;
    update();
}

void WarpWindow::setBSPolynomDegree(int degree){
    grid->setPolynomDegree(degree);
    recalcAll = true;
    update();
}

void WarpWindow::showCPGrid(bool show){
    showCtrlPtsGrid = show;
    update();
}

void WarpWindow::showBSplines(bool show){
    showSplines = show;
    update();
}
// PUBLIC SLOTS <<<<<--------------------------------------


void WarpWindow::save(const QString filename){
    warpedImage->img->save(filename);
}

int WarpWindow::getGridPolyDegree(){
    return grid->getPolynomDegree();
}

int WarpWindow::getGridApproxPrecision(){
    return grid->getApproxPrecision();
}

QSize WarpWindow::getGridSize(){
    return grid->getGridSize();
}

QSize WarpWindow::getGridResolution(){
    return grid->getGridResolution();
}

QVector3D WarpWindow::getCommonCtrlPnt(QPoint position){
    return grid->getCommonCtrlPnt(position);
}

void WarpWindow::setCommonCtrlPnt(QPoint ctrlPoint, QPoint newPosition){
    if(grid->ctrlPtsAvailable)grid->setCommonCtrlPnt(ctrlPoint, newPosition);
    recalcAll = true;
}

void WarpWindow::setCommonCtrlPnt(QPoint ctrlPoint, QVector3D newPosition){
    if(grid->ctrlPtsAvailable)grid->setCommonCtrlPnt(ctrlPoint, newPosition);
    recalcAll = true;
}

QSize WarpWindow::setGridSize(QSize newSize){
    newSize = grid->setGridSize(newSize);
    recalcAll = true;
    return newSize;
}

void WarpWindow::loadSrcImage(const QString filename)
{
    draw = false;
    srcImage->loadNewImg(filename);
    widgetSize = srcImage->size;
    setMaximumSize(widgetSize);
    setMinimumSize(widgetSize);

    warpedImage->createNewImg(widgetSize);

    grid->resetGrid(widgetSize.width(), widgetSize.height());

    draw = true;
    recalcAll = true;
    update();
}

void WarpWindow::setInterpMethod(int method){
    recalcAll = true;
    switch (method)
    {
    case 0:
        intMethod = ImgInterpolator::NEAREST;
        update();
        break;
    case 1:
        intMethod = ImgInterpolator::BILINEAR;
        update();
        break;
    case 2:
        intMethod = ImgInterpolator::BICUBIC;
        update();
        break;
    default:
        intMethod = ImgInterpolator::NEAREST;
        update();
        break;
    }
}

void WarpWindow::transformSrcImg()
{
    if(grid->mappingAvailable){
        draw = false;
        for(int i = 0; i < grid->getGridResolution().height(); i++){
            for(int j = 0; j < grid->getGridResolution().width(); j++){
                switch (intMethod)
                {
                case ImgInterpolator::NEAREST:
                    warpedImage->img->setPixel(QPoint(j, i), srcImage->iClamp(grid->pixelMap[i][j]));
                    break;
                case ImgInterpolator::BILINEAR:
                    warpedImage->img->setPixel(QPoint(j, i), srcImage->iBilinear(grid->pixelMap[i][j]));
                    break;
                case ImgInterpolator::BICUBIC:
                    warpedImage->img->setPixel(QPoint(j, i), srcImage->iBicubic(grid->pixelMap[i][j]));
                    break;
                default:
                    warpedImage->img->setPixel(QPoint(j, i), srcImage->iClamp(grid->pixelMap[i][j]));
                    break;
                }
            }
        }
        draw = true;
    }else{
        QMessageBox::information(0, "Bacha!", "Calling WarpWidget::transformSrcImg but mapping haven't been done!");
    }
}

void WarpWindow::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.setPen(*pen);
    painter.setBrush(*brush);
    if (antialiased)
        painter.setRenderHint(QPainter::Antialiasing, true);

    if(draw){
        /////////////////////////////////////////////////////////////////
        // IMAGE
        if(recalcAll){
            if(grid->ctrlPtsAvailable){
                grid->computeSplines();
            }
            if(grid->splinesAvailable){
                grid->mapWholeImg();
            }
            if(grid->mappingAvailable){
                transformSrcImg();
            }

            recalcAll = false;
        }

        painter.drawImage(QPoint(0, 0), *warpedImage->img);
        // --------------------------------------------------------------

        /////////////////////////////////////////////////////////////////
        // SPLINE-Y

        if(grid->splinesAvailable && showSplines){
            this->pen->setWidth(2);
            this->pen->setColor(QColor(Qt::black));
            painter.setPen(*this->pen);

            QPointF *splinePoints;
            int splineLength;

            for (int r = 0; r <= grid->getGridSize().height(); r++){
                splinePoints = grid->horizontalSplines[r]->calcIntervalArray();
                splineLength = grid->horizontalSplines[r]->getIArrLength();
                for (int i = 0; i < splineLength - 1; i++){
                    painter.drawLine(splinePoints[i], splinePoints[i+1]);
                }
            }
            for (int c = 0; c <= grid->getGridSize().width(); c++){
                splinePoints = grid->verticalSplines[c]->calcIntervalArray();
                splineLength = grid->verticalSplines[c]->getIArrLength();
                for (int i = 0; i < splineLength - 1; i++){
                    painter.drawLine(splinePoints[i], splinePoints[i+1]);
                }
            }
        }
        // SPLINE-Y
        // --------------------------------------------------------------

        /////////////////////////////////////////////////////////////////
        // CONTROL GRID

        if(grid->ctrlPtsAvailable && showCtrlPtsGrid){
            this->pen->setWidth(1);
            this->pen->setColor(QColor(Qt::blue));
            painter.setPen(*this->pen);

            for (int r = 0; r < grid->getGridSize().height(); r++){
                for (int c = 0; c < grid->getGridSize().width(); c++){
                    painter.drawLine(grid->getCommonCtrlPnt(QPoint(c, r)).toPointF(),
                                     grid->getCommonCtrlPnt(QPoint(c + 1, r)).toPointF());
                    painter.drawLine(grid->getCommonCtrlPnt(QPoint(c, r)).toPointF(),
                                     grid->getCommonCtrlPnt(QPoint(c, r + 1)).toPointF());
                }
            }
        }
        // CONTROL GRID
        // --------------------------------------------------------------
    }
}
