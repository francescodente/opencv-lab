#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <cstdio>
#include <fstream>

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>


using namespace cv;
using namespace std;


const char SAVE_SCREEN_KEY = 's';
const char SAVE_FILE_KEY = 'p';
const char CLEAN_ALL_KEY = 'c';
const char CLEAN_KEY = 'k';
const char CAPTURE_CALIBRATION = ' ';

void save_img_on_file(string output_folder, Mat img, string prefix = "");
void save_points_on_file(string output_folder, vector<Point2f> points);
