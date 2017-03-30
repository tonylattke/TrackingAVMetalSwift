//
//  ViewController.swift
//  MetalImageFilters
//
//  Created by Tony Lattke on 26.02.17.
//
//

import UIKit
import MetalPerformanceShaders
import MetalKit

// MARK: ViewController
class ViewController: MTKViewController {
    
    // Properties
    var tap:Bool = false
    var startTracking:Bool = false
    var initTracking:Bool = false
    var stopTracking:Bool = false
    var tracker:TrackingWrapper?
    
    // Camera delegate
    lazy var videoImageTextureProvider: VideoImageTextureProvider? = {
        let provider = VideoImageTextureProvider(delegate: self)
        return provider
    }()
    
    // MARK: Init
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Setting Tap recognition
        let tapProperty = UITapGestureRecognizer(target: self, action: #selector(ViewController.handleTap(_:)))
        self.view.addGestureRecognizer(tapProperty)
        // Setting Tracker
        tracker = TrackingWrapper()
        tracker?.addArrowPoints()
    }
    
    // MARK: Init main loop
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        videoImageTextureProvider?.startRunning()
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        tracker = nil
    }
    
    // Handle Tap
    func handleTap(_ sender: UITapGestureRecognizer) {
        tap = !tap
        if (tap) {
            startTracking = true
            initTracking = true
        } else {
            startTracking = false
            initTracking = false
            stopTracking = true
        }
    }
}

// Array of Float to Matrix
func floatArrayToFloat4x4(array: [Float]) -> float4x4 {
    var matrix = float4x4()
    if !array.isEmpty {
        var i = 0
        while i < 4 {
            var j = 0
            while j < 4 {
                matrix[i][j] = array[i*4 + j]
                j += 1
            }
            i += 1
        }
    }
    return matrix
}

// MARK: AVCaptureVideoDataOutputSampleBufferDelegate
extension ViewController: VideoImageTextureProviderDelegate {
    
    func videoImageTextureProvider(_: VideoImageTextureProvider, pixelBuffer: CVPixelBuffer) {
        
        DispatchQueue.global(qos: .userInitiated).sync {
            // Set image in the render engine
            self.backgroundPlane.loadTexture(pixelBuffer: pixelBuffer)
            // Set image in the tracking function
            self.tracker?.setImage(pixelBuffer)
            
            // Tracking
            if (self.startTracking && self.initTracking){ // Init
                self.tracker?.setPunkteB()
                self.tracker?.mouseDotsClear()
                let aspectRatio: Float = Float(self.view.bounds.size.width / self.view.bounds.size.height)
                self.tracker?.blackBoxDefineProjection(Int32(self.view.bounds.size.width), Int32(self.view.bounds.size.height), aspectRatio, 2)
                self.showCubeUpdate(value: true)
                self.tracker?.paintMouseDots()
                self.initTracking = false
            } else if (self.startTracking) { // Loop
                self.tracker?.track()
                if (self.tracker?.getMViewN() != 0 && self.tracker?.getMViewM() != 0) {
                    // mview of tracking set in cube
                    self.cube.mV = floatArrayToFloat4x4(array: self.tracker?.getMViewP() as! [Float])
                }
            } else if (self.stopTracking){ // Finish
                self.tracker?.clearAll()
                self.stopTracking = false
                self.tracker?.addArrowPoints()
                self.showCubeUpdate(value: false)
            }
            
            // Update cube information
            let newPosition: float3 = self.cube.position
            let newRotation: float3 = float3(self.cube.rotation.x-0.05,self.cube.rotation.y-0.05,self.cube.rotation.z)
            let newScale: float3 = self.cube.scale
            self.updateCube(position: newPosition, rotation: newRotation, scale: newScale)
        
            // Call the MTKView's draw() method whenever the camera provides a new video frame
            DispatchQueue.main.async {
                self.mtkView.draw()
            }
            
        }
    }
}

