#ifndef IPMMAPPER_H
#define IPMMAPPER_H
#include "precompiled.h"


struct HomographyParams
{
    double alpha;
    double beta;
    double gamma;
    double f;
    double d;
};

inline cv::Mat applyIPM(cv::Mat input, HomographyParams params)
{
    cv::Size inp_size = input.size();
    double w = (double)inp_size.width;
    double h = (double)inp_size.height;
    // Projection 2D -> 3D matrix
    cv::Mat A1 = (cv::Mat_<float>(4, 3) <<
                  1, 0, -w / 2,
                  0, 1, -h / 2,
                  0, 0,    0,
                  0, 0,    1);
    // Rotation matrices around the X,Y,Z axis
    cv::Mat RX = (cv::Mat_<float>(4, 4) <<
                  1,          0,           0, 0,
                  0, cos(params.alpha), -sin(params.alpha), 0,
                  0, sin(params.alpha),  cos(params.alpha), 0,
                  0,          0,           0, 1);
    cv::Mat RY = (cv::Mat_<float>(4, 4) <<
                  cos(params.beta), 0, -sin(params.beta), 0,
                  0, 1,          0, 0,
                  sin(params.beta), 0,  cos(params.beta), 0,
                  0, 0,          0, 1);
    cv::Mat RZ = (cv::Mat_<float>(4, 4) <<
                  cos(params.gamma), -sin(params.gamma), 0, 0,
                  sin(params.gamma),  cos(params.gamma), 0, 0,
                  0,          0,           1, 0,
                  0,          0,           0, 1);
    // Composed rotation matrix with (RX,RY,RZ)
    cv::Mat R = RX * RY * RZ;
    // Translation matrix on the Z axis change dist will change the height
    cv::Mat T = (cv::Mat_<float>(4, 4) <<
                 1, 0, 0, 0,
                 0, 1, 0, 0,
                 0, 0, 1, params.d,
                 0, 0, 0, 1);
    // Camera Intrisecs matrix 3D -> 2D
    cv::Mat A2 = (cv::Mat_<float>(3, 4) <<
                  params.f, 0, w / 2, 0,
                  0, params.f, h / 2, 0,
                  0, 0,   1, 0);
    // Final and overall transformation matrix
    cv::Mat transform = A2 * (T * (R * A1));
    cv::Mat warped_img;
    cv::warpPerspective(input, warped_img, transform, inp_size, cv::INTER_CUBIC | cv::WARP_INVERSE_MAP);
    return warped_img;
}

#endif // IPMMAPPER_H
