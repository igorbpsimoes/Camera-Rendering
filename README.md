### Virtual Camera Implementation, Object Shading and Texturing, and Ray Tracing Project
First project of the Graphic Processing (IF680) course done in a group composed of the following members:
* Igor Beltrão Pereira Simões (ibps)
* Victor Edmond Freire Gaudiot (vefg)
* Maria Augusta Mota Borba (mamb2)
* Hugo Soares Ramos Falcão (hsrf)

### Pre-Requisites
You need to have installed:
- g++
- SDL2

### How to run:
The project was developed on Ubuntu and it is recommended to run it on this OS due to the ease of using the library included by the course monitor. To run it, follow these steps:
Compile in the terminal:
```
g++ main.cpp ImGUI/imgui_draw.cpp lodepng.cpp ImGUI/imgui_sdl.cpp ImGUI/imgui_widgets.cpp ImGUI/imgui.cpp -g -O3 -w -lSDL2main -lSDL2 -o Renderer.exe
```
Then run Renderer.exe:
```
./Renderer.exe
```

The project skeleton was developed by the course monitor Ivan Neves Silveira Bandeira de Souza.
