Kuroko Engine 0.7

This is a videogame engine in process, developed by Rodrigo de Pedro Lombao and Lucas García Mateu.

Features of the engine:

Lucas García Mateu
- Information about hardware and application.
- Application log and performance indicators as UI items.
- Rendering and window options.
- About tab with information about the engine and links to the used libraries.
- Primitive shapes (Sphere and Cylinder).
- File import through drag and drop.
- Load and Save of the editor configuration through JSON (open tabs and window options).
- UI docking.
- Own File Format for meshes and all textures.
- Quadtree.
- Save and Load of scene.
- Time control implemented, with serialization of scene reload when stopping time.
- Resource manager with online functionalities (add, modify and delete resources).

Rodrigo De Pedro Lombao
- Geometry storage and render in real time.
- Primitive shapes.
- Hierarchy viewer and game object inspector.
- Game object system, with parenting and components.
- Camera as component with frustum, frustum culling and rendering window.
- Gizmo implemented, with toolbar (does not work in ortographic mode).
- Mouse Picking.


INNOVATION: 
Rodrigo De Pedro Lombao
- File import through windows explorer
- Camera views are controllable by the user and can display their depth buffer.
- Notifications of memory leaks in console
- Skybox
- Viewports
- Transform with Global and Local modes and constraints.
- Camera menu with information about all game cameras and tweakable options.
- Asset viewer window with previews and info about the files in Asset folder.
- Color picker

Lucas García Mateu
- Using "map" container to store resources.
- When loading a scene, the engine will keep the name of the scene, and if "save" is selected it will be automatically saved
in the "scene folder" in assets, while if "save as" is clicked it will allow to rename it and save as a different scene.
- Menu with options on the quadtree, included how many objects are being discarted.
- "Duplicate" button

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
- ImGuiColoredTextEditor (https://github.com/BalazsJako/ImGuiColorTextEdit)
- ImGui		(https://github.com/ocornut/imgui)
- Wren 		(http://wren.io/)
Extra information:

- Press "Space" to see all the viewports at the same time
- Use "Empty object" to create an empty object, which will be a child of the currently selected object
- You can add mesh components to an object, and assign them their material and mesh
- Using "Import File" or "Drag and drop" on files will add them to the assets folder of the project. 
- Scenes can be loaded by double clicking them on asset window or using "Load Scene" in main menu.
- Double click any element in the asset window to add it to the scene.
- Camera views can be controled as the background camera by focusing their UI window. Mouse picking is also
  enabled and usable.
- The update ratio of the resource manager can be changed in the "Resources" window.



Link to github: https://github.com/Skyway666/Kuroko-Engine

Github profiles:
- Rodrigo De Pedro - (https://github.com/rodrigodpl)
- Lucas García Mateu - (https://github.com/Skyway666)



