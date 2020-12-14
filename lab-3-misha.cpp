//#define MISHA
#ifdef MISHA
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <cmath>

using namespace cv;

void main() {
	Mat img1, img2;
	img1 = imread("C:\\vlcsnap-2020-11-25-10h10m50s000.png");
	img2 = imread("C:\\vlcsnap-2020-11-25-10h10m42s459.png");
	Mat razn;
	// ���������� ����������� �����������
	absdiff(img1, img2, razn);
	imshow("�������", razn);
	Mat canny, thresh, ba;
	// ���������� ����� �����
	Canny(img2, canny, 50, 200);
	imshow("������� �����", canny);
	std::cout << canny.rows << " " << canny.cols << std::endl;
	std::cout << razn.rows << " " << razn.cols << std::endl;
	Mat razn2, canny2;
	cvtColor(razn, razn2, COLOR_RGB2GRAY);
	
	bitwise_and(canny, razn2, ba);
	imshow("��������� �", ba);
	threshold(ba, ba, 5, 255, THRESH_BINARY_INV);
	imshow("��������� �����������", ba);
	Mat element = getStructuringElement(MORPH_RECT, Size(6, 6), Point(3, 3));
	morphologyEx(ba, ba, MORPH_ERODE, element);
	imshow("��������������� ����������", ba);

	waitKey(0);

}

#endif