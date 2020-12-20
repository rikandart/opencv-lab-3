#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
using namespace cv;

int main(int argc, char* argv[])
{
	Mat image = imread("E:/wearing.jpg");
	// число кластеров
	const int K = 7;
	Mat rgb_data = Mat::zeros(image.cols * image.rows, 3, CV_32F);
	Mat labels;
	for (int i = 0; i < image.cols * image.rows; i++) {
		rgb_data.at<float>(i, 0) = (float)image.at<Vec3b>(i)[0] / 255.0;
		rgb_data.at<float>(i, 1) = (float)image.at<Vec3b>(i)[1] / 255.0;
		rgb_data.at<float>(i, 2) = (float)image.at<Vec3b>(i)[2] / 255.0;
	}
	kmeans(rgb_data, K, labels, TermCriteria(TermCriteria::EPS + TermCriteria::MAX_ITER, 10, 1.0), 5, KMEANS_PP_CENTERS);
	// окраска кластеризованных пикселей
	int colors[K];
	for (int i = 0; i < K; i++) colors[i] = 255 / (i + 1);
	Mat res = Mat(image.rows, image.cols, CV_32F);
	for (int i = 0; i < image.cols * image.rows; i++)
	{
		res.at<float>(i / image.cols, i % image.cols) = (float)(colors[labels.at<int>(0, i)]);
	}
	res.convertTo(res, CV_8U);
	imshow("Изображение", image);
	imshow("Кластеризация", res);
	waitKey(0);
	return 0;
}