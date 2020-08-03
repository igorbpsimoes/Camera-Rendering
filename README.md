### Projeto de Implementação de Camera Virtual, Sombreamento e Texturização de Objetos e Traçado de Raios
Primeiro projeto da disciplina Processamento Gráfico(IF680) feito em grupo composto pelos seguintes integrantes:
* Igor Beltrão Pereira Simões (ibps)
* Victor Edmond Freire Gaudiot (vefg)
* Maria Augusta Mota Borba (mamb2)
* Hugo Soares Ramos Falcão (hsrf)
### Como executar:
O projeto foi desenvolvido no Ubuntu e solicita-se que seja executado nele devido a facilidade para utilizar a lib incluída pelo monitor da disciplina. Para executar basta seguir os seguintes passos:
Compile no terminal:
```
g++ main.cpp ImGUI/imgui_draw.cpp lodepng.cpp ImGUI/imgui_sdl.cpp ImGUI/imgui_widgets.cpp ImGUI/imgui.cpp -g -O3 -w -lSDL2main -lSDL2 -o Renderer.exe
```
Então execute Renderer.exe:
```
./Renderer.exe
```

O esqueleto do projeto foi desenvolvido pelo monitor da disciplina Ivan Neves Silveira Bandeira de Souza.
