#ifndef DBSCAN_H
#define DBSCAN_H

#include "precompiled.h"
#include "RectangleParticle.h"

class DBSCAN {
public:
    template <typename T>
    static inline QVector<T> getClusters(QVector<T>& particles, float epsilon, uint8_t min_points)
    {
        QVector<T> result;
        auto clusters = 0;
        for(auto& p : particles)
        {
            if(p.isVisited)
                continue;
            p.isVisited = true;
            auto sphere_particles = regionQuery(p, particles, epsilon);
            if(sphere_particles.size() >= min_points)
            {
                result.push_back(T());
                expandCluster(p, sphere_particles, result, particles, clusters, epsilon, min_points);
                clusters++;
            }
            sphere_particles.clear();
        }

        return result;
    }

    template <typename T>
    static inline float normL2Sqr(T p1, T p2)
    {
        return ((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
    }

private:
    template <typename T>
    static inline void expandCluster(T& p_, std::set<quint16>& sphere_particles,
                                                           QVector<T>& result,
                                                           QVector<T>& particles,
                                                           int cluster, float epsilon, uint8_t min_points)
    {
        p_.cluster = cluster+1;
        auto x = float(p_.x * p_.weight);
        auto y = float(p_.y * p_.weight);
        auto dx = (float)p_.dx*p_.weight;
        auto dy = (float)p_.dy*p_.weight;
        auto w = float(p_.width * p_.weight);
        auto h = float(p_.height * p_.weight);
        auto r = float(p_.r * p_.weight);
        auto t = float(p_.theta * p_.weight);
        auto tot_weight = p_.weight;
        auto count = 1;
        for(auto itt = std::begin(sphere_particles);  itt != std::end(sphere_particles); ++itt)
        {
            auto& p = particles[*itt];
            if(p.isVisited)
                continue;
            p.isVisited = true;

            auto sphere_particles_ = regionQuery(p, particles, epsilon);
            if (sphere_particles_.size() >= min_points)
                sphere_particles.insert(sphere_particles_.begin(), sphere_particles_.end());

            if(p.cluster == 0)
            {
                x += p.x * p.weight;
                y += p.y * p.weight;
                dx += (float)p.dx * p.weight;
                dy += (float)p.dy * p.weight;
                w += p.width * p.weight;
                h += p.height * p.weight;
                r += p.r * p.weight;
                t += p.theta * p.weight;
                tot_weight += p.weight;
                sphere_particles_.clear();
                p.cluster = cluster+1;
                ++count;
            }
        }

        result[cluster].x       = x/tot_weight;
        result[cluster].y       = y/tot_weight;
        result[cluster].dx      = dx/tot_weight;
        result[cluster].dy      = dy/tot_weight;
        result[cluster].width   = w/tot_weight;
        result[cluster].height  = h/tot_weight;
        result[cluster].r       = r/tot_weight;
        result[cluster].theta   = t/tot_weight;
        result[cluster].weight  = tot_weight;
    }

    template <typename T>
    static inline std::set<quint16> regionQuery(T& p, QVector<T>& particles, float epsilon)
    {
        std::set<quint16> set;
        auto nTot = particles.size();
        for(auto i = 0; i < nTot; ++i)
            if(normL2Sqr(p.center(), particles[i].center()) <= epsilon*epsilon )
                set.insert(i);
        return set;
    }
};

#endif // DBSCAN_H
