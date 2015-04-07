Old Ones Demo
======

This branch contains a complex demo called "Old Ones", where a strange alien entity visits a city. This demo contains a substantial amount of features:

* A post-process system allowing for various effects
* Shadow-casting sunlight, using shadow maps
* Multiple complex meshes loaded in using the AssImp library, included with Manbil
* A 3D fractal, rendered using an elegant and fast form of ray-tracing known as "Distance-field Raymarching".

All content is stored in the "Dependencies/Include In Build/Universal/Content/Old Ones" folder.

##Demo Licensing##

* The audio file "OldOneAppear.wav" is a modified form of the following sound file: https://www.freesound.org/people/yottasounds/sounds/176727/




Manbil
======

A free, open-source 3D game engine in C++/OpenGL. Currently undergoing refactoring and [documentation](https://github.com/heyx3/ManbilDocumentation) to turn it into an educational resource for anybody who wants to learn how to write a basic game engine.

Uses some libraries here and there -- tinyXML for serialization, OVR for Oculus Rift support, SFML for window management and loading textures, FreeType for loading font glyphs, etc. These plugins are in the "Plugins" folder in the repo.

##Documentation##

The documentation is available in [another Git repo](https://github.com/heyx3/ManbilDocumentation).

##Licensing##

Portions of this software (including but not limited to the basic GUI system and anything that uses it) make use of The FreeType Project (www.freetype.org). All rights reserved.
    
Portions of this software (including but not limited to the "world" system and anything that uses it) make use of the SFML library (http://www.sfml-dev.org/license.php). All rights reserved.

Portions of this software (including but not limited to the "AssImpTestWorld" class) make use of the Open Asset Import Library (http://assimp.sourceforge.net/main_license.html).  All rights reserved.

Portions of this software (including but not limited to the "world" system and anything that uses it) make use of the GLEW library (http://glew.sourceforge.net/credits.html). All rights reserved.

Portions of this software (including but not limited to the "RiftTestWorld" class) make use of the Oculus Rift SDK (https://developer.oculus.com/licenses/license-3.2/). All rights reserved.

Portions of this software (including but not limited to the "Serialization" system and anything that uses it) make use of the TinyXML library (https://github.com/leethomason/tinyxml2). All rights reserved.

Everything else belongs to William Manning, available under the [Creative Commons 3.0 Attribution license](https://creativecommons.org/licenses/by/3.0/us/).
