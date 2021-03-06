//
//  MTKViewController.swift
//  MetalImageFilters
//
//  Created by Tony Lattke on 27.02.17.
//
//

import UIKit
import MetalPerformanceShaders
import MetalKit

class MTKViewController: UIViewController {
    // MARK: IB Outlets
    @IBOutlet weak var mtkView: MTKView!
    
    // MARK: Metal Properties
    let device = MTLCreateSystemDefaultDevice()!
    var commandQueue: MTLCommandQueue!
    var sourceTexture: MTLTexture?
    var bufferProvider: BufferProvider!
    var renderPassDescriptor: MTLRenderPassDescriptor!
    var textureLoader: MTKTextureLoader! = nil
    
    // Pipelines
    var pipelineState: MTLRenderPipelineState! = nil        // All shaders
    var basicPipelineState: MTLRenderPipelineState! = nil   // Without lights
    var cameraPipelineState: MTLRenderPipelineState! = nil  // Without lights and projection
    
    // DepthStencils
    var depthStencilState: MTLDepthStencilState! = nil      // Depth buffer (GLDepthMask) -> true
    var cameraDepthStencilState: MTLDepthStencilState! = nil// Depth buffer (GLDepthMask) -> false
    
    // MARK: Scene objects
    var camera: Camera!
    var cube: Cube!
    var backgroundPlane: Plane!
    var light: Light!
    var showCube: Bool = false
    var point: Point!
    var mv: float4x4?
    
    // MARK: Scene init
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Metal Setup
        setupMetal()
        
        // Initialize Buffer
        let sizeOfUniformsBuffer = MemoryLayout<Float>.size * float4x4.numberOfElements() * 2 + Light.size()
        bufferProvider = BufferProvider(device: device, inflightBuffersCount: 3, sizeOfUniformsBuffer: sizeOfUniformsBuffer)
        
        // Create camera far plane
        backgroundPlane = Plane(name: "Plane", device: device, commandQ: commandQueue, textureLoader: textureLoader, srcImage: "black", typeImage: "jpg")
        backgroundPlane.scale = float3(x:282, y: 500, z: 1) // 14:25 * 20
        backgroundPlane.position.z = -994.0 // -1000 (far plane) + 5 (camera position) +1
        
        // Create a cube
        cube = Cube(name: "Cube", device: device, commandQ: commandQueue, textureLoader: textureLoader, srcImage: "cube", typeImage: "png")
        
        // Create Light
        light = Light(color: (1.0,1.0,1.0), ambientIntensity: 0.2, direction: (0.0, 0.0, 1.0), diffuseIntensity: 0.8, shininess: 10, specularIntensity: 2)
        
        point = Point(name: "Point", device: device, commandQ: commandQueue, textureLoader: textureLoader, srcImage: "black", typeImage: "jpg")
    }
    
    // MARK: Metal Setup
    private func setupMetal() {
        // Init Camera
        let position = float3(0,0,5)
        let lookAt = float3(0,0,0)
        let up = float3(0,1,0)
        let aspectRatio: Float = Float(self.view.bounds.size.width / self.view.bounds.size.height)
//        print("width \(self.view.bounds.size.width )")
//        print("height \(self.view.bounds.size.height )")
        camera = Camera(position: position, lookAt: lookAt, up: up, aspectRatio: aspectRatio, angleDregrees: 65.0, nearPlan: 2, farPlan: 1000.0)
        
        // Init Device
        textureLoader = MTKTextureLoader(device: device)
        mtkView.device = device
        
        // Setting depthStencilDescriptor
        let depthStencilDescriptor = MTLDepthStencilDescriptor()
        depthStencilDescriptor.depthCompareFunction = .less // less comparisions
        depthStencilDescriptor.isDepthWriteEnabled = true
        depthStencilState = device.makeDepthStencilState(descriptor: depthStencilDescriptor)
        
        let cameraDepthStencilDescriptor = MTLDepthStencilDescriptor()
        cameraDepthStencilDescriptor.depthCompareFunction = .less
        cameraDepthStencilDescriptor.isDepthWriteEnabled = false
        cameraDepthStencilState = device.makeDepthStencilState(descriptor: cameraDepthStencilDescriptor)
        
        // Setting command queue
        commandQueue = device.makeCommandQueue()
        
        // Create a connection with the Shaders
        let defaultLibrary = device.newDefaultLibrary()
        // Vertex Shaders
        let vertexProgram = defaultLibrary!.makeFunction(name: "basic_vertex")
        let cameraVertexProgram = defaultLibrary!.makeFunction(name: "camera_vertex")
        // Fragment Shaders
        let basicFragmentProgram = defaultLibrary!.makeFunction(name: "basic_fragment")
        let lightFragmentProgram = defaultLibrary!.makeFunction(name: "light_complex_fragment")
        
        // Create a Render Pipeline
        let pipelineStateDescriptor = MTLRenderPipelineDescriptor()
        pipelineStateDescriptor.vertexFunction = vertexProgram
        pipelineStateDescriptor.fragmentFunction = lightFragmentProgram
        pipelineStateDescriptor.colorAttachments[0].pixelFormat = .bgra8Unorm
        // Create a Render Pipeline without lights
        let basicPipelineStateDescriptor = MTLRenderPipelineDescriptor()
        basicPipelineStateDescriptor.vertexFunction = vertexProgram
        basicPipelineStateDescriptor.fragmentFunction = basicFragmentProgram
        basicPipelineStateDescriptor.colorAttachments[0].pixelFormat = .bgra8Unorm
        // Create a Render Pipeline without lights and projections
        let cameraPipelineStateDescriptor = MTLRenderPipelineDescriptor()
        cameraPipelineStateDescriptor.vertexFunction = cameraVertexProgram
        cameraPipelineStateDescriptor.fragmentFunction = basicFragmentProgram
        cameraPipelineStateDescriptor.colorAttachments[0].pixelFormat = .bgra8Unorm
        
        // Pipeline connection
        do {
            pipelineState = try device.makeRenderPipelineState(descriptor: pipelineStateDescriptor)
            basicPipelineState = try device.makeRenderPipelineState(descriptor: basicPipelineStateDescriptor)
            cameraPipelineState = try device.makeRenderPipelineState(descriptor: cameraPipelineStateDescriptor)
        } catch  {
            print("Error creating pipeline")
        }
        
        // View setup
        mtkView.framebufferOnly = false
        mtkView.isPaused = true
        mtkView.delegate = self
        mtkView.device = device
        mtkView.colorPixelFormat = .bgra8Unorm
        
        // Render Pass descriptor
        renderPassDescriptor = MTLRenderPassDescriptor()
    }
    
    // MARK: Start loop
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
    }
    
    public func updateCube(position:float3, rotation:float3, scale:float3){
         /*cube.position = position
         cube.rotation = rotation
         cube.scale = scale*/
    }
    
    public func showCubeUpdate(value: Bool){
        showCube = value
    }
}

// MARK: MTKViewDelegate
extension MTKViewController: MTKViewDelegate {
    
    // Update the view content (for example the manage of rotation)
    func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {
    }
    
    func draw(in view: MTKView) {
        // Use a guard to ensure the method has a valid current drawable
        guard
            let currentDrawable = view.currentDrawable
            else {
                return
        }
        let destinationTexture = currentDrawable.texture
        
        // Background
        renderPassDescriptor.colorAttachments[0].texture = destinationTexture
        renderPassDescriptor.colorAttachments[0].loadAction = .clear
        renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColor(red: 1.0, green: 1.0, blue: 1.0, alpha: 1.0)
        
        // Wait to the next Buffer to draw
        _ = bufferProvider.availableResourcesSemaphore.wait(timeout: .distantFuture)
        
        // Get available buffer and create a command buffer
        let commandBuffer = commandQueue.makeCommandBuffer()
        commandBuffer.addCompletedHandler { (commandBuffer) -> Void in
            self.bufferProvider.availableResourcesSemaphore.signal()
        }
        
        // Create and setting a Render Command Encoder
        let renderEncoderOpt = commandBuffer.makeRenderCommandEncoder(descriptor: renderPassDescriptor)
        renderEncoderOpt.setCullMode(MTLCullMode.front)
        
        // Set depth stencil with depth mask false
        renderEncoderOpt.setDepthStencilState(cameraDepthStencilState)
        
        // Render plane with the current content of the camera
        backgroundPlane.render(pipelineState: cameraPipelineState, camera: camera, renderEncoderOpt: renderEncoderOpt, bufferProvider: bufferProvider, light: light, mView:backgroundPlane.modelMatrix())
  
        // Set depth stencil with depth mask true
        renderEncoderOpt.setDepthStencilState(depthStencilState)

        //point.render(pipelineState: basicPipelineState, camera: camera, renderEncoderOpt: renderEncoderOpt, bufferProvider: bufferProvider, light: light, mView: point.modelMatrix())
        
        // Render the cube
        if (showCube) {
            if let mview  = cube.mV {
                //mview.translate(-0.7, y: 0, z: 0)
                cube.render(pipelineState: pipelineState, camera: camera, renderEncoderOpt: renderEncoderOpt, bufferProvider: bufferProvider, light: light, mView:mview)
            } else {
                cube.render(pipelineState: pipelineState, camera: camera, renderEncoderOpt: renderEncoderOpt, bufferProvider: bufferProvider, light: light, mView:cube.modelMatrix())
            }
        }
        
        // Encode
        renderEncoderOpt.endEncoding()
        
        // Schedule a presentation.
        commandBuffer.present(currentDrawable)
        
        // Commit the command buffer to the GPU.
        commandBuffer.commit()
    }
}
