#include "SIFT.h"
#define DIST_RATIO_THR 0.49

//실습에서 작성한 코드 사용
int MatchDescriptor(const Mat &descriptor1, const Mat &descriptor2, vector<int> &matchingIdx) {
	int numMatches = 0;
	int i, j, k, r1 = descriptor1.rows, r2 = descriptor2.rows, c2 = 128;
	int i1, j1, flag = 0;
	float d1 = FLT_MAX, d2 = FLT_MAX, sum;
	double temp = 0;
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
		if (flag && ((d1 / d2) < DIST_RATIO_THR)) {
			numMatches++;
			matchingIdx[i1] = j1;
		}
		else matchingIdx[i1] = -1;
	}
	return numMatches;
}


Mat stack_imgs(const Mat &im1, const Mat &im2) {
	//두개의 이미지를 하나의 이미지로 합치는 함수
	//두개의 이미지 크기가 다를 수 있기 때문에 opencv의 hconcat, hstack을 사용하지 않고, copyTo 함수를 사용한다.
	//두개의 이미지가 들어갈수있는 큰 하나의 Mat stacked을 만들고 (Mat type은 이미지1, 2와 같게 한다),
	//copyTo 함수를 사용해 이미지1과 이미지2를 stacked에 복사한다.

	Mat stacked;
	int row;
	int col=im1.cols+im2.cols;
	if (im1.rows > im2.cols) row = im1.rows;
	else row = im2.rows;
	stacked.create(row, col, im1.type());

	//img1, img2를 stacked에 복사
	//내용 구현
	im1.copyTo(stacked(Rect(0,0,im1.cols,im1.rows)));
	im2.copyTo(stacked(Rect(im1.cols,0,im2.cols,im2.rows)));

	return stacked;
}


Mat DrawMatches(const Mat &im1, const Mat &im2, vector<KeyPoint> &keypoints1, vector<KeyPoint> &keypoints2, vector<int> &matchingIdx) {
	//매칭페어의 정보를 받아서 매칭된 키포인트의 위치와 매칭 페어 사이의 연결 선을 그리는 함수
	//stack_imgs() 함수를 사용해 2개의 이미지를 연결해 하나의 이미지로 만듦
	//키포인트의 위치는 opencv의 circle() 함수 사용, 연결선은 opencv의 line() 함수 사용

	Mat result_im;
	result_im = stack_imgs(im1, im2);//
	int i, j;
	//Draw keypoint location and line between matching pair
	//내용 구현
	for (i = 0; i < keypoints1.size(); i++) {
		if (matchingIdx[i] >= 0) {
			circle(result_im, Point(keypoints1[i].pt.x, keypoints1[i].pt.y), 1, Scalar(0, 255, 0));
			circle(result_im, Point(keypoints2[matchingIdx[i]].pt.x + im1.cols, keypoints2[matchingIdx[i]].pt.y), 1, Scalar(0, 255, 0));
			line(result_im, Point(keypoints1[i].pt.x, keypoints1[i].pt.y), Point(keypoints2[matchingIdx[i]].pt.x + im1.cols, keypoints2[matchingIdx[i]].pt.y),Scalar(0,255,0));
		}
	}
	return result_im;
}