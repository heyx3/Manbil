Manbil
======

My 3D engine in C++/OpenGL. Uses some libraries here and there -- tinyXML for serialization, SFML for window management and loading textures, and FreeType for font rendering.

Everything else is done by me.


An overview of the different systems by folder follows. NOTE: use the filter layout in Visual Studio; the actual folder hierarchy is a little messy and still needs to be cleaned up (a lot of older files are sitting in the root of the solution folder).

-"Events": support for timers and timer management. "EventManager.h" is not used yet and is still WIP.

-"Input": support for abstracted input.
  --Each input instance can inherit from BoolInput, FloatInput, or Vector2Input.
  --For example, mouse clicks and keyboard presses are BoolInputs, while mouse movement is a Vector2Input.

-"Math": high-level and low-level mathematics. Provides "2D" and "3D" arrays that are actually just one-dimensional arrays in order to facilitate better cache usage.
  --FastRand.h: an efficient PRNG.
  --Vectors.h, Matrix4f.h, Quaternion.h: Provide the basic framework for 3D math -- vectors, matrices, and quaternions.
  --Interval.h: represents a floating-point interval and provides different functions related to that.
  --CachedFunc.h: stores the results of some complex function at a constant interval and interpolates between those intervals for a quick approximation.
  --"Higher Math": provides various high-level mathematics utilities, including basic geometry, terrain, and converting from a bumpmap to a normalmap.
  --The "noise generation" folder provides various 2D and 3D noise generation classes, designed to work together.
    ---The basic generation algorithms provided are white noise, Perlin noise, Worley noise, and Diamond-Square noise (only available in 2D). The rest of the generators operate on other generators (e.x. filterers or interpolators).
    ---The generators have a tree structure, such that a noise generation tree can be assembled (e.x. putting a "add noise" filter on top of a "scale up and interpolate" generator on top of a "white noise" generator) and then executed in one line.
  --The "shapes" folder provides support for 3D shapes - detecting collision, casting rays, finding a certain point on the surface, etc.
    ---The shapes that are currently functional are Cube, Sphere, and Capsule. Triangle and Polygon are not yet fully functional.
    
-"MGame": an old attempt at an entity component system that will likely be trashed.

-"Oculus Rift Stuff": has only one .h/.cpp pair: "OculusDevice". It handles support for the Oculus Rift.
  --Support for the Rift is not fully integrated yet; the only thing that is fully functional is the rotation sensor.
  
-"Worlds": The backbone for game loop
  --Provides timing/timers, texture management, window management, and a game loop.
  
-"Rendering": covers a vast range of different systems that use OpenGL.
  --"Texture Management": handles textures -- loading, getting/setting pixel data, quality/usage settings, etc.
  --"Helper Classes": various wrappers for OpenGL stuff, as well as a "DrawingQuad" class.
  --"Materials": the biggest part of the engine.
    ---Provides a system for defining a "material", which is a vertex/fragment shader (and optionally a geometry shader).
    ---When a material is to be rendered, it is provided with a list of meshes to render it with and a set of parameters for the shaders.
    ---The material class supports any kind of vertex -- anything from a basic position/UV/normal to the "particleID"/"RandSeeds" vertices I use for the GPU particles.
    ---"Data Nodes": the system used to generate materials without having to write your own shader code.
      ----Based off of the material system in Unreal 4, but lower-level - a Directed Acyclic Graph of expressions plugging into other expressions.
      ----Each "node" represents some expression/function in the shader. It can have any number of inputs and outputs, based on what the node actually does.
        -----For example, a "MultiplyNode" can have any number of inputs, and has exactly one output.
      ----DataNode inputs/outputs are all combinations of floats - "float", "vec2", "vec3", or "vec4".
      ----A "DataLine" is either a constant value or one of the outputs of a DataNode.
      ----Various aspects of the shaders (mainly vertex output and fragment output) are described with a DataLine.
      ----Used for 3D surfaces, post-processing, and GPU particles.
  --"Post-Processing": uses my material "DataNode" system to define post-process effects
    ---PostProcessChain.h/.cpp: a sequence of post-processing effects done one after the other. Simple effects can be stacked and done in one pass.
    ---PostProcessEffect.h/.cpp: inherits from "DataNode". A base class for different effects. Currently the only effects I have are fog, contrast, color tint, and Gaussian Blur (WIP). More complex effects are still being worked on.
  --"Objects": some complex renderable objects.
    ---Foliage.h/.cpp: an experiment with foliage and will probably be trashed.
    ---Water.h/.cpp, WaterRendering.h/.cpp: provides functionality and rendering for water, using my material node system.
      ----Exposes some DataNodes for vertex displacement in the vertex shader and surface distortion/normal calculation in the fragment shader.
  --"GUI": the GUI system for my engine. Not yet functional.
  --"GPU Particles": My system for particle effects done on the GPU.
    ---Particles are kinematic; they don't react to collisions or anything. This makes them faster but less dynamic.
    ---The low-level particle system ("GPUP" for "GPU Particles") just provides an output system similar to the DataNode system.
      ----Outputs are "world position" (vec3), "rotation" (float), "size" (vec2), and "color" (vec4). These then get converted into the regular outputs used in the DataNode system (as well as some geometry shader behavior).
    ---The high-level particle system ("HGP" for "High-level Gpu Particles") provides an "HGPComponent" system for more high-level behaviors. These behaviors get converted into DataLines for use in the GPUP system.
      ----These components are similar to DataNodes, in that they "output" a float value given other input values.
      ----Some example components:
        -----"ConstantAcceleration": outputs world position given "acceleration", "initial velocity", and "initial position" components
        -----"SpherePosition": outputs a random position inside a given sphere.
      ----Helpful global information is provided in the "HGPGlobalData" namespace.
