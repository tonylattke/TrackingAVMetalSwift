//
//  TrackingWrapper.h
//  TrackingSwift
//
//  Created by Tony Lattke on 16.02.17.
//  Copyright © 2017 HSB. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface TrackingWrapper : NSObject

- (instancetype)init;
- (void)addArrowPoints;
- (void)setPunkteB;
- (void)mouseDotsClear;
- (void)BlackBoxDefineProjection:(int)width :(int)height :(float)aspectRatio :(float)focalLength :(NSString*) name;
- (void)track;
- (int)getMViewN;
- (int)getMViewM;
- (int)getImageWidth;
- (int)getImageHeight;
- (NSArray*)getMViewP;
- (void)clearAll;
- (void)paintMouseDots;
- (void)setImage: (CVPixelBufferRef) pixelBuffer;

@end
