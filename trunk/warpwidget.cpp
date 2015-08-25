/** Projekt: Warp
*   Autor:   Miroslav Schery
*            Zsolt Horváth
*
*   Posledna uprava: 6.5.2011
*/

#include <QtGui>

#include "warpwidget.h"
#include "gridctrlnode.h"

WarpWidget::WarpWidget(QSize gridSize, QSize size, QWidget *parent)
    : QGraphicsView(parent)
{
    showCtrlPoints = true;

    // load / create new image
    wWindow = new WarpWindow(gridSize, size);
    wWindowSize = wWindow->sizeHint();

    // create scene with size of WarpWindow
    scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(0, 0, wWindowSize.width(), wWindowSize.height());
    setScene(scene);

    // add warp window to the scene
    scene->addWidget(wWindow);

    // set up some params
    // setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    //scale(qreal(0.8), qreal(0.8));
    currentScale = QSizeF(1.0, 1.0);

    // setMaximumSize(wWindowSize.width()+20, wWindowSize.height()+20);

    // create nodes for spline control pts
    GridCtrlNode *node;

    for(int i = 0; i < wWindow->getGridSize().height() + 1; i++){
        for(int j = 0; j < wWindow->getGridSize().width() + 1; j++){
            node = new GridCtrlNode(QPoint(j, i), this);
            scene->addItem(node);
            items.append(node);
        }
    }
}

void WarpWidget::setScale(int newScale){
    QSizeF scaleReset;
    scaleReset.setWidth((1.0 / currentScale.width()) * qreal(newScale/100.0));
    scaleReset.setHeight((1.0 / currentScale.height()) * qreal(newScale/100.0));

    scale(scaleReset.width(), scaleReset.height());
    currentScale = QSizeF(newScale/100.0, newScale/100.0);
}

void WarpWidget::clearScene()
{
    GridCtrlNode *node;
    while (!items.isEmpty()){
        node = (GridCtrlNode *) items.takeFirst();
        scene->removeItem(node);
        delete node;
    }
}

void WarpWidget::initScene()
{
    GridCtrlNode *node;
    for(int i = 0; i < wWindow->getGridSize().height() + 1; i++){
        for(int j = 0; j < wWindow->getGridSize().width() + 1; j++){
            node = new GridCtrlNode(QPoint(j, i), this);
            items.append(node);
            scene->addItem(node);
            if(!showCtrlPoints) node->hide();
        }
    }
}

void WarpWidget::showCtrlPts(bool show)
{
    for (int i = 0; i < items.size(); ++i) {
        show ? items.at(i)->show() : items.at(i)->hide();
    }
    showCtrlPoints = show;
}

void WarpWidget::save(const QString filename)
{
    wWindow->save(filename);
}

void WarpWidget::loadSrcImage(const QString fileName)
{
    wWindow->loadSrcImage(fileName);
    wWindowSize = wWindow->sizeHint();
    scene->setSceneRect(0, 0, wWindowSize.width(), wWindowSize.height());

    //setMaximumSize(wWindowSize.width()+20, wWindowSize.height()+20);

    scene->advance();
}

WarpWindow *WarpWidget::getWarpWindow()
{
    return wWindow;
}
