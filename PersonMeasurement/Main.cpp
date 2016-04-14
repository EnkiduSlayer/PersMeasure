#include "CameraCalibration.h"
#include "VanishingPointDetector.h"
#include "PersonDetector.h"
#include "PersonMeasurement.h"
#include "HoughLinesDetector.h"
#include "EdgeLineDetector.h"
#include "Calibrate.h"
#include <io.h>    // for _setmode()
#include <fcntl.h> // for _O_U16TEXT

string ymlFilename, imageFilename;

int main(int argc, char* argv[])
{
	CameraCalibration cc = CameraCalibration();
	VanishingPointDetector vpd = VanishingPointDetector();
	HoughLinesDetector hld = HoughLinesDetector();
	EdgeLineDetector el = EdgeLineDetector();
	PersonDetector pd = PersonDetector();
	PersonMeasurement pm = PersonMeasurement();
	CalibrateCam cl = CalibrateCam();

	Mat dist, undist, undistEdge, undistHoughLines, undistVL;

#pragma region Introduction

	cout << " Firstly, I want to thanks to my teacher for the support he gave me," <<
		"\nto all the developers, students, lecturers that shared their work." <<
		"\n This application was developesd in the summer semester of 2016, at " <<
		"\nthe Technical Univerity - Brno, Czech Republic, by a student from " <<
		"\noTechnical University of Cluj-Napoca, Romania." <<
		"\n It is freely to use any part of the code long as you point for " <<
		"\nreferences of this work." <<
		"\n\tWork done on the foundation created by Ing. Adam Olejar" <<
		"\n\tWith the supervision of" <<
		"\n\t\t\tdoc. Ing. Kamil Riha, Ph.D. - VUT Brno-CZ" <<
		"\n\t\t\tdoc. Ing. Raul MALUTAN - UT Cluj-Napoca-RO" <<
		"\n\n\t\t\t\t\tMarius N.\n\n";

	cout << "*****\nApplication folder structure\n*****\n\nMainFolder";
	cout << "\n-> calibration";
	cout << "\n  --> input(images from which the information will be extracted)";
	cout << "\n  --> output (Images took within the process)";
	cout << "\n  --> calibfiles ('default.xml', 'imageList.xml'";
	cout << "\n      yml / xml with calibration output)";
	cout << "\n     ---> calibImages(Images used for calibrating the camera)";
	cout << "\n\nThe process consists of having a picture with a person\n";
	cout << "within it, detecting edges in order to detect the \n";
	cout << "vanishing lines (and points), necessary later for \n";
	cout << "calculating the height of the person by knowing \n";
	cout << "the measure of one object/distance from the image.\n";
	cout << "\nA brief tree of the process is showed below.\n\n";
	//_setmode(_fileno(stdout), _O_U16TEXT);
	//// Borders (all thick line)
	//const wchar_t BL = L'\x2514';
	//const wchar_t BR = L'\x251C';
	//cout << "Is the camera calibrated? (Camera lenses usually distort the image)\n";
	//cout << BR << " No -> Calibrate the camera with the calibration process\n";
	//cout << "" << BL << " Yes -> Calibrate the image?\n";
	//cout << " " << BR << " Yes --> The image is undistorted\n";
	//cout << " " << BL << " No  --> Continue with detecting edges\n";
	//cout << "  " << BL << " Detect Edges\n";
	//cout << "   " << BL << " Detect vanishing lines\n";
	//cout << "    " << BL << " Calculate vanishing points\n";
	//cout << "     " << BL << " Detect person\n";
	//cout << "      " << BL << " Select the known object size and enter it\n";
	//cout << "       " << BL << " Calculate the height of the person(s).\n";
	cout << "Is the camera calibrated? (Camera lenses usually distort the image).\n";
	cout << " | No -> Calibrate the camera with the calibration process.\n";
	cout << " > Yes -> No calibration needed. Continue with\n";
	cout << " > Opening the image. (image in *.jpg format in 'input' folder)\n";
	cout << "  > Is the loaded image distorted?\n";
	cout << "   | No --> The image is undistorted with the calibration matrix.\n";
	cout << "   > Yes  --> Continue with \n";
	cout << "    > Detect Edges...\n";
	cout << "     > Detect vanishing lines...\n";
	cout << "      > Calculate vanishing points...\n";
	cout << "       > Detect person(s)...\n";
	cout << "        > Select the known object size and enter it.\n";
	cout << "         > Calculate the height of the person(s).\n";
	do
	{
		cout << endl
			<< '\n'
			<< "After you read all the information, please contine by" << endl
			<< "Pressing ENTER.";
	} while (cin.get() != '\n');

#pragma endregion

	/// Calibrate camera function
	char calib;
	cout << endl << "Is the camera calibrated? y/n" << endl;
	cin >> calib;
	if (calib == 'n')
	{
		/*if (calib != 'y' || calib != 'n')
		{
			cout << "The input is not valid.";
			return -1;
		}*/
		cout << "*****\nWhen the process is finished, press 's' to save an image,";
		cout << "\nor 'ESC' to go to the next step while vieweing an image.\n*****\n";
		cl.calibrateCam(argc, argv);
		cout << endl << "Calibration finished.";
	}
	///*Load image*/
	string in = "13";
	cout << endl << "Please type image name (without extension): ";
	cin >> in;
	string inputImg = "calibration/input/" + in + ".jpg";
	dist = imread(inputImg, CV_LOAD_IMAGE_COLOR);

	if (!dist.data)                              // Check for invalid input
	{
		cout << "Could not open or find the distorted image!!" << std::endl;
		return 0;
	}
	else
		cout << "*******Image loaded with success!*******" << std::endl;
	undist.create(dist.size(), dist.type());
	
	///*Remove distortion from loaded image*/
	/*Calibrate Camera with defined calibration*/
	undist = imread("calibration/input/" + in + ".jpg", CV_LOAD_IMAGE_COLOR);
	if (!undist.data)                              // Check for invalid input
	{
		cout << "Could not open or find the undistorted image!!" << std::endl;
		return 0;
	}
	char clb;
	clb = 'y';
	cout << "Do you want to calibrate the image? y/n" << endl;
	cin >> clb;
	if (clb == 'y')
	{
		cc.LoadCalibration("calibration/calibfiles/4kUltra.yml");
		cout << "*Calibration matrices were loaded with success!*" << endl;
		// Undistort the image
		cc.RemoveDistortion(dist, undist);
		imwrite("calibration/output/UndistortedImag.jpg", undist);
		cout << "***The image has been undistorted with success!***" << std::endl;
		cout << "It can be found in 'calibration/output/UndistortedImag.jpg'" << std::endl << std::endl;
	}

	///*Draw Edges*/
	///*Draw Hough Lines*/
	///*Find vanishin points*/
	vpd.computeVanishingPoints(undist);
	undistVL = undist;
	//imshow("Undistorted&VP", undistVL);

	///*Draw VanishPoints*/
	vpd.DrawVanishingPoints(undistVL);
	//namedWindow("Undistorted&VP", 0);
	//imshow("Undistorted&VP", undist);

	///*Find person*/
	vector<Rect> persons = pd.FindPerson(undist);
	persons.push_back(Rect());

	///*Compute height*/
	pm.setVariables(vpd.verticalVP, vpd.horizontalVP, vpd.depthVP, persons, undist);

	//Wait for key to be pressed to end
	char c = (char)waitKey();
	if (c == 'q' || c == 'Q')
	{
		cout << endl << "\n***\nAn output file has been saved in 'output.txt'.\nProgram will now exit.\n***";
		freopen("calibration/output/output.txt", "w", stdout);
		return 0;
	}
	freopen("calibration/output/outputErr.txt", "w", stdout);
	waitKey(0);
	freopen("calibration/output/outputErr(afe.txt", "w", stdout);
}
