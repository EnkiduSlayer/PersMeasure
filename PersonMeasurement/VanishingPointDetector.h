#ifndef VanishingPointDetector_H
#define VanishingPointDetector_H

#include "opencv2\opencv.hpp"
#include <iostream>


using namespace std;
using namespace cv;

class VanishingPointDetector {
public:
	Point horizontalVP,verticalVP,depthVP;
	VanishingPointDetector::VanishingPointDetector();
	~VanishingPointDetector();
	void computeVanishingPoints(Mat& _frame);
	void VanishingPointDetector::DrawVanishingPoints(Mat & image);
private:
	void getBeams(const vector<Vec6f> input,vector<Vec6f>& vertical,vector<Vec6f>& horizontal,vector<Vec6f>& depth);
	void getSlopes(const vector<Vec4i> lines,vector<Vec6f>& product);
	void getVanishingPoint();
	void selectFloor(vector<Vec6f>& input);
	void getVanishingPoint(const vector<Vec6f> lines,Point& vp);
	void getIntersection(const Vec6f line1,const Vec6f line2,Point& product);
	//void houghLinesDetector(Mat & image);
	void drawLines(const vector<Vec6f> lines,Mat& frame,Scalar color,String name);
	void removeDuplicate(vector<Vec6f>& beamsTmp);
};
#endif
