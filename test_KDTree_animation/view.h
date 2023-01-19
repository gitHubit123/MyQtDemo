#ifndef VIEW_H
#define VIEW_H

#include <QtWidgets>


class View : public QGraphicsView
{
    Q_OBJECT
public:
    View(QWidget *parent = nullptr);

signals:
    void mouseMove(const QPointF& p);

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // VIEW_H
