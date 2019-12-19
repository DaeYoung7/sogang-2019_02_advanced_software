#include <iostream>
#include <time.h>// 시간 측정 위해서
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;

Mat input_im, dst,src;
clock_t start_time, end_time;
void moving_opencvCommand(int ksize, void *userData);
string type2str(int type);

using namespace std;

typedef struct pqueue {
	int v;
	pqueue* link,*mid;
}pqueue;
//1->필터가 이미지의 열을 이동함에 따라 ksize만큼 큐에서 빼고 더할 때 사용되는 큐(세로)
//2->필터가 이미지에서 위치한 행이 바뀔 때 ksize만큼 큐에서 빼고 더할 때 사용되는 큐(가로)
//mid,mid2->2번째 큐에서 ksize만큼 값을 변경시킬 때 사용
pqueue* front1, *rear1,*front2,*rear2,*mid2,*walk;
pqueue* temp1,*temp2;

int inqueue(int *arr,int r,int c,int ksize,int flag) {
	int i, j,sum,t=0,k=0;
	if (flag==1)//맨 처음, 필터와 이미지가 겹치는 부분 전부 큐에 넣기
	{
		for (j = 0; j < ksize; j++) 
		{
			sum = 0;
			for (i = 0; i < ksize; i++)//행->열이 아닌 열->행으로 접근(필터가 오른쪽으로 이동하기 때문)
			{
				t++;
				temp1 = new pqueue;
				temp2 = new pqueue;
				temp1->v = (int)src.at<uchar>(i, j);
				temp2->v = temp1->v;
				temp1->mid = temp2->mid = NULL;
				temp1->link = temp2->link = NULL;
				sum += src.at<uchar>(i, j);
				if (t == (ksize + 1)) {
					mid2 = temp2;
					walk = mid2;
				}
				else if ((t%ksize == 1)&&(j!=0))
				{
					walk->mid = temp2;
					walk = temp2;
				}
				if ((i == 0) && (j == 0)) {
					front1 = rear1 = temp1;
					front2 = rear2 = temp2;
				}
				else {
					rear1->link = temp1;
					rear2->link = temp2;
					rear1 = temp1;
					rear2 = temp2;
				}
			}
			arr[i] = sum;
		}
	}
	else if (flag == 2)//이미지에서 필터의 행이 바뀔 때
	{
		temp2 = front2;
		front2 = front2->link;
		delete temp2;
		t = ksize-2;
		walk = mid2;
		while (walk)
		{
			arr[c - 1] -= walk->v;
			walk->v = (int)src.at<uchar>(r, c);
			arr[c - 1] += walk->v;
			walk = walk->mid;
			c++;
		}
		mid2 = mid2->link;
		temp2 = new pqueue;
		temp2->v = (int)src.at<uchar>(r, c);
		temp2->link = temp2->mid = NULL;
		rear2->link = temp2;
		rear2 = temp2;
	}
	else//이미지에서 필터의 열이 바뀔 때
	{
		sum = 0;
		for (i = 0; i < ksize; i++)
		{
			temp1 = new pqueue;
			temp1->link = NULL;
			temp1->v = (int)src.at<uchar>(r + i, c);
			sum+= src.at<uchar>(r + i, c);
			rear1->link = temp1;
			rear1 = temp1;
		}
		arr[ksize - 1] = sum;
	}
	sum = 0;
	for (i = 0; i < ksize; i++)
		sum += arr[i];
	sum=sum / (ksize*ksize);
	
	return sum;
}

void dequeue(int*arr,int ksize)
{
	int i;
	for (i = 0; i < ksize; i++)
	{
		arr[i] = arr[i + 1];
		temp1 = front1;
		front1 = front1->link;
		delete temp1;
	}
}

void moving_opencvCommand(
	int ksize, void *userData) {
	int i,j,t,check=0;
	string &win_name =
		*(static_cast<string*>(userData));
	if (ksize == 0) {
		imshow(win_name, input_im);
		return;
	}
	if (ksize % 2 == 0) ksize++;
	int half = ksize / 2;
	copyMakeBorder(input_im, src,  half, half, half, half, BORDER_CONSTANT, Scalar(0));
	dst = Mat(input_im.rows, input_im.cols, CV_8UC1, Scalar(0));
	//filter = Mat(ksize, ksize, CV_8UC1, Scalar(0));

	start_time = clock();
	// 시간 측정할 함수 호출
	if (ksize > 2) {
		int*arr = new int[ksize];
		for (i = 0; i < input_im.rows; i++)
		{
			for (j = 0; j < input_im.cols; j++)
			{
				if ((j == 0)&&(i==0))
					t=inqueue(arr,0,0,ksize,1);
				else if ((i != 0) && (j == 0))
				{
					t = inqueue(arr, i+ksize-1, 0, ksize, 2);
				}
				else {
					dequeue(arr, ksize);
					t = inqueue(arr, i, j + ksize - 1, ksize, 0);
				}
				dst.at<uchar>(i, j) = t;
			}
		}
		while (front1)
		{
			temp1= front1;
			front1 = front1->link;
			delete temp1;
		}
		while (front2)
		{
			temp2 = front2;
			front2 = front2->link;
			delete temp2;
		}
		//delete[] arr;
	}
	end_time = clock();
	cout << "Kernel Size : " << ksize
		<< ", Exec Time : "
		<< (double)(end_time - start_time)
		<< " (msec)" << endl;
	imshow(win_name, dst);
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
		cout << "명령문 : snnnnnnHW01.exe imageFile KernalSize<ent>" << endl;
		return 0;
	}
	//파일 주소는 argv[1]로 들어옴
	input_im = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	if (input_im.empty())
	{
		cout << "File open Error!" << endl;
		return -1;
	}
	cout << "Filename = "<<argv[1]<<"Image size :" << input_im.size() << ", Type:" << type2str(input_im.type()) << endl;

	string window_name = "movingAvrg HW";
	namedWindow(window_name);

	//Create track bar to change kernel size
	int start_value = 0;
	int max_value = 15;
	int ksize = 0;
	for (int i = 0; i < strlen(argv[2]); i++)
		ksize += ((argv[2][i]-'0') * pow(10, strlen(argv[2]) - 1 - i));
	moving_opencvCommand(ksize,static_cast<void*>(&window_name));
	waitKey(0);

	return 0;
}
