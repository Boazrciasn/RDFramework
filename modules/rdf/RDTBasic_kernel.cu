#include <opencv2/core/mat.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/highgui.hpp>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true)
{
    if (code != cudaSuccess)
    {
        fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
        if (abort) exit(code);
    }
}

__device__ inline float norm_device(float3 in1, float3 in2)
{
    float a[3];
    a[0] = in1.x - in2.x;
    a[1] = in1.y - in2.y;
    a[2] = in1.z - in2.z;
    return (a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
}

__global__ void compute_layered_hist_gpu(cv::cuda::PtrStepSz<uchar3> img,
                                         cv::cuda::PtrStepSz<float> layered_hist,
                                         cv::cuda::PtrStepSz<int> tree_nodes,
                                         cv::cuda::PtrStepSz<float>features,
                                         int lbl_count,
                                         int padding_x,
                                         int padding_y)
{
    int xIndex = blockIdx.x * blockDim.x + threadIdx.x + padding_x;
    int yIndex = blockIdx.y * blockDim.y + threadIdx.y + padding_y;

    if(xIndex >= (img.cols - padding_x) || yIndex >= (img.rows - padding_y))
        return;

    int n = (int)sqrtf((float)features.cols);  // N is one side of a squared feature
    int* current_node = tree_nodes.ptr(0);

    int rightID = current_node[0];
    int leftID  = 1;
    int ftrID, tau, fr1_x, fr1_y, fr2_x, fr2_y;
    float* feature;

    while(rightID != -1)
    {
        ftrID   = current_node[1];
        feature = features.ptr(ftrID);
        tau     = current_node[2];
        fr1_x   = xIndex + current_node[3];
        fr1_y   = yIndex + current_node[4];
        fr2_x   = xIndex + current_node[5];
        fr2_y   = yIndex + current_node[6];

        float3 ftr1{0};
        float3 ftr2{0};


        if(ftrID == 0)
        {
            uchar3 pxVal = img(fr1_y, fr1_x);
            ftr1.x = pxVal.x;
            ftr1.y = pxVal.y;
            ftr1.z = pxVal.z;

            pxVal = img(fr2_y, fr2_x);
            ftr2.x = pxVal.x;
            ftr2.y = pxVal.y;
            ftr2.z = pxVal.z;
        }
        else
            for(int i = 0; i < n*n; ++i)
            {
                uchar3 pxVal = img(fr1_y + i/n, fr1_x + i%n);
                ftr1.x += pxVal.x*feature[i];
                ftr1.y += pxVal.y*feature[i];
                ftr1.z += pxVal.z*feature[i];
            }

        if(norm_device(ftr1,ftr2) <= tau*tau) {
            current_node = tree_nodes.ptr(leftID);
            leftID++;
        }
        else {
            current_node = tree_nodes.ptr(rightID);
            leftID = rightID + 1;
        }
        rightID = current_node[0];
    }

    for (int i = 0; i < lbl_count; ++i)
        layered_hist(yIndex - padding_y, (xIndex - padding_x)*lbl_count + i) = current_node[i+1]/100.0f;
}






////////////////////////////////////////////////////////////////////////////////////
/// \brief compute_layered_hist_gpu
/// \param img
/// \param layered_hist
/// \param tree_nodes
/// \param features
/// \param lbl_count
/// \param padding_x
/// \param padding_y
///////////////////////////////////////////////////////////////////////////////////////

__global__ void compute_layered_hist_gpu_2b(cv::cuda::PtrStepSz<uchar> img,
                                            cv::cuda::PtrStepSz<float> layered_hist,
                                            cv::cuda::PtrStepSz<int> tree_nodes,
                                            cv::cuda::PtrStepSz<float>features,
                                            int lbl_count,
                                            int padding_x,
                                            int padding_y)
{
    int xIndex = blockIdx.x * blockDim.x + threadIdx.x + padding_x;
    int yIndex = blockIdx.y * blockDim.y + threadIdx.y + padding_y;

    if(xIndex >= (img.cols - padding_x) || yIndex >= (img.rows - padding_y))
        return;
    if(img(yIndex, xIndex) == 0)
        return;

    int n = (int)sqrtf((float)features.cols);  // N is one side of a squared feature
    int* current_node = tree_nodes.ptr(0);

    int rightID = current_node[0];
    int leftID  = 1;
    int ftrID, tau, fr1_x, fr1_y, fr2_x, fr2_y;
    float* feature;

    while(rightID != -1)
    {
        ftrID   = current_node[1];
        feature = features.ptr(ftrID);
        tau     = current_node[2];
        fr1_x   = xIndex + current_node[3];
        fr1_y   = yIndex + current_node[4];
        fr2_x   = xIndex + current_node[5];
        fr2_y   = yIndex + current_node[6];

        float ftr1{0};
        float ftr2{0};


        if(ftrID == 0)
        {
            ftr1 = img(fr1_y, fr1_x);
            ftr2 = img(fr2_y, fr2_x);
        }
        else
            for(int i = 0; i < n*n; ++i)
                ftr1 = (float)img(fr1_y + i/n, fr1_x + i%n)*feature[i];

        if((ftr1 - ftr2) <= tau) {
            current_node = tree_nodes.ptr(leftID);
            leftID++;
        }
        else {
            current_node = tree_nodes.ptr(rightID);
            leftID = rightID + 1;
        }
        rightID = current_node[0];
    }

    for (int i = 0; i < lbl_count; ++i)
        layered_hist(yIndex - padding_y, (xIndex - padding_x)*lbl_count + i) = current_node[i+1]/100.0f;
}


void computeLayeredHist_gpu(const cv::Mat& img,
                            cv::Mat_<float>& layered_hist,
                            const cv::Mat_<int>& tree_nodes,
                            const cv::Mat_<float>& features,
                            int lbl_count,
                            int padding_x,
                            int padding_y)
{
    using namespace cv;
    using namespace cv::cuda;

    dim3 block_size(32,32); // 512 threads
    dim3 grid_size;
    grid_size.x = (img.cols + block_size.x - 1)/block_size.x;  // Greater than or equal to image width
    grid_size.y = (img.rows + block_size.y - 1)/block_size.y;  // Greater than or equal to image height

    cv::Mat img_padded = img.clone();
    cv::copyMakeBorder(img_padded, img_padded, padding_y, padding_y, padding_x, padding_x, cv::BORDER_CONSTANT);

    GpuMat img_d(img_padded);
    GpuMat layered_hist_d(layered_hist);
    GpuMat nodes_d(tree_nodes);
    GpuMat features_d(features);


//    compute_layered_hist_gpu<<<grid_size,block_size>>>(img_d, layered_hist_d, nodes_d, features_d, lbl_count, padding_x, padding_y);
    compute_layered_hist_gpu_2b<<<grid_size,block_size>>>(img_d, layered_hist_d, nodes_d, features_d, lbl_count, padding_x, padding_y);
    gpuErrchk( cudaPeekAtLastError() );
    cudaDeviceSynchronize();
    layered_hist_d.download(layered_hist);
}
