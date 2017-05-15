#ifndef TYPEDEFS_H
#define TYPEDEFS_H

using BinaryImage       = cv::Mat_<quint8>;
using GrayImage         = cv::Mat_<quint8>;
using ColorImage        = cv::Mat_<cv::Vec3b>;
using ShortImage        = cv::Mat_<quint16>;
using IntImage          = cv::Mat_<qint32>;
using FloatImage        = cv::Mat_<float>;
using DoubleImage       = cv::Mat_<double>;

using BinaryMat         = cv::Mat_<quint8>;
using GrayMat           = cv::Mat_<quint8>;
using ColorMat          = cv::Mat_<cv::Vec3b>;
using ShortMat          = cv::Mat_<quint16>;
using IntMat            = cv::Mat_<qint32>;
using FloatMat          = cv::Mat_<float>;
using DoubleMat         = cv::Mat_<double>;

using featureType       = float;
using ftrIdType         = uint8_t;
using tauType           = int32_t;
using MatFeature        = cv::Mat_<featureType>;

using tableLookupType   = float;

#endif // TYPEDEFS_H
