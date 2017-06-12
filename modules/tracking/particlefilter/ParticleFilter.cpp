#include "precompiled.h"
#include <chrono>
#include "tracking/particlefilter/ParticleFilter.h"
#include <thread>

ParticleFilter::ParticleFilter(
        uint16_t frameWidth, uint16_t frameHeight, uint16_t nParticles, uint8_t nIters,
        uint16_t particleWidth, uint16_t particleHeight) : m_img_width(frameWidth), m_img_height(frameHeight),
                                                           m_num_particles(nParticles), m_num_iters(nIters),
                                                           m_particle_width(particleWidth),
                                                           m_particle_height(particleHeight) {
    m_rand_dice = std::uniform_real_distribution<>(0, 1);
    m_rand_distortion = std::normal_distribution<>();
    m_xRange = (m_img_width - m_particle_width);
    m_yRange = (m_img_height - m_particle_height);

    // Default
    setDBSCANEps(8.0f);
    setDBSCANMinPts(20);
}

void ParticleFilter::exec(const cv::Mat &inputImg, cv::Mat &imgOut)
{
    {   /// RDF
        cv::cvtColor(inputImg, m_img, CV_BGR2Lab);
        m_img = m_forest->computeLabels(m_img);
        cv::medianBlur(m_img, m_img, 3);
    }

    {   /// BINARIZE
        cv::threshold(m_img , m_img , 0, 255, cv::THRESH_BINARY);
        cv::dilate(m_img, m_img, cv::Mat(), cv::Point(-1,-1));
        cv::erode(m_img, m_img, cv::Mat(), cv::Point(-1,-1), 2);
    }

    cv::imshow("Test", m_img);

    //    { /// Blob Detection
    //    std::vector<cv::Rect>  bBox_vec;
    //    auto m_aspectMax = 1.30;
    //    auto m_aspectMin = 0.30;
    //    auto m_BBoxMinSize = 500;
    //    auto m_BBoxMaxSize = 5000;
    //    bBox_vec = Util::calculateBoundingBoxRect(m_img, m_BBoxMinSize, m_BBoxMaxSize, m_aspectMax, m_aspectMin);
    //    m_img = inputImg.clone();
    //    Util::drawBoundingBox(m_img, bBox_vec);
    //    }


    {
        cv::integral(m_img, m_integralMat);
        m_img = inputImg;
        processImage();
    }



//    std::chrono::seconds dura(1);
//    std::this_thread::sleep_for( dura );



    //    m_tracked_clusters = DBSCAN::getClusters(m_tracking_particles, m_dbscan_eps, m_dbscan_min_pts);
    //    qDebug() << "Cluster count: " << m_tracked_clusters.size();
    //    addNewTrackers();
    //    showTopNParticles(m_tracking_particles.size());

//    {
//        std::sort(std::begin(m_search_particles), std::end(m_search_particles),
//                  [](RectangleParticle P1, RectangleParticle P2) {
//            return P1.weight > P2.weight;
//        });
//        m_search_particles.resize(200);
//        auto all_clusters = DBSCAN::getClusters(m_search_particles, m_dbscan_eps, m_dbscan_min_pts);
//        showRects(m_img, all_clusters, cv::Scalar(0, 130, 0), 2);
//        //    showDetections(m_search_particles, m_img);
//    }
//    showRects(m_img, m_tracked_clusters, cv::Scalar(0, 130, 0), 2);
    imgOut = m_img;
}

void ParticleFilter::processImage()
{
    updateTrackers();
    setSTDforTrackers();
    initializeParticles();
    detectObjects();
    addNewTrackers();
    updateWeights(m_tracking_particles);
}

void ParticleFilter::resample(QVector<RectangleParticle> &particles, bool isSearch) {
    auto nParticles = particles.size();
    if (!isSearch)
        nParticles = m_nTrackers * m_tracked_clusters.size();
    if (nParticles == 0) return;
    // compute max weight
    auto p_indx = std::max_element(std::begin(particles), std::end(particles),
                                   [](RectangleParticle& P1, RectangleParticle& P2) {
                                       return P1.weight < P2.weight;
                                   });
    auto max_w = particles[std::distance(std::begin(particles), p_indx)].weight;

    // resample particles
    for (int j = 0; j < nParticles; ++j) {
        auto p_new = particles[resamplingWheel(particles, max_w)].clone();
        isSearch ? distortSearchParticle(p_new) : distortParticle(p_new);
        m_particlesNew.push_back(p_new);
    }

    for (auto &p : m_particlesNew)
        ++p.age;

    // update particles
    particles.clear();
    particles = m_particlesNew;
    m_particlesNew.clear();
}

void ParticleFilter::setSTDforTrackers()
{
    std::vector<cv::Point2f> diff(m_tracked_clusters.size());
    std::vector<cv::Point2f> diff_dim(m_tracked_clusters.size());
    std::vector<uint32_t> counter(m_tracked_clusters.size(),0);
    auto nP = m_tracking_particles.size();
    for(auto i = 0; i < nP; ++i)
    {
        auto& p = m_tracking_particles[i];
        auto c  = p.cluster - 1;
        if (c < 0) continue;
        auto pt = p.center() - m_tracked_clusters[c].center();
        diff[c].x += std::pow(pt.x,2);
        diff[c].y += std::pow(pt.y,2);
        diff_dim[c].x += std::pow(p.width - m_tracked_clusters[c].width,2);
        diff_dim[c].y += std::pow(p.height - m_tracked_clusters[c].height,2);
        ++counter[c];
    }

    for(auto& p : m_tracking_particles)
    {
        auto c = p.cluster-1;
        if (c < 0) continue;
        p.sig_x = std::sqrt(diff[c].x/counter[c]);
        p.sig_y = std::sqrt(diff[c].y/counter[c]);
        p.sig_w = 1;//std::sqrt(diff_dim[c].x/counter[c]);
        p.sig_h = 1;//std::sqrt(diff_dim[c].y/counter[c]);
    }
    diff.clear();
    counter.clear();
}

void ParticleFilter::showTopNParticles(int count) {
    auto size = m_tracking_particles.size();
    // sort descending
    std::sort(std::begin(m_tracking_particles), std::end(m_tracking_particles),
              [](RectangleParticle P1, RectangleParticle P2) {
                  return P1.weight > P2.weight;
              });

    count = (count > size) ? size : count;
    for (auto i = 0; i < count; ++i) {
        auto &p = m_tracking_particles[i];
        auto rgb = colors.colors[p.cluster % colors.colors.size()];
        rectangle(m_img, p.boundingBox(), cv::Scalar(rgb[0], rgb[1], rgb[2]), 1);
    }
}

ParticleFilter::~ParticleFilter() {
    m_search_particles.clear();
    m_particlesNew.clear();
    delete m_forest;
}
