#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/core/cuda.hpp>

#define N 512

__global__ void add(int *a, int *b, int *c, cv::cuda::PtrStepSz<uchar> asdf) {
	c[blockIdx.x] = a[blockIdx.x] + b[blockIdx.x];
}

void kernel_wrapper(int *a, int *b, int *c)
{
	int *d_a, *d_b, *d_c;
	int size = N*sizeof(int);

	// Alloc space for device copies of a, b, c
	cudaMalloc((void **)&d_a, size);
	cudaMalloc((void **)&d_b, size);
	cudaMalloc((void **)&d_c, size);

    // Copy inputs to device
    cudaMemcpy(d_a, a, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, b, size, cudaMemcpyHostToDevice);

	cv::cuda::GpuMat test(cv::Mat_<uchar>::zeros(2,2));

    // Launch add() kernel on GPU with N blocks
    add<<<N,1>>>(d_a, d_b, d_c,test);
//	gpuErrchk( cudaPeekAtLastError() );
    // Copy result back to host
    cudaMemcpy(c, d_c, size, cudaMemcpyDeviceToHost);
    cudaFree(a); cudaFree(b); cudaFree(c);
}


__global__ void img_proc_kernel(cv::cuda::PtrStepSz<uchar3> img,
								cv::cuda::PtrStepSz<float> layered_hist,
								cv::cuda::PtrStepSz<int> tree_nodes,
								cv::cuda::PtrStepSz<float>features,
								int lbl_count,
								int padding)
{
	int xIndex = blockIdx.x * blockDim.x + threadIdx.x + padding;
	int yIndex = blockIdx.y * blockDim.y + threadIdx.y + padding;

	int n = features.cols/2;
	int* current_node = tree_nodes.ptr(0);

	int leftID = current_node[0];
	int ftrID, tau, fr1_x, fr1_y, fr2_x, fr2_y;
	float* feature;

	while(leftID != -1)
	{
		ftrID   = current_node[1];
		feature = features.ptr(ftrID);
		tau     = current_node[2];
		fr1_x   = xIndex + current_node[3];
		fr1_y   = yIndex + current_node[4];
		fr2_x   = xIndex + current_node[5];
		fr2_y   = yIndex + current_node[6];

		uchar3 ftr1{0};
		uchar3 ftr2{0};

		if(ftrID == 0)
		{
			ftr1 = img(fr1_y, fr1_x);
			ftr2 = img(fr2_y, fr2_x);
		}
		else
		{
			for (int i = 0; i < 2 * n; ++i) {
				uchar3 pxVal = img(fr1_y + i / n, fr1_x + i % n);
				ftr1.x = (uchar) (pxVal.x * feature[i]);
				ftr1.y = (uchar) (pxVal.y * (uchar) feature[i]);
				ftr1.z = (uchar) (pxVal.z * (uchar) feature[i]);
			}
		}

		uchar a[3];
		a[0] = ftr1.x-ftr2.x;
		a[1] = ftr1.y-ftr2.y;
		a[2] = ftr1.z-ftr2.z;
		uchar val = a[0]*a[0] + a[1]*a[1] + a[2]*a[2];

		if(val <= tau*tau)
			current_node = tree_nodes.ptr(leftID);
		else
			current_node = tree_nodes.ptr(leftID+1);
		leftID     = current_node[0];
	}

	img(xIndex,yIndex).x = 255;
	img(xIndex,yIndex).y = 0;
	img(xIndex,yIndex).z = 0;
}

void img_proc(cv::Mat& img, cv::Mat_<float>& layered_hist, cv::Mat_<int>& tree_nodes, cv::Mat_<float>& features, int lbl_count, int padding)
{
	dim3 block_size(32,16); // 512 threads
	dim3 grid_size;
	grid_size.x = (img.cols + block_size.x - 1)/block_size.x;  // Greater than or equal to image width
	grid_size.y = (img.rows + block_size.y - 1)/block_size.y;  // Greater than or equal to image height

	cv::copyMakeBorder(img, img, padding, padding, padding, padding, cv::BORDER_CONSTANT);

	cv::cuda::GpuMat img_d(img);
	cv::cuda::GpuMat layered_hist_d(layered_hist);
	cv::cuda::GpuMat nodes_d(tree_nodes);
	cv::cuda::GpuMat features_d(features);

	img_proc_kernel<<<grid_size,block_size>>>(img_d, layered_hist_d, nodes_d, features_d, lbl_count, padding);
	img_d.download(img);
}































