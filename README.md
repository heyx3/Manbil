Two Triangles
=====

This branch of Manbil provides a utility similar to [ShaderToy](shadertoy.com), but with a slightly different feature-set.

The basic idea is that the user provides the body of a fragment shader in a text file, and the program runs it on a quad that covers the whole screen. Effectively, it allows the user to easily create interesting visuals, including raytraced 3D scenes!

##Inputs

Quite a few built-in constants are provided for the user to utilize in their shader:

* **in_UV** -- The UV coordinates for the quad, as a vec2 ranging from {0, 0} to {1, 1}.
* **u_elapsed_seconds** -- elapsed time since world start, in seconds, as a float.
* **u_shader_seconds** -- elapsed time since shader start, in seconds, as a float.
* **u_cam_width** -- the width of the screen in pixels, as a float.
* **u_cam_height** -- the height of the screen in pixels, as a float.

The shader must output the color of the pixel to the vec4 **out_FinalColor**.

Additionally, the world simulates a 3D camera controlled with WASD/EQ and the mouse, and passes it into the shader with additional uniforms. Use the Space bar to enable/disable this feature.

* **u_cam_pos** -- the position of the camera as a vec3.
* **u_cam_forward** -- the forward vector of the camera as a normalized vec3.
* **u_cam_upward** -- the upward vector of the camera as a normalized vec3.
* **u_cam_sideways** -- the sideways vector of the camera as a normalized vec3.

Finally, two texture samplers are provided:
* **u_noiseSampler** -- a texture with random greyscale noise, stored in "Content/Textures".
* **u_mysampler** -- a texture specified by the user when first loading a shader that uses it.

##Controls##

While running a shader, you can use the following controls:

* *Space* -- enable/disable camera control.
* *1* -- reload the current shader from its text file.
* *2* -- load a new shader.
* *3* -- reload the current user-specified texture from its file.
* *4* -- load a new user-specified texture.
* *Escape* -- quit.

##Examples##

Quite a few sample shaders are provided in the "Shaders" folder; they are all well-commented.

##Licensing##

This project uses [tinydir](https://github.com/cxong/tinydir/), which uses the [Simplified BSD License](http://www.freebsd.org/copyright/freebsd-license.html).

The rest of the project is covered under the original Manbil branch's "Licensing" section (available below).


Manbil
======

A free, open-source 3D game engine in C++/OpenGL. Currently undergoing refactoring and [documentation](https://github.com/heyx3/ManbilDocumentation) to turn it into an educational resource for anybody who wants to learn how to write a basic game engine.

Uses some libraries here and there -- tinyXML for serialization, SFML for window management and loading textures, FreeType for loading font glyphs, etc. These plugins are in the "Plugins" folder in the repo.

##Documentation##

The documentation is available in [another Git repo](https://github.com/heyx3/ManbilDocumentation).

##Licensing##

Portions of this software (including but not limited to the basic GUI system and anything that uses it) make use of The FreeType Project (www.freetype.org). All rights reserved.
    
Portions of this software (including but not limited to the "world" system and anything that uses it) make use of the SFML library (http://www.sfml-dev.org/license.php). All rights reserved.

Portions of this software (including but not limited to the "AssetImporterWorld" class) make use of the Open Asset Import Library (http://assimp.sourceforge.net/main_license.html).  All rights reserved.

Portions of this software (including but not limited to the "world" system and anything that uses it) make use of the GLEW library (http://glew.sourceforge.net/credits.html). All rights reserved.

Portions of this software (including but not limited to the "Serialization" system and anything that uses it) make use of the TinyXML library (https://github.com/leethomason/tinyxml2). All rights reserved.

Everything else belongs to William Manning, available under the [Creative Commons 3.0 Attribution license](https://creativecommons.org/licenses/by/3.0/us/).
