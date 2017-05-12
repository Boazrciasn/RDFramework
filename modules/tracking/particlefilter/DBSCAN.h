#ifndef DBSCAN_H
#define DBSCAN_H

#include "precompiled.h"
#include "RectangleParticle.h"

class DBSCAN {
public:

    template <typename T>
    static inline QVector<QVector<T>> getClusters(QVector<T>& particles, float epsilon, uint8_t min_points)
    {
        QVector<QVector<T>> result;
        auto clusters = 1;
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
    static inline QVector<cv::Rect> getClusters_(QVector<T>& particles, float epsilon, uint8_t min_points)
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
                result.push_back(cv::Rect(0,0,0,0));
                expandCluster_(p, sphere_particles, result, particles, clusters, epsilon, min_points);
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
                                                           int cluster, float epsilon, uint8_t min_points)
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
    static inline void expandCluster_(T& p_, std::set<quint16>& sphere_particles,
                                                           QVector<cv::Rect>& result,
                                                           QVector<T>& particles,
                                                           int cluster, float epsilon, uint8_t min_points)
    {
        p_.cluster = cluster;
        auto x = p_.x * p_.weight;
        auto y = p_.y * p_.weight;
        auto w = p_.width * p_.weight;
        auto h = p_.height * p_.weight;
        auto tot_weight = p_.weight;
        for(auto itt = std::begin(sphere_particles);  itt != std::end(sphere_particles); ++itt)
        {
            auto& p = particles[*itt];
            if(p.isVisited)
                continue;
            p.isVisited = true;

            auto sphere_particles_ = regionQuery(p, particles, epsilon);
            if (sphere_particles_.size() >= min_points)
                sphere_particles.insert(sphere_particles_.begin(), sphere_particles_.end());

            x += p.x * p.weight;
            y += p.y * p.weight;
            w += p.width * p.weight;
            h += p.height * p.weight;
            tot_weight += p.weight;
            sphere_particles_.clear();
            p.cluster = cluster;
        }

        result[cluster].x       = x/tot_weight;
        result[cluster].y       = y/tot_weight;
        result[cluster].width   = w/tot_weight;
        result[cluster].height  = h/tot_weight;
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
