#ifndef NODE_H
#define NODE_H

#include "precompiled.h"
#include "memory"
#include "PixelCloud.h"

struct Node
{
    quint32 id;
    quint32 leftCount;
    qint16 tau;
    cv::Point teta1, teta2;

    cv::Mat_<float> hist;

    Node() : Node(0)
    {
    }

    Node(quint32 nodeId): id(nodeId)
    {
        hist.create(1, 1);
        hist.setTo(0);
    }

    ~Node()
    {
        hist.release();
    }

    template<class Archive>
    void serialize(Archive &archive)
    {
        archive( tau, teta1, teta2, id, hist);
    }
};

#endif // NODE_H
