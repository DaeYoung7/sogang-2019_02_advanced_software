#include <iostream>
#include <time.h>// 시간 측정 위해서
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;

Mat input_im, dst, src;
clock_t start_time, end_time;

void blur_maked(int threshold_value, void *userData);
void blur_opencvCommand(int kernelsize, void *userData);
string type2str(int type);

using namespace std;

void blur_opencvCommand(
	int threshold_value, void *userData) {

	Size ksize;
	Point anchor = Point(-1, -1);
	int borderType = BORDER_DEFAULT;

	string &win_name =
		*(static_cast<string*>(userData));
	start_time = clock();
	// 시간 측정할 함수 호출
	if((threshold_value!=0)&&(threshold_value % 2 == 0)) threshold_value++;
	if(threshold_value!=0)
	{
		ksize = Size(threshold_value, threshold_value);
		blur(input_im, dst, ksize, anchor,
			borderType); // call OpenCV function
	}
	end_time = clock();
	cout << "blur value : " << threshold_value
		<< ", Exec Time : "
		<< (double)(end_time - start_time)
		<< " (msec)" << endl;
	imshow(win_name, dst);
}

void blur_maked(int threshold_value,void *userData)
{
	int temp;
	string &win_name =
		*(static_cast<string*>(userData));
	
	if ((threshold_value != 0) && (threshold_value % 2 == 0)) threshold_value++;
	int half = threshold_value / 2;
	dst = Mat(input_im.rows, input_im.cols,CV_8UC1, Scalar(0));
	src = Mat(input_im.rows + 2 * half, input_im.cols + 2 * half,CV_8UC1, Scalar(0));
	int** kernel = new int*[threshold_value];
	int ex = threshold_value * threshold_value;
	start_time = clock();
	if (threshold_value > 2)
	{
		for (int i = 0; i < threshold_value; i++) {
			kernel[i] = new int[threshold_value];
			for (int j = 0; j < threshold_value; j++)
				kernel[i][j] = 1;
		}

		for (int i = half; i < (src.rows-half); i++) {
			for (int j = half; j < (src.cols-half); j++) {
				src.at<uchar>(i, j) = input_im.at<uchar>(i-half, j-half);
			}
		}

		for (int i = 0; i < (src.rows - threshold_value); i++)
		{
			for (int j = 0; j < (src.cols - threshold_value); j++)
			{
				temp = 0;
				for (int k = 0; k < threshold_value; k++)
				{
					for (int p = 0; p < threshold_value; p++)
					{
						temp += kernel[k][p] * src.at<uchar>(i + k, j + p);
					}
				}
				temp /= ex;
				dst.at<uchar>(i, j) = temp;
			}
		}
		for (int i = 0; i < threshold_value; i++) 
			delete kernel[i];
		delete[] kernel;
	}
	end_time = clock();
	cout << "blur value : " << threshold_value
		<< ", Exec Time : "
		<< (double)(end_time - start_time)
		<< " (msec)" << endl;
	if(threshold_value<3) imshow(win_name, input_im);
	else imshow(win_name, dst);
}

string type2str(int type) {
	// https://stackoverflow.com/questions/10167534/how-to-find-out-what-type-of-a-mat-object-is-with-mattype-in-opencv
	string r;

	uchar depth = type & CV_MAT_DEPTH_MASK;
	uchar chans = 1 + (type >> CV_CN_SHIFT);

	switch (depth) {
	case CV_8U:  r = "8U"; break;
	case CV_8S:  r = "8S"; break;
	case CV_16U: r = "16U"; break;
	case CV_16S: r = "16S"; break;
	case CV_32S: r = "32S"; break;
	case CV_32F: r = "32F"; break;
	case CV_64F: r = "64F"; break;
	default:     r = "User"; break;
	}

	r += "C";
	r += (chans + '0');

	return r;
}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		cout << "[프로그램 사용법]" << endl;
		cout << "명령문 : snnnnnnHW01.exe imageFile flag<ent>" << endl;
		cout << "flag : 1 - opencv function 사용" << endl;
		cout << "flag : 2 - loop로 brute force 사용" << endl;
		return 0;
	}
	//파일 주소는 argv[1]로 들어옴
	input_im = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	if (input_im.empty())
	{
		cout << "File open Error!" << endl;
		return -1;
	}
	cout << "Image size :" << input_im.size() << ", Type:" << type2str(input_im.type()) << endl;

	string window_name = "Image Binary Thresholding Window";
	namedWindow(window_name);

	//Create track bar to change kernel size
	int start_value = 0;
	int max_value = 15;
	if(*argv[2]=='1') createTrackbar("KernalSize", window_name, &start_value, max_value, blur_opencvCommand, static_cast<void*>(&window_name));
	else if(*argv[2]=='2') 
		createTrackbar("KernelSize", window_name, &start_value, max_value, blur_maked, static_cast<void*>(&window_name));
	else
	{
		cout << "flag Error!" << endl;
		return -1;
	}
	imshow(window_name, input_im);
	waitKey(0);

	return 0;
}

