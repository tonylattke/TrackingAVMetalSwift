//
//  VideoImageTextureProviderDelegate.swift
//  MetalImageFilters
//
//  Created by Tony Lattke on 28.02.17.
//
//

import UIKit
import AVFoundation

// MARK: VideoImageTextureProvider

/// Provides an interface for sending/receiving a stream of Metal textures.
protocol VideoImageTextureProviderDelegate: class {
    func videoImageTextureProvider(_: VideoImageTextureProvider, pixelBuffer: CVPixelBuffer)
    //func convertPixelBufferToMetalTexture(pixelBuffer: CVImageBuffer) -> MTLTexture?
}
