//
//  Point.swift
//  MetalImageFilters
//
//  Created by Tony Lattke on 09.03.17.
//
//

import UIKit
import MetalKit

class Point: Node {
    
    // Initialization
    init(name: String, device: MTLDevice, commandQ: MTLCommandQueue, textureLoader :MTKTextureLoader, srcImage: String, typeImage: String){
        // Create vertices at the origin
        let A = Vertex(x: 0.0, y:   0.0, z:   0, r:  0.0, g:  0.0, b:  0.0, a:  1.0, s: 0.0, t: 0.0, nX: 0.0, nY: 0.0, nZ: 1.0)
        
        // Array of vertices
        let verticesArray:Array<Vertex> = [A]
        
        // Create a default black texture
        let path = Bundle.main.path(forResource: srcImage, ofType: typeImage)!
        let data = NSData(contentsOfFile: path) as! Data
        let texture = try! textureLoader.newTexture(with: data, options: [MTKTextureLoaderOptionSRGB : (false as NSNumber)])
        
        // Initialize Node
        super.init(name: name, vertices: verticesArray, device: device, texture: texture)
    }
    
    // Update Delta
    override func updateWithDelta(delta: CFTimeInterval) {
        super.updateWithDelta(delta: delta)
    }
    
    override func render(pipelineState: MTLRenderPipelineState, camera: Camera, renderEncoderOpt: MTLRenderCommandEncoder, bufferProvider: BufferProvider, light: Light, mView:float4x4){
        
        renderEncoderOpt.setRenderPipelineState(pipelineState)
        
        let viewMatrix: float4x4 = camera.getViewMatrix()
        let projectionMatrix: float4x4 = camera.getProjectionCamera()
        
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
        renderEncoderOpt.drawPrimitives(type: .point, vertexStart: 0, vertexCount: 1, instanceCount: 1)
    }
}
