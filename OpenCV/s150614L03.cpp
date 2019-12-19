#include "SIFT.h"
#define DIST_RATIO_THR 0.49



void customGaussianBlur(const Mat &src, Mat &dst, double sigma) {
	////////////////////////////////////////////////////////////
	// ����þ� Ŀ���� ������ �̹����� �����ϴ� �Լ�
	// Ŀ���� ũ��� 2*(4*sigma)+1 (�Ҽ����� �ø� �Ǵ� �ݿø����� ó���Ѵ�)
	// ����þ� Ŀ���� float���� ���� ����
	// ����þ� Ŀ���� Ŀ�� �� ��� �� ��ü�� �������� ������ 0~1������ ���� �������� ����ȭ
	// ����þ� Ŀ���� �̹����� �����Ҷ��� filter2D() �Լ��� ���
	////////////////////////////////////////////////////////////

	// Declare variable. (Kernel, Kernel size)
	int kernel_size;
	Mat gau_kernel;
	//���� ����
	int i, j, mid, x, y;
	float temp, sum = 0;
	double temp2;
	temp2 = 2 * ceil(4 * sigma) + 1;
	kernel_size = (int)temp2;
	gau_kernel.create(kernel_size, kernel_size, CV_32F);
	mid = kernel_size / 2;
	// Compute gaussian kernel value
	//���� ����
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
	//���� ����
	for (i = 0; i < kernel_size; i++)
		for (j = 0; j < kernel_size; j++) 
			gau_kernel.at<float>(i, j) /= sum;
	// Apply Gaussian kernel
	//���� ����
	filter2D(src, dst, -1, gau_kernel);
}
//////////////////////////////////////////////


int MatchDescriptor(const Mat &descriptor1, const Mat &descriptor2, vector<int> &matchingIdx) {
	///////////////////////////////////////////////////////////////
	// �̹���1�� ��ũ���͵�� �̹���2�� ��ũ���͵� �� ��Ī�Ǵ� ����� ������ ����ϴ� �Լ�
	// �̹���1�� 1���� ��ũ���Ϳ� �̹���2�� ��� ��ũ���� ������ L2 �Ÿ��� ��� 
	// ���� ����� �Ÿ� (d1) �� �ι�°�� ����� �Ÿ�(d2) �� ���� (d1/d2) �� ���� ������ �Ӱ谪 (DIST_RATIO_THR) ���� �۴ٸ� ��Ī���� �Ǵ�
	// machingIdx ���� ��Ī�� �� �ε����� ���� (machingIdx[idx1] = idx2, idx1�� �̹���1�� ��ũ������ �ε����̰�, idx2�� ��Ī�� �̹���2�� ��ũ������ �ε���)
	// �Լ��� ���ϰ����δ� ��Ī�� ����� ������ ���
	//////////////////////////////////////////////////////////////
	int numMatches = 0;
	int i, j, k, r1 = descriptor1.rows, r2 = descriptor2.rows, c2 = 128;
	int i1,j1,flag=0;
	float d1=FLT_MAX, d2=FLT_MAX,sum;
	double temp=0;
	matchingIdx.resize(descriptor1.rows);
	//���� ����
	
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