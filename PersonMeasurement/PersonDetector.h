#ifndef PersonDetector_H
#define PersonDetector_H

#include "opencv2\opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;


class PersonDetector {
public:
	PersonDetector::PersonDetector();
	~PersonDetector();
	vector<Rect> PersonDetector::FindPerson(Mat &  _frame);
};

#endif // !1