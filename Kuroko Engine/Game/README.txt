Kuroko Engine 0.451

This is a videogame engine in process, developed by Rodrigo de Pedro Lombao and Lucas García Mateu.

Features of the engine:

- Information about the hardware and application
- Rendering and window options
- Geometry storage and render in real time
- Primitive shapes (Plane, Cube, Sphere and Cylinder)
- Game object system, with parenting and components
- Load and Save of the editor configuration through JSON

- File import through windows explorer or drag and drop
- About tab with information about the engine and links to the used libraries
- UI docking
- Inspector with information about the transform, mesh and material used by the gameobject.

Libraries used:
- Assimp 	(http://www.assimp.org/)
- Glew		(http://glew.sourceforge.net/)
- DevIL		(http://openil.sourceforge.net/)
- MathGeoLib	(http://clb.demon.fi/MathGeoLib/)
- SDL		(https://wiki.libsdl.org/FrontPage)
- OpenGL, GLSL	(https://www.opengl.org/
- Parson	(https://github.com/kgabis/parson)
- PCG Random	(http://www.pcg-random.org)

Link to github: https://github.com/Skyway666/Kuroko-Engine

Github profiles:
- Rodrigo De Pedro - (https://github.com/rodrigodpl)
- Lucas García Mateu - (https://github.com/Skyway666)

=======
Models provided:

- Baker house:
"BakerHouse.fbx" and "Baker_house.png"

- Penguin
"PenguinBaseMesh.fbx" and "Penguin Diffuse.dds"

- Toothbrush and tube
"Toothpaste and Brish Autodesk FBX.fbx", "Paste Diffuse.dds" for tube and "Brush Diffuse.dds" for brush.

With drag and drop, the textures will be applyed to all the meshes, so in order to apply each texture to the 
appropiated mesh, it has to be done by navigating the hierarchy and assigning the correct texture to the material
of each mesh.

