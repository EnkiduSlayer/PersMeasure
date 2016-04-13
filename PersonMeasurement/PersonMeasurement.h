#ifndef PersonMeasurement_H
#define PersonMeasurement_H

#include "opencv2\opencv.hpp"
#include <iostream>


using namespace std;
using namespace cv;

class PersonMeasurement {
public:
	PersonMeasurement::PersonMeasurement();
	void PersonMeasurement::setVariables(Point verticalVP,Point horizontalVP,Point depthVP,vector<Rect> Person,Mat & frame);
	//void compute();
	//static void on_mouse(int event,int x,int y,int flags,void* param);
	/*void getGroundPlaneVanishingLine();
	void getVanishingPointU();
	void getIntersection(const Vec4i line1,const Vec4i line2,Point & cross);
	float getDistance(const Point p1,const Point p2);
	void drawLine(Mat & frame,const Vec4i line,Scalar color);
	void recomputePoints();*/
};
#endif