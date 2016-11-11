#ifndef NODE_H
#define NODE_H

#include "precompiled.h"
#include "memory"
#include "PixelCloud.h"

struct Node
{
    bool m_isLeaf;
    quint32 m_id;
    qint16 m_tau;
    Coord m_teta1, m_teta2;
    Coord m_dataRange;
    quint32 m_leftCount;


    cv::Mat_<float> m_hist;

    Node() : Node(0, false)
    {
    }

    Node(quint32 id, bool isLeaf): m_id(id), m_isLeaf(isLeaf)
    {
        m_hist.create(1, 1);
        m_hist.setTo(0);
    }

    template<class Archive>
    void serialize(Archive &archive)
    {
        archive( m_tau, m_teta1, m_teta2, m_id, m_isLeaf, m_hist);
    }

    ~Node()
    {
    }
};

using node_ptr  = std::shared_ptr<Node>;

#endif // NODE_H
