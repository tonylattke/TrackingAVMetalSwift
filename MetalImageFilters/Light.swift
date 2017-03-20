//
//  Light.swift
//  MetalCube
//
//  Created by Tony Lattke on 24.11.16.
//  Copyright © 2016 Hochschule Bremen. All rights reserved.
//

import Foundation

struct Light {
    
    // Color
    var color: (Float, Float, Float)
    
    // Ambient
    var ambientIntensity: Float
    
    // Diffuse
    var direction: (Float, Float, Float)
    var diffuseIntensity: Float
    
    // Specular
    var shininess: Float // It is not a parameter of light, it’s more like a parameter of object material
    var specularIntensity: Float
    
    static func size() -> Int {
        return MemoryLayout<Float>.size * 12
    }
    
    func raw() -> [Float] {
        let raw = [color.0, color.1, color.2, ambientIntensity, direction.0, direction.1, direction.2, diffuseIntensity, shininess, specularIntensity]
        return raw
    }
}
