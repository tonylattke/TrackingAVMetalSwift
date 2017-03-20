//
//  TrackingWrapper.m
//  TrackingSwift
//
//  Created by Tony Lattke on 16.02.17.
//  Copyright © 2017 HSB. All rights reserved.
//

#import "TrackingWrapper.h"
#import <opencv2/opencv.hpp>

#include "Tracking.h"

@interface TrackingWrapper()
// C++ Properties
@property Tracking *cppItem;
// Functions
// None
@end

@implementation TrackingWrapper

- (instancetype)init{
    if (self = [super init]) {
        self.cppItem = new Tracking();
    }
    return self;
}

- (void)addArrowPoints{
    self.cppItem->addArrowPoints();
}

- (void)dealloc{
    delete self.cppItem;
    //[super dealloc];
}

- (void)setImage: (CVPixelBufferRef) pixelBuffer{
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    
    unsigned char *base = (unsigned char *)CVPixelBufferGetBaseAddress(pixelBuffer);
    size_t width = CVPixelBufferGetWidth(pixelBuffer);
    size_t height = CVPixelBufferGetHeight(pixelBuffer);
    
    //size_t stride = CVPixelBufferGetBytesPerRow(pixelBuffer);
    cv::Mat bgraImage = cv::Mat((int)width, (int)height, CV_8UC4, base);
    //cv::Mat bgraImageCopy = cv::Mat((int)height, (int)width, CV_8UC4);
    //cv::flip(bgraImage, bgraImage,1);
    //cv::transpose(bgraImage, bgraImage);
    
    cv::Mat bgraImageCopy = bgraImage.t();
    bgraImageCopy.data = base;
    
   // cv::circle(bgraImageCopy, cv::Point2i(240,320), 4, cv::Scalar(0,255,0), -1);
    
//    for (int i = 0; i<height; i++) {
//            for (int j = 0; j<width; j++) {
//                cv::circle(bgraImageCopy, cv::Point2i(j,i), 1, cv::Scalar(0,255,0), -1);
//            }
//    }
    
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    self.cppItem->setImage(bgraImageCopy);
}

- (void)setPunkteB{
    self.cppItem->punkteB = self.cppItem->mouseDots;
}

- (void)mouseDotsClear{
    self.cppItem->mouseDots.clear();
}

- (void) BlackBoxDefineProjection:(int)width :(int)height :(float)aspectRatio :(float)focalLength :(NSString*) name {
    self.cppItem->BlackBox.DefineProjection(width,height,aspectRatio,focalLength);
    char* nameChar = strdup([name UTF8String]);
    self.cppItem->BlackBox.Projection.Print(nameChar);
}

- (void)track{
    self.cppItem->track();
}

- (int)getMViewN{
    return self.cppItem->mView.n;
}

- (int)getMViewM{
    return self.cppItem->mView.m;
}

- (int)getImageWidth{
    return self.cppItem->bild.rows;
}

- (int)getImageHeight{
    return self.cppItem->bild.cols;
}

- (NSArray*)getMViewP{
    //;
    int size=self.getMViewM*self.getMViewN;
    NSMutableArray *tmpArray;
    
    tmpArray = [NSMutableArray arrayWithCapacity:size]; 
    for(int i = 0; i < size; i++)
    [tmpArray addObject:[NSNumber numberWithFloat:self.cppItem->mView.p[i]]];
    
    NSArray *myArray = [NSArray arrayWithArray:tmpArray];
    return myArray;
}

- (void)clearAll{
    self.cppItem->clearAll();
}

- (void)paintMouseDots{
    self.cppItem->paintMouseDots();
}



@end
