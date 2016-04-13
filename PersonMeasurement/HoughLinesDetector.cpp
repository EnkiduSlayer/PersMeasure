#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <cstdio>
#include "LSWMS.h"
#include "HoughLinesDetector.h"
#include "EdgeLineDetector.h"

using namespace cv;
using namespace std;

#ifndef LINE_AA
#define LINE_AA CV_AA
#endif

/** General variables */
Mat src2, detectedHL;
//Mat edges2 = detected_edges.clone();
Mat probabilistic_hough;
Mat lineSegments;
Mat edges2(detected_edges.size(), detected_edges.type());
int min_threshold = 50;
int max_trackbar = 150;
vector<Vec4i> p_lines;

const char* window_name_hough = "Hough Lines";
const char* window_name_lineSeg = "Line Segments";

int p_trackBarHL = max_trackbar;
int minLineLengthHL = 30;
int maxLineGapHL = 10;

/// @function Probabilistic_Hough
void Probabilistic_Hough(int, void*)
{
	cvtColor(edges2, probabilistic_hough, COLOR_GRAY2BGR);

	/// Use Probabilistic Hough Transform
	HoughLinesP(edges2, p_lines, 1, CV_PI / 180, min_threshold + p_trackBarHL, minLineLengthHL, maxLineGapHL);

	/// Show the result
	for (size_t i = 0; i < p_lines.size(); i++)
	{
		Vec4i l = p_lines[i];
		line(probabilistic_hough, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 0, 0), 3, LINE_AA);
	}
	imshow(window_name_hough, probabilistic_hough);
}
/* 
void LineSegmentDetect(int, void*)
{
	cvtColor(src_gray, lineSegments, COLOR_GRAY2BGR);

	// Line segments (LSWMS and PPHT)
	std::vector<LSEG> lSegs, lSegsPPHT;
	std::vector<double> errors;

	int width = src2.cols;
	int height = src2.rows;
	Size procSize = cv::Size(width, height);
	int numMaxLSegs = 300;
	bool verbose = false;

	// Create and init LSWMS
	int R = 3;
	LSWMS lswms(procSize, R, numMaxLSegs, verbose);

	lswms.run(src_gray, lSegs, errors);
	lswms.drawLSegs(lineSegments, lSegs, errors);
	imshow(window_name_lineSeg, lineSegments);
}
*/
void HoughLinesDetector::houghLinesDetector(Mat& frame)
{
	cout << endl << "\nLoaded the image for *Hough Limes* manual adjustment.";
	cout << endl << "                Press 'q' to finish.";
	//imwrite("calibration/output/frame_from_HLD.jpg", frame);
	
	/// Apply Canny edge detector
	//Canny(frame, edges2, 50, 200, 3);
	detected_edges.copyTo(edges2);
	//imwrite("calibration/output/frame_from_HLD_gray_edges2.jpg", edges2);

	/// Create Trackbars for Thresholds
	char thresh_label[50];
	sprintf(thresh_label, "Thres: %d + input", min_threshold);

	namedWindow(window_name_hough, WINDOW_NORMAL);
	createTrackbar(thresh_label, window_name_hough, &p_trackBarHL, max_trackbar, Probabilistic_Hough);
	createTrackbar("min Line Length", window_name_hough, &minLineLengthHL, max_trackbar, Probabilistic_Hough);
	createTrackbar("max Line Gap", window_name_hough, &maxLineGapHL, max_trackbar, Probabilistic_Hough);
	///Get variables

	/// Initialize
	Probabilistic_Hough(0, 0);
	cout << endl;
	/*try
	{
		LineSegmentDetect(0, 0);
	}
	catch (const std::exception&) 
	{
	}*/
	char c = (char)waitKey();
	if (c == 'q' || c == 'Q')
	{
		cout << endl << endl << "***Results***";
		cout << endl << "The minimum number of intersections in order to detect a line is: " << p_trackBarHL;
		cout << endl << "The minimum number of points that can form a line: " << minLineLengthHL;
		cout << endl << "The maximum gap between two points to be considered in the same line is: " << maxLineGapHL;
		imwrite("calibration/output/Edges_HLprobabilistic_hough.jpg", probabilistic_hough);
		cout << endl << "\nImage with the Hough Lines manually adjusted has been saved.";
		return;
	}
	waitKey(0);
}

class variableHL
{
public:
	int p_trackBarHL, minLineLengthHL, maxLineGapHL;
	vector<Vec4i> p_lines;
};

void HoughLinesDetector::houghLinesVariables(int p_trackBarHL, int minLineLengthHL, 
	int maxLineGapHL, Mat detectedHL, vector<Vec4i> p_lines)
{
	class name : public variableHL
	{
	public:
		int p_trackBarHL;
		int minLineLengthHL;
		int maxLineGapHL;
		Mat detectedHL;
		vector<Vec4i> p_lines;
	};
}
