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

func floatArrayToFloat4x4(array: [Float]) -> float4x4 {
    var matrix = float4x4()
    
    matrix[0][0] = array[0]
    matrix[0][1] = array[1]
    matrix[0][2] = array[2]
    matrix[0][3] = array[3]
    
    matrix[1][0] = array[4]
    matrix[1][1] = array[5]
    matrix[1][2] = array[6]
    matrix[1][3] = array[7]

    
    matrix[2][0] = array[8]
    matrix[2][1] = array[9]
    matrix[2][2] = array[10]
    matrix[2][3] = array[11]
    
    
    matrix[3][0] = array[12]
    matrix[3][1] = array[13]
    matrix[3][2] = array[14]
    matrix[3][3] = array[15]
    /*
    var i = 0
    while i < 4 {
        var j = 0
        while j < 4 {
            matrix[i][j] = array[i*4 + j]
            j += 1
        }
        i += 1
    }
    */
    return matrix
}

// MARK: AVCaptureVideoDataOutputSampleBufferDelegate
extension ViewController: VideoImageTextureProviderDelegate {
    
    func videoImageTextureProvider(_: VideoImageTextureProvider, pixelBuffer: CVPixelBuffer) {
        
        DispatchQueue.global(qos: .userInitiated).sync {
            self.backgroundPlane.loadTexture(pixelBuffer: pixelBuffer)

            self.tracker?.setImage(pixelBuffer)
            
            if (self.startTracking && self.initTracking){
                self.tracker?.setPunkteB()
                self.tracker?.mouseDotsClear()
                let aspectRatio: Float = Float(self.view.bounds.size.width / self.view.bounds.size.height)
                self.tracker?.blackBoxDefineProjection(Int32(self.view.bounds.size.width), Int32(self.view.bounds.size.height), aspectRatio, 2, "Test")
                //self.tracker?.blackBoxDefineProjection(640, 480, 1.33, 2, "Test")
                self.showCubeUpdate(value: true)
                self.tracker?.paintMouseDots()
                self.initTracking = false
            } else if (self.startTracking) {
                self.tracker?.track()
                if (self.tracker?.getMViewN() != 0 && self.tracker?.getMViewM() != 0) {
                    // mview of tracking set in cube
                    self.cube.mV = floatArrayToFloat4x4(array: self.tracker?.getMViewP() as! [Float])
                }
            } else if (self.stopTracking){
                self.tracker?.clearAll()
                self.stopTracking = false
                self.tracker?.addArrowPoints()
                self.showCubeUpdate(value: false)
            }
            
            if (self.showCube){
                let newPosition: float3 = self.cube.position
                let newRotation: float3 = float3(self.cube.rotation.x-0.05,self.cube.rotation.y-0.05,self.cube.rotation.z)
                let newScale: float3 = self.cube.scale
                self.updateCube(position: newPosition, rotation: newRotation, scale: newScale)
            }
            
            //call the MTKView's draw() method whenever the camera provides a new video frame
            DispatchQueue.main.async {
                self.mtkView.draw()
            }
            
        }
/*
        // Update background
        DispatchQueue.main.sync {
            sourceTexture = texture
        }
        
        if (!testing){
            tracker?.setImage(pixelBuffer)
            
            if (self.startTracking && self.initTracking){
                self.tracker?.setPunkteB()
                self.tracker?.addArrowPoints() // check
                self.tracker?.blackBoxDefineProjection((self.tracker?.getImageWidth())!, (self.tracker?.getImageHeight())!, 1.7777, 1, "Test")
                showCubeUpdate(value: true)
                self.tracker?.paintMouseDots()
                self.initTracking = false
            } else if (self.startTracking) {
                self.tracker?.track()
                if (self.tracker?.getMViewN() != 0 && self.tracker?.getMViewM() != 0) {
                    // mview of tracking set in cube
                }
            } else if (self.stopTracking){
                self.tracker?.clearAll()
                self.stopTracking = false
                self.tracker?.mouseDotsClear() // check
                showCubeUpdate(value: false)
            }
        }
        
        if (testing){
            showCube = true
        }

        if (showCube){
            let newPosition: float3 = cube.position
            let newRotation: float3 = float3(cube.rotation.x-0.05,cube.rotation.y-0.05,cube.rotation.z)
            let newScale: float3 = cube.scale
            updateCube(position: newPosition, rotation: newRotation, scale: newScale)
        }
        
        //call the MTKView's draw() method whenever the camera provides a new video frame
        DispatchQueue.main.async {
            self.mtkView.draw()
        }
         */
    }
}

