# UnrealGPUBoids
 
This project is a minimal example for using compute shaders in the Unreal Engine. The other examples out there are overly complicated for someone getting started, in my opinion.

``FComputeShaderDeclaration`` in [ComputeShaderTestComponent.h](Source/ComputeShaderExample/ComputeShaderTestComponent.h) is how we bind our buffers from our Unreal classes to the compute shader. You write one of these for each of your compute shaders. 


``UComputeShaderTestComponent`` in [ComputeShaderTestComponent.h](Source/ComputeShaderExample/ComputeShaderTestComponent.h) contains the structured-buffer and unordered-access-views (UAVs) that we bind to the compute shader. We setup and bind to our compute shader in TickComponent, on the render command queue.

``UDrawPositionsComponent`` in [ComputeShaderTestComponent.h](Source/ComputeShaderExample/DrawPositionsComponent.h) draws the positions from the ``UComputeShaderTestComponent`` using an ``UInstancedStaticMeshComponent`` (ISMC).

I couldn't figure out how to bind the compute shader to the ISMCs internal buffers. So, we are doing unnecessary copying back to the CPU and back to the GPU. I also copy from a FVector array to a FTransform array, that's a little ugly.

The [compute shader](Shaders/Boid.usf) is super basic, it just modulates positions with a sine function.

# Gotchas
* The ``LoadingPhase`` in the uproject file needs to be ``PostConfigInit``
* You must let Unreal know where to find your shader directly. I set this in the game mode in the module interface in [ComputeShaderExample.h](Source/ComputeShaderExample/ComputeShaderExample.h)
