#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <cstdio>
#include <fstream>
#include "utils.hpp"

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utils/filesystem.hpp>

using namespace cv;
using namespace std;



string name_file(string root_name, int id, string extension) {
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];
	char buffer_number[20];  // maximum expected length of the float
	snprintf(buffer_number, 20, "%03d", id);
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%d-%m-%Y_%H-%M-%S", timeinfo);
	return root_name + "_" + string(buffer_number) + "_" + string(buffer) + extension;
}

string name_img(string prefix) {
	static int id = 0;
	return name_file("screenshot_" + prefix, id++, ".png");
}

string name_txt() {
	static int id = 0;
	return name_file("file", id++, ".txt");
}


void save_img_on_file(string output_folder, Mat img, string prefix) {
	if (!cv::utils::fs::exists(output_folder))
	{
		cv::utils::fs::createDirectory(output_folder);
	}
	try
	{
		imwrite((output_folder + "/" + name_img(prefix)).c_str(), img);
	}
	catch (const cv::Exception& ex)
	{
		fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
	}
}

void save_points_on_file(string output_folder, vector<Point2f> points) {
	if (!cv::utils::fs::exists(output_folder))
	{
		cv::utils::fs::createDirectory(output_folder);
	}
	ofstream fout(output_folder + "/" + name_txt());
	try
	{
		for (Point2f point: points) {
			fout << "[" << point.x << "," << point.y << "]" << endl;
		}
	}
	catch (const cv::Exception& ex)
	{
		fprintf(stderr, "Exception converting points to file: %s\n", ex.what());
	}
	fout.close();
}
