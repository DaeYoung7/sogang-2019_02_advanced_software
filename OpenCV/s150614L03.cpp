#include "SIFT.h"
#define DIST_RATIO_THR 0.49



void customGaussianBlur(const Mat &src, Mat &dst, double sigma) {
	////////////////////////////////////////////////////////////
	// 가우시안 커널을 생성해 이미지에 적용하는 함수
	// 커널의 크기는 2*(4*sigma)+1 (소수점은 올림 또는 반올림으로 처리한다)
	// 가우시안 커널은 float형의 값을 가짐
	// 가우시안 커널은 커널 값 계산 후 전체의 총합으로 나눠서 0~1사이의 값을 가지도록 정규화
	// 가우시안 커널을 이미지에 적용할때는 filter2D() 함수를 사용
	////////////////////////////////////////////////////////////

	// Declare variable. (Kernel, Kernel size)
	int kernel_size;
	Mat gau_kernel;
	//내용 구현
	int i, j, mid, x, y;
	float temp, sum = 0;
	double temp2;
	temp2 = 2 * ceil(4 * sigma) + 1;
	kernel_size = (int)temp2;
	gau_kernel.create(kernel_size, kernel_size, CV_32F);
	mid = kernel_size / 2;
	// Compute gaussian kernel value
	//내용 구현
	for (i = 0; i < kernel_size; i++) {
		for (j = 0; j < kernel_size; j++) {
			x = i - mid;
			y = j - mid;
			temp = float(exp((((pow(x, 2) + pow(y, 2))*(-1.0)) / (2 * pow(sigma, 2)))) / (2 * CV_PI*pow(sigma, 2)));
			sum += temp;
			gau_kernel.at<float>(i, j) = temp;
		}
	}
	// Normalize the kernel
	//내용 구현
	for (i = 0; i < kernel_size; i++)
		for (j = 0; j < kernel_size; j++) 
			gau_kernel.at<float>(i, j) /= sum;
	// Apply Gaussian kernel
	//내용 구현
	filter2D(src, dst, -1, gau_kernel);
}
//////////////////////////////////////////////


int MatchDescriptor(const Mat &descriptor1, const Mat &descriptor2, vector<int> &matchingIdx) {
	///////////////////////////////////////////////////////////////
	// 이미지1의 디스크립터들과 이미지2의 디스크립터들 중 매칭되는 페어의 개수를 계산하는 함수
	// 이미지1의 1개의 디스크립터와 이미지2의 모든 디스크립터 사이의 L2 거리를 계산 
	// 가장 가까운 거리 (d1) 와 두번째로 가까운 거리(d2) 의 비율 (d1/d2) 을 구해 설정된 임계값 (DIST_RATIO_THR) 보다 작다면 매칭으로 판단
	// machingIdx 에는 매칭이 된 인덱스를 저장 (machingIdx[idx1] = idx2, idx1은 이미지1의 디스크립터의 인덱스이고, idx2는 매칭된 이미지2의 디스크립터의 인덱스)
	// 함수의 리턴값으로는 매칭된 페어의 개수를 출력
	//////////////////////////////////////////////////////////////
	int numMatches = 0;
	int i, j, k, r1 = descriptor1.rows, r2 = descriptor2.rows, c2 = 128;
	int i1,j1,flag=0;
	float d1=FLT_MAX, d2=FLT_MAX,sum;
	double temp=0;
	matchingIdx.resize(descriptor1.rows);
	//내용 구현
	
	for (i = 0; i < r1; i++) {
		d1 = FLT_MAX;
		d2 = FLT_MAX;
		flag = 0;
		for (j = 0; j < r2; j++) {
			temp = 0;
			for (k = 0; k < c2; k++) {
				temp += pow((descriptor1.at<float>(i, k) - descriptor2.at<float>(j, k)), 2);
			}
			sum = float(sqrt(temp));
			if (j == 0) {
				d1 = sum;
				i1 = i;
				j1 = 0;
			}
			else {
				if (d1 > sum) {
					d2 = d1;
					d1 = sum;
					flag = 1;
					j1 = j;
				}
			}
		}
		if (flag&&((d1 / d2) < DIST_RATIO_THR)) {
			numMatches++;
			matchingIdx[i1] = j1;
		}
		else matchingIdx[i1] = -1;
	}
	return numMatches;
}