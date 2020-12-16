//#define MISHA
#ifdef MISHA
// opencv lib
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

// c++
#include <iostream>
#include <vector>
#include <cstdlib>
#include <math.h>

using namespace cv;

int main(int argc, char* argv[])
{
	Mat pic = imread("C:/vitr1.jpg");
	Mat edit = Mat(pic.rows, pic.cols, CV_32F);
	const int K = 10;
	//чет ряды, нечет колонки
	int centr_xy[K*2];
	int rasst[K];
	for (int i = 0; i < K; i++) {
		centr_xy[i*2] = rand() % pic.rows;
		centr_xy[i*2+1] = rand() % pic.cols;
	}
	//ргб центров
	double centr_rgb[K * 3];
	for (int i = 0; i < K; i++) {
		centr_rgb[i * 3] = pic.at<Vec3b>(centr_xy[i * 2], centr_xy[i * 2 + 1])[0];
		centr_rgb[i * 3 + 1] = pic.at<Vec3b>(centr_xy[i * 2], centr_xy[i * 2 + 1])[1];
		centr_rgb[i * 3 + 2] = pic.at<Vec3b>(centr_xy[i * 2], centr_xy[i * 2 + 1])[2];
	}
	int mult = pic.rows * pic.cols;
	Mat labels = Mat::zeros(1, mult, CV_8U);
	for (int a = 0; a < 10; a++) {
		for (int i = 0; i < pic.rows; i++)
			for (int j = 0; j < pic.cols; j++) {
				for (int k = 0; k < K; k++) {
					double r = pic.at<Vec3b>(i, j)[0] - centr_rgb[k * 3];
					double g = pic.at<Vec3b>(i, j)[1] - centr_rgb[k * 3 + 1];
					double b = pic.at<Vec3b>(i, j)[2] - centr_rgb[k * 3 + 2];
					rasst[k] = sqrt(r * r + g * g + b * b);
				}
				int b = 0;
				int min = rasst[0];
				for (int k = 0; k < K; k++) {
					if (rasst[k] < min) {
						min = rasst[k];
						b = k;
					}
				}
				labels.at<unsigned char>(0, i * pic.cols + j) = b;
			}
		int sum[K * 3];
		int n[K];
		//сумма по всему кластеру
		for (int b = 0; b < labels.cols; b++)
			for(int k = 0; k < K; k++)
				if (labels.at<unsigned char>(0, b) == k) {
					sum[k * 3] += pic.at<Vec3b>(b / pic.cols, b % pic.cols)[0];
					sum[k * 3 + 1] += pic.at<Vec3b>(b / pic.cols, b % pic.cols)[1];
					sum[k * 3 + 2] += pic.at<Vec3b>(b / pic.cols, b % pic.cols)[2];
					n[k]++;
				}
		//центры
		int povt = 0;
		for (int k = 0; k < K; k++) {
			int tmpr = centr_rgb[k * 3], tmpg = centr_rgb[k * 3 + 1],
				tmpb = centr_rgb[k * 3 + 2];
			if (n[k] != 0) {
				centr_rgb[k * 3] = ((double)sum[k * 3]) / n[k];
				centr_rgb[k * 3 + 1] = ((double)sum[k * 3 + 1]) / n[k];
				centr_rgb[k * 3 + 2] = ((double)sum[k * 3 + 2]) / n[k];
				if (centr_rgb[k * 3] == tmpr && centr_rgb[k * 3 + 1] == tmpg &&
					centr_rgb[k * 3 + 2] == tmpb) povt++;
			}
		}
		if (povt == K) break;
	}
	//заполнение
	int colors[K];
	for (int i = 0; i < K; i++) {
		colors[i] = 255 / (i + 1);
	}
	for (int i = 0; i < mult; i++) {
		edit.at<float>(i / pic.cols, i % pic.cols) =
			(float)(colors[labels.at<unsigned char>(0, i)]);
	}
	edit.convertTo(edit, CV_8U);
	imshow("Pic", pic);
	imshow("Edit", edit);
	waitKey(0);
	return 0;
}
#endif

