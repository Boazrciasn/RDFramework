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
        auto total = particles.size();
        for(auto i = 0;  i < total; ++i)
        {
            if(particles[i].isVisited())
                continue;
            particles[i].setIsVisited(true);
            auto sphere_particles = regionQuery(i, particles, epsilon);
            if(sphere_particles.size() >= min_points)
            {
                result.push_back(QVector<T>());
                expandCluster(particles[i].clone(), sphere_particles, result, particles, clusters++, epsilon, min_points);
            }
            sphere_particles.clear();
        }

        return result;
    }

private:
    template <typename T>
    static inline void expandCluster(T p, QVector<quint16>& sphere_particles,
                                                           QVector<QVector<T>>& result,
                                                           QVector<T>& particles,
                                                           int cluster, float epsilon, quint8 min_points)
    {
        result[cluster].push_back(p);
        for(auto i = 0;  i < sphere_particles.size(); ++i)
        {
            auto index = sphere_particles[i];
            if(particles[index].isVisited())
                continue;

            particles[index].setIsVisited(true);
            auto sphere_particles_ = regionQuery(i, particles, epsilon);
            if (sphere_particles_.size() >= min_points)
                merge(sphere_particles, sphere_particles_);

            sphere_particles_.clear();
            result[cluster].push_back(particles[index].clone());
        }
    }

    template <typename T>
    static inline QVector<quint16> regionQuery(int index, QVector<T>& particles, float epsilon)
    {
        QVector<quint16> result{};
        auto total = particles.size();
        for(auto i = 0; i < total; ++i)
            if(distSquared(particles[i],particles[index]) <= epsilon*epsilon )
                result.push_back(i);
        return result;
    }

    template <typename T>
    static inline float distSquared(T& p1, T& p2)
    {
        return ((p1.x() - p2.x())*(p1.x() - p2.x()) + (p1.y() - p2.y())*(p1.y() - p2.y()));
    }

    static inline void merge(QVector<quint16>& set1, QVector<quint16>& set2)
    {
        for(auto val : set2)
            if(!set1.contains(val))
                set1.push_back(val);
    }
};

#endif // DBSCAN_H
