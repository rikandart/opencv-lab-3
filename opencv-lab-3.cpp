#define MISHA
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

// генерация случайных индексов в матрице
void generate_rand_rgb(const Mat& data, const unsigned K, std::vector<Vec3b>& res) {
	std::vector<unsigned> i_res, j_res;
	std::default_random_engine generator;
	std::normal_distribution<double> distribution_i(0, data.rows/3);
	std::normal_distribution<double> distribution_j(0, data.cols/3);
	unsigned rand_i = 0, rand_j = 0;
	bool done = false;
	while (!done) {
		rand_i = abs(round(distribution_i(generator)));
		rand_j = abs(round(distribution_j(generator)));
		if(rand_i > data.rows) rand_i = data.rows;
		if(rand_j > data.cols) rand_j = data.cols;
		if (std::find(i_res.begin(), i_res.end(), rand_i) != i_res.end()
			&& std::find(j_res.begin(), j_res.end(), rand_j) != j_res.end()) continue;
		else {
			i_res.push_back(rand_i);
			j_res.push_back(rand_j);
		}
		if (i_res.size() == K && j_res.size() == K) done = true;
	}
	for (unsigned i = 0; i < K; i++) res.push_back(Vec3b(data.at<Vec3b>(i_res[i], j_res[i])));
}

// заливка кластеров
void cluster_fill(const unsigned K, const std::vector<std::vector<Point>>& clusters, Mat& out) {
	// заполним матрицу-результат для отображения на экране
	out = Mat(out.rows, out.cols, CV_32F);
	for (unsigned k = 0; k < K; k++) {
		unsigned color = 255 / (k + 1);
		for (Point i : clusters[k])	out.at<float>(i.x, i.y) = (float)(color);
	}
	out.convertTo(out, CV_8U);
}

void kmeans(const Mat& data, const unsigned K, Mat& out, unsigned& it, const unsigned max_iter = 10) {
	std::vector<Vec3b> centroids;
	std::vector<std::vector<Point>> clusters(K);
	std::vector<double> distances(K);
	generate_rand_rgb(data, K, centroids);
	bool done = false;
	it = 0;
	// ждем пока все центройды повторятся, либо пройдет максимальное кол-во итераций
	while(!done && it != max_iter) {
		// предварительная очитска кластеров
		for (unsigned k = 0; k < K; k++) clusters[k].clear();
		for (unsigned i = 0; i < data.rows; i++)
			for (unsigned j = 0; j < data.cols; j++) {
				for (unsigned k = 0; k < K; k++) {
					// евклидово расстояние
					distances[k] = sqrt(pow(data.at<Vec3b>(i, j)[0] - centroids[k][0], 2) +
						pow(data.at<Vec3b>(i, j)[1] - centroids[k][1], 2) +
						pow(data.at<Vec3b>(i, j)[2] - centroids[k][2], 2));
				}
				// ищем минимальное расстояние = ищем индекс минимального эл-та
				unsigned k = std::min_element(distances.begin(), distances.end()) - distances.begin();
				clusters[k].push_back(Point(i, j));
			}
		// пересчет центроидов
		unsigned done_count = 0;
		for (unsigned k = 0; k < K; k++) {
			unsigned cl_size = clusters[k].size();
			if (cl_size) {
				Vec3b tmp(centroids[k]);
				Vec3d sum(0, 0, 0);
				for (Point i : clusters[k]) sum += data.at<Vec3b>(i.x, i.y);
				// среднее по всем каналам
				centroids[k] = Vec3b(sum[0] / cl_size, sum[1] / cl_size, sum[2] / cl_size);
				if (centroids[k] == tmp) done_count++;
			}
		}
		// если все центроиды повторились, заканчиваем расчет
		if (done_count == K) done = true;
		it++;
	}
	cluster_fill(K, clusters, out);
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	Mat img = imread("E:/kmeans_img.png"), res = Mat::zeros(img.rows, img. cols, CV_8U);
	auto show_lam = [&]() {show(img, res, "Original", "Clustered");};
	while (true) {
		std::cout << "Введите кол-во кластеров, -1 для выхода > ";
		int k = 0, max_iter = 0;
		std::cin >> k;
		std::cout << std::endl;
		if (k == -1) break;
		std::cout << "Введите максимальное кол-во итераций > ";
		std::cin >> max_iter;
		std::cout << std::endl;
		if (k * max_iter <= 0) std::cout << "Данные введены неверно" << std::endl;
		else {
			unsigned it = 0;
			kmeans(img, k, res, it, max_iter);
			std::thread(show_lam).detach();
			std::cout << "Прошло итераций: " << it << std::endl;
		}
		system("pause");
		system("cls");
	}
	return 0;
}
#endif

