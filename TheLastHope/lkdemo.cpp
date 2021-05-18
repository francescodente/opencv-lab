#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>
#include <ctype.h>

#define MAX_CORNERS 500

using namespace cv;
using namespace std;

static void help()
{
    // print a welcome message, and the OpenCV version
    cout << "\nThis is a demo of Lukas-Kanade optical flow lkdemo(),\n"
            "Using OpenCV version " << CV_VERSION << endl;
    cout << "\nIt uses camera by default, but you can provide a path to video as an argument.\n";
    cout << "\nHot keys: \n"
            "\tESC - quit the program\n"
            "\tr - auto-initialize tracking\n"
            "\tc - delete all the points\n"
            "\tn - switch the \"night\" mode on/off\n"
            "To add/remove a feature point click it\n" << endl;
}

Point2f point;
bool addRemovePt = false;

static void onMouse( int event, int x, int y, int /*flags*/, void* /*param*/ )
{
    if( event == EVENT_LBUTTONDOWN )
    {
        point = Point2f((float)x, (float)y);
        addRemovePt = true;
    }
}


static void drawArrows(Mat& frame, const vector<Point2f>& prevPts, const vector<Point2f>& nextPts, const vector<uchar> status)
{
    for (size_t i = 0; i < prevPts.size(); i++)
    {
        if (status[i])
        {
            circle(frame, prevPts[i], 2, Scalar(0, 0, 255), 2);
            arrowedLine(frame, prevPts[i], nextPts[i], Scalar(0, 255, 0), 4);
        }
    }
}
double quality = 1 / 100.0;
bool needToInit = true;

static void change_quality(int value, void*)
{
    quality = (((double)value) / 100);
    quality = MAX(quality, 0.01);
    quality = MIN(quality, 0.99);

    needToInit = true;
}

int main( int argc, char** argv )
{
    VideoCapture cap;
    TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,20,0.03);
    Size subPixWinSize(10,10), winSize(31,31);

    const int MAX_COUNT = 500;
    bool nightMode = false;
    double minCornerDist = 10.0;
    int quality_int = 1;

    help();
    cv::CommandLineParser parser(argc, argv, "{@input|0|}");
    string input = parser.get<string>("@input");

    if( input.size() == 1 && isdigit(input[0]) )
        cap.open(input[0] - '0');
    else
        cap.open(input);

    if( !cap.isOpened() )
    {
        cout << "Could not initialize capturing...\n";
        return 0;
    }

    char name[] = "LK Demo";
    namedWindow( name, 1 );
    //setMouseCallback( "LK Demo", onMouse, 0 );
    createTrackbar("Quality", name, &quality_int, 99, change_quality);

    Mat gray, prevGray, image, frame;
    vector<Point2f> points[2];

    auto start_time = std::chrono::high_resolution_clock::now();
    for(;;)
    {
        cap >> frame;
        if( frame.empty() )
            break;
        auto end_time = std::chrono::high_resolution_clock::now();
        auto time = end_time - start_time;
        start_time = end_time;
        long delay_millis = time / std::chrono::milliseconds(1);
        unsigned int fps = 1000 / (delay_millis + 1);
        flip(frame, frame, 1);
        frame.copyTo(image);
        cvtColor(image, gray, COLOR_BGR2GRAY);

        if( nightMode )
            image = Scalar::all(0);

        if( needToInit )
        {
            // automatic initialization
            goodFeaturesToTrack(gray, points[1], MAX_COUNT, 0.01, 10, Mat(), 3, 3, 0, 0.04);
            cornerSubPix(gray, points[1], subPixWinSize, Size(-1,-1), termcrit);
            addRemovePt = false;
        }
        else if( !points[0].empty() )
        {
            vector<uchar> status;
            vector<float> err;
            if(prevGray.empty())
                gray.copyTo(prevGray);
            calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err, winSize,
                                 3, termcrit, 0, 0.001);

            drawArrows(image, points[0], points[1], status);
            vector<Point2f> filteredPoints;
            for (size_t i = 0; i < points[1].size(); i++)
            {
                if (status[i])
                {
                    filteredPoints.push_back(points[1][i]);
                }
            }
            putText(image, std::string("Tracked features: " + std::to_string(filteredPoints.size())), Point(40, 40), FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 255, 0), 1, LINE_8);
            putText(image, std::string("FPS: ") + std::to_string(fps), Point(40, 80), FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 255, 0), 1, LINE_8);

            if(points[1].size() < MAX_CORNERS) {
                vector<Point2f> additionalFeatures;

                Mat mask = Mat(gray.size(), CV_8UC1);
                mask = Scalar(255);
                for (size_t i = 0; i < points[1].size(); i++) {
                    circle(mask, points[1][i], (int)minCornerDist / 2, Scalar(0), -1);
                }
                goodFeaturesToTrack(gray, additionalFeatures, MAX_CORNERS - points[1].size(), quality, minCornerDist, mask, 3, 3, false, 0.05);
                if (additionalFeatures.size() > 0) {
                    cornerSubPix(gray, additionalFeatures, subPixWinSize, Size(-1, -1), termcrit);
                }
                points[1].insert(points[1].end(), additionalFeatures.begin(), additionalFeatures.end());
            }

            size_t i, k;
            for( i = k = 0; i < points[1].size(); i++ )
            {
                if( addRemovePt )
                {
                    if( norm(point - points[1][i]) <= 5 )
                    {
                        addRemovePt = false;
                        continue;
                    }
                }

                if( !status[i] )
                    continue;

                points[1][k++] = points[1][i];
                circle( image, points[1][i], 3, Scalar(0,255,0), -1, 8);
            }
            points[1].resize(k);
        }

        if( addRemovePt && points[1].size() < (size_t)MAX_COUNT )
        {
            vector<Point2f> tmp;
            tmp.push_back(point);
            cornerSubPix( gray, tmp, winSize, Size(-1,-1), termcrit);
            points[1].push_back(tmp[0]);
            addRemovePt = false;
        }

        needToInit = false;
        imshow(name, image);

        char c = (char)waitKey(10);
        if( c == 27 )
            break;
        switch( c )
        {
        case 'r':
            needToInit = true;
            break;
        case 'c':
            points[0].clear();
            points[1].clear();
            break;
        case 'n':
            nightMode = !nightMode;
            break;
        }

        std::swap(points[1], points[0]);
        cv::swap(prevGray, gray);
    }

    return 0;
}
