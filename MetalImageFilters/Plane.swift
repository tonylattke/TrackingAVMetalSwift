//
//  Cube.swift
//  MetalCube
//
//  Created by Tony Lattke on 24.11.16.
//  Copyright © 2016 Hochschule Bremen. All rights reserved.
//

import UIKit
import MetalKit

class Plane: Node {
    
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
        
        // Initialize Node
        super.init(name: name, vertices: verticesArray, device: device, texture: texture)
    }
   
    // Update Delta
    override func updateWithDelta(delta: CFTimeInterval) {
        super.updateWithDelta(delta: delta)
    }
}
