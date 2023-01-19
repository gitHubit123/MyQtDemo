#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <float.h>
#include <synchapi.h>
#include <queue>

static double random(double low, double heigh)
{
    return ((heigh - low) * (rand() / (double)RAND_MAX)) + low;
}
QList<QPointF> points;
QList<QGraphicsPathItem*> ps;

int k = 1;
//typedef std::pair<double, KDTree*> T; //std::pair<距离, KD树节点>  距离是搜索点到KD树节点的距离
std::priority_queue<T> knn; //K近邻列表 按优先级值从小到大排序 默认用的std::less
//int r = 3;
QList<QColor> cl;
KDTree* bestNode = Q_NULLPTR;
double bestDist = DBL_MAX;
int count = 0;
int count_MAX = 0;
int count_MIN = INT32_MAX;
int count_AVG = 0;
int pointCreateCount = 1000;
int sleepMScs = 0;
QPixmap *pixmap = Q_NULLPTR;
QList<QGraphicsLineItem*> ll; //搜索过程高亮的线段

void MySleep(int mscs)
{
    if (mscs > 0) {
        Sleep(mscs);
        qApp->processEvents();
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cl << QColor(222, 0, 0) << QColor(0, 222, 0) << QColor(0, 0, 222)
       << QColor(222, 222, 0) << QColor(222, 0, 222) << QColor(0, 222, 222);

    m_scene = new Scene(this);
    ui->centralWidget->setScene(m_scene);
    initScene();
    connect(ui->centralWidget, SIGNAL(mouseMove(QPointF)), this, SLOT(onMouseMove(QPointF)));
    ui->textBrowser->setText("KD树 数量、点半径大小建议\n"
                             "11 9\n"
                             "111 7\n"
                             "1111 5\n");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initScene()
{
    m_searchPointItem = new QGraphicsEllipseItem;m_scene->addItem(m_searchPointItem);
    m_circle = new QGraphicsEllipseItem;m_scene->addItem(m_circle);m_circle->setPen(QPen(QColor(0, 0, 255), 2));
    m_circleRadius = new QGraphicsLineItem;m_scene->addItem(m_circleRadius);m_circleRadius->setPen(QPen(QColor(0, 0, 255), 2));

}

void MainWindow::resetHighlightedLines()
{
    foreach (QGraphicsLineItem* l, ll) {
        l->setPen(QPen(Qt::gray, 1));
    }
    ll.clear();
}

void MainWindow::searchNearestPoint(const QPointF &p)
{
    if (!m_KDTree)
        return;
    *m_searchPoint = p;
    resetHighlightedLines();


    m_searchPointItem->setPos(p);
    double r = 5;
    m_searchPointItem->setRect(QRectF(QPointF(-r/2,-r/2), QSize(r, r)));m_searchPointItem->setBrush(QColor(250, 0, 0));

    k = ui->lineEdit_k->text().toInt();
    if (k == 1) {
        //    m_nearestPoint = m_KDTree->nearestPoint(*m_point);
        count_AVG = 0;
        bestNode = Q_NULLPTR;
        bestDist = DBL_MAX;
        count = 0; //距离对比次数

//        QPointF np = m_KDTree->nearestPoint(m_searchPointItem->pos());
//        bestDist = QVector2D(np-m_searchPointItem->pos()).length();

        KDTree::NearestNodeSearch(m_KDTree, m_searchPointItem->pos(), m_circle, m_circleRadius);
        count_MAX = qMax(count_MAX, count);
        count_MIN = qMin(count_MIN, count);
        m_nearestPoint = bestNode->m_point;

        m_circle->setRect(QRectF(m_searchPointItem->pos().x() - bestDist, m_searchPointItem->pos().y()- bestDist, bestDist*2, bestDist*2));
        m_circleRadius->setLine(QLineF(bestNode->m_point->scenePos(), m_searchPointItem->pos()));
    }
    else if (k > 1) {
        while(!knn.empty()) {
            const T& top = knn.top();
            delete top.second->c;
            delete top.second->cr;
            knn.pop();
        }
        MySleep(sleepMScs);

        knn = std::priority_queue<T>(); //K近邻列表 按优先级值从小到大排序 默认用的std::less

        KDTree::KNNSearch(m_KDTree, m_searchPointItem->pos(), m_scene);
//        qDebug() << "knn.size==" << knn.size() << k;
    }
    MySleep(sleepMScs);
}

void MainWindow::onMouseMove(const QPointF &p)
{
//    qDebug() << __PRETTY_FUNCTION__;
    searchNearestPoint(p);
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (!m_searchPoint) {
        m_searchPoint = new QPointF;
    }

    qDebug() << qApp->widgetAt(mapToGlobal(event->pos())) << ui->centralWidget << ui->centralWidget->geometry();
    if (ui->centralWidget->geometry().contains(event->pos()))
        searchNearestPoint(ui->centralWidget->mapToScene(ui->centralWidget->mapFrom(this, event->pos())));

    return QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    return QMainWindow::mouseReleaseEvent(event);
}

//view的位置去view执行
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
//    if (!m_searchPoint) {
//        m_searchPoint = new QPointF;
//    }
//    if (ui->centralWidget->geometry().contains(event->pos()))
//        searchNearestPoint(event->pos());/*qDebug() << __PRETTY_FUNCTION__;*/
    return QMainWindow::mouseMoveEvent(event);
}

void MainWindow::on_pushButton_createPoints_clicked()
{
    on_pushButton_clearScene_clicked();
    points.clear();
    pointCreateCount = ui->lineEdit_createPointsCount->text().toInt();
    int l = centralWidget()->geometry().left();
    int t = centralWidget()->geometry().top();
    int w = centralWidget()->geometry().width();
    int h = centralWidget()->geometry().height();
    for (int i = 0; i < pointCreateCount; i++)
    {
        points << QPointF(random(l, l+w),random(t, t+h));
    }
    int r = ui->lineEdit_createPointsRadius->text().toInt();
    for (int i = 0; i < points.count(); i++)
    {
        QPainterPath path;
        path.addEllipse(QPointF(), r, r);
//        painter.fillPath(path, Qt::black);
        QGraphicsPathItem* p = new QGraphicsPathItem(path);
        p->setZValue(100);
        m_scene->addItem(p);
        p->setPos(points.at(i));
        ps << p;
    }
//    qDebug() << points;
}

void MainWindow::on_pushButton_createKDTree_clicked()
{
    if (m_KDTree) {
        delete m_KDTree;
    }
    m_KDTree = new KDTree;
    m_KDTree->insert(ps);
    int l = centralWidget()->geometry().left();
    int t = centralWidget()->geometry().top();
    int w = centralWidget()->geometry().width();
    int h = centralWidget()->geometry().height();
//    m_KDTree->drawPoint(painter, l, t, w, h);
    if (!m_KDTree)
        return;
    m_KDTree->draw(m_scene, l, t, w, h);
//    painter.setPen(QPen(Qt::blue, 2));
//    painter.drawRect(centralWidget()->geometry());
    qDebug() << centralWidget()->geometry();
}


void MainWindow::on_verticalSlider_sleepMscs_valueChanged(int value)
{
    sleepMScs = value;
    ui->label_sleepMscs->setText(QString("延时: %1ms").arg(value));
}

//场景初始化
void MainWindow::on_pushButton_clearScene_clicked()
{
    points.clear();
    ps.clear();
    if (m_KDTree) {
        KDTree::clear(m_KDTree);
    }
    m_KDTree = Q_NULLPTR;
    m_scene->clear();
    initScene();
    ll.clear();
    knn = std::priority_queue<T>(); //K近邻列表 按优先级值从小到大排序 默认用的std::less
}
