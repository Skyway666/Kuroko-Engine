Kuroko Engine 0.5

This is a videogame engine in process, developed by Rodrigo de Pedro Lombao and Lucas García Mateu.

Features of the engine:

Lucas García Mateu
- Information about hardware and application.
- Rendering and window options.
- Primitive shapes (Sphere and Cylinder).
- File import through drag and drop.
- Load and Save of the editor configuration through JSON (open tabs and window options).
- Own File Format for meshes and all textures.
- Quadtree.
- UI docking.
- Save and Load of scene.
- About tab with information about the engine and links to the used libraries.
- Time control implemented, with serialization of scene reload when stopping time.
- Resource manager with online functionalities (add, modify and delete resources)
- Application log and performance indicators as UI items.

Rodrigo De Pedro Lombao
- Geometry storage and render in real time.
- Primitive shapes .
- Hierarchy viewer and game object inspector.
- Game object system, with parenting and components.
- Camera as component with frustum, frustum culling and rendering window
- Gizmo implemented, with toolbar.
- Mouse Picking
- Asset viewer window with previews and info about the files in Asset folder.

INNOVATION: 
Rodrigo De Pedro Lombao
- File import through windows explorer
- Camera can display viewports and the depth buffer
- Notifications of memory leaks in console
- Skybox
- Viewports
- Transform with Global and Local modes and constraints.
- Camera menu with information about all game cameras and tweakable options.

Lucas García Mateu
- Using "map" container to store resources
- When loading a scene, the engine will keep the name of the scene, and if "save" is selected it will be automatically saved
in the "scene folder" in assets, while if "save as" is clicked it will allow to rename it and save as a different scene


Libraries used:
- Assimp 	(http://www.assimp.org/)
- Glew		(http://glew.sourceforge.net/)
- DevIL		(http://openil.sourceforge.net/)
- MathGeoLib	(http://clb.demon.fi/MathGeoLib/)
- SDL		(https://wiki.libsdl.org/FrontPage)
- OpenGL, GLSL	(https://www.opengl.org/
- Parson	(https://github.com/kgabis/parson)
- PCG Random	(http://www.pcg-random.org)
- ImGuizmo      (https://github.com/CedricGuillemet/ImGuizmo)
- ImGui		(https://github.com/ocornut/imgui)

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
"Paste Tube.fbx" and "Paste Diffuse.dds"

