#include "opencv2/opencv.hpp"//opencv librarys , these are needed to run the code , you must manually add them from https://opencv.org/releases.html
#include "opencv2/highgui/highgui.hpp"//opencv lib
#include "opencv2/imgproc/imgproc.hpp"//opencv lib
#include <iostream>//c++ library 

using namespace cv;//opencv namespace
using namespace std;//standard namespace, dont have to use std::cout ect..

void linehistDisplay(int histogram[], const int width, const char* name) //creates and displayes the line to track on histogram
{
 int hist[width];
	for (int i = 0; i < width; i++)//sets i at 0 , and when i is less than the width (640) then increment the value of i by 1 until it matches the width
	{
		hist[i] = histogram[i];//initializes hist to ake it equal the histogram value [1]
	}
	// draw the histogram
	int hist_w = width; int hist_h = 256;//sets the value of the histogram width to "width" and makes the height 256 and this will display and draw the histogram
	int bin_w = cvRound((double)hist_w / width);//declares bin_w using cvRound(rounds the foating point number (from width and his_w) to the nearest integer)

// create image for the histogram
	Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(255, 255, 255));//calls array height,array width ,CV_8UC1 - 1 channel (greyscale), 255 x3 makes the histogram white

				//draws the intensity line for histogram 
	for (int i = 0; i < width; i++)//sames loop as above , sets the pixel line to track the location 
	{
		line(histImage, Point(bin_w*(i), hist_h), Point(bin_w*(i), hist_h - hist[i]), Scalar(0, 0, 0), 1, 8, 0);//draws the intensity line to track location
               /*points to the width and height values which creates the line ,bin_w(final value from the cvround operation with value of i),
				, hist_h - hist[i] takes away original height with width, allows to make horizontal line to track the middle of the screen
				1,8,0 mean thickness , linetype*/
	}


	// displays histogram
	namedWindow(name, CV_WINDOW_AUTOSIZE);//creates window Autosize means the image is created in its original size, the original size is 640x480p 
	imshow(name, histImage);//displays histogram
}


int main(int, char**)
{
	int intensity[640];// sets intensity values 
	int filtintensity[640];// sets intensity values the filter just makes the lines more clear 
	int left_location;// location values left movement , used to work out the final position 
    int right_location;// location values right movement  , used to work out the final position 
	int x_co;//stores the calculated value

	printf("Started\n");//stating that the program has started

	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened())  // check if we succeeded
		return -1;// if not end

	Mat frame;//Matrix to store frame															  
	Mat grey;//Matrix to store the frame as greyscale

	namedWindow("Grey", 1);//creates a window named grey
	for (;;)// for loop same as using while
	{
		cap >> frame; // get a new frame from camera
		cvtColor(frame, grey, CV_BGR2GRAY);//converts the frame to greyscale , so only 1 channel instead of 3 channels 
 // Draw the line intensity
		int y = 240; //midpoint vertical point (640x480p) - half of 480
		for (int x = 0; x < grey.cols; x++)  //initializes x to equal 0 , then the loop tracks the video columns
		{
			intensity[x] = (int)grey.at<uchar>(y, x); //original intensity value , (int) allows the value from the uchar(unsigned character) to be turned into a int.
		}

		linehistDisplay(intensity, 640, "Original Histogram");  //display window , tracking  intensity values , 640 is related to 640x480p

		for (int x = 2; x < (grey.cols - 2); x++)  //this is the exact same as above however the filter is making the histogram lines smooth, better for tracking
		{
			filtintensity[x] = ((int)grey.at<uchar>(y, x - 2) + (int)grey.at<uchar>(y, x - 1) + (int)grey.at<uchar>(y, x) + (int)grey.at<uchar>(y, x + 1) + (int)grey.at<uchar>(y, x + 2)) / 5;  //new intensity calculation
				   //this is the calculation used to smoothen out the lines , the 2 is just used to smooth out the lines this could be changed to lower or higher if needed to make it smoother or more rugged
		}

		linehistDisplay(filtintensity, 640, "Filtered Histogram");  //NEW display window , tracking x intensity values , 640 is related to 640x480p

		left_location = 0;   //make sure location from left is set at 0
        right_location = 640;   //make sure location from right is set at 640

		for (int i = 120; i < (grey.cols - 10); i++)  //iteration for the start point of where it is determined across the column lines (allow 10mm from the left boundary of the wheel edge)
													  //120 can be changed depending if we want the histogram to start tracking at a certain height , 120 is a good value for the bike wheel 
		{
			if (filtintensity[i + 10] >(filtintensity[i] + 10))//condition for if the pixel is 10 pixels above the iterative line being scanned 
				   //if the filterintensity is 10pixels above the normal threshold then it will start track the location
			{
				left_location = i;   //initialise location_x to the new value which is the location
				break;  //terminates loop
			}
		}
        
		for (int j = 520; j < (grey.cols - 10); j--)  //iteration for the start point of where it is determined across the column lines (allow 10mm from the left boundary of the wheel edge)
													  //120 can be changed depending if we want the histogram to start tracking at a certain height , 120 is a good value for the bike wheel 
		{
			if (filtintensity[j + 10] >(filtintensity[j] + 10))//condition for if the pixel is 10 pixels above the iterative line being scanned 
																   //if the filterintensity is 10pixels above the normal threshold then it will start track the location
			{
                right_location = j;   //initialise location to the right to the new value which is the location
				break;  //terminates loop
			}
		}
		
        printf("location left = %d, location right = %d \n", left_location, right_location);   //print to the user the column being detected for highest intensity value's position
		imshow("Grey", grey);//Displays the video capture in greyscale
		x_co = (((left_location + right_location)/2)-320) / 10;//calculated the value by taking the locations dividing by 2 and then minusing 320 assume the pixel value is 10 pixel = 1mm

		printf("X:%d\n", x_co); //prints out the final value to show how much the wheel has buckled

		(waitKey(33) >= 0);//waits for a key to be pressed , videos wont run without this line of code in , as this function delays the capture by so much milliseconds
	}
	return 0;
}
//10 pixels is 1mm
