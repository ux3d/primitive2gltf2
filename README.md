[![](glTF.png)](https://github.com/KhronosGroup/glTF/tree/master/specification/2.0)

# Generate primitive glTF 2.0 content

primitive2gltf2 is a command line tool for generating primitives for glTF 2.0.  
A plane, disc, cube, sphere and torus can be generated. The glTF is minimalistic and valid.  

Usage: `primitive2gltf2.exe [-p cube -s 1.0]`  

`-p cube` Default generated primitive `cube`. Alternatives are `plane`, `disc`, `sphere` and `torus`.  
`-s 1.0` Default scale.  


## Software Requirements

* C/C++ 17 compiler e.g. gcc or Visual C++
* [Eclipse IDE for C/C++ Developers](https://www.eclipse.org/downloads/packages/release/2021-03/r/eclipse-ide-cc-developers) or  
* [CMake](https://cmake.org/)  


## Import the generated glTF

Import the generated glTF in e.g. [Gestaltor](https://gestaltor.io/) and reuse in your scene.  
