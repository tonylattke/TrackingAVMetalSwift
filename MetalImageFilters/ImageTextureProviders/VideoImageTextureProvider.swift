/*
    Copyright (C) 2017 Apple Inc. All Rights Reserved.
    See LICENSE.txt for this sample’s licensing information
    
    Abstract:
    Video Image Texture Provider for MetalImageFilters.
            Uses CoreVideo buffers to load a stream of AVFoundation video images into a Metal texture.
            The Metal textures are delivered to the View Controller via a protocol delegate.
 */

import UIKit
import AVFoundation

/// Initializes an AVFoundation capture session for streaming real-time video.
class VideoImageTextureProvider: NSObject {
       let captureSession = AVCaptureSession()
    let sampleBufferCallbackQueue = DispatchQueue(label: "MetalImageFiltersQueue")
    weak var delegate: VideoImageTextureProviderDelegate!
    
    // MARK: Initialization

    /// Returns an initialized VideoImageTextureProvider object with an associated Metal device and delegate, or nil in case of failure.
    required init?(delegate: VideoImageTextureProviderDelegate) {
        super.init()
        
               self.delegate = delegate
        
        // Class initialization fails if the capture session could not be initialized.
        if(!didInitializeCaptureSession()) {
            return nil
        }
    }
    
    /// Attempts to initialize a capture session.
    func didInitializeCaptureSession() -> Bool {
        
        /* The capture session preset is fixed at a 960x540 pixel resolution that matches the MTKView pixel resolution.
           This ensures screen size compatibility with all target iOS devices, without having to downsample or transform the video image.
        */
        captureSession.sessionPreset = AVCaptureSessionPreset640x480
      //  captureSession.sessionPreset = AVCaptureSessionPresetLow
        
        // Use a guard to ensure the method can access a video capture device with a given camera position
        guard let camera = AVCaptureDevice.defaultDevice(withDeviceType: .builtInWideAngleCamera,
                                                         mediaType: AVMediaTypeVideo,
                                                         position: .back)
            else {
                print("Unable to access camera.")
                return false
        }
        
        do {
            let input = try AVCaptureDeviceInput(device: camera)
            if(captureSession.canAddInput(input)) {
                captureSession.addInput(input)
            }
            else {
                print("Unable to add camera input.")
                return false
            }
        }
        catch let error as NSError {
            print("Error accessing camera input: \(error)")
            return false
        }
        
        /* Creates a video data output object with a 32-bit BGRA pixel format.
           Setting self to the output object's sample buffer delegate allows this class to respond to every
           frame update.
         */
        let videoOutput = AVCaptureVideoDataOutput()
        videoOutput.videoSettings = [kCVPixelBufferPixelFormatTypeKey as AnyHashable: Int(kCVPixelFormatType_32BGRA)]
        videoOutput.setSampleBufferDelegate(self, queue: sampleBufferCallbackQueue)
        
        if captureSession.canAddOutput(videoOutput) {
            captureSession.addOutput(videoOutput)
        }
        else {
            print("Unable to add camera input.")
            return false
        }
        
        return true
    }
    
    // MARK: Capture Session Controls
    func startRunning() {
        sampleBufferCallbackQueue.async {
            self.captureSession.startRunning()
        }
    }
    
    func stopRunning() {
        captureSession.stopRunning()
    }
}

// MARK: AVCaptureVideoDataOutputSampleBufferDelegate
extension VideoImageTextureProvider: AVCaptureVideoDataOutputSampleBufferDelegate {
    func captureOutput(_ captureOutput: AVCaptureOutput!, didOutputSampleBuffer sampleBuffer: CMSampleBuffer!, from connection: AVCaptureConnection!) {
        
        //change
        connection.videoOrientation = AVCaptureVideoOrientation(rawValue: UIApplication.shared.statusBarOrientation.rawValue)! // UIDevice.current.orientation.rawValue
        
            let pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer)
        delegate.videoImageTextureProvider(self, pixelBuffer: pixelBuffer!)
       
    }
    
}
