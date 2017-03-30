/*
 * Mat.cpp
 *
 *  a lot of fun with matrices
 *
 *  author: Martin Hering-Bertram, 2015
 */

#include "Mat.h"
#include "Assert.hpp"
#include <stdio.h>
#include <math.h>

Mat::Mat():m(0),n(0),p(nullptr)
{

}

Mat::~Mat()
{
    if (m*n>0 )
        delete  [] p;
}

Mat::Mat( int m1, int n1):m(m1),n(n1){
	
	p = new float[m*n]();
}

Mat::Mat( int m1, int n1, float *x):m(m1),n(n1){
	
	p = new float[m*n];
    memcpy(p, x, m*n*sizeof(float));
	
}
Mat::Mat(const Mat& mat):Mat(mat.m,mat.n,mat.p){
}
Mat::Mat(const Mat&& mat):m(mat.m),n(mat.n),p(mat.p){
    
}
Mat &Mat::operator=( const Mat &a){
	if (&a != this){
        if (a.n != this->n || a.m != this->m) {
            m = a.m;
            n = a.n;
            if(p!=nullptr)
                delete [] p;
            p = new float[m*n];
        }
        memcpy(p, a.p, m*n*sizeof(float));
	}
	return *this;
}

Mat &Mat::operator+=( const Mat &a){
	Assert( m==a.m && n==a.n, "Mat dimensions must agree");
	for(int i=0; i<m*n; i++) p[i]+=a.p[i];
	return *this;
}
Mat &Mat::operator-=( const Mat &a){
	Assert( m==a.m && n==a.n, "Mat dimensions must agree");
	for(int i=0; i<m*n; i++) p[i]-=a.p[i];
	return *this;
}
Mat &Mat::operator*=( float sc){
	for(int i=0; i<m*n; i++) p[i]*=sc;
	return *this;
}
Mat &Mat::operator/=( float sc){
    Assert(sc != 0.0, "Division by Zero operator /= scalar 0 ");
	for(int i=0; i<m*n; i++) p[i]/=sc;
	return *this;
}

Mat operator-( const Mat &a){
	Mat c ( a.m, a.n);
	for( int i=0; i<a.m*a.n; i++) c.p[i] = -a.p[i];
	return c;
}
Mat operator+( const Mat &a, const Mat &b){
	Assert( a.m==b.m && a.n==b.n, "Mat dimensions must agree");
	Mat c ( a.m, a.n);
	for( int i=0; i<a.m*a.n; i++) c.p[i] = a.p[i] + b.p[i];
	return c;
}
Mat operator-( const Mat &a, const Mat &b){
	Assert( a.m==b.m && a.n==b.n, "Mat dimensions must agree");
	Mat c ( a.m, a.n);
	for( int i=0; i<a.m*a.n; i++) c.p[i] = a.p[i] - b.p[i];
	return c;
}
Mat operator+( const Mat &a, float sc){
	Mat c ( a.m, a.n);
	for( int i=0; i<a.m*a.n; i++) c.p[i] = a.p[i] + sc;
	return c;
}
Mat operator-( const Mat &a, float sc){
	Mat c ( a.m, a.n);
	for( int i=0; i<a.m*a.n; i++) c.p[i] = a.p[i] - sc;
	return c;
}
Mat operator+( float sc, const Mat &a){
	Mat c ( a.m, a.n);
	for( int i=0; i<a.m*a.n; i++) c.p[i] = a.p[i] + sc;
	return c;
}
Mat operator-( float sc, const Mat &a){
	Mat c ( a.m, a.n);
	for( int i=0; i<a.m*a.n; i++) c.p[i] = sc - a.p[i];
	return c;
}
Mat operator%( const Mat &a, const Mat &b){ // component wise mult
	Assert( a.m==b.m && a.n==b.n, "Mat dimensions must agree");
	Mat c ( a.m, a.n);
	for( int i=0; i<a.m*a.n; i++) c.p[i] = a.p[i] * b.p[i];
	return c;
}
Mat operator*( const Mat &a, const Mat &b){ // matrix mult
	Assert( a.n==b.m, "Mat dimensions not compatible");
	Mat c( a.m, b.n);
	for( int i=0; i<c.m; i++){
		for( int j=0; j<c.n; j++){
			c.p[i+j*c.m] = 0;
			for( int k=0; k<a.n; k++){
				c.p[i+j*c.m] += a.p[i+k*a.m] * b.p[k+j*b.m];
			}
		}
	}
	return c;
}
Mat operator*( const Mat &a, float sc){
	Mat c ( a.m, a.n);
	for( int i=0; i<a.m*a.n; i++) c.p[i] = a.p[i] * sc;
	return c;
}
Mat operator*( float sc, const Mat &a){
	Mat c ( a.m, a.n);
	for( int i=0; i<a.m*a.n; i++) c.p[i] = a.p[i] * sc;
	return c;
}

Mat operator/( const Mat &a, const Mat &b){ // component wise div
	Assert( a.m==b.m && a.n==b.n, "Mat dimensions must agree");
	Mat c ( a.m, a.n);
    for( int i=0; i<a.m*a.n; i++){
        Assert(b.p[i] != 0.0, "Division by Zero matrix element is 0");
        c.p[i] = a.p[i] / b.p[i];
    }
	return c;
}

Mat operator/( const Mat &a, float sc){
    Assert(sc != 0.0, "Division by Zero operator / scalar 0");
	Mat c ( a.m, a.n);
	for( int i=0; i<a.m*a.n; i++) c.p[i] = a.p[i] / sc;
	return c;
}
Mat operator/( float sc, const Mat &a){
	Mat c ( a.m, a.n);
    for( int i=0; i<a.m*a.n; i++){
        Assert(a.p[i] != 0.0, "Division by Zero C=s/A matrix element ist 0");
        c.p[i] = sc / a.p[i];
    }
	return c;
}

Mat T( const Mat &a){ // transpose
	Mat c(a.n, a.m);
	for( int i=0; i<c.m; i++){
		for( int j=0; j<c.n; j++){
			c.p[ i+j*c.m] = a.p[ j+i*a.m];
		}
	}
	return c;
}

Mat Mat::Eye(int n){
	Mat c (n, n);
	for( int i=0; i<n; i++) c.p[ i*(1+n)] = 1;
	return c;
}
Mat Mat::Ones(int m, int n){
	Mat c( m, n);
	for( int i=0; i<m*n; i++) c.p[i] = 1;
	return c;
}

Mat OnTop( const Mat &a, const Mat &b){
	Assert( a.n==b.n, "Matrix dimensions must agree in OnTop");
	Mat c( a.m+b.m, a.n);
	for( int j=0; j<a.n; j++){
		for( int i=0; i<a.m; i++) c.p[i+j*c.m] = a.p[i+j*a.m];
		for( int i=0; i<b.m; i++) c.p[i+a.m+j*c.m] = b.p[i+j*b.m];
	}
	return c;
}

Mat NextTo( const Mat &a, const Mat &b){
	Assert( a.m==b.m, "Matrix dimensions must agree in OnTop");
	Mat c ( a.m, a.n+b.n);
	for( int i=0; i<a.m; i++){
		for( int j=0; j<a.n; j++) c.p[i+j*c.m] = a.p[i+j*a.m];
		for( int j=0; j<b.n; j++) c.p[i+(j+a.n)*c.m] = b.p[i+j*b.m];
	}
	return c;
}

Mat LU( const Mat &a){ // LU decomposition in one matrix
	Assert( a.m==a.n, "Mat must be square");
	int n = a.n;
	Mat c ( n, n);
	    for(int k=0; k<n; k++){
            for(int i=k; i<n; i++){
                float sum=0;
                for(int p=0; p<k; p++) sum += c.p[i+p*n] * c.p[p+k*n];
				c.p[i+k*n] = a.p[i+k*n]-sum;
			}
			for(int j=k+1; j<n; j++){
			float sum=0;
			for(int p=0; p<k; p++) sum += c.p[k+p*n] * c.p[p+j*n];
			c.p[k+j*n] = (a.p[k+j*n] - sum) / c.p[k+k*n];
		}
	}
	return c;
}

Mat Solve( const Mat &a, const Mat &b){
	Assert( a.m == a.n && a.m == b.m && b.n ==1, "Mat incompatible dimensions");
	Mat lu = LU( a);
	int n = a.n;
	Mat x (n,1);
	Mat y (n,1);
	for(int i=0; i<n; i++){
		float sum=0;
		for(int k=0; k<i; k++) sum += lu.p[i+k*n] * y.p[k];
		y.p[i] = (b.p[i] - sum) / lu.p[i+i*n];
	 }
	 for(int i=n-1; i>=0; i--){
		float sum=0;
		for(int k=i+1; k<n; k++) sum += lu.p[i+k*n] * x.p[k];
		x.p[i] = y.p[i] - sum;
   }
   return x;
}

Mat LSQ( const Mat &a, const Mat &b){
	return Solve( T(a)*a, T(a)*b);
}
Mat Cross( const Mat &a, const Mat &b){
	Assert( a.m == 3 && b.m == 3 && a.n == b.n, "dimension mismatch in cross");
	Mat c ( 3, a.n);
	for( int i=0; i<a.n; i++){
		c.Set( 0, i, a.p[ 1+i*3]*b.p[2+i*3] - a.p[ 2+i*3]*b.p[1+i*3]);
		c.Set( 1, i, a.p[ 2+i*3]*b.p[0+i*3] - a.p[ 0+i*3]*b.p[2+i*3]);
		c.Set( 2, i, a.p[ 0+i*3]*b.p[1+i*3] - a.p[ 1+i*3]*b.p[0+i*3]);
	}
	return c;
}
Mat Cross( const Mat &a){
	Mat c( 3,3);
	Assert( a.m*a.n == 3, "dimension mismatch in cross");
	c.Set( 2, 1, a.p[0]);
	c.Set( 1, 2, -a.p[0]);
	c.Set( 0, 2, a.p[1]);
	c.Set( 2, 0, -a.p[1]);
	c.Set( 1, 0, a.p[2]);
	c.Set( 0, 1, -a.p[2]);
	return c;
}
float Norm( const Mat &a){
	float d=0;
	for( int i=0; i<a.n*a.m; i++) d += a.p[i]*a.p[i];
	return sqrt( d);
}

void Mat::Set( int i, int j, float s){
	Assert( i>=0 && j>=0 && i<m && j<n, "Mat invalid index");
	p[ i+j*m] = s;
}
void Mat::Set( int i, int j, const Mat &a){
	Assert( i>=0 && j>=0 && i+a.m<=m && j+a.n<=n,
		"Mat invalid index");
	for( int k=0; k<a.m; k++){
		for( int l=0; l<a.n; l++){
			p[ i+k+(j+l)*m] = a.p[ k+l*a.m];
		}
	}
}
void Mat::Set( float *vec){
	for( int i=0; i<m*n; i++){
		p[ i] = vec[ i];
	}
}
void Mat::SetRows( float *vec){
	for( int i=0; i<m; i++){
		for( int j=0; j<n; j++){
			p[ i+j*m] = vec[ i*n+j];
		}
	}
}
float Mat::Get( int i, int j){
	return p[i+j*m];
}
Mat Mat::Get( int i, int j, int m1, int n1){
	Assert( i>=0 && j>=0 && i+m1<=m && j+n1<=n,
		"Mat invalid index");
	Mat c( m1, n1);
	for( int k=0; k<m1; k++){
		for( int l=0; l<n1; l++){
			c.p[ k+l*m1] = p[ i+k+(j+l)*m];
		}
	}
	return c;
}

Mat Mat::Row( int i){
	return Get( i, 0, 1, n);
}
Mat Mat::Col( int j){
	return Get( 0, j, m, 1);
}
void Mat::Print( char *c){
	if(c!=NULL) printf("%s:\n", c);
    else
        printf("MAT :\n");
	for( int i=0; i<m; i++){
			for( int j=0; j<n; j++){
				printf(" %8.4lf", p[i+j*m]);
			}
			printf("\n");
		}
    
}

