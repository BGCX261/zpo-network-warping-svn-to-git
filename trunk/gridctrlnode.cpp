/** Projekt: Warp
*   Autor:   Miroslav Schery
*            Zsolt Horváth
*
*   Posledna uprava: 6.5.2011
*/

#include "gridctrlnode.h"

GridCtrlNode::GridCtrlNode(QPoint locationInGrid, WarpWidget *warpWidget)
    : wWidget(warpWidget),
      gridPtLocation(locationInGrid)
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(1);
    setPos(wWidget->getWarpWindow()->getCommonCtrlPnt(gridPtLocation).toPointF());
}

void GridCtrlNode::advance(int phase)
{
    if(phase){
        setPos(wWidget->getWarpWindow()->getCommonCtrlPnt(gridPtLocation).toPointF());
    }
}

QRectF GridCtrlNode::boundingRect() const
{
    qreal adjust = 2;

    return QRectF( -5 - adjust, -5 - adjust,
                  10 + adjust, 10 + adjust);
}

QPainterPath GridCtrlNode::shape() const
{
    QPainterPath path;

    path.addEllipse(-5, -5, 10, 10);

    return path;
}

void GridCtrlNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    painter->setPen(Qt::NoPen);
    //painter->setBrush(Qt::darkGray);
    //painter->drawEllipse(-7, -7, 20, 20);

    if (option->state & QStyle::State_Sunken) {
        painter->setBrush(Qt::blue);
    } else {
        painter->setBrush(Qt::green);
    }

    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(-5, -5, 10, 10);
}

QVariant GridCtrlNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case ItemPositionHasChanged:
        if (scene()) {
            // value is the new position.
            QPointF newPos = value.toPointF();
            QRectF rect = scene()->sceneRect();

            if(gridPtLocation.x() == 0){
                newPos.setX(0);
            }
            if(gridPtLocation.y() == 0){
                newPos.setY(0);
            }
            if(gridPtLocation.x() == wWidget->getWarpWindow()->getGridSize().width()){
                newPos.setX(wWidget->getWarpWindow()->sizeHint().width()-1);
            }
            if(gridPtLocation.y() == wWidget->getWarpWindow()->getGridSize().height()){
                newPos.setY(wWidget->getWarpWindow()->sizeHint().height()-1);
            }

            if (!rect.contains(newPos)) {
                // Keep the item inside the scene rect.
                newPos.setX(qMin(rect.right()-1, qMax(newPos.x(), rect.left())));
                newPos.setY(qMin(rect.bottom()-1, qMax(newPos.y(), rect.top())));
                this->setPos(newPos);
                return newPos;
            }

            this->setPos(newPos);
            wWidget->getWarpWindow()->setCommonCtrlPnt(gridPtLocation, QVector3D(newPos));
            wWidget->getWarpWindow()->update();
        }
        return QGraphicsItem::itemChange(change, value);
        break;
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}

void GridCtrlNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mousePressEvent(event);
}

void GridCtrlNode::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}


