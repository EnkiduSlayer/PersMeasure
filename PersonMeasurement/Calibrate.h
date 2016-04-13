#pragma once
#ifndef Calibrate_H
#define Calibrate_H

#include "opencv2\opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

class CalibrateCam {
public:
	void calibrateCam(int argc, char * argv[]);
};
#endif