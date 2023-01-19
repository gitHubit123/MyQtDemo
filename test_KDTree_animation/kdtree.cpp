#include "kdtree.h"
#include <queue>
#include <synchapi.h>

extern QList<QColor> cl;
extern int count;
extern int sleepMScs;
extern QList<QGraphicsLineItem*> ll; //搜索过程高亮的线段

extern int k;
//typedef std::pair<double, KDTree*> T; //std::pair<距离, KD树节点>  距离是搜索点到KD树节点的距离
extern std::priority_queue<T> knn; //K近邻列表 按优先级值从小到大排序 默认用的std::less

void MySleep(int mscs);

//返回深度对应的颜色 值
QColor DepthToColor(int depth = 0)
{
    return cl.at(depth%cl.count());
}

KDTree::KDTree()
{

}

bool MyLessThanX(QGraphicsPathItem* p1,QGraphicsPathItem* p2)
{
    return p1->x() < p2->x();
}
bool MyLessThanY(QGraphicsPathItem* p1,QGraphicsPathItem* p2)
{
    return p1->y() < p2->y();
}
bool MyLessThanX1(const QPointF& p1,QGraphicsPathItem* p2)
{
    return p1.x() < p2->x();
}
bool MyLessThanY1(const QPointF& p1,QGraphicsPathItem* p2)
{
    return p1.y() < p2->y();
}

//pl: 一组点
//k: 1==X轴 2==Y轴
void KDTree::insert(QList<QGraphicsPathItem*> &pl, int depth)
{
    if (pl.count() == 0 or depth < 0) {
        return;
    }
    qSort(pl.begin(), pl.end(), (depth%2==0 ? MyLessThanX : MyLessThanY));
    int midIdx = pl.count()/2;
    int leftEndIdx = midIdx-1;
    int rightBeginIdx = midIdx+1;
    int leftCount = leftEndIdx-0+1;
    int rightCount = pl.count()-1 - rightBeginIdx+1;
    setPoint(pl.at(midIdx));

    if (leftCount > 0) {
        m_leftNode = new KDTree;
        m_leftNode->m_parentNode = this;
        QList<QGraphicsPathItem*> pl_left = pl.mid(0, leftCount);
        m_leftNode->insert(pl_left, depth+1);
    }
    if (rightCount > 0) {
        m_rigthNode = new KDTree;
        m_rigthNode->m_parentNode = this;
        QList<QGraphicsPathItem*> pl_right = pl.mid(rightBeginIdx, rightCount);
        m_rigthNode->insert(pl_right, depth+1);
    }
}

//对根节点调这个方法来清空整个树
void KDTree::clear(KDTree* node)
{
    if (node == Q_NULLPTR) return;
    if (node->m_leftNode) clear(node->m_leftNode);
    if (node->m_rigthNode) clear(node->m_rigthNode);
    delete node;
    node = Q_NULLPTR;
}
//p: 待搜索点
//返回值: 树中距离p比较近的点 粗糙版
QPointF KDTree::nearestPoint(const QPointF& p, int depth)
{
    typedef bool (*pMyLessThan)(const QPointF& p1,QGraphicsPathItem* p2);
    pMyLessThan MyLessThan;
    MyLessThan = (depth%2==0 ? MyLessThanX1 : MyLessThanY1);

    if (MyLessThan(p, m_point)) {
        if (m_leftNode) {
            return m_leftNode->nearestPoint(p, depth + 1);
        }
        return m_point->scenePos();
    }
    else {
        if (m_rigthNode) {
            return m_rigthNode->nearestPoint(p, depth + 1);
        }
        return m_point->scenePos();
    }
}

#include <float.h>

//定义被搜索点q
extern KDTree* bestNode;
extern double bestDist;
//返回值: 树中距离p最近的点 准确版
void KDTree::NearestNodeSearch(KDTree* node, const QPointF& p, QGraphicsEllipseItem* c, QGraphicsLineItem* cr, int depth)
{
    if (!node)
        return;

    double curr_dist = KDTree::distance(node, p);
    if (curr_dist < bestDist) {
        bestDist = curr_dist;
        bestNode = node;
    }
    count++;

    node->m_LineItem->setPen(QPen(Qt::gray, 4));
    ll << node->m_LineItem; //高亮线段存储
    c->setRect(QRectF(p.x() - curr_dist, p.y()- curr_dist, curr_dist*2, curr_dist*2));
    cr->setLine(QLineF(node->m_point->scenePos(), p));
    MySleep(sleepMScs);

    typedef bool (*pMyLessThan)(const QPointF& p1,QGraphicsPathItem* p2);
    pMyLessThan MyLessThan;
    MyLessThan = (depth%2==0 ? MyLessThanX1 : MyLessThanY1);

    bool hasSearchedLeft = true;
    if (MyLessThan(p, node->m_point)) {
        hasSearchedLeft = true;
        NearestNodeSearch(node->m_leftNode, p, c, cr, depth + 1);
    }
    else {
        hasSearchedLeft = false;
        NearestNodeSearch(node->m_rigthNode, p, c, cr, depth + 1);
    }
    if (KDTree::straightDistance(node, p, depth) < bestDist) {
        KDTree* otherNode = (hasSearchedLeft ? node->m_rigthNode : node->m_leftNode);
        NearestNodeSearch(otherNode, p, c, cr, depth + 1);
    }
}

void KDTree::KNNSearch(KDTree *node, const QPointF &p, Scene* s, int depth)
{
    if (!node)
        return;

    node->m_LineItem->setPen(QPen(Qt::gray, 4));
    ll << node->m_LineItem; //高亮线段存储

    int n = knn.size();
    double curr_dist = KDTree::distance(node, p);
    const T& item= knn.top();
    double dist_tail = (n > 0 ? KDTree::distance(item.second->node, p) : 0);
    if (n < k) {
        NodeInfo* ni = new NodeInfo;
        ni->node = node;
        ni->c = new QGraphicsEllipseItem; s->addItem(ni->c);
        ni->cr = new QGraphicsLineItem; s->addItem(ni->cr);
        ni->c->setRect(QRectF(p.x() - curr_dist, p.y()- curr_dist, curr_dist*2, curr_dist*2));
        ni->cr->setLine(QLineF(node->m_point->scenePos(), p));
        knn.push(std::make_pair(curr_dist, ni));
    }
    else {
        if (curr_dist < dist_tail) {
            NodeInfo* ni = item.second;
            ni->node = node;
            ni->c->setRect(QRectF(p.x() - curr_dist, p.y()- curr_dist, curr_dist*2, curr_dist*2));
            ni->cr->setLine(QLineF(node->m_point->scenePos(), p));
            knn.pop();
            knn.push(std::make_pair(curr_dist, ni));
        }
    }

    MySleep(sleepMScs);


    typedef bool (*pMyLessThan)(const QPointF& p1,QGraphicsPathItem* p2);
    pMyLessThan MyLessThan;
    MyLessThan = (depth%2==0 ? MyLessThanX1 : MyLessThanY1);

    bool hasSearchedLeft = true;
    if (MyLessThan(p, node->m_point)) {
        hasSearchedLeft = true;
        KNNSearch(node->m_leftNode, p, s, depth + 1);
    }
    else {
        hasSearchedLeft = false;
        KNNSearch(node->m_rigthNode, p, s, depth + 1);
    }
    if (n < k or KDTree::straightDistance(node, p, depth) < dist_tail) {
        KDTree* otherNode = (hasSearchedLeft ? node->m_rigthNode : node->m_leftNode);
        KNNSearch(otherNode, p, s, depth + 1);
    }
}

void KDTree::draw(Scene* s, int l, int t, int w, int h, int depth)
{
    MySleep(sleepMScs);
    if (depth%2==0) { //x
        this->m_LineItem = new QGraphicsLineItem(QLineF(QPointF(m_point->x(), t), QPointF(m_point->x(), t+h)));
        s->addItem(this->m_LineItem);
    }
    else { //y
        this->m_LineItem = new QGraphicsLineItem(QLineF(QPointF(l, m_point->y()), QPointF(l+w, m_point->y())));
        this->m_LineItem->setPen(QPen(Qt::gray, 1));
        s->addItem(this->m_LineItem);
    }
    this->m_point->setBrush(DepthToColor(depth));
    if (depth%2==0) { //x
        if (m_leftNode)
            m_leftNode->draw(s, l, t, m_point->x()-l, h, depth+1);
        if (m_rigthNode)
            m_rigthNode->draw(s, m_point->x(), t, w-(m_point->x()-l), h, depth+1);

    }
    else { //y
        if (m_leftNode)
            m_leftNode->draw(s, l, t, w, m_point->y()-t, depth+1);
        if (m_rigthNode)
            m_rigthNode->draw(s, l, m_point->y(), w, h-(m_point->y()-t), depth+1);
    }
}

KDTree::distance(KDTree *node, const QPointF &p)
{
    return QVector2D(p-node->m_point->scenePos()).length();
}

KDTree::straightDistance(KDTree *node, const QPointF &p, int depth)
{
    if (depth%2==0) { //x
        return qAbs(node->m_point->x() - p.x());
    }
    else { //y
        return qAbs(node->m_point->y() - p.y());
    }
}

QGraphicsPathItem* KDTree::point() const
{
    return m_point;
}

void KDTree::setPoint(QGraphicsPathItem *point)
{
    m_point = point;
}
