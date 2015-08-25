/** Projekt: Warp
*   Autor:   Miroslav Schery
*            Zsolt Horváth
*
*   Posledna uprava: 6.5.2011
*/

#ifndef WARPWIDGET_H
#define WARPWIDGET_H

#include "warpwindow.h"

#include <QtGui>

class WarpWidget : public QGraphicsView
{
    Q_OBJECT

public slots:

    void showCtrlPts(bool show);
    void setScale(int newScale);

public:
    WarpWidget(QSize gridSize, QSize size, QWidget *parent = 0);

    void save(const QString filename);

    void loadSrcImage(const QString fileName);

    WarpWindow *getWarpWindow();

    void clearScene();
    void initScene();

    WarpWindow *wWindow;

private:
    QSizeF currentScale;

    QGraphicsScene *scene;
    QSize wWindowSize;

    QList<QGraphicsItem *> items;

    bool showCtrlPoints;
};

#endif // WARPWIDGET_H
