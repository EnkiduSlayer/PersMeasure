/**
* @file CannyDetector_Demo.cpp
* @brief Sample code showing how to detect edges using the Canny Detector
* @author OpenCV team
*/

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "EdgeLineDetector.h"

using namespace cv;

/// Global variables

Mat src3, src3_gray;
Mat dst, detected_edges;

int kernel_size = 3;
int lowThresholdED;
int ratioED;
int const max_lowThreshold = 100;
int const max_Ratio = 100;
int const max_kernel_size = 3;
const char* window_name = "Edge Map";

/**
* @function CannyThreshold
* @brief Trackbar callback - Canny thresholds input with a ratio 1:3
*/
static void CannyThreshold(int, void*)
{
	/// Reduce noise with a kernel 3x3
	blur(src3_gray, detected_edges, Size(3, 3));

	/// Canny detector
	Canny(detected_edges, detected_edges, lowThresholdED, lowThresholdED*ratioED, kernel_size);

	/// Using Canny's output as a mask, we display our result
	dst = Scalar::all(0);

	src3_gray.copyTo(dst, detected_edges);
	imshow(window_name, dst);
}

/**
* @function main
*/
void EdgeLineDetector::edgeLineDetector(Mat& frame)
{
	cout << endl << "Loaded the image for *edges* manual adjustment.";
	cout << endl << "           Press 'q' to finish.";

	/// Create a matrix of the same type and size as src3 (for dst)
	dst.create(frame.size(), frame.type());

	/// Convert the image to grayscale
	cvtColor(frame, src3_gray, COLOR_BGR2GRAY);
	imwrite("calibration/output/bggray.jpg", src3_gray);
	/// Create a window
	char thresh_label[50];
	namedWindow(window_name, WINDOW_NORMAL);
	sprintf(thresh_label, "Threshold: %d + input", lowThresholdED);
	/// Create a Trackbar for user to enter threshold
	createTrackbar(thresh_label, window_name, &lowThresholdED, max_lowThreshold, CannyThreshold);
	createTrackbar("Ratio:", window_name, &ratioED, max_Ratio, CannyThreshold);

	/// Show the image
	CannyThreshold(0, 0);
	char c = (char)waitKey();
	if (c == 'q' || c == 'Q')
	{
		cout << endl << endl << "***Results***";
		cout << endl << "The threshold is: " << lowThresholdED;
		cout << endl << "The ratio is    : " << ratioED;
		return;
	}
	waitKey(0);
}


class variableED
{
public:
	int lowThresholdED, ratioED;
	Mat detected_edges;
};

void EdgeLineDetector::edgeLineVariables(int lowThresholdED, int ratioED, Mat detected_edges)
{
	class name : public variableED
	{
	public:
		int lowThresholdED;
		int ratioED;
		Mat detected_edges;
	};
}
