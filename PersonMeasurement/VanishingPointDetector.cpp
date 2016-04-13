#include "VanishingPointDetector.h"
#include "opencv\highgui.h"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "HoughLinesDetector.h"
#include "EdgeLineDetector.h"
#include "LSWMS.h"

#include <iostream>
#include <cstdio>

Mat src, edges;
double x = 100;
vector<Vec6f> slopes;
vector<Vec6f> horizontalLines;
vector<Vec6f> verticalLines;
vector<Vec6f> depthLines;

static const uchar VERTICAL = 0;
static const uchar HORIZONTAL = 1;
static const uchar DEPTH = 2;

int width, height;

CvPoint horizontalVP, verticalVP, depthVP;

HoughLinesDetector hld = HoughLinesDetector();
EdgeLineDetector el = EdgeLineDetector();
VanishingPointDetector::VanishingPointDetector() {}

// main central method which contains whole procedure
void VanishingPointDetector::computeVanishingPoints(Mat& frame) {

	width = frame.cols;
	height = frame.rows;

	///Detect Edge Lines
	char aEL;
	cout << endl << "Do you want to manually detect the edge lines? y/n" << endl;
	cin >> aEL;
	if (aEL == 'y')
	{
		el.edgeLineDetector(frame);

		//namedWindow("Edges", WINDOW_NORMAL);
		//imshow("Edges", detected_edges);
		imwrite("calibration/output/Edges_ManVP.jpg", detected_edges);
		cout << endl << "Image with the Edge Lines manually adjusted has been saved.";
	}
	else
	{
		Mat frameGray;
		frameGray.create(frame.size(), CV_8U);
		cvtColor(frame, frameGray, CV_RGB2GRAY, 0);
		equalizeHist(frameGray, frameGray);

		Ptr<CLAHE> clahe = createCLAHE(2.0, Size(8, 8));
		Mat equalized;
		clahe->apply(frameGray, equalized);

		//width = frame.cols;
		//height = frame.rows;
		int lowThreshold = 40; 								// used treshold. 40
		int const max_lowThreshold = 10;					// maximum value of treshold.. 10
		int ratio = 14;										// .14
		int kernel_size = 3;								// size of sobel operator
		Canny(frameGray, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size);
		cout << endl << "The image has been processed with the default input.";
		//namedWindow("Edges", WINDOW_NORMAL);
		//imshow("Edges", edges);
		imwrite("calibration/output/Edge_Default.jpg", detected_edges);
	}

	///Detect Hough Lines
	char aHL;
	cout << endl << "\nDo you want to manually detect the lines? y/n\n";
	cin >> aHL;
	if (aHL == 'y')
	{
		hld.houghLinesDetector(detected_edges);
	}
	else
	{
		int rho = 1;						// resolution in pixel
		double theta = CV_PI / 180;			// resolution in radians  (use one degree)
		int treshold = p_trackBarHL;		//Value taken from manual adjustment
		int minLinLength = minLineLengthHL; //Value taken from manual adjustment
		int maxLineGap = maxLineGapHL;		//Value taken from manual adjustment
		HoughLinesP(detected_edges, p_lines, rho, theta, treshold, minLinLength, maxLineGap); // we got lines.
		cout << endl << "The image has been processed with the default input for Hough Lines.";
	}

	if (p_lines.size() == 0)
	{
		cout << endl << "***\nNo Lines have been detected!\nProgram will exit.\n***";
		return;
	}

	vector<Vec4i> lines = p_lines;			// to store houg probablistitic transform product

	getSlopes(lines, slopes);											// return new vector with slopes and angles
	getBeams(slopes, verticalLines, horizontalLines, depthLines);		// return best matches in all directions

	Scalar colorVertical(0, 32, 255);			// vertical	  (Blue)
	Scalar colorHorizontal(0, 200, 255);		// horizontal (Yellow)
	Scalar colorDepth(255, 0, 0);				// depth	  (Red)
	Scalar colorSlopes(200, 255, 0);			// Slopes	  (Yellow)

	drawLines(slopes, frame, colorSlopes, "Slopes");
	drawLines(horizontalLines, frame, colorHorizontal, "Horizontal");
	drawLines(verticalLines, frame, colorVertical, "Vertical");
	drawLines(depthLines, frame, colorDepth, "Depth");

	cout << endl << "\nFound " << verticalLines.size() << " vertical lines.";
	cout << endl << "Found " << horizontalLines.size() << " horizontal Lines.";
	cout << endl << "Found " << depthLines.size() << " depth Lines.\n";
	if (verticalLines.size() == 0 || horizontalLines.size() == 0 || depthLines.size() == 0)
	{
		cout << endl << "Not all lines have been found. Cannot continue." << endl;
		return;
	}

	getVanishingPoint(horizontalLines, horizontalVP);
	getVanishingPoint(verticalLines, verticalVP);
	getVanishingPoint(depthLines, depthVP);

	cout << endl << "Image resolution: " << width << "x" << height << endl;
	cout << "Vertical VP coordinates : x:" << verticalVP.x << ", y:" << verticalVP.y << endl;
	cout << "Horizontal VP coordinates : x:" << horizontalVP.x << ", y:" << horizontalVP.y << endl;
	cout << "Depth VP coordinates : x:" << depthVP.x << ", y:" << depthVP.y << endl << endl;

	//circle(image, horizontalTmp, size, color, -1, other);
	circle(edges, depthVP, 50, (0, 255, 0), -10, CV_AA);
	// DrawVanishingPoints();
}

/*
void VanishingPointDetector::Probabilistic_Hough(int, void*)
{
	vector<Vec4i> p_lines;
	cvtColor(edges, probabilistic_hough, COLOR_GRAY2BGR);

	/// 2. Use Probabilistic Hough Transform
	HoughLinesP(edges, p_lines, 1, CV_PI / 180, min_threshold + p_trackbar, minLineLength, maxLineGap);

	/// Show the result
	for (size_t i = 0; i < p_lines.size(); i++)
	{
		Vec4i l = p_lines[i];
		line(probabilistic_hough, Point(l[0], l[1]), Point(l[2], l[3]),	Scalar(255, 0, 0), 3, LINE_AA);
	}

	imshow(window_name_hough, probabilistic_hough);
}

void VanishingPointDetector::LineSegmentDetect(int, void*)
{
	cvtColor(src_gray, lineSegments, COLOR_GRAY2BGR);

	// Line segments (LSWMS and PPHT)
	std::vector<LSEG> lSegs, lSegsPPHT;
	std::vector<double> errors;

	int width = src.cols;
	int height = src.rows;
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

void VanishingPointDetector::houghLinesDetector(int argc, char** argv)
{
	/// Read the image
	src = 1;

	if (src.empty())
	{
		cout << endl << "The image cannot be processed!";
		return;
	}

	/// Pass the image to gray
	cvtColor(src, src_gray, COLOR_RGB2GRAY);

	/// Apply Canny edge detector
	Canny(src_gray, edges, 50, 200, 3);

	/// Create Trackbars for Thresholds
	char thresh_label[50];
	sprintf(thresh_label, "Thres: %d + input", min_threshold);

	namedWindow(window_name_hough, WINDOW_NORMAL);
	createTrackbar(thresh_label, window_name_hough, &p_trackbar, max_trackbar, Probabilistic_Hough);
	createTrackbar("minLineLength", window_name_hough, &minLineLength, max_trackbar, Probabilistic_Hough);
	createTrackbar("maxLineGap", window_name_hough, &maxLineGap, max_trackbar, Probabilistic_Hough);

	/// Initialize
	Probabilistic_Hough(0, 0);
	LineSegmentDetect(0, 0);
	waitKey(0);
	return;
}
*/

//draw entered vectors of lines
void VanishingPointDetector::drawLines(const vector<Vec6f> lines, Mat& frame, const Scalar color, const String name) {
	CvPoint p1, p2;
	Mat tmp;
	if (name != "")
	{											// if without name, draw into entered image
		tmp.create(frame.size(), frame.type()); // initialize
		frame.copyTo(tmp);						// create duplicate
	}
	else tmp = frame;

	if (!(lines.empty())) {
		for each (Vec6f lineTmp in lines) {
			p1.x = lineTmp[0];
			p1.y = lineTmp[1];
			p2.x = lineTmp[2];
			p2.y = lineTmp[3];
			float slope = lineTmp[5];
			//line(Mat& img, pt1, pt2, Scalar&color, int thickness=1, int lineType=8, int shift=0)
			line(tmp, p1, p2, Scalar(255, 0, 255), 2, 8, 0); // draw line into it
			// this serve to compute edge points of the line in image 
			p1.y = (slope * (0 - p1.x)) + p1.y;
			if (p1.y >= 0) {
				p1.x = 0;
			}
			else {
				p1.x = ((0 - p1.y) / slope) + p1.x;
				p1.y = 0;
			}
			p2.y = (slope * (width - 1 - p1.x)) + p1.y;
			if (p2.y >= width) {
				p2.x = width - 1;
			}
			else {
				p2.x = ((height - 1 - p1.y) / slope) + p1.x;
				p2.y = height - 1;
			}
			line(tmp, p1, p2, color, 1, 8, 0); // draw line into it
		}
	}
	if (name != "") {
		//namedWindow(name, WINDOW_NORMAL);			// show slope/vert/horizont/depth
		imwrite("calibration/output/z" + name + ".jpg", tmp);
		if (imwrite)
		{
			cout << endl << "Image with detected " + name + " lines is saved.";
		}
	}
	else {
		String newName = "DrawLine";
		namedWindow(name, WINDOW_NORMAL);	// show
		imshow(newName, tmp); imwrite("calibration/output/y" + newName + ".jpg", tmp);
	}

}

//compute slopes and angles
void VanishingPointDetector::getSlopes(const vector<Vec4i> lines, vector<Vec6f>& product)
{
	// return  angle as [4] member and slope as [5] member

	for each (Vec4i line in lines) {
		int x1, y1, x2, y2;			// initialize coordinates
		float slope, angle;
		x1 = line[0];
		y1 = line[1];
		x2 = line[2];
		y2 = line[3];

		if (x2 == x1)
		{		// if line is vertical = angle 90 (undefined slope)	 
							// because x2-x1 vould result in 0 and there will be y2-y1/0 ...division by zero 
			angle = 90;
			slope = (float) 99999999.999;
		}
		else {
			slope = (float)((float)(y1 - y2) / (float)(x1 - x2));		 // find value of slope				
			angle = atan(slope) * 180 / CV_PI;							 // find angle in degrees agains x;
			if (angle < 0) {
				angle += 180;
			}
			//cout << "angle :" << angle << "    Slope: " << slope << endl;
		}
		Vec6f tmp;								// store result in better one data type
		tmp[0] = (float)x1;
		tmp[1] = (float)y1;
		tmp[2] = (float)x2;
		tmp[3] = (float)y2;
		tmp[4] = (float)angle;
		tmp[5] = (float)slope;

		product.push_back(tmp);
	}
}

//return vertical,horizontal and depth line beams
void VanishingPointDetector::getBeams(const vector<Vec6f> inputConst, vector<Vec6f>& vertical, vector<Vec6f>& horizontal, vector<Vec6f>& depth) {

	vector<Vec6f> input(inputConst);
	removeDuplicate(input);
	int horizontalRange = 5; // 20 degree on both sides
	int verticalRange = 5;

	for (int i = 0; i < 3; i++) {
		float declination;
		for (int beamPos = 0; beamPos < input.size(); beamPos++) {		// thru all the beams we are looking for 
																		// biggest vector with right direction 
			switch (i) {

			case VERTICAL:												// chose vertical lines
				declination = abs(90 - abs(input.at(beamPos)[4]));		// compute declination		
				if (declination < verticalRange) {						// compare with defined value
					vertical.push_back(input.at(beamPos));				// store line
					input.erase(input.begin() + beamPos);				// erase once used line
					beamPos--;											// return iterator one position back because 
													// we just erased actual value, and whole vector will move
				}
				break;

			case HORIZONTAL:											// choose horizontal lines	
				if (input.at(beamPos)[4] > 90)
				{						// if bigger than 90deg, need to switch to 180dgr
					declination = abs(180 - input.at(beamPos)[4]);
				}
				else
				{
					declination = abs(0 - input.at(beamPos)[4]);		// if less than 90, compute from 0;
				}
				if (declination < horizontalRange)
				{														// compare with defined value
					horizontal.push_back(input.at(beamPos));			// store line
					input.erase(input.begin() + beamPos);				// erase once used line
					beamPos--;											// return iterator one position back 
											//because we just erased actual value, and whole vector will move
				}
				break;

			case DEPTH:								// for third dimension	
				selectFloor(input);					// only choose those lines defined int method body     
				depth = input;						// save values 
				break;

			default:
				break;
			}
		}
	}
}

// select lines which should be floor / depth
void VanishingPointDetector::selectFloor(vector<Vec6f>& input)
{
	// select vectors which are in bottom half of the image (probably floor) and which don't colide with other ways 
	float y1, y2, angle;;		// initialize coordinates
	int range = 5;				// variable setting angle distance from 90 and 0 degree
	vector<Vec6f> left;			// storage for left heading lines
	vector<Vec6f> right;		// storage for right heading lines

	for (size_t i = 0; i < input.size(); i++) {
		y1 = input.at(i)[1];
		y2 = input.at(i)[3];
		angle = input.at(i)[4];

		if (((y1 < (height / 9) * 5) || (y2 < (height / 9) * 5)) ||
			(((angle < range) && (angle > 90 - range)) || ((angle < 90 + range) && (angle > 180 - range))))
		{													 // if its in not in 1/3 of image,erase
			input.erase(input.begin() + i);									// remove
			i--;															// step on next element

		}
		// we want only one way if there are two, their crossing will damage selection, we choose bigger one
		else if ((angle > range) && (angle < 90 - range)) {	// save right
			right.push_back(input.at(i));
		}
		else if ((angle > 90 + range) && (angle < 180 - range)) { // save left
			left.push_back(input.at(i));
		}
	}

	if (left.size() > right.size()) {   // use bigger vector
		input = left;
	}
	else {
		input = right;
	}
}

//return vanishing point from Lines entered
void VanishingPointDetector::getVanishingPoint(vector<Vec6f> lines, Point& vp)
{
	vector<Point> intersections;					// vector for storing intersections

	for (int i = 0; i < lines.size(); i++)
	{												// get all combination crossings
		Point cross;								// temporary point value
		for (int s = 0; s < lines.size(); s++)
		{
			if (s != i)
			{
				getIntersection(lines.at(i), lines.at(s), cross); // compute intersection
				intersections.push_back(cross);		// save
			}
		}
	}

	///////////////// compute median
	vector<int> xval;								// vectors to store x and y coordinates	
	vector<int> yval;
	int medianX, medianY;							// median value 

	for each (Point pt in intersections) {			// fill vectors
		xval.push_back(pt.x);
		yval.push_back(pt.y);
	}

	sort(xval.begin(), xval.end());					// sort them
	sort(yval.begin(), yval.end());

	if (xval.size() % 2) {							// if odd, 
		medianX = xval.at(xval.size() / 2);			// get integer value of middle element
		medianY = yval.at(yval.size() / 2);
	}
	else
	{									// if even, get average of two middle elements
		medianX = ((xval.at(xval.size() / 2)) + xval.at((xval.size() / 2) - 1)) / 2;
		medianY = ((yval.at(yval.size() / 2)) + yval.at((yval.size() / 2) - 1)) / 2;
	}

	vp.x = medianX;						// return average values as vanishing point
	vp.y = medianY;
}

//return intersection
void VanishingPointDetector::getIntersection(const Vec6f line1, const Vec6f line2, Point& product) {
	float Ax1, Ay1, ASlope, Ab;			// initialize first line
	Ax1 = line1[0];
	Ay1 = line1[1];
	ASlope = line1[5];
	Ab = Ay1 - ASlope*Ax1;


	float Bx1, By1, BSlope, Bb;			// initialize second line
	Bx1 = line2[0];
	By1 = line2[1];
	BSlope = line2[5];
	Bb = By1 - BSlope*Bx1;

	float xCross, yCross;				// tmp values
	if (ASlope != BSlope) {				// if not paralel lines	
		xCross = (Bb - Ab) / (ASlope - BSlope);	// compute X coordinate of crossing
		yCross = ASlope*xCross + Ab;			// use xcross to compute Y coordinate

		product.x = cvFloor(xCross);			// save values and return
		product.y = cvFloor(yCross);
		//	return true;
	}
	else
	{
		// paralel
		// cout << "Paralel lines, can't compute intersection" << endl;
		return;
	}

}

// remove duplicate lines
void VanishingPointDetector::removeDuplicate(vector<Vec6f> & beamsTmp) {

	for (int Vvec1 = 0; Vvec1 < beamsTmp.size(); Vvec1++)
	{																	// For all vectors
		for (int Vvec2 = 0; Vvec2 < beamsTmp.size(); Vvec2++)
		{																// Against all vectors

			if (Vvec1 != Vvec2) {										// Cannot compare same vectors

				if (beamsTmp.at(Vvec1)[0] == beamsTmp.at(Vvec2)[0] &&
					beamsTmp.at(Vvec1)[1] == beamsTmp.at(Vvec2)[1] &&
					beamsTmp.at(Vvec1)[2] == beamsTmp.at(Vvec2)[2] &&
					beamsTmp.at(Vvec1)[3] == beamsTmp.at(Vvec2)[3] &&
					beamsTmp.at(Vvec1)[4] == beamsTmp.at(Vvec2)[4])
				{													// compare all coordinates
					beamsTmp.erase(beamsTmp.begin() + Vvec2);		// erase the same;
					Vvec2--;										// get one back to get right position 
																	// after removal 
				}
			}
		}
	}
}

//create new image and draw vanishingpoints 
void VanishingPointDetector::DrawVanishingPoints(Mat & image)
{

	int Left = 0, Right = 0, Up = 0, Down = 0;
	//drawLines(verticalLines, image, Scalar(255, 0, 0), "Vrt.jpg");			// draw lines who lead to points
	//drawLines(horizontalLines, image, Scalar(0, 255, 0), "Hrz.jpg");
	//drawLines(depthLines, image, Scalar(0, 0, 255), "Dph.jpg");

	// get new boundaries of the image
	Left = std::min({ verticalVP.x,horizontalVP.x,depthVP.x });
	Right = std::max({ verticalVP.x,horizontalVP.x,depthVP.x });
	Up = std::min({ verticalVP.y,horizontalVP.y,depthVP.y });
	Down = std::max({ verticalVP.y,horizontalVP.y,depthVP.y });

	// we dont want to shrink image if VP lies within

	if (Left >= 0) {
		Left = 0;
	}
	if (Right < width) {
		Right = width;
	}
	if (Up >= 0) {
		Up = 0;
	}
	if (Down < height) {
		Down = height;
	}

	Size newSize(abs(Left) + Right + 10, abs(Up) + Down + 10);	// compute new size

	Mat newImage(newSize, CV_8UC3);
	newImage.setTo(Scalar(0, 0, 0));

	int zeroX = abs(Left);	// +10;
	int zeroY = abs(Up);	// +10;

	for (float i = 0; i < (height / 2 * 2); i++)
	{
		for (float j = 0; j < (width / 2 * 2); j++)
		{
			newImage.at < Vec3b >(i + zeroY, j + zeroX) = image.at<Vec3b>(i, j);
			// copy old image into new one with movement (why?)
		}
	}

	Point horizontalTmp, verticalTmp, depthTmp;

	horizontalTmp.x = horizontalVP.x + abs(Left) + 5;
	horizontalTmp.y = horizontalVP.y + abs(Up) + 5;

	verticalTmp.x = verticalVP.x + abs(Left) + 5;
	verticalTmp.y = verticalVP.y + abs(Up) + 5;

	depthTmp.x = depthVP.x + abs(Left) + 5;
	depthTmp.y = depthVP.y + abs(Up) + 5;

	Scalar colorVertical(0, 32, 255);			// vert (Blue)
	Scalar colorHorizontal(0, 200, 0);			// hor	 (Green)
	Scalar colorDepth(255, 0, 0);				// depth (Red)
	int radius = 10;							// radius of the cirles

	circle(newImage, verticalTmp, radius, colorVertical, -1);
	circle(newImage, horizontalTmp, radius, colorHorizontal, -1);
	circle(newImage, depthTmp, radius, Scalar(255, 0, 0), -1);

	Mat tmp;
	tmp.create(Size(newImage.cols, newImage.rows), newImage.type());
	resize(newImage, tmp, tmp.size(), 0, 1, 1);

	//namedWindow("VPs", WINDOW_NORMAL);
	//imshow("VPs", tmp);
	imwrite("calibration/output/VPsImag.jpg", tmp);
	if (imwrite)
	{
		cout << "***The image with VPs has saved with success!***" << std::endl;
		cout << "It can be found in 'calibration/output/zVPsImag.jpg'" << std::endl << std::endl;
	}
}

VanishingPointDetector::~VanishingPointDetector() {}
