#pragma once
#ifndef HoughLinesDetector_H
#define HoughLinesDetector_H

#include "opencv2\opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

extern int p_trackBarHL;
extern int minLineLengthHL;
extern int maxLineGapHL;
extern Mat detectedHL;
extern vector<Vec4i> p_lines;


class HoughLinesDetector {
public:
	void houghLinesDetector(Mat & frame);
	void houghLinesVariables(int p_trackBarHL, int minLineLengthHL, 
		int maxLineGapHL, Mat detectedHL, vector<Vec4i> p_lines);
};
#endif
