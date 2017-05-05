#ifndef DBSCAN_H
#define DBSCAN_H

#include "precompiled.h"
#include "RectangleParticle.h"

class DBSCAN {
public:

    template <typename T>
    static inline QVector<QVector<T>> getClusters(QVector<T>& particles, float epsilon, quint8 min_points)
    {
        QVector<QVector<T>> result;
        auto clusters = 0;
        for(auto& p : particles)
        {
            if(p.isVisited)
                continue;
            p.isVisited = true;
            auto sphere_particles = regionQuery(p, particles, epsilon);
            if(sphere_particles.size() >= min_points)
            {
                result.push_back(QVector<T>());
                result[clusters].push_back(p.clone());
                expandCluster(sphere_particles, result, particles, clusters, epsilon, min_points);
                clusters++;
            }
            sphere_particles.clear();
        }

        return result;
    }

    template <typename T>
    static inline QVector<cv::Rect> getClusters_(QVector<T>& particles, float epsilon, quint8 min_points)
    {
        QVector<cv::Rect> result;
        auto clusters = 0;
        for(auto& p : particles)
        {
            if(p.isVisited)
                continue;
            p.isVisited = true;
            auto sphere_particles = regionQuery(p, particles, epsilon);
            if(sphere_particles.size() >= min_points)
            {
                result.push_back(p.boundingBox());
                expandCluster_(sphere_particles, result, particles, clusters, epsilon, min_points);
                clusters++;
            }
            sphere_particles.clear();
        }

        return result;
    }

    template <typename T>
    static inline float normL2Sqr(T& p1, T& p2)
    {
        return ((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
    }

private:
    template <typename T>
    static inline void expandCluster(std::set<quint16>& sphere_particles,
                                                           QVector<QVector<T>>& result,
                                                           QVector<T>& particles,
                                                           int cluster, float epsilon, quint8 min_points)
    {

        for(auto itt = std::begin(sphere_particles);  itt != std::end(sphere_particles); ++itt)
        {
            auto& p = particles[*itt];
            if(p.isVisited())
                continue;
            p.setIsVisited(true);

            auto sphere_particles_ = regionQuery(p, particles, epsilon);
            if (sphere_particles_.size() >= min_points)
                sphere_particles.insert(sphere_particles_.begin(), sphere_particles_.end());

            result[cluster].push_back(p.clone());
            sphere_particles_.clear();
        }
    }

    template <typename T>
    static inline void expandCluster_(std::set<quint16>& sphere_particles,
                                                           QVector<cv::Rect>& result,
                                                           QVector<T>& particles,
                                                           int cluster, float epsilon, quint8 min_points)
    {

        auto clusterSize = 1;
        for(auto itt = std::begin(sphere_particles);  itt != std::end(sphere_particles); ++itt)
        {
            auto& p = particles[*itt];
            if(p.isVisited)
                continue;
            p.isVisited = true;

            auto sphere_particles_ = regionQuery(p, particles, epsilon);
            if (sphere_particles_.size() >= min_points)
                sphere_particles.insert(sphere_particles_.begin(), sphere_particles_.end());

            result[cluster].x += p.x;
            result[cluster].y += p.y;
            result[cluster].width += p.width;
            result[cluster].height += p.height;
            ++clusterSize;
            sphere_particles_.clear();
        }

        result[cluster].x       /= clusterSize;
        result[cluster].y       /= clusterSize;
        result[cluster].width   /= clusterSize;
        result[cluster].height  /= clusterSize;
    }

    template <typename T>
    static inline std::set<quint16> regionQuery(T& p, QVector<T>& particles, float epsilon)
    {
        std::set<quint16> set;
        auto nTot = particles.size();
        for(auto i = 0; i < nTot; ++i)
            if(normL2Sqr(p, particles[i]) <= epsilon*epsilon )
                set.insert(i);
        return set;
    }
};

#endif // DBSCAN_H
