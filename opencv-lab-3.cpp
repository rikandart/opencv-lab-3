//#define MISHA
#ifndef MISHA
// opencv lib
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

// c++
#include <iostream>
#include <vector>
#include <thread>
#include <math.h>

#define THRESHOLD 65
#define THRESHOLD_2 20

using namespace cv;

void show(const Mat& frame_1, const Mat& frame_2, const char* name_1, const char* name_2) {
	namedWindow(name_1, WINDOW_NORMAL | WINDOW_FREERATIO | WINDOW_GUI_EXPANDED);
	resizeWindow(name_1, frame_1.cols, frame_1.rows);
	moveWindow(name_1, 0, 0);
	imshow(name_1, frame_1);
	namedWindow(name_2, WINDOW_NORMAL | WINDOW_FREERATIO | WINDOW_GUI_EXPANDED);
	resizeWindow(name_2, frame_2.cols, frame_2.rows);
	moveWindow(name_2, frame_1.cols, 0);
	imshow(name_2, frame_2);
	waitKey(0);
}

void show(const Mat& frame_1, const char* name_1) {
	namedWindow(name_1, WINDOW_NORMAL | WINDOW_FREERATIO | WINDOW_GUI_EXPANDED);
	resizeWindow(name_1, frame_1.cols, frame_1.rows);
	moveWindow(name_1, 0, 0);
	imshow(name_1, frame_1);
	waitKey(0);
}

// квантование
void editQuantizeLevel(const unsigned level, const Mat& img, Mat& dest_img) {
	if (remainder(256.0, level) != 0.0 || level == 0 || level == 1) {
		std::cout << "Incorrect quantize level" << std::endl;
		return;
	}
	// заполнение матрицы нулями
	dest_img = Mat::zeros(img.rows, img.cols, CV_8U);
	// шаг квантования
	const double step = 255.0 / (level - 1);
	// пробегаем по пикселям x и y
	for (unsigned i = 0; i < img.cols; i++) {
		for (unsigned j = 0; j < img.rows; j++) {
			unsigned br = img.at<unsigned char>(j, i);
			for (unsigned k = 0; k < level; k++) {
				// смотрим в каком интервале лежит число
				if (br >= k * step && br <= (k + 1) * step / 2) {
					// смотрим, какая разница меньше и присваиваем нужную градацию
					dest_img.at<unsigned char>(j, i) = k * step;
					break;
				}
				else if (br >= k * step / 2 && br <= (k + 1) * step) {
					dest_img.at<unsigned char>(j, i) = (k + 1) * step;
					break;
				}
			}
		}
	}
}

// гистограммы проекций
void getHist(const Mat& image, Mat* out_hist_arr)
{
	// гор. гистограмма
	out_hist_arr[0] = Mat::zeros(image.cols, image.rows, CV_8U);
	// верт. гистограмма
	out_hist_arr[1] = Mat::zeros(image.rows, image.cols, CV_8U);
	for (int i = 0; i < image.rows; i++) {
		int hist_i = -1;
		for (int j = 0; j < image.cols; j++) {
			if (image.at<unsigned char>(i, j) > 0) {
				out_hist_arr[0].at<unsigned char>(image.cols-1 - ++hist_i, i) = 255;
			}
		}
	}
		
	for (int j = 0; j < image.cols; j++) {
		int hist_i = -1;
		for (int i = 0; i < image.rows; i++) {
			if (image.at<unsigned char>(i, j) > 0) {
				out_hist_arr[1].at<unsigned char>(image.rows-1 - ++hist_i, j) = 255;
			}
		}
	}
	for(int i = 0; i < out_hist_arr[0].cols; i++)
		out_hist_arr[0].at<unsigned char>(out_hist_arr[0].rows-1-THRESHOLD, i) = 255;
	for (int i = 0; i < out_hist_arr[1].cols; i++)
		out_hist_arr[1].at<unsigned char>(out_hist_arr[1].rows - 1 - THRESHOLD_2, i) = 255;
	bitwise_not(out_hist_arr[0], out_hist_arr[0]);
	bitwise_not(out_hist_arr[1], out_hist_arr[1]);
}

// получение стробов
void proj(const Mat& image, Mat* strobes) {
	// x strobe
	strobes[0] = Mat::zeros(image.rows, image.cols, CV_8U);
	// y stobe
	strobes[1] = Mat::zeros(image.rows, image.cols, CV_8U);
	for (int i = 0; i < image.rows; i++) {
		unsigned count_i = 0;
		for (int j = 0; j < image.cols; j++) {
			static bool write = false;
			if (image.at<unsigned char>(i,j) > 0 && !write)
				count_i++;
			// сравниваем с порогом
			if (count_i > THRESHOLD && !write) {
				j = 0;
				write = true;
			}
			if (write)
				strobes[0].at<unsigned char>(i, j) = image.at<unsigned char>(i, j);
		}
	}
	for (int j = 0; j < image.cols; j++) {
		unsigned count_j = 0;
		for (int i = 0; i < image.rows; i++) {
			static bool write = false;
			if (image.at<unsigned char>(i, j) > 0)
				count_j++;
			if (count_j > THRESHOLD_2 && !write) {
				i = 0;
				write = true;
			}
			if (write)
				strobes[1].at<unsigned char>(i, j) = image.at<unsigned char>(i, j);
		}
	}
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	Mat frame_1, frame_2, outline, opened, closed;
	Mat ones = Mat::ones(2, 2, CV_8U);
	return 0;
}
#endif

