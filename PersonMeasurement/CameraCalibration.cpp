#include "CameraCalibration.h"
#include "PersonDetector.h"

vector<Mat> images;									//Vector of loaded images
std::string path = "calibration/";						//Path to images

Size imageResolution;								//pixel resolution
Size chessPatternSize;								//number of columns and rows in chessboard pattern
int chessPaternResolution;							// actual number of points in chessboard pattern

vector <vector <Point2f>> projectivePlanePoints;				//points of projective 2D plane
vector <vector <Point3f>> realSpacePoints;					//transformed points in space

Mat						cameraMatrix;															//intrinsic parameters matrix
vector<Mat>				translationVector;															//translation vectors 
vector<Mat>				rotationMatrix;															//rotation matrix 
Mat						distortionMatrix;											//distortion matrix


vector<Mat> webcamImages;

//calibrate with entered images
void CameraCalibration::Calibrate(vector<Mat> & imgs, string filename) {
	images = imgs;
	/*images[0] = imread("calibration\\found\\00.JPG",CV_LOAD_IMAGE_COLOR);*/
	//for ( int i = 0; i < images.size(); i++ ) {
	//	if ( images.at(i).empty() ) {
	//		//cout << "Image " << i << "was not loaded" << endl;
	//		cout << "Obraz " << i << "nebol nahratý" << endl;
	//	}
	//}

	getImageProperities(images.at(0));


	//cout << "Images loaded" << endl;

	/*for ( int i = 0; i < 10; i++ ) {
		namedWindow("povodny obraz"+to_string(i),WINDOW_NORMAL);
		imshow("povodny obraz" + to_string(i),images[i]);
		}*/

	for (int f = 0; f < images.size(); f++) {
		Point3f point3D;
		vector<Point3f> points3D;
		vector<Point2f> points2D;

		//	cvtColor(images[f],images[f],CV_BGR2GRAY);
		//	equalizeHist(images[f],images[f]);

		cout << f << "Detecting image... " << endl;
		bool find = false;
		find = findChessboardCorners(images.at(f), chessPatternSize, points2D,
			CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
		bool ret = true;
		if (find) {
			cout << "Pattern found" << endl;
			projectivePlanePoints.push_back(points2D);
			drawChessboardCorners(images.at(f), chessPatternSize, points2D, 1);
			namedWindow("image" + to_string(f), WINDOW_NORMAL);
			imshow("image" + to_string(f), images.at(f));

			for (int i = 0; i < chessPatternSize.height; i++)											//smyèka pro prùchod pøes všechny body šachovnice
				for (int j = 0; j < chessPatternSize.width; j++) {
					point3D.x = float(j);														//sloupcová souøadnice = smìr x
					point3D.y = float(i);														//øádková souøadnice = smìr y
					point3D.z = 0.0;															//z-ová souøadnice je nulová, protože zobrazujeme rovinu

					points3D.push_back(point3D);												//zápis jednoho bodu do vektoru bodù

					//				cout << "x" << i * RozmSachov.width + j <<":\t"<< bod3D.x << "\t" << "y" << i * RozmSachov.width + j <<":\t"<< bod3D.y << "\n";
				}
			realSpacePoints.push_back(points3D);//zápis bodù pro jeden obraz do vektoru 
		}


	}

	calibrateCamera(realSpacePoints, projectivePlanePoints, imageResolution, cameraMatrix, distortionMatrix, rotationMatrix, translationVector, 0);

	SaveCalibration(filename);


}

//sets image properities to global variables
void CameraCalibration::getImageProperities(Mat img) {
	imageResolution.width = img.rows;
	imageResolution.height = img.cols;

	chessPatternSize = Size(8, 6); // my testing pattern,

	chessPaternResolution = chessPatternSize.width * chessPatternSize.height;
}

//calibrate throu webcam
void CameraCalibration::WebCamCalib(std::string filename) {
	VideoCapture cap(0);					//  the default camera
	cap.open(0);							// open camera
	Sleep(1000);							// important to wait until it initialize camera/ if not, next step fails
	if (!cap.isOpened()) {				// check if we succeeded
		cout << "video not working" << endl;
	}

	Mat undistorted;
	namedWindow("Video", 1);
	int imCounter = 0;
	int numOfCalibrationImages = 20;
	Mat  frame;
	bool first = true;
	bool loaded = false;


	for (;; ) {
		// get a new frame from camera
		cap >> frame;
		if (first) {													// no need do this in every loop

			getImageProperities(frame);
			loaded = (LoadCalibration(filename));
			first = false;
		}


		if ((imCounter < numOfCalibrationImages) && !loaded) {
			Point3f point3D;
			vector<Point3f> points3D;
			vector <Point2f> points2D;
			bool find = findChessboardCorners(frame, chessPatternSize, points2D,
				CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
			if (find) {
				imCounter++;
				cout << "Pattern found!" << endl;
				webcamImages.push_back(frame);
				projectivePlanePoints.push_back(points2D);
				drawChessboardCorners(frame, chessPatternSize, points2D, 1);

				for (int i = 0; i < chessPatternSize.height; i++)											//smyèka pro prùchod pøes všechny body šachovnice
					for (int j = 0; j < chessPatternSize.width; j++) {
						point3D.x = float(j);														//sloupcová souøadnice = smìr x
						point3D.y = float(i);														//øádková souøadnice = smìr y
						point3D.z = 0.0;															//z-ová souøadnice je nulová, protože zobrazujeme rovinu

						points3D.push_back(point3D);												//zápis jednoho bodu do vektoru bodù

						//				cout << "x" << i * RozmSachov.width + j <<":\t"<< bod3D.x << "\t" << "y" << i * RozmSachov.width + j <<":\t"<< bod3D.y << "\n";
					}
				realSpacePoints.push_back(points3D);//zápis bodù pro jeden obraz do vektoru 
			}


			if (imCounter == numOfCalibrationImages) {

				calibrateCamera(realSpacePoints, projectivePlanePoints, imageResolution, cameraMatrix, distortionMatrix, rotationMatrix, translationVector, 0);
				SaveCalibration(filename);
			}

			imshow("Video", frame);
		}

		else {
			undistort(frame, undistorted, cameraMatrix, distortionMatrix);

			double x = 100;
			Canny(undistorted, undistorted, x, x * 4, 3);
			vector<Vec4i> lines;
			HoughLinesP(undistorted, lines, 1, CV_PI / 180, 50, 50, 10);
			for (size_t i = 0; i < lines.size(); i++) {
				Vec4i l = lines[i];
				line(undistorted, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 255, 255), 3, CV_AA);
			}
			imshow("Video", undistorted);
		}

		if (waitKey(30) >= 0) break;
	}
}

//saves calibration file
void CameraCalibration::SaveCalibration(std::string filename) {

	FileStorage fs(filename, FileStorage::WRITE);
	fs << "Camera_Matrix" << cameraMatrix;
	fs << "Distortion_Coefficients" << distortionMatrix;
	fs << "Image_points" << projectivePlanePoints;
	fs << "Object_points" << realSpacePoints;
	fs << "Rotation_Matrix" << rotationMatrix;
	fs << "Translation_Vector" << translationVector;
	//cout << "calibration saved" << endl;
	cout << "Kalibracia " << filename << " ulozena" << endl;
	fs.release();

};

//load calibration file with parameters
bool CameraCalibration::LoadCalibration(std::string filename, Mat _cameraMatrix, Mat _distortionMatrix) {

	FileStorage fs(filename, FileStorage::READ);

	if (fs.isOpened()) {
		fs["Camera_Matrix"] >> cameraMatrix;
		fs["Distortion_Coefficients"] >> distortionMatrix;
		fs["Rotation_Matrix"] >> rotationMatrix;
		fs["Translation_Vector"] >> translationVector;

		_cameraMatrix.create(cameraMatrix.size(), cameraMatrix.type());
		_distortionMatrix.create(distortionMatrix.size(), distortionMatrix.type());

		cameraMatrix.copyTo(_cameraMatrix);
		distortionMatrix.copyTo(_distortionMatrix);

		cout << "Calibration loaded" << endl;
		fs.release();
		return true;
	}
	else return false;
}; // unused

//load calibration file
bool CameraCalibration::LoadCalibration(std::string filename) {

	FileStorage fs(filename, FileStorage::READ);

	if (fs.isOpened()) {
		fs["Camera_Matrix"] >> cameraMatrix;
		fs["Distortion_Coefficients"] >> distortionMatrix;
		fs["Rotation_Matrix"] >> rotationMatrix;
		fs["Translation_Vector"] >> translationVector;

		fs.release();
		return true;
	}
	else return false;
};
// getters and setters
Mat CameraCalibration::getCameraMatrix() {
	return cameraMatrix;
}
Mat CameraCalibration::getDistortionMatrix() {
	return distortionMatrix;
}
//method which realize undistortion process 
void CameraCalibration::RemoveDistortion(Mat src, Mat dst) {
	if (cameraMatrix.empty() || distortionMatrix.empty()) {
		bool loaded = LoadCalibration("calibration/calibfiles/4kUltra.yml");
		if (!loaded) {
			cout << "!!!Calibration matrices could not be loaded" << endl;
		}
		else
		{
			cout << "*Calibration matrices were loaded with success!*" << endl;
		}
	}
	else {
		//Mat tmp(src.size(),src.type());
		undistort(src, dst, getCameraMatrix(), getDistortionMatrix());
		/*dst.create(tmp.size(),tmp.type());
		fastNlMeansDenoisingColored(tmp,dst,3,3,9,21);*/
		//cout << "Image undistorted" << endl;

	}
}