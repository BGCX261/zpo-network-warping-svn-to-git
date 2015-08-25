/** Projekt: Warp
*   Autor:   Miroslav Schery
*            Zsolt Horváth
*
*   Posledna uprava: 6.5.2011
*/


#ifndef GRIDCTRLNODE_H
#define GRIDCTRLNODE_H

#include <QGraphicsItem>
#include "warpwidget.h"

class GridCtrlNode : public QGraphicsItem
{
public:
    GridCtrlNode(QPoint locationInGrid, WarpWidget *warpWidget);

    void advance(int phase);

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

public slots:

private:
    QPointF newPos;
    WarpWidget *wWidget;

    QPoint gridPtLocation;
    QSize maxIndex;
};

#endif // GRIDCTRLNODE_H
