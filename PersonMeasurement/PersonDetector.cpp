#include "PersonDetector.h"

HOGDescriptor hog;
vector<Rect> found, foundFiltered;
PersonDetector::~PersonDetector() {}

//constructor
PersonDetector::PersonDetector() {
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
}

//search image for person
vector<Rect> PersonDetector::FindPerson(Mat & _frame) {
	vector<Rect> found, foundFiltered;
	double t = (double)getTickCount();
	hog.detectMultiScale(_frame, found, 0.0, Size(8, 8), Size(32, 32), 1.05, 1);
	t = (double)getTickCount() - t;

	size_t i, j;

	for (i = 0; i < found.size(); i++) {
		Rect r = found[i];
		for (j = 0; j < found.size(); j++)
			if (j != i && (r & found[j]) == r)
				break;
		if (j == found.size())
			foundFiltered.push_back(r);
	}

	vector<Rect> returned;

	for (int i = 0; i < foundFiltered.size(); i++) {
		if (foundFiltered.at(i).area() < ((_frame.cols*_frame.rows) / 10)) {
			foundFiltered.erase(foundFiltered.begin() + i);
			i--;
		}
	}

	for (i = 0; i < foundFiltered.size(); i++) {
		Rect r = foundFiltered[i];
		// the HOG detector returns slightly larger rectangles than the real objects.
		// so we slightly shrink the rectangles to get a nicer output.

		r.x += cvRound(r.width*0.1);
		r.width = cvRound(r.width*0.8);
		r.y += cvRound(r.height*0.090);
		r.height = cvRound(r.height*0.73);

		/*r.x += cvRound(r.width*0.1);
		r.width = cvRound(r.width*0.8);
		r.y += cvRound(r.height*0.006);
		r.height = cvRound(r.height*0.9);*/

		returned.push_back(r);

		int textSize = 1;
		int thickness = 2;

		putText(_frame, "Person " + to_string(i + 1), Point(r.x, r.y - 10), FONT_HERSHEY_SIMPLEX, textSize, CV_RGB(255, 255, 0), thickness);
		rectangle(_frame, r.tl(), r.br(), cv::Scalar(0, 255, 0), 3);
	}

	if (returned.size() > 0)
	{
		cout << endl << ("Searched in the image in ");
		cout << ("", t*1000. / cv::getTickFrequency());
		cout << "ms" << endl << "Found " << returned.size() << " person(s)." << endl;
		return returned;
	}
	else
	{
		cout << endl << ("**********\n\nThe search was made in ");
		cout << ("", t*1000. / cv::getTickFrequency());
		cout << (" ms.\nNo person(s) detected. Program will exit.\n\n**********");
		if (returned.size() == 0);
		{
			quick_exit;
			exit;
		}
	}
}