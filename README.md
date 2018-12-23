# Kuroko engine

By [Lucas García Mateu](https://github.com/Skyway666) and Rodrigo Pedro de Lombao (https://github.com/rodrigodpl).

## Introduction

"Kuroko engine" is a project developed for the subject Engines in the third year of [Bachelor’s Degree in
Video Games by UPC at CITM](https://www.citm.upc.edu/ing/estudis/graus-videojocs/), under the supervision of the lecturer [Ricard Pillosu](https://es.linkedin.com/in/ricardpillosu).

The objective is to develop the basis for a Game Engine, and a subsystem of choice. Kuroko Engine implements a scripting system with [Wren](http://wren.io/).

## Members of the team

### Lucas García Mateu

![](https://github.com/Skyway666/Kuroko-Engine/raw/master/Website%20assets/Lucas.jpg)

Contributions:

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
- Scripting system core functionality

### Rodrigo Pedro de Lombao

![](https://github.com/Skyway666/Kuroko-Engine/raw/master/Website%20assets/Rodri.jpg)

- Geometry storage and render in real time.
- Primitive shapes (Cube and Plane)
- Hierarchy viewer and game object inspector.
- Game object system, with parenting and components.
- Camera as component with frustum, frustum culling and rendering window.
- Gizmo implemented, with toolbar (does not work in ortographic mode).
- Mouse Picking.
 File import through windows explorer
- Camera views are controllable by the user and can display their depth buffer.
- Notifications of memory leaks in console
- Skybox
- Viewports
- Transform with Global and Local modes and constraints.
- Camera menu with information about all game cameras and tweakable options.
- Asset viewer window with previews and info about the files in Asset folder.
- Color picker
- Scripting system structure in engine

## Scripting sub-system

### Features 

- Right click on asset window to add a new script. The name of the script will be the one
of the class defined in it. The script will automatically include the base libraries to interact
with the attached gameobject, as well as a constructor, Update() and Start() functions, and a comment with instructions
on how to use it.

- Attatch a script to a gameobject by pressing "Add script". This will display a window with all the scripts contained
in the asset window.

- The same script can be attached to multiple gameobjects, and the variables declared in its class will be displayed
in the editor. Choose a variable type and assign a value to them. You can force types, which will unable the user to change 
the variable type from the editor, as well as make variables public or private, which will enable/disable them being shown.
This is done through the asset inspector, select the script in the asset window and apply the desired changes. The script
component can be erased using the "Remove" button.

- Variables in the editor will display the value of the variable in the script, even if it is initialized in the "Start()"
function or modified at runtime.

- Scripts are executed according to the time in the world. "Start()" is executed in the first frame after pressing Play,
and Update() is executed every frame afterwards. The simulation can be paused as well.

- The system features hot reloading, meaning that scripts can be modified while the engine is running. This will recompile
all the scripts, but the values assigned to the variables in the editor will remain the same for variables which name's
are not changed.

- If a script fails to compile, a set of errors will display in the Log. When trying to access components with this script,
a message will appear warning the user. This can also be seen in the asset window and asset inspector.

### Constraints

- Can't modify scripts while the simulation is running.

### Advices

- Remember to initialize variables in the "Start()" function of the script or the editor, not doing it will lead
to undetermined behaviours.

### Demonstrations

![](https://github.com/Skyway666/Kuroko-Engine/raw/master/Website%20assets/variable%20mod.gif)
*Variable modification*

![](https://github.com/Skyway666/Kuroko-Engine/raw/master/Website%20assets/hot%20reloading.gif)
*Hot reloading*

# Tank scene setup

<iframe width="560" height="315" src="https://www.youtube.com/embed/gAQpB8cvS-Q" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>


[Github repository](https://github.com/Skyway666/Kuroko-Engine)

[Latest Release]()

MIT License

Copyright (c) 2018 Skyway666

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.



