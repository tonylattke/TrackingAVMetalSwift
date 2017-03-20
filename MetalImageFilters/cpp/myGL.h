/*
 * myGL.h
 *
 *  a simple OpenGL-"emulator" and
 *  reconstruction of modelview matrices from screen coordinates of 3D markers
 *
 *  author: Martin Hering-Bertram, 2015
 */

#ifndef MYGL_H_
#define MYGL_H_

#include "Mat.h"
#include <math.h>
#include <stdio.h>

class myGL
{
   
public:
    float tempProj[16];
	int WinWidth, WinHeight;
	float AspectRatio, FocalLength;
	float ReconsError;
	Mat Modelview, Projection; // modelview and projection matrices
    
	//  constructors
	myGL(void);
	~myGL(void);

	// define projection matrix and viewport
	// width, height: Screen resolution
	// aspectRatio: width/height if pixels are squares
	// focalLength: distance between eye point and image plane
	// returns P: projection 4x4-matrix
	Mat DefineProjection( int width, int height, float aspectRatio, float focalLength);

	// recover modelview matrix from marker projections
	// ! projection matrix needs to be defined first !
	// scx, scy: 1xn-matrices with screen coordinates of 3D markers
	// L: 3xn-matrix with 3D marker coordinates
	// returns M: modelview 4x4-matrix
	Mat RecoverModelview( const Mat &scx, const Mat &scy, const Mat &L);

	// projection matrix from clipping planes
	Mat glFrustum( float left, float right, float top, float bottom, float near, float far);

	// rotation matrix around arbitrary axis
	// alpha in degrees
	Mat glRotate( const Mat &axis, float alpha );

	// modelview matrix for translation
	Mat glTranslate( const Mat &vec);

	// transforms viewport into screen coordinates ([-1, 1] into [0, w])
	// vx: vector of x (or y) viewport coordinates in [-1, 1]
	// w:  screen width (or height)
	// returns sx: screen coordinates in [0, w]
	Mat ApplyViewport( const Mat &vx, float w);

	// inverse viewport transform ([0, w] into [-1, 1])
	// returns vx: vector of x (or y) viewport coordinates in [-1, 1]
	// w:  screen width (or height)
	// sx: screen coordinates in [0, w]
	Mat ApplyInverseViewport( const Mat &sx, float w);

	// compute camera and viewport coordinates
	// L: 3xn-matrix of points in local coordinates
	// M: modelview matrix
	// P: projektion matrix
	// returns V: 2xn-matrix with viewport coodrinates
	// computes C: 3xn-matrix of points in camera coordinates
	Mat ApplyProjection( Mat *C, const Mat &L, const Mat &M, const Mat &P);

	// apply all transforms: modelview, projection and viewport
	Mat Local2Screen( const Mat &L);

	// Jacobi matrix for reconstruction
	Mat Jacobi( Mat *V, const Mat &L, const Mat &M, const Mat &P);

	void TestIt();
};



#endif /* MYGL_H_ */
