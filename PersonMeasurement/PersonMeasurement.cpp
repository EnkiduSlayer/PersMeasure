#include "PersonMeasurement.h"
#define EXIT_SUCCESS 0

Point horizontalVP, verticalVP, depthVP;
vector<Vec4i> people;
Vec4i knownHeight;
float realKnownHeight;		// = 175; in cm
int realPersonHeight;
bool first = false;
bool second = false;
Mat  image;
Mat original;
Vec4i groundPlaneVanishingLine;
int imWidth, imHeight;

void getGroundPlaneVanishingLine();
void getVanishingPointU();
void getIntersection(const Vec4i line1, const Vec4i line2, Point & cross);
float getDistance(const Point p1, const Point p2);
void drawLine(Mat & frame, const Vec4i line, Scalar color);
void recomputePoints();
void on_mouse(int event, int x, int y, int flags, void* param);

// initialize whole measurement

void PersonMeasurement::setVariables(const Point verticalVPTmp, const Point horizontalVPTmp, const Point depthVPTmp, const vector<Rect> PersonRect, Mat & frametmp) {
	horizontalVP = horizontalVPTmp;
	verticalVP = verticalVPTmp;

	verticalVP.y -= 20000;
	depthVP = depthVPTmp;
	imWidth = frametmp.cols;
	imHeight = frametmp.rows;
	Vec4i tmp;

	// save all person found
	for each(Rect r in PersonRect)
	{

		tmp[0] = r.x + r.width / 2;
		tmp[1] = r.y + r.height;
		tmp[2] = r.x + r.width / 2;
		tmp[3] = r.y;
		//if (!(tmp[0]=tmp[1]=tmp[2]=tmp[3]))

		//tmp[0] = 1566;
		//tmp[1] = 775;
		//tmp[2] = 1572;
		//tmp[3] = 2536;

		if (tmp[1] < tmp[3])
		{						// switch two points if first one is on the top
			int buffer = tmp[3];
			tmp[3] = tmp[1];
			tmp[1] = buffer;

			buffer = tmp[2];
			tmp[2] = tmp[0];
			tmp[0] = buffer;
		}
		if ((tmp[0] != 0)) { cout << "Original person position. " << tmp << endl; }
		//cout << "Original person. " << tmp << endl;
		// now need to correct position of the top point because it must lay on the line from bottom 
		// point to vertical vanishingpoint but their distance have to be conserved
		float xPerson = tmp[0] - tmp[2]; // x vector
		float yPerson = tmp[1] - tmp[3]; // y vector
		float lengthPerson = sqrt((xPerson*xPerson) + (yPerson*yPerson));		// length of original line segment

		float vxPerson = tmp[0] - verticalVP.x;							// x vector
		float vyPerson = tmp[1] - verticalVP.y;							// y vector
		float magPerson = sqrt(vxPerson*vxPerson + vyPerson*vyPerson);	// length

		vxPerson /= magPerson;							// normalize to vector unit length
		vyPerson /= magPerson;

		tmp[2] = (int)((double)tmp[0] - vxPerson * lengthPerson);
		tmp[3] = (int)((double)tmp[1] - vyPerson * lengthPerson);

		//if ( tmp[1] < tmp[3] ) {		// switch two points if first one is on the top
		//	int buffer = tmp[3];
		//	tmp[3] = tmp[1];
		//	tmp[1] = buffer;
		//	buffer = tmp[2];
		//	tmp[2] = tmp[0];
		//	tmp[0] = buffer;
		//}
		if ((tmp[0] != 0)) { cout << "The corrected person position. " << tmp << endl; }
		people.push_back(tmp);
	}

	// make copy
	original = frametmp.clone();
	image = frametmp.clone();
	getGroundPlaneVanishingLine();		// intialize ground plane vanishingline
										//show
	namedWindow("Measurement", WINDOW_NORMAL);
	imshow("Measurement", image);
	imwrite("calibration/output/Measurement.jpg", image);
	setMouseCallback("Measurement", on_mouse, 0);

}

//performs computing
void compute() {
	// initialize variables
	Point u, _t1, T1, T2;
	Vec4i line3;

	cout << endl << "Enter real height of selected object: ";
	cin >> realKnownHeight;
	while (!(realKnownHeight))
	{
		cin >> realKnownHeight; // input real known height selected
		cin.clear();
		cin.ignore(99999, '\n');
		cout << "You did not enter a number, please try again: ";
	}

	if (realKnownHeight == 0) {
		exit;
	}

	//cout << "Enter real height of selected object: " << endl;
	//cin >> realKnownHeight; // input real known height selected

	int i = 1;

	//	for each (Vec4i person in people) {
	Vec4i person = people.at(0);

	Mat construct;
	image.copyTo(construct);

	Vec4i b1b2(person[0], person[1], knownHeight[0], knownHeight[1]);

	line(construct, Point(groundPlaneVanishingLine[0], groundPlaneVanishingLine[1]), Point(groundPlaneVanishingLine[2], groundPlaneVanishingLine[3]), Scalar(255, 255, 255), 2);

	getIntersection(b1b2, groundPlaneVanishingLine, u);	 // compute intersection thrue bases of object and ground vanishingline

//	line(construct,Point(b1b2[0],b1b2[1]),Point(b1b2[2],b1b2[3]),Scalar(255,255,255),2);
	line(construct, Point(b1b2[0], b1b2[1]), u, Scalar(0, 0, 255), 2);
	line(construct, Point(b1b2[2], b1b2[3]), u, Scalar(0, 0, 255), 2);

	cout << endl << "Point u : " << u << endl;

	Vec4i ut1Line(u.x, u.y, person[2], person[3]);

	// line(construct,Point(ut1[0],ut1[1]),Point(ut1[2],ut1[3]),Scalar(0,255,255),2);
	// getIntersection(knownHeight,ut1Line,_t1); // compute intersectino of VP u,
	// top point of person thrue known height
	// compute line L3 paralel with line of person

	line3[0] = knownHeight[0];
	line3[1] = knownHeight[1];

	int distx = person[0] - knownHeight[0];
	int disty = person[1] - knownHeight[1];

	line3[2] = person[2] + distx;
	line3[3] = person[3] + disty;

	Vec3f p11, p12, L1, p21, p22, L2;

	p11[0] = person[0];
	p11[1] = person[1];
	p11[2] = 1;
	p12[0] = person[2];
	p12[1] = person[3];
	p12[2] = 1;

	L1 = p11.cross(p12);

	p21[0] = knownHeight[0];
	p21[1] = knownHeight[1];
	p21[2] = 1;
	p22[0] = knownHeight[2];
	p22[1] = knownHeight[3];
	p22[2] = 1;

	L2 = p21.cross(p22);
	Vec3f p = L1.cross(L2);

	cout << p[0] / p[2] << "," << p[1] / p[2] << endl;

	//getIntersection(Vec4i(person[2],person[3],_t1.x,_t1.y),line3,T1); // transform person into line 3
	getIntersection(ut1Line, line3, T1); // transform person into line 3

	Vec4i heightToL3Line(person[2], person[3], knownHeight[2], knownHeight[3]);

	getIntersection(heightToL3Line, line3, T2); // transform known height into line 3

	//zlta
	line(construct, Point(heightToL3Line[0], heightToL3Line[1]), T2, Scalar(0, 255, 255), 2);

	/*line(construct,Point(ut1Line[0],ut1Line[1]),Point(ut1Line[2],ut1Line[3]),Scalar(255,0,255),2);
	line(construct,Point(ut1Line[0],ut1Line[1]),T1,Scalar(255,0,255),2);*/

	//biela
	line(construct, Point(line3[0], line3[1]), T1, Scalar(255, 255, 255), 2);
	line(construct, Point(line3[0], line3[1]), T2, Scalar(255, 255, 255), 2);
	//modra
	line(construct, Point(knownHeight[0], knownHeight[1]), Point(knownHeight[2], knownHeight[3]), Scalar(255, 0, 0), 2);
	//zelena
	line(construct, Point(person[0], person[1]), Point(person[2], person[3]), Scalar(0, 255, 0), 2);


	// compute length ratio
	Point start(line3[0], line3[1]);
	float lengthPerson, lengthKnown, personHeight;

	lengthPerson = getDistance(start, T1);
	lengthKnown = getDistance(start, T2);

	personHeight = (float)(lengthPerson*realKnownHeight) / lengthKnown; // get height from ratio

	int textSize = 3;
	int thickness = 2;

	namedWindow("construct", WINDOW_NORMAL);
	imshow("construct", construct);
	imwrite("calibration/output/xConstruct.jpg", construct);

	Mat tmp = image.clone();
	putText(tmp, to_string((int)personHeight), Point(person[2], person[3]), FONT_HERSHEY_SIMPLEX, textSize, CV_RGB(255, 255, 0), thickness);
	namedWindow("Measurement", WINDOW_NORMAL);
	imshow("Measurement", tmp);

	cout << "Persons " << i << " height is : " << personHeight << " [unit is same as entered object height]" << endl; // output
	i++;
	//}
}
//recompute top point from selected height so it have correct way and call compute() to get product
void recomputePoints() {
	if (first && second) {
		if (knownHeight[1] < knownHeight[3]) {		// switch two points if first one is on the top
			int buffer = knownHeight[3];
			knownHeight[3] = knownHeight[1];
			knownHeight[1] = buffer;

			buffer = knownHeight[2];
			knownHeight[2] = knownHeight[0];
			knownHeight[0] = buffer;
		}

		cout << "Known height " << knownHeight << endl;

		float xHeight = knownHeight[0] - knownHeight[2];
		float yHeight = knownHeight[1] - knownHeight[3];
		float  lengthHeight = sqrt((xHeight*xHeight) + (yHeight*yHeight));			// musnt change the original length of line

		float vxHeight = knownHeight[0] - verticalVP.x; // x vector
		float vyHeight = knownHeight[1] - verticalVP.y; // y vector
		float magHeight = sqrt((vxHeight*vxHeight) + (vyHeight*vyHeight)); // length

		vxHeight /= magHeight;							// normalize to vector unit length
		vyHeight /= magHeight;

		knownHeight[2] = (int)((double)knownHeight[0] - vxHeight * lengthHeight);
		knownHeight[3] = (int)((double)knownHeight[1] - vyHeight * lengthHeight);

		cout << "Corected Known height " << knownHeight << endl;

		compute();

		first = false;
		second = false;
		realKnownHeight = 0;
		knownHeight = Vec4i(0, 0, 0, 0);
	}
}
//mouse controll
void on_mouse(int event, int x, int y, int flags, void* param) {
	//funkce pro provedení operace pøi událostech myši

	Point point = Point(x, y);
	Mat tmp = original.clone();
	int crossSize = 20;
	int textSize = 3;
	int thickness = 2;

	switch (event) {
	case CV_EVENT_LBUTTONDOWN:
	{
		if (second) {
			Point point = Point(knownHeight[2], knownHeight[3]);

			putText(tmp, "P2", point, FONT_HERSHEY_SIMPLEX, textSize, CV_RGB(255, 0, 0), thickness);

			line(tmp, cvPoint(point.x - crossSize, point.y), cvPoint(point.x + crossSize, point.y), CV_RGB(255, 0, 0), thickness, 8, 0);
			line(tmp, cvPoint(point.x, point.y - crossSize), cvPoint(point.x, point.y + crossSize), CV_RGB(255, 0, 0), thickness, 8, 0);
		}

		first = true;
		knownHeight[0] = x;
		knownHeight[1] = y;

		Point point = Point(x, y);

		putText(tmp, "P1", point, FONT_HERSHEY_SIMPLEX, textSize, CV_RGB(255, 0, 0), thickness);

		line(tmp, cvPoint(point.x - crossSize, point.y), cvPoint(point.x + crossSize, point.y), CV_RGB(255, 0, 0), thickness, 8, 0);
		line(tmp, cvPoint(point.x, point.y - crossSize), cvPoint(point.x, point.y + crossSize), CV_RGB(255, 0, 0), thickness, 8, 0);

		cout << "P1: x = " << x << " y = " << y << endl;
		imshow("Measurement", tmp);

		waitKey();

		recomputePoints();
		break;
	}
	case CV_EVENT_RBUTTONDOWN:
	{
		if (first) {
			Point point = Point(knownHeight[0], knownHeight[1]);

			putText(tmp, "P1", point, FONT_HERSHEY_SIMPLEX, textSize, CV_RGB(255, 0, 0), thickness);

			line(tmp, cvPoint(point.x - crossSize, point.y), cvPoint(point.x + crossSize, point.y), CV_RGB(255, 0, 0), thickness, 8, 0);
			line(tmp, cvPoint(point.x, point.y - crossSize), cvPoint(point.x, point.y + crossSize), CV_RGB(255, 0, 0), thickness, 8, 0);
		}
		second = true;
		knownHeight[2] = x;
		knownHeight[3] = y;

		Point point = Point(x, y);

		putText(tmp, "P2", point, FONT_HERSHEY_SIMPLEX, textSize, CV_RGB(255, 0, 0), thickness);

		line(tmp, cvPoint(point.x - crossSize, point.y), cvPoint(point.x + crossSize, point.y), CV_RGB(255, 0, 0), thickness, 8, 0);
		line(tmp, cvPoint(point.x, point.y - crossSize), cvPoint(point.x, point.y + crossSize), CV_RGB(255, 0, 0), thickness, 8, 0);

		cout << "P2: x = " << x << " y = " << y << endl;
		imshow("Measurement", tmp);

		waitKey();

		recomputePoints();
		break;
	}
	case CV_EVENT_MBUTTONDOWN:
	{
		cout << "Mouse wheel Pressed. Program will end." << endl;
		exit;
		break;
		return;
	}
	case CV_EVENT_FLAG_ALTKEY:
		cout << "You pressed 'Alt' key. Program will end.";
		break;
		return;
	}
}

//initialize ground vanishing line
void getGroundPlaneVanishingLine() {
	groundPlaneVanishingLine[0] = horizontalVP.x;
	groundPlaneVanishingLine[1] = horizontalVP.y;
	groundPlaneVanishingLine[2] = depthVP.x;
	groundPlaneVanishingLine[3] = depthVP.y;

	/*int diffx,diffy, c =-500;
	diffx = 0 - groundPlaneVanishingLine[0]-c;
	diffy = 0 - groundPlaneVanishingLine[1]-c;

	groundPlaneVanishingLine[0] += diffx;
	groundPlaneVanishingLine[1] += diffy;
	groundPlaneVanishingLine[2] += diffx;
	groundPlaneVanishingLine[3] += diffy;*/
}
//return intersection entered lines
void getIntersection(const Vec4i line1, const Vec4i line2, Point & cross) {

	Vec3f p11, p12, p21, p22, L1, L2, point;
	p11[0] = line1[0];
	p11[1] = line1[1];
	p11[2] = 1;
	p12[0] = line1[2];
	p12[1] = line1[3];
	p12[2] = 1;
	p21[0] = line2[0];
	p21[1] = line2[1];
	p21[2] = 1;
	p22[0] = line2[2];
	p22[1] = line2[3];
	p22[2] = 1;

	L1 = p11.cross(p12);
	L2 = p21.cross(p22);

	point = L1.cross(L2);

	if (point[2] == 0) {
		cout << "Paralel lines, cannot compute intersection" << point << endl;
		return;
	}
	else {
		cross.x = point[0] / point[2];
		cross.y = point[1] / point[2];
	}
	/*
	//float Ax1,Ay1,ASlope,Ab;			// initialize first line
	//Ax1 = line1[0];
	//Ay1 = line1[1];
	//ASlope = (float) ((float) (line1[1] - line1[3]) / (float) (line1[0] - line1[2]));
	//Ab = Ay1 - ASlope*Ax1;


	//float Bx1,By1,BSlope,Bb;			// initialize second line
	//Bx1 = line2[0];
	//By1 = line2[1];
	//BSlope = (float) ((float) (line2[1] - line2[3]) / (float) (line2[0] - line2[2]));
	//Bb = By1 - BSlope*Bx1;

	//float xCross,yCross;							// tmp values
	//if ( ASlope != BSlope ) {   // if not paralel lines
	//	xCross = (Bb - Ab) / (ASlope - BSlope);	//  compute X coordinate of crossing
	//	yCross = ASlope*xCross + Ab;			// use xcross to compute Y coordinate

	//	cross.x = cvRound(xCross);			// save values and return
	//	cross.y = cvRound(yCross);
	//	//	return true;
	//}
	//else { // paralel
	////	cout << "Paralel lines, cant compute intersection" << endl;
	//	return;
	//}*/

}
//compute distance of two points
float getDistance(const Point p1, const Point p2) {
	float dx, dy;
	dx = abs(p1.x - p2.x);
	dy = abs(p1.y - p2.y);
	return  sqrt((dx*dx) + (dy*dy));
}

void drawLine(Mat & frame, const Vec4i lineDrawn, const Scalar color) {

	Point p1(lineDrawn[0], lineDrawn[1]);
	Point p2(lineDrawn[2], lineDrawn[3]);
	float slope = (float)(p1.y - p2.y) / (p1.x - p2.x);
	float b = p1.y - slope*p1.x;

	int y;
	y = slope * 0 + b;

	if (y >= 0) {
		p1.x = 0;
	}
	else {
		p1.x = ((0 - p1.y) / slope) + p1.x;
		p1.y = 0;
	}


	y = (slope * (imWidth - 1 - p1.x)) + p1.y;
	if (y >= imWidth) {
		p2.x = imWidth - 1;
	}
	else {
		p2.x = ((imHeight - 1 - p1.y) / slope) + p1.x;
		p2.y = imHeight - 1;
	}

	line(frame, p1, p2, color, 1, 8, 0); // draw line into it
	imshow("Measurement", frame);
	imwrite("calibration/input/zMeasurement.jpg", frame);
}

PersonMeasurement::PersonMeasurement()
{

}