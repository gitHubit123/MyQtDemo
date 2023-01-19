#include "view.h"

View::View(QWidget *parent) : QGraphicsView(parent)
{
//    setMouseTracking(true);
}


void View::mousePressEvent(QMouseEvent *event)
{
    qDebug() << __PRETTY_FUNCTION__;
    return QGraphicsView::mousePressEvent(event);
}

void View::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << __PRETTY_FUNCTION__;
    return QGraphicsView::mouseReleaseEvent(event);
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMove(mapToScene(event->pos()));
//    qDebug() << __PRETTY_FUNCTION__;
    return QGraphicsView::mouseMoveEvent(event);
}
