#ifndef IMGPROCESSOR_H
#define IMGPROCESSOR_H

class ImgProcessor
{
public:
    virtual void setIMG(cv::Mat *pImage) = 0;
    virtual cv::Mat getIMG() const = 0;
    virtual void processImage() = 0;
};

#endif // IMGPROCESSOR_H
