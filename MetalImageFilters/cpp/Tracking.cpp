#include "Tracking.h"
using namespace std;


Tracking::Tracking(){
    fileWrite = false;

}

Tracking::~Tracking(){

}

double markersX[7] = { -0.5,  0.5,  0.5,  1.0,  0.0, -1.0, -0.5 };
double markersY[7] = { -2.0, -2.0,  0.0,  0.0,  1.5,  0.0,  0.0 };
double markersZ[7] = {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0 };

double markersX2[7] = { -0.5,  0.5,  0.5,  1.0,  0.0, -1.0, -0.5 };
double markersY2[7] = { -2.0, -2.0,  0.0,  0.0,  1.5,  0.0,  0.0 };
double markersZ2[7] = {  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0 };


void Tracking::track(){

		cvtColor(bild, gray, cv::COLOR_BGR2GRAY);
			
		vector<float> err;
		if(prevGray.empty()) gray.copyTo(prevGray);	// only the first time


		if(	!punkteA.empty() ) {
			calcOpticalFlowPyrLK(prevGray, gray, punkteA, punkteB, status, err, cv::Size(31,31), 3, 
						cv::TermCriteria(cv::TermCriteria::MAX_ITER  | cv::TermCriteria::EPS, 20, 0.01), 0, 0.001);
          
			size_t i;
			for( i = 0; i < punkteB.size(); i++ ){
				if (status[i] ){ 
					cv::circle(bild, punkteB.at(i), 4, cv::Scalar(0,0,255), -1, CV_AA);
				} else {
					clearVecs();
                }
			}	
			writeVecs();
			calcMVP();
			clearVecs();
		}
		std::swap(punkteB, punkteA);
		cv::swap(prevGray, gray);
}



void Tracking::writeVecs(){
	for (int i = 0 ; i < punkteB.size(); i++) {

		if (status[i]) {
			screenX_VEC.push_back(punkteB.at(i).x);
			screenY_VEC.push_back(punkteB.at(i).y);

			markers_VEC_X.push_back(markersX[i]); //markersX
			markers_VEC_Y.push_back(markersY[i]); //markersY
			markers_VEC_Z.push_back(markersZ[i]);
		}
	} 
}



void Tracking::calcMVP(){

    Mat sx = Mat(1, (int) screenX_VEC.size());		for(int i = 0; i < screenX_VEC.size(); i++) sx.Set(0, i, screenX_VEC.at(i));
	Mat sy = Mat(1, (int) screenY_VEC.size());		for(int i = 0; i < screenY_VEC.size(); i++) sy.Set(0, i, screenY_VEC.at(i));
	Mat L  = Mat(3, (int) markers_VEC_X.size());
    Mat L1 = Mat(3, (int) markers_VEC_X.size());
	
	for(int i = 0; i < markers_VEC_X.size(); i++) {
		L.Set(0, i, markers_VEC_X.at(i));
		L.Set(1, i, markers_VEC_Y.at(i));
		L.Set(2, i, 0);
	}
    
    for(int i = 0; i < markers_VEC_X.size(); i++) {
        L1.Set(0, i, markers_VEC_X.at(i));
        L1.Set(1, i, markers_VEC_Y.at(i));
        L1.Set(2, i, 1.f);
    }

	mView = BlackBox.RecoverModelview(sx, sy, L);
    
    //  Mat p=BlackBox.Local2Screen(testpoint);
	Mat S = BlackBox.Local2Screen(L);
    Mat S1 = BlackBox.Local2Screen(L1);
    float vec3[]={0,0,1};
    Mat mvec3(3,1,vec3);

    for (int i = 0; i < S.n; i++){
        cv::circle(bild, cv::Point2d(S.Get(0,i), S.Get(1,i)), 3, cv::Scalar(255,0,0), -1, CV_AA);
        
        cv::circle(bild, cv::Point2d(S1.Get(0,i), S1.Get(1,i)), 3, cv::Scalar(0,255,0), -1, CV_AA);

        cv::line(bild, cv::Point2d(S.Get(0,i), S.Get(1,i)), cv::Point2d(S1.Get(0,i), S1.Get(1,i)),cv::Scalar(255,255,255));

        cv::line(bild, cv::Point2d(S.Get(0,0), S.Get(1,0)), cv::Point2d(S.Get(0,1), S.Get(1,1)),cv::Scalar(255,255,255));
        cv::line(bild, cv::Point2d(S.Get(0,1), S.Get(1,1)), cv::Point2d(S.Get(0,2), S.Get(1,2)),cv::Scalar(255,255,255));
        cv::line(bild, cv::Point2d(S.Get(0,2), S.Get(1,2)), cv::Point2d(S.Get(0,3), S.Get(1,3)),cv::Scalar(255,255,255));
        cv::line(bild, cv::Point2d(S.Get(0,3), S.Get(1,3)), cv::Point2d(S.Get(0,4), S.Get(1,4)),cv::Scalar(255,255,255));
        cv::line(bild, cv::Point2d(S.Get(0,4), S.Get(1,4)), cv::Point2d(S.Get(0,5), S.Get(1,5)),cv::Scalar(255,255,255));
        cv::line(bild, cv::Point2d(S.Get(0,5), S.Get(1,5)), cv::Point2d(S.Get(0,6), S.Get(1,6)),cv::Scalar(255,255,255));
        cv::line(bild, cv::Point2d(S.Get(0,6), S.Get(1,6)), cv::Point2d(S.Get(0,0), S.Get(1,0)),cv::Scalar(255,255,255));
        
        cv::line(bild, cv::Point2d(S1.Get(0,0), S1.Get(1,0)), cv::Point2d(S1.Get(0,1), S1.Get(1,1)),cv::Scalar(255,0,0));
        cv::line(bild, cv::Point2d(S1.Get(0,1), S1.Get(1,1)), cv::Point2d(S1.Get(0,2), S1.Get(1,2)),cv::Scalar(255,0,0));
        cv::line(bild, cv::Point2d(S1.Get(0,2), S1.Get(1,2)), cv::Point2d(S1.Get(0,3), S1.Get(1,3)),cv::Scalar(255,0,0));
        cv::line(bild, cv::Point2d(S1.Get(0,3), S1.Get(1,3)), cv::Point2d(S1.Get(0,4), S1.Get(1,4)),cv::Scalar(255,0,0));
        cv::line(bild, cv::Point2d(S1.Get(0,4), S1.Get(1,4)), cv::Point2d(S1.Get(0,5), S1.Get(1,5)),cv::Scalar(255,0,0));
        cv::line(bild, cv::Point2d(S1.Get(0,5), S1.Get(1,5)), cv::Point2d(S1.Get(0,6), S1.Get(1,6)),cv::Scalar(255,0,0));
        cv::line(bild, cv::Point2d(S1.Get(0,6), S1.Get(1,6)), cv::Point2d(S1.Get(0,0), S1.Get(1,0)),cv::Scalar(255,0,0));
    }
   
	if (fileWrite) {

		ofstream file ("_Koordinaten.txt", ios::out|ios::app);

		 if (file.is_open()) {
			file << "Screen Koordinaten" << endl;
			for(int i = 0; i < screenX_VEC.size(); i++) file << screenX_VEC.at(i) << "		" << screenY_VEC.at(i) << endl;
			file << "\nMarker Koordinaten" << endl;
			for (int i = 0; i < markers_VEC_X.size(); i++) file << markers_VEC_X.at(i) << "	" << markers_VEC_Y.at(i) << "	" << markers_VEC_Z.at(i) << endl;

			file << "\n\n\n";
			file.close();
		 }	
		fileWrite = false;  
	}
}


void Tracking::paintMouseDots(){
	for (int i = 0; i < mouseDots.size(); i++ ){ 
		cv::circle(bild, mouseDots.at(i),4, cv::Scalar(0,0,255), -1, CV_AA);
	}
}

void Tracking::addArrowPoints(){
    
    // Platini / 2 ---- (x/2+80, y/2+200)
    /*
    mouseDots.push_back(cv::Point2f(215,415));
    mouseDots.push_back(cv::Point2f(264,415));
    mouseDots.push_back(cv::Point2f(264,320));
    mouseDots.push_back(cv::Point2f(287.5,320));
    mouseDots.push_back(cv::Point2f(240,248.5));
    mouseDots.push_back(cv::Point2f(192.5,320));
    mouseDots.push_back(cv::Point2f(215,320));
    */
    
    // Platini centered
    mouseDots.push_back(cv::Point2f(200,430+20));
    mouseDots.push_back(cv::Point2f(298,430+20));
    mouseDots.push_back(cv::Point2f(298,240+20));
    mouseDots.push_back(cv::Point2f(345,240+20));
    mouseDots.push_back(cv::Point2f(250,97+20));
    mouseDots.push_back(cv::Point2f(155,240+20));
    mouseDots.push_back(cv::Point2f(200,240+20));

    /* Platini
    mouseDots.push_back(cv::Point2f(270,430));
    mouseDots.push_back(cv::Point2f(368,430));
    mouseDots.push_back(cv::Point2f(368,240));
    mouseDots.push_back(cv::Point2f(415,240));
    mouseDots.push_back(cv::Point2f(320,97));
    mouseDots.push_back(cv::Point2f(225,240));
    mouseDots.push_back(cv::Point2f(270,240));
    */
}


void Tracking::clearAll(){
	punkteA.clear();
	punkteB.clear();
	mouseDots.clear();
	status.clear();
	
	mView.m = mView.n = 0;
	clearVecs();
}

void Tracking::clearVecs(){
	screenX_VEC.clear();
	screenY_VEC.clear();
	markers_VEC_X.clear();
	markers_VEC_Y.clear();
	markers_VEC_Z.clear();
}
