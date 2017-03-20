/*
 * myGL.cpp
 *
 *  a simple OpenGL-"emulator" and
 *  reconstruction of modelview matrices from screen coordinates of 3D markers
 *
 *  author: Martin Hering-Bertram, 2015
 */
#include "myGL.h"
#include "Assert.hpp"
#include "Mat.h"
#include <stdio.h>
#include <math.h>
#include "matrix.h"

//#define M_PI 3.1415926535897931

//  constructors
myGL::myGL(void):Modelview(Mat::Eye(4)),Projection(Mat::Eye(4)),WinWidth(640.0),WinHeight(480.0),FocalLength(2.4),AspectRatio ( 1.5){
	
}
myGL::~myGL(void){
}

// define projection matrix and viewport
	// width, height: Screen resolution
	// aspectRatio: width/height if pixels are squares
	// focalLength: distance between eye point and image plane
	// returns P: projection 4x4-matrix
Mat myGL::DefineProjection( int width, int height, float aspectRatio, float focalLength){
	WinWidth = width;
	WinHeight = height;
	AspectRatio = aspectRatio;
	FocalLength = focalLength;

	/*@autor: Jassin 
	die Projection Matrix der Reale Kamera auf die Virtuelle Kamera abbilden
	Mat myGL::glFrustum( float left, float right, float bottom, float top, float near, float far)

	glFrustum( -4.0/3.0, 4.0/3.0,	1, -1, 2.0, 1000);
	*/
    Projection = glFrustum( -AspectRatio,AspectRatio, 1, -1, focalLength, 1000.0);
    
    Projection.Print("opengl Projection ");
   

	ReconsError = 100;
	return Projection;
}

// recover modelview matrix from marker projections
// ! projection matrix needs to be defined first !
// scx, scy: 1xn-matrices with screen coordinates of 3D markers
// L: 3xn-matrix with 3D marker coordinates
// returns M: modelview 4x4-matrix
Mat myGL::RecoverModelview( const Mat &scx, const Mat &scy, const Mat &L){
	
    float x[] = {1, 0, 0}, y[] = {0,1, 0}, z[] = {0, 0, 1};
	Mat J, V, err, delta;
	Mat M;
	// guess initial modelview matrix
	if (ReconsError < 5) M = Modelview;
	else {
		float t0[] = { 0, 0, -5.0};
		M = glTranslate( Mat(3, 1, t0) );
	}

 	 
  // Bug: return value erase the content of Projection
   Mat P=Projection;
    
//	Mat P (Projection.m,Projection.n,Projection.p);
    
	Mat vx = ApplyInverseViewport( scx, WinWidth);
	Mat vy = ApplyInverseViewport( scy, WinHeight);


	for( int k = 0; k < 2; k++){
		Mat J = Jacobi( &V, L, M, P);
		//sx = ApplyViewport( V.Row(0), w);
		//sy = ApplyViewport( V.Row(1), h);
		err = NextTo( vx-V.Row(0), vy-V.Row(1));
		//printf( "Err=%9.6lf\n", Norm( err));

		// least squares fitting
		delta = LSQ( J, T(err));
        
		// apply parameter changes
        M = glRotate( Mat(3,1,x), delta.Get(3,0)*180/M_PI) * M;
        M = glRotate( Mat(3,1,y), delta.Get(4,0)*180/M_PI) * M;
        M = glRotate( Mat(3,1,z), delta.Get(5,0)*180/M_PI) * M;
       
        M = glTranslate( delta.Get(0,0,3,1)) * M;
        
        //test
       // M = M*ref;
       
		// sanity check
		if( M.Get(2,3) > 0){
			M.Set(2,3, -M.Get(2,3));
		}
	}
	// Abfrage zum Berechnen des Errors aus der Schleife genommen.
	ReconsError = Norm(err);
    
	Modelview = M;
	return Modelview;
}


Mat myGL::glFrustum( float left, float right, float bottom, float top, float near, float far){
	// projection matrix from clipping planes
	Mat P(4,4);
	P.Set(0, 0, 2*near/(right  -left));
	P.Set(0, 2, (right + left) / (right - left));
	P.Set(1, 1, 2*near/(top - bottom));
	P.Set(1, 2, (top + bottom)/(top - bottom));
	P.Set(2, 2, (far + near)/(near-far));
	P.Set(2, 3, 2*far*near/(near - far));
	P.Set(3, 2, -1.0);
	return P;
}
Mat myGL::glRotate( const Mat &axis, float alpha){
	// rotation matrix around arbitrary axis
	// alpha in degrees
	Assert( axis.m==3 && axis.n==1, "axis is not a 3-vector in glRotate");
	Mat a = axis / Norm(axis);
	float c = cos( alpha*M_PI/180);
	float s = sin( alpha*M_PI/180);

	Mat Aij = a * T(a);

	Mat Ax =  Cross( a);

	Mat R = Mat::Eye(3)*c + Aij*(1-c) + Ax*s; // R = c*I + (1-c)*Aij + s*Ax;
	Mat M = Mat::Eye(4);
	M.Set( 0, 0, R);

	return M;
}
Mat myGL::glTranslate( const Mat &vec){
	// modelview matrix for translation
	Assert( (vec.m == 3 && vec.n == 1), "axis is not a 3-vector in glTranslate");
	Mat M = Mat::Eye(4);
	M.Set( 0, 3, vec);
	return M;
}
Mat myGL::ApplyViewport( const Mat &vx, float w){
	// transforms viewport into screen coordinates ([-1, 1] into [0, w])
	// vx: vector of x (or y) viewport coordinates in [-1, 1]
	// w:  screen width (or height)
	// sx: screen coordinates in [0, w]
	Mat sx = 0.5 * w * (vx + 1.0);
	return sx;
}
Mat myGL::ApplyInverseViewport( const Mat &sx, float w){
	// inverse viewport transform ([0, w] into [-1, 1])
	// vx: vector of x (or y) viewport coordinates in [-1, 1]
	// w:  screen width (or height)
	// sx: screen coordinates in [0, w]
	Mat vx = 2.0/w*sx - 1;
	return vx;
}

Mat myGL::ApplyProjection( Mat *C, const Mat &L, const Mat &M, const Mat &P){
	// compute camera and viewport coordinates
	// L: 3xn-matrix of points in local coordinates
	// M: modelview matrix
	// P: projektion matrix
	// returns V: 2xn-matrix with viewport coodrinates
	// computes C: 3xn-matrix of points in camera coordinates
	Assert( (P.m == 4 && P.n == 4), "P is not a 4x4-matrix in ApplyProjection");
	Assert( (M.m == 4 && M.n == 4), "M is not a 4x4-matrix in ApplyProjection");
	Assert((L.m == 3), "L is not a 4xn-matrix in ApplyProjection");

	// transform using homogeneous coordinates
	*C = M * OnTop( L, Mat::Ones( 1, L.n));
	Mat X = P * *C;
	Mat V = OnTop( X.Row(0)/X.Row(3), X.Row(1)/X.Row(3)); // perspective division
	*C = C->Get(0,0,3,L.n); // skip last row
	return V;
}

Mat myGL::Local2Screen( const Mat &L){
	// apply all transforms: modelview, projection and viewport
	Mat V, C, sx, sy;
	V = ApplyProjection( &C, L, Modelview, Projection);
    sx = ApplyViewport( V.Row(0), WinWidth);//V.Row(0);
    sy = ApplyViewport( V.Row(1), WinHeight);//V.Row(1);
    
	return OnTop( sx, sy);
}

Mat myGL::Jacobi( Mat *V, const Mat &L, const Mat &M, const Mat &P){
	// compute partial derivatives of viewport coordinates
	// L: 3xn-matrix of points in local coordinates
	// M: modelview matrix
	// P: projektion matrix
	// computes V: 2xn-matrix with viewport coodrinates
	// returns J: Jacobian 2nx6-matrix
	Assert( (P.m == 4 && P.n == 4), "P is not a 4x4-matrix in ApplyProjection");
	Assert( (M.m == 4 && M.n == 4), "M is not a 4x4-matrix in ApplyProjection");
	Assert((L.m == 3), "L is not a 4xn-matrix in ApplyProjection");

	// transform using homogeneous coordinates
	int n = L.n;
	Mat C = M * OnTop( L, Mat::Ones( 1, n));
	Mat X = P*C;
	Mat X1 = X.Row(0)/(X.Row(3)%X.Row(3));
	Mat X2 = X.Row(1)/(X.Row(3)%X.Row(3));
	*V = OnTop( X.Row(0)/X.Row(3), X.Row(1)/X.Row(3));

	float vcx[] = {1,0,0,1}, vcy[] = {0,1,0,1}, vcz[] = {0,0,1,1};
	Mat Xx = P * Mat(4,1,vcx);
	Mat Xy = P * Mat(4,1,vcy);
	Mat Xz = P * Mat(4,1,vcz);


	/* Änderung 20.01 
	float vca[] = {1,0,0,0, 0,0,-1,0, 0,1,0,0, 0,0,0,1};
	float vcb[] = {0,0,1,0, 0,1,0,0, -1,0,0,0, 0,0,0,1};
	float vcc[] = {0,-1,0,0, 1,0,0,0, 0,0,1,0, 0,0,0,1};
	*/

	float vca[] = {0,0,0,0,	0,0,-1,0,	0,1,0,0,	0,0,0,1};
	float vcb[] = {0,0,1,0,	0,0,0,0,	-1,0,0,0,	0,0,0,1};
	float vcc[] = {0,-1,0,0,	1,0,0,0,	0,0,0,0,	0,0,0,1};






	Mat Xa = P * T(Mat(4,4,vca)) * C;
	Mat Xb = P * T(Mat(4,4,vcb)) * C;
	Mat Xc = P * T(Mat(4,4,vcc)) * C;

	// set the Jacobian
	Mat J( 2*n, 6);
	J.Set( 0, 0, T( Xx.Get(0,0)/X.Row(3) - X1*Xx.Get(3,0)));
	J.Set( n, 0, T( Xx.Get(1,0)/X.Row(3) - X2*Xx.Get(3,0)));
	J.Set( 0, 1, T( Xy.Get(0,0)/X.Row(3) - X1*Xy.Get(3,0)));
	J.Set( n, 1, T( Xy.Get(1,0)/X.Row(3) - X2*Xy.Get(3,0)));
	J.Set( 0, 2, T( Xz.Get(0,0)/X.Row(3) - X1*Xz.Get(3,0)));
	J.Set( n, 2, T( Xz.Get(1,0)/X.Row(3) - X2*Xz.Get(3,0)));
	J.Set( 0, 3, T( Xa.Row(0)/X.Row(3) - X1%Xa.Row(3)));
	J.Set( n, 3, T( Xa.Row(1)/X.Row(3) - X2%Xa.Row(3)));
	J.Set( 0, 4, T( Xb.Row(0)/X.Row(3) - X1%Xb.Row(3)));
	J.Set( n, 4, T( Xb.Row(1)/X.Row(3) - X2%Xb.Row(3)));
	J.Set( 0, 5, T( Xc.Row(0)/X.Row(3) - X1%Xc.Row(3)));
	J.Set( n, 5, T( Xc.Row(1)/X.Row(3) - X2%Xc.Row(3)));

	return J;
}


void myGL::TestIt(){

	// !!!!!!!!!!!!!!!!to be completed
	// set projection matrix and viewport
    Mat P=glFrustum( -1.3333, 1.3333, -1, 1, 2, 10000);
	float h = 786, w = 1024; // resolution

	// define an arbitrary modelview matrix
	float x[] = {1, 0, 0}, y[] = {0, 1, 0}, z[] = {0, 0, 1}, t[] = {.4, -.9, -6};
	Mat M = glRotate( Mat(3,1,x), 30);
	M = glRotate( Mat(3,1,y), -35) * M;
	M = glTranslate( Mat(3,1,t)) * M;

	M.Print( "Rot");

	// compute projection of a cube
	float vL[] = {0,0,0,0,1,1,1,1, 0,0,1,1,1,1,0,0, 0,1,1,0,0,1,1,0};
	Mat L = T(Mat(8,3,vL));
	L.Print( "L");
	Mat V, C;
	V = ApplyProjection( &C, L, M, P);
	//C.Print( "C");
	//V.Print( "V");
	Mat sx1 = ApplyViewport( V.Row(0), w);
	Mat sy1 = ApplyViewport( V.Row(1), h);
	sx1.Print("Sx1");
	sy1.Print("Sy1");

#if 0
	float VTest[]= {0.1333,-0.0366,-0.1571, 0.0405, 0.3717, 0.2023, 0.3063, 0.4493,
	                 0.4000, 0.7057, 0.2917, 0.0162, 0.0181, 0.3306, 0.7915, 0.4423};
	(V-T(Mat(8,2,VTest))).Print("should be zeros"); // sanity check
#endif


	// now, reconstruct M
	float vt0[] = { 0, 0, -5};
	M = glTranslate( Mat(3,1,vt0));
	Mat vx = ApplyInverseViewport( sx1, w);
	Mat vy = ApplyInverseViewport( sy1, h);
	Mat J, sx, sy, err, delta;

	for( int k=0; k<10; k++){
		Mat J = Jacobi( &V, L, M, P);
		sx = ApplyViewport( V.Row(0), w);
		sy = ApplyViewport( V.Row(1), h);
		err = NextTo( vx-V.Row(0), vy-V.Row(1));
		printf( "Err=%9.6lf\n", Norm( err));

		delta = LSQ( J, T(err));

		M = glRotate( Mat(3,1,x), delta.Get(3,0)*180/M_PI) * M;
		M = glRotate( Mat(3,1,y), delta.Get(4,0)*180/M_PI) * M;
		M = glRotate( Mat(3,1,z), delta.Get(5,0)*180/M_PI) * M;
		M = glTranslate( delta.Get(0,0,3,1)) * M;

		// sanity check
		if( M.Get(2,3) > 0){
			M.Set( 2,3, -M.Get(2,3));
		}
	}
	V = ApplyProjection( &C, L, M, P);

	// screen coordinates
	sx = ApplyViewport( V.Row(0), w);
	sy = ApplyViewport( V.Row(1), h);
	sx.Print("sx");
	sy.Print("sy");
	M.Print("M");

	return;


#if 0
    float v[] = {1, 2, 3};
	float rmat[] = { 0.8756,   -0.3818,    0.2960,         0,
				0.4200,    0.9043,   -0.0762,         0,
				-0.2386,    0.1910,    0.9522,         0,
				0,         0,         0,    1.0000};
	Mat RMat(4,4);
	RMat.SetRows( rmat);
	Mat axis(3, 1, v);
	Mat R = glRotate( axis, 30);
	//cout << cos(30*M_PI/180) << endl;
	//Mat R = glRotate( axis, 20);
	R-=RMat;
    (R).Print();
	P.Print();
#endif

#if 0
	float x[]={1, 0, 3, 4, 5, 6, 7, 8, 9};
	float z[]={ 4, -1, 2};
	float y[]={6, 7, 8, 9, 10, 11};
	myGL gl( 1024, 768, 2);

	gl.TestIt();


	cout << "Here we go!" << endl;

	Mat a = Mat(3,3, x);
	//a.Row(1).Print();
	//a.Col(1).Print();
	a.Print();
	Mat b = Mat( 3, 1, z);
	b.Print();
	Mat c = LSQ( a, b);
	c.Print();

	Mat M = Eye(5);
	M = 5 * M;
	M.Print();

	Mat a=Mat(2,3, x);
	Mat b = Eye(5);
	b.Print();

	Vec v=Vec(3, x);
	Vec w=Vec(3, y);
	a.Print();
	T(a).Print();
	Mat(v).Print();
	b=a*Mat(v);
	b.Print();


	//v.Print();
	//char c1;
	//cin >> c1;
#endif
}


