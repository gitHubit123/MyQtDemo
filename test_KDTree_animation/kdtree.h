#ifndef KDTREE_H
#define KDTREE_H

#include <QtWidgets>
#include <vector>
#include <memory>
#include "scene.h"

class KDTree;

//被搜索的K个节点的相关信息
class NodeInfo
{
public:
    KDTree* node;
    QGraphicsEllipseItem* c;
    QGraphicsLineItem* cr;
};

typedef std::pair<double, NodeInfo*> T; //std::pair<距离, KD树节点>  距离是搜索点到KD树节点的距离


//待分开为 树+节点的形式
class KDTree
{
public:
    KDTree();
    void insert(QList<QGraphicsPathItem *> &pl, int depth = 0);
    static void clear(KDTree *node);
    QPointF nearestPoint(const QPointF& p, int depth = 0);
    static void NearestNodeSearch(KDTree *node, const QPointF &p, QGraphicsEllipseItem* c, QGraphicsLineItem* cr, int depth = 0); //查找最近点
    static void KNNSearch(KDTree *node, const QPointF &p, Scene *s, int depth = 0); //查找K个最近点 k nearest node search
    void draw(Scene *s, int l, int t, int w, int h, int depth = 0);

    static distance(KDTree* node, const QPointF& p);
    static straightDistance(KDTree* node, const QPointF& p, int depth = 0);

    QGraphicsPathItem* point() const;
    void setPoint(QGraphicsPathItem* point);
public:
    QGraphicsPathItem* m_point;
    KDTree* m_parentNode = Q_NULLPTR;
    KDTree* m_leftNode = Q_NULLPTR;
    KDTree* m_rigthNode = Q_NULLPTR;
    QGraphicsLineItem* m_LineItem = Q_NULLPTR;
};

#endif // KDTREE_H
