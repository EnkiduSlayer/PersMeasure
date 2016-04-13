#pragma once
#ifndef EdgeLineDetector_H
#define EdgeLineDetector_H

#include "opencv2\opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

extern int lowThresholdED;
extern int ratioED;
extern Mat detected_edges;
extern char output_path;

class EdgeLineDetector {
public:
	void edgeLineDetector(Mat & frame);
	void edgeLineVariables(int lowThresholdED, int ratioED, Mat detected_edges);
};
#endif