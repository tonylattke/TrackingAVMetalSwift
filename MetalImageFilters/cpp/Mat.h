 /*
 * Mat.h
 *
 *  a lot of fun with matrices
 *
 *  author: Martin Hering-Bertram, 2015
 */

#ifndef MAT_H_
#define MAT_H_

#include <math.h>
#include <stdio.h>
#define PI 3.1415926535897931
class Mat
{
public:
	int m, n;
	float *p;
    
	Mat(void);
	virtual ~Mat(void);
    //copy constructor
    Mat (const Mat& );
    //Move constructor
    Mat (const Mat&& );
	//  constructors
	Mat( int m, int n);
	Mat( int m, int n, float *x);

	// operations on a single matrix
	Mat &operator=( const Mat &a);
	Mat &operator+=( const Mat &a);
	Mat &operator-=( const Mat &a);
	Mat &operator*=( float sc);
	Mat &operator/=( float sc);

	// operations with matrices or matrix-scalar
	friend Mat operator-( const Mat &a);
	friend Mat operator+( const Mat &a, const Mat &b);
	friend Mat operator-( const Mat &a, const Mat &b);
	friend Mat operator+( const Mat &a, float sc);
	friend Mat operator-( const Mat &a, float sc);
	friend Mat operator+( float sc, const Mat &a);
	friend Mat operator-( float sc, const Mat &a);
	friend Mat operator%( const Mat &a, const Mat &b); // component wise mult
	friend Mat operator*( const Mat &a, const Mat &b); // matrix mult
	friend Mat operator*( const Mat &a, float sc);
	friend Mat operator*( float sc, const Mat &a);
	friend Mat operator/( const Mat &a, const Mat &b); // component wise div
	friend Mat operator/( const Mat &a, float sc);
	friend Mat operator/( float sc, const Mat &a);

	friend Mat T( const Mat &a); // transpose
	friend Mat OnTop( const Mat &a, const Mat &b);
	friend Mat NextTo( const Mat &a, const Mat &b);
	friend Mat LU( const Mat &a);
	friend Mat Solve( const Mat &a, const Mat &b); // solve system of equations
	friend Mat LSQ( const Mat &a, const Mat &b); // solve by least squares

	// vector functions
	friend Mat Cross( const Mat &a, const Mat &b);
	friend Mat Cross( const Mat &a);
	friend float Norm( const Mat &a);

	// getter and setter
	void Set( int i, int j, float s);
	void Set( int i, int j, const Mat &a);
	void Set( float *vec);
	void SetRows( float *vec);
	float Get( int i, int j);
	Mat Get( int i, int j, int m, int n);
	Mat Row( int i);
	Mat Col( int j);

    static Mat Eye( int n);
    static Mat Ones( int m, int n);
	
	void Print( char *c = NULL);
};
//Mat &Eye( int n);
//Mat &Ones( int m, int n);

#endif /* MAT_H_ */
