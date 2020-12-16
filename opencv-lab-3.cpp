//#define MISHA
#ifndef MISHA
// opencv lib
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

// c++
#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib>
#include <random>
#include <algorithm>
#include <numeric>
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

// генераци€ случайных индексов в матрице
void generate_rand_indices(const Mat& data, const unsigned K, std::vector<Point>& res) {
	std::vector<unsigned> i_res, j_res;
	std::default_random_engine generator;
	std::normal_distribution<double> distribution_i(0, data.rows/1.5);
	std::normal_distribution<double> distribution_j(0, data.cols/1.5);
	unsigned rand_i = 0, rand_j = 0;
	bool done = false;
	while (!done) {
		rand_i = abs(round(distribution_i(generator)));
		rand_j = abs(round(distribution_j(generator)));
		if (std::find(i_res.begin(), i_res.end(), rand_i) != i_res.end()
			&& std::find(j_res.begin(), j_res.end(), rand_j) != j_res.end()) continue;
		else {
			i_res.push_back(rand_i);
			j_res.push_back(rand_j);
		}
		if (i_res.size() == K && j_res.size() == K) done = true;
	}
	for (unsigned i = 0; i < K; i++) res.push_back(Point(i_res[i], j_res[i]));
}

// заливка кластеров
void cluster_fill(const unsigned K, const std::vector<Point>& centroids,
				const std::vector<std::vector<Point>>& clusters, Mat& out) {
	// заполним матрицу-результат дл€ отображени€ на экране
	out = Mat(out.rows, out.cols, CV_32F);
	for (unsigned k = 0; k < K; k++) {
		unsigned color = 255 / (k + 1);
		for (Point i : clusters[k])	out.at<float>(i.x, i.y) = (float)(color);
	}
	out.convertTo(out, CV_8U);
}

void kmeans(const Mat& data, const unsigned K, Mat& out, const unsigned iter = 10) {
	std::vector<Point> centroids;
	std::vector<std::vector<Point>> clusters(K);
	std::vector<double> distances(K);
	generate_rand_indices(data, K, centroids);
	bool done = false;
	auto acc_func = [](Point& p1, Point& p2)->Point {
		return Point(p1.x+p2.x, p1.y+p2.y);
	};
	for (unsigned it = 0; it < iter; it++) {
		// предварительна€ очитска кластеров
		for (unsigned k = 0; k < K; k++) clusters[k].clear();
		for (unsigned i = 0; i < data.rows; i++)
			for (unsigned j = 0; j < data.cols; j++) {
				for (unsigned k = 0; k < K; k++) {
					// евклидово рассто€ние
					distances[k] = sqrt(pow(data.at<Vec3b>(i, j)[0] - data.at<Vec3b>(centroids[k].x, centroids[k].y)[0], 2) +
						pow(data.at<Vec3b>(i, j)[1] - data.at<Vec3b>(centroids[k].x, centroids[k].y)[1], 2) +
						pow(data.at<Vec3b>(i, j)[2] - data.at<Vec3b>(centroids[k].x, centroids[k].y)[2], 2));
				}
				// ищем минимальное рассто€ние = ищем индекс минимального эл-та
				unsigned k = std::min_element(distances.begin(), distances.end()) - distances.begin();
				clusters[k].push_back(Point(i, j));
			}
		unsigned done_count = 0;
		// пересчет центроидов
		for (unsigned k = 0; k < K; k++) {
			if (clusters[k].size()) {
				Point sum = std::accumulate(clusters[k].begin(), clusters[k].end(), centroids[k], acc_func);
				Point avg(sum.x / clusters[k].size(), sum.y / clusters[k].size());
				if (avg.x == centroids[k].x && avg.y == centroids[k].y) done_count++;
				else centroids[k] = avg;
			}
		}
		if (done_count == K) done = true;
	}
	cluster_fill(K, centroids, clusters, out);
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	Mat img = imread("E:/kmeans_img.png"), res = Mat::zeros(img.rows, img. cols, CV_8U);
	auto show_lam = [&]() {show(img, res, "Original", "Clustered");};
	while (true) {
		std::cout << "¬ведите кол-во кластеров, -1 дл€ выхода > ";
		int k = 0, iter = 0;
		std::cin >> k;
		std::cout << std::endl;
		if (k == -1) break;
		std::cout << "¬ведите кол-во итераций > ";
		std::cin >> iter;
		std::cout << std::endl;
		if (k * iter <= 0) std::cout << "ƒанные введены неверно" << std::endl;
		kmeans(img, k, res, iter);
		std::thread(show_lam).detach();
		system("cls");
	}
	return 0;
}
#endif

