#pragma once
//#include "opencv2/video/tracking.hpp"
#include <stdio.h>
#include <iostream>
#include "myGL.h"
#include "Mat.h"
#include <vector>
#include <fstream>
#include <opencv2/opencv.hpp>
using namespace std;


class Tracking {

public:
	Tracking();
	virtual ~Tracking();
	
	
	void addArrowPoints();
	void track();
	void clearAll();
	void paintMouseDots();
    

	myGL BlackBox;
	Mat mView, S;
    Mat proj;

	double screenX_Array[7];
	double screenY_Array[7];

	vector<double> screenX_VEC;
	vector<double> screenY_VEC;
	vector<double> markers_VEC_X;
	vector<double> markers_VEC_Y;
	vector<double> markers_VEC_Z;

	vector<cv::Point2f> punkteA, punkteB, mouseDots;
	cv::Mat gray, prevGray;
	cv::Mat bild;

	
	void setImage( cv::Mat bild) { this->bild = bild;		}
	bool fileWrite;
    
    vector<double> screenPoints;

private:

    void writeVecs();
	void calcMVP();
	void clearVecs();

	vector<uchar> status;
};
