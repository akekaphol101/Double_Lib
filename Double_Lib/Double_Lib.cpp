#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <vector>
#include <string>
#include <stdio.h>


using namespace cv;
using namespace std;
using namespace std::chrono;
int largest_area = 0;
int largest_contour_index = 0;
Rect bounding_rect;
Mat src_gray;
RNG rng(12345);
Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));

void show_histogram(string const& name, Mat1b const& image)
{
	float max = 0;
	for (int i = 0; i < image.cols; i++)
	{
		int column_sum = 0;
		for (int k = 0; k < image.rows; k++)
		{
			column_sum += image.at<unsigned char>(k, i);
			if (column_sum > max) {
				max = column_sum;
			}
		}
	}
	//cout << "Max " << max  << endl;

	// Set histogram bins count
	int bins = image.cols;
	// Set ranges for histogram bins
	float lranges[] = { 0, bins };
	const float* ranges[] = { lranges };
	// create matrix for histogram
	Mat hist;
	int channels[] = { 0 };
	float maxN = max / 10;
	float const hist_height = maxN;
	Mat3b hist_image = Mat3b::zeros(hist_height + 10, bins + 20);

	int countA = 0;
	float height_A[630];
	float high_1 = 0;
	float high_2 = 0;
	int col_high_1 = 0;
	int col_high_2 = 0;
	Mat dst;
	int sum_col = 0;
	for (int i = 0; i < image.cols; i++)
	{
		float column_sum = 0;
		
		for (int k = 0; k < image.rows; k++)
		{
			column_sum += image.at<unsigned char>(k, i);
		}

		float const height = cvRound(column_sum * hist_height / max);
		line(hist_image, Point(i + 10, (hist_height - height) + 20), Point(i + 10, hist_height), Scalar::all(255));
		//cout << "hishigh " << hist_height << "------ " << hist_height - height << endl;
		if (height > high_1 && i < 500) {
			high_1 = height;
			col_high_1 = i;
		}

		if(height > high_2 && i >= 1000) {
			high_2 = height;
			col_high_2 = i;
		}
		sum_col += column_sum;
		
		//cout << "column summ  " << column_sum << endl;
	}
	int AVG_sum = sum_col/(image.cols * image.rows);
	cout << "summ  " << AVG_sum << endl;
	cout << "high1   " << high_1<< " col_1  " << col_high_1 << endl;
	cout << "high 2  " << high_2<< " col 2 "<< col_high_2 << endl;


	float H_AVG = 0;
	// Loop find Average low
	for (int i = 0; i < image.cols; i++)
	{
		float column_sum = 0;

		for (int k = 0; k < image.rows; k++)
		{
			column_sum += image.at<unsigned char>(k, i);
		}

		float const height = cvRound(column_sum * hist_height / max);

		if (i >= col_high_1  && i <= col_high_2)
		{
			//cout << "H--" << height << endl;
			H_AVG += height;

		}
	}
	H_AVG = H_AVG/10000;			//best value for average 
	//float H_Minus = H_AVG - high;

	//cout << "high " << high << endl;
	//cout << "col high " << col_high << endl;
	cout << "high Average------ " << H_AVG << endl;
	//cout << "high-- Minus==== " << H_Minus << endl;



	Mat canny_output;
	Canny(hist_image, canny_output, 50, 50 * 2);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	
	// create hull array for convex hull points
	vector< vector<Point> > hull(contours.size());
	for (int i = 0; i < contours.size(); i++) {
		convexHull(Mat(contours[i]), hull[i], 1);
	}

	drawContours(hist_image, contours, 0, Scalar(0, 0, 255), 2, 8, vector<Vec4i>(), 0, Point());
	//drawContours(hist_image, hull, 0, Scalar(0, 255, 0), 2, 8, vector<Vec4i>(), 0, Point());
	//cout << " AreaC: " << contourArea(contours[0]) << endl;
	//cout << " AreaH: " << contourArea(hull[0]) << endl;
	float reN = 0;
	reN = contourArea(hull[0]) - contourArea(contours[0]);
	//cout << " Result: " << reN << endl;
	if (H_AVG > 7) {
		cout << " Defect Detection  " << endl;
		cout << "===================" << endl;
	}
	else {
		cout << " Non defect Detection  " << endl;
		cout << "===================" << endl;
	}
	imshow(name, hist_image);
	//This Code tell runtime this program.
	vector<int> values(10000);
	auto f = []() -> int { return rand() % 10000; };
	generate(values.begin(), values.end(), f);
	auto start = high_resolution_clock::now();
	sort(values.begin(), values.end());
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(stop - start);
	cout << "Time taken by function: " << duration.count() << " milliseconds" << endl;
	//imshow(name, hist_image);
}




int Recheck(Mat imageOriginal) {
	Mat imgG, imgCut, imgRz, imgTh, imgCanny;
	int status = 0;
	imgRz = imageOriginal.clone();
	cvtColor(imgRz, imgG, COLOR_BGR2GRAY);
	

	Rect myROI(0, 0, 110, 500);
	Mat croppedRef(imgRz, myROI);

	Mat imgCrop;
	// Copy the data into new matrix
	croppedRef.copyTo(imgCrop);
	


	imshow("REz", imgCrop);
	
	Mat imgSobelx;
	Sobel(imgCrop, imgSobelx, CV_8U, 1, 0, 3, 1, 0, BORDER_DEFAULT);
	rotate(imgSobelx, imgSobelx, ROTATE_90_COUNTERCLOCKWISE);
	imshow("Sobel X ", imgSobelx);

	show_histogram("name", imgSobelx);

	return status;
}



int main(int argc, const char* argv[]) {

	Mat imgOri;
	Mat imgRz, imgG, imgCn, imgMr, imgPoLog, imgPoLin, imgRePoLin, imgRePoLog;
	int recheck = 0;

	string folder("img/*.jpg");
	vector<String> fn;
	glob(folder, fn, false);
	vector<Mat> images;
	size_t count = fn.size(); //number of png files in images folder.\

	//Check number of images.
	cout << "image in folder  " << count << endl;

	//Main LooB.
	for (size_t i = 0; i < count; i++)
	{
		//Preprocessing
		imgOri = imread(fn[i]);
		resize(imgOri, imgRz, Size(), 0.5, 0.5); //Half Resize 1280*1040 to 640*520 pixcel.
		imshow("ori", imgRz);

		recheck = Recheck(imgRz);
		//imshow("Resize", imgRz);
		//cout << " Last status" << recheck << endl;

		waitKey(0);
	}
	waitKey(0);
	return 0;

}