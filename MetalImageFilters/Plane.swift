//
//  Cube.swift
//  MetalCube
//
//  Created by Tony Lattke on 24.11.16.
//  Copyright © 2016 Hochschule Bremen. All rights reserved.
//

import UIKit
import MetalKit

class Plane {
    
    // Name of model
    let name: String
    var mV: float4x4?
    
    // Vertex info
    var vertexCount: Int
    var vertexBuffer: MTLBuffer
    var device: MTLDevice
    var textureCache : CVMetalTextureCache?
    // Transformation
    var position:float3 = float3(0,0,0)
    var rotation:float3 = float3(0,0,0)
    var scale:float3    = float3(1.0,1.0,1.0)
    
    // Time
    var time:CFTimeInterval = 0.0
    
    // Texture
    var texture: MTLTexture?
    lazy var samplerState: MTLSamplerState? = Plane.defaultSampler(device: self.device)
    
    // Initialization
    init(name: String, device: MTLDevice, commandQ: MTLCommandQueue, textureLoader :MTKTextureLoader, srcImage: String, typeImage: String){
        // Create vertices at the origin
        let A = Vertex(x: -1.0, y:   1.0, z:   0, r:  0.0, g:  0.0, b:  0.0, a:  1.0, s: 0.0, t: 0.0, nX: 0.0, nY: 0.0, nZ: 1.0)
        let B = Vertex(x: -1.0, y:  -1.0, z:  0, r:  0.0, g:  0.0, b:  0.0, a:  1.0, s: 0.0, t: 1.0, nX: 0.0, nY: 0.0, nZ: 1.0)
        let C = Vertex(x:  1.0, y:  -1.0, z:  0, r:  0.0, g:  0.0, b:  0.0, a:  1.0, s: 1.0, t: 1.0, nX: 0.0, nY: 0.0, nZ: 1.0)
        let D = Vertex(x:  1.0, y:   1.0, z:  0, r:  0.0, g:  0.0, b:  0.0, a:  1.0, s: 1.0, t: 0.0, nX: 0.0, nY: 0.0, nZ: 1.0)
        
        // Array of vertices
        let verticesArray:Array<Vertex> = [
            A,B,C,
            A,C,D
        ]
        
        // Create a default black texture
        let path = Bundle.main.path(forResource: srcImage, ofType: typeImage)!
        let data = NSData(contentsOfFile: path) as! Data
        let texture = try! textureLoader.newTexture(with: data, options: [MTKTextureLoaderOptionSRGB : (false as NSNumber)])
        
        // Vertex data
        var vertexData = Array<Float>()
        for vertex in verticesArray{
            vertexData += vertex.floatBuffer()
        }
        
        CVMetalTextureCacheCreate(kCFAllocatorDefault,
                                  nil,
                                  device,
                                  nil,
                                  &textureCache)
        
        
        // Init vertex buffer
        let dataSize = vertexData.count * MemoryLayout.size(ofValue: vertexData[0])
        vertexBuffer = device.makeBuffer(bytes: vertexData, length: dataSize)
        vertexCount = verticesArray.count
        
        // Set name, device and texture
        self.name = name
        self.device = device
        self.texture = texture
    }
    
    deinit {
        //TODO: dealloc(1) to each var
    }
    
    func loadTexture(pixelBuffer: CVPixelBuffer){
        var cameraTexture: CVMetalTexture?
        let cameraTextureWidth = CVPixelBufferGetWidthOfPlane(pixelBuffer, 0)
        let cameraTextureHeight = CVPixelBufferGetHeightOfPlane(pixelBuffer, 0)
        CVMetalTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                  textureCache!,
                                                  pixelBuffer,
                                                  nil,
                                                  MTLPixelFormat.bgra8Unorm_srgb,
                                                  cameraTextureWidth,
                                                  cameraTextureHeight,
                                                  0,
                                                  &cameraTexture)
        
        texture = CVMetalTextureGetTexture(cameraTexture!)
    }
    
    // Render Scene
    func render(pipelineState: MTLRenderPipelineState, camera: Camera, renderEncoderOpt: MTLRenderCommandEncoder, bufferProvider: BufferProvider, light: Light, mView:float4x4){
        
        renderEncoderOpt.setRenderPipelineState(pipelineState)
        
        let viewMatrix: float4x4 = camera.getViewMatrix()
        let projectionMatrix: float4x4 = camera.getProjectionCamera()
        
        // Set memory buffer
        mV = self.modelMatrix()
        
        
        var value = mView
        value.multiplyLeft(viewMatrix)
        
        let uniformBuffer = bufferProvider.nextUniformsBuffer(projectionMatrix: projectionMatrix, modelViewMatrix: value, light: light)
        
        renderEncoderOpt.setVertexBuffer(vertexBuffer, offset: 0, at: 0)
        renderEncoderOpt.setFragmentBuffer(uniformBuffer, offset: 0, at: 1)
        renderEncoderOpt.setFragmentTexture(texture, at: 0)
        if let samplerState = samplerState{
            renderEncoderOpt.setFragmentSamplerState(samplerState, at: 0)
        }
        renderEncoderOpt.setVertexBuffer(uniformBuffer, offset: 0, at: 1)
        
        // Draw primitives
        renderEncoderOpt.drawPrimitives(type: .triangle, vertexStart: 0, vertexCount: vertexCount, instanceCount: vertexCount/3)
    }
    
    func modelMatrix() -> float4x4 {
        var matrix = float4x4()
        matrix.translate(position.x, y: position.y, z: position.z)
        matrix.rotateAroundX(rotation.x, y: rotation.y, z: rotation.z)
        matrix.scale(scale.x, y: scale.y, z: scale.z)
        return matrix
    }
   
    // Update Delta
    func updateWithDelta(delta: CFTimeInterval) {
        time += delta
    }
    
    class func defaultSampler(device: MTLDevice) -> MTLSamplerState {
        let pSamplerDescriptor:MTLSamplerDescriptor? = MTLSamplerDescriptor()
        
        if let sampler = pSamplerDescriptor {
            sampler.minFilter             = MTLSamplerMinMagFilter.linear
            sampler.magFilter             = MTLSamplerMinMagFilter.linear
            sampler.mipFilter             = MTLSamplerMipFilter.linear
            sampler.maxAnisotropy         = 1
            sampler.sAddressMode          = MTLSamplerAddressMode.clampToEdge
            sampler.tAddressMode          = MTLSamplerAddressMode.clampToEdge
            sampler.rAddressMode          = MTLSamplerAddressMode.clampToEdge
            sampler.normalizedCoordinates = true
            sampler.lodMinClamp           = 0
            sampler.lodMaxClamp           = FLT_MAX
        } else {
            print("ERROR: Failed creating a sampler descriptor!")
        }
        return device.makeSamplerState(descriptor: pSamplerDescriptor!)
    }
}
