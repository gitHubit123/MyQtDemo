#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QMainWindow>
#include "kdtree.h"
#include "view.h"
#include "scene.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void initScene();
    void resetHighlightedLines();

    void searchNearestPoint(const QPointF &p);
public slots:
    void onMouseMove(const QPointF &p);
private:
    Ui::MainWindow *ui;
    KDTree* m_KDTree = Q_NULLPTR;
    QPointF* m_searchPoint = Q_NULLPTR;
    QGraphicsPathItem* m_nearestPoint;

    View* m_view = Q_NULLPTR;
    Scene* m_scene = Q_NULLPTR;
    QGraphicsEllipseItem* m_searchPointItem = Q_NULLPTR;
    QGraphicsEllipseItem* m_circle = Q_NULLPTR;
    QGraphicsLineItem* m_circleRadius = Q_NULLPTR;


    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
private slots:
    void on_pushButton_createPoints_clicked();
    void on_pushButton_createKDTree_clicked();
    void on_verticalSlider_sleepMscs_valueChanged(int value);
    void on_pushButton_clearScene_clicked();
};



#endif // MAINWINDOW_H
