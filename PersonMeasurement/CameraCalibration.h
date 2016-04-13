#ifndef CameraCalibration_H
#define CameraCalibration_H
#include "opencv2\opencv.hpp"
#include <iostream>
#include <windows.h>

using namespace std;
using namespace cv;


class CameraCalibration {

public:
	void CameraCalibration::RemoveDistortion(Mat src, Mat dst);
	//void CameraCalibration::Calibrate(vector<Mat> & imgs, string filename);
	bool CameraCalibration::LoadCalibration(std::string filename);
	//void CameraCalibration::WebCamCalib(std::string fileName);

private:
	//void CameraCalibration::getImageProperities(Mat img);

	void CameraCalibration::SaveCalibration(std::string filename);
	bool CameraCalibration::LoadCalibration(std::string filename, Mat _cameraMatrix, Mat _distortionMatrix);
	Mat CameraCalibration::getCameraMatrix();
	Mat CameraCalibration::getDistortionMatrix();

};
#endif