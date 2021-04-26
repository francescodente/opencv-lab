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

string name_img() {
	static int id = 0;
	return name_file("screenshot", id++, ".png");
}

string name_txt() {
	static int id = 0;
	return name_file("file", id++, ".txt");
}


void save_img_on_file(Mat img) {
	try
	{
		imwrite(name_img().c_str(), img);
	}
	catch (const cv::Exception& ex)
	{
		fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
	}
}

void save_points_on_file(vector<Point2f> points) {
	ofstream fout(name_txt());
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

void add_key_handler(char key, Mat img, vector<Point2f>* points, Mat *mask) {
	if (key == SAVE_SCREEN_KEY)
	{
		save_img_on_file(img);
	}
	else if (key == SAVE_FILE_KEY)
	{
		save_points_on_file(*points);
	}
	else if (key == CLEAN_ALL_KEY)
	{
		*mask = Mat(img.size(), img.type(), Scalar(0, 0, 0));
	}
	else if (key == CLEAN_KEY)
	{
		(*points).pop_back();
		Mat working_mask(img.size(), img.type(), Scalar(0, 0, 0));
		for (Point2f p : *points) {
			circle(working_mask, p, 2, Scalar(0, 255, 255), 2);
		}
		*mask = working_mask;
	}
}