
#include <string>
#include <math.h>
#include "camera.h"

#include "ImGUI/imgui_sdl.h"
#include "ImGUI/imgui.h"

const matrix44 moveXRightMatrix(1.f, 0.f, 0.f, 0.f,
								0.f, 1.f, 0.f, 0.f,
								0.f, 0.f, 1.f, 0.f,
								-0.05f, 0.f, 0.f, 1.f);

const matrix44 moveXLeftMatrix(1.f, 0.f, 0.f, 0.f,
								0.f, 1.f, 0.f, 0.f,
								0.f, 0.f, 1.f, 0.f,
								0.05f, 0.f, 0.f, 1.f);

const matrix44 moveYUpMatrix(1.f, 0.f, 0.f, 0.f,
								0.f, 1.f, 0.f, 0.f,
								0.f, 0.f, 1.f, 0.f,
								0.f, 0.05f, 0.f, 1.f);

const matrix44 moveYDownMatrix(1.f, 0.f, 0.f, 0.f,
								0.f, 1.f, 0.f, 0.f,
								0.f, 0.f, 1.f, 0.f,
								0.f, -0.05f, 0.f, 1.f);

const matrix44 scaleUpMatrix(1.1f, 0.f, 0.f, 0.f,
								0.f, 1.1f, 0.f, 0.f,
								0.f, 0.f, 1.1f, 0.f,
								0.f, 0.f, 0.f, 1.f);

const matrix44 scaleDownMatrix(0.9f, 0.f, 0.f, 0.f,
								0.f, 0.9f, 0.f, 0.f,
								0.f, 0.f, 0.9f, 0.f,
								0.f, 0.f, 0.f, 1.f);

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {

		SDL_Window* window = SDL_CreateWindow("Camera Render Project", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
		SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        if ( window && renderer ) {
            SDL_bool done = SDL_FALSE;
			SDL_SetRelativeMouseMode(SDL_FALSE);

			std::vector<Obj> objects;
            objects.push_back( Obj("objects/elf.obj", "objects/Elf_Albedo.png") );

			ImGui::CreateContext();
			ImGuiSDL::Initialize(renderer, WIDTH, HEIGHT);

            camera cam(vec3(0, 0, 3), vec3(0, 0, -1), vec3(0, 1, 0), 90.0f, 1.f, 10000.f, WIDTH, HEIGHT);

			float my_color[4];
			bool my_tool_active;

            while (!done) {
                SDL_Event event;
				ImGuiIO& io = ImGui::GetIO();
                std::clock_t then = std::clock();

				int mouseX, mouseY;
				const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

				io.DeltaTime = 1.0f / 60.0f;
				io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
				io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
				io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);

				ImGui::NewFrame();

				// Criar tela chamada "My First Tool", com barra de menu.
				ImGui::Begin("Tools", &my_tool_active, ImGuiWindowFlags_MenuBar);
				if (ImGui::BeginMenuBar())
				{
					if (ImGui::BeginMenu("File"))
					{
						if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
						if (ImGui::MenuItem("Save", "Ctrl+S"))   { /* Do stuff */ }
						if (ImGui::MenuItem("Close", "Ctrl+W"))  { my_tool_active = false; }
						ImGui::EndMenu();
					}
					ImGui::EndMenuBar();
				}

				// Editar cor (armazenar como ~4 floats)
				ImGui::ColorEdit4("Color", my_color);
				// Calcular alguns valores
				const float my_values[] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f };
				ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));

				// Exibir conteudo em regiao "scrollable"
				ImGui::TextColored(ImVec4(1,1,0,1), "Important Stuff");
				ImGui::BeginChild("Scrolling");
				ImGui::Text("Random Message\n");
				ImGui::EndChild();
				ImGui::End();

				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
				SDL_RenderClear(renderer); // Limpa frame da imagem gerada anteriormente

                cam.render_scene(objects, renderer); // Inserir triangle data no renderer
				cam.update_pixel_coordinates();

				ImGui::Render();
				ImGuiSDL::Render(ImGui::GetDrawData());
                SDL_RenderPresent(renderer); // Apresentar na tela triangle data gerada

                while (SDL_PollEvent(&event)) {

					if( event.type == SDL_KEYDOWN){
						//Translada a camera para a esquerda (Usar seta para esquerda)
						if (event.key.keysym.sym == SDLK_LEFT) {
							cam._from += cam.axisX * (0.1f);
							cam._at += cam.axisX * (0.1f);
						}
						//Translada a camera para a direita (Usar seta para direita)
						else if (event.key.keysym.sym == SDLK_RIGHT) {
							cam._from -= cam.axisX * (0.1f);
							cam._at -= cam.axisX * (0.1f);
						}
						//Translada a camera para cima (Usar seta para cima)
						else if (event.key.keysym.sym == SDLK_UP) {
							cam._from += cam.axisY * (0.1f);
							cam._at += cam.axisY * (0.1f);
						}
						//Translada a camera para baixo (Usar seta para baixo)
						else if (event.key.keysym.sym == SDLK_DOWN) {
							cam._from -= cam.axisY * (0.1f);
							cam._at -= cam.axisY * (0.1f);
						}
						//Rotaciona a camera para a esquerda (Usar tecla Insert)
						else if (event.key.keysym.sym == SDLK_INSERT) {
							cam.rotateY(-1);
						}
						//Rotaciona a camera para a direita (Usar tecla Home)
						else if (event.key.keysym.sym == SDLK_HOME) {
							cam.rotateY(1);
						}
						//Rotaciona a camera no próprio eixo para a esquerda (Usar tecla End)
						else if (event.key.keysym.sym == SDLK_END) {
							cam.rotateZ(-1);
						}
						//Rotaciona a camera no próprio eixo para a direita (Usar tecla Del)
						else if (event.key.keysym.sym == SDLK_DELETE) {
							cam.rotateZ(1);
						}
						//Rotaciona camera verticalmente para cima (Usar PgUp)
						else if (event.key.keysym.sym == SDLK_PAGEUP) {
							cam.rotateX(-1);
						}
						//Rotaciona a camera verticalmente para baixo (Usar PgDn)
						else if (event.key.keysym.sym == SDLK_PAGEDOWN) {
							cam.rotateX(1);
						}
						//rotaciona objeto em torno do eixo X de forma anti-horária
						else if(event.key.keysym.sym == SDLK_KP_2) {
							objects[0].rotateXCounterClockwise();
						}
						//rotaciona objeto em torno do eixo X de forma horária
						else if(event.key.keysym.sym == SDLK_KP_8) {
							objects[0].rotateXClockwise();
						}
						//rotaciona objeto em torno do eixo Y de forma anti-horária
						else if (event.key.keysym.sym == SDLK_KP_7) {
							objects[0].rotateYCounterClockwise();
						}
						//rotaciona objeto em torno do eixo Y de forma horária
						else if (event.key.keysym.sym == SDLK_KP_9) {
							objects[0].rotateYClockwise();
						}
						//rotaciona objeto em torno do eixo Z de forma anti-horária
						else if(event.key.keysym.sym == SDLK_KP_4) {
							objects[0].rotateZCounterClockwise();
						}
						//rotaciona objeto em torno do eixo Z de forma horária
						else if(event.key.keysym.sym == SDLK_KP_6) {
							objects[0].rotateZClockwise();
						}//Translada o objeto para a direita (Usar tecla D)
						else if (event.key.keysym.sym == SDLK_d) {
							//matrix44 normalMatrix = moveXRightMatrix.inverse().transpose();
							for (int i = 0; i < objects.at(0).mesh.tris.size(); i++) {
								moveXRightMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[0].pos, objects[0].mesh.tris[i].vertex[0].pos);
								moveXRightMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[1].pos, objects[0].mesh.tris[i].vertex[1].pos);
								moveXRightMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[2].pos, objects[0].mesh.tris[i].vertex[2].pos);
								//Normais comentadas por algum bug na interaçao com a luz apos algumas transformacoes seguidas

								//normalMatrix.mult_vec_matrix(objects[0].mesh.tris[i].vertex[0].normal, objects[0].mesh.tris[i].vertex[0].normal);
								//normalMatrix.mult_vec_matrix(objects[0].mesh.tris[i].vertex[1].normal, objects[0].mesh.tris[i].vertex[1].normal);
								//normalMatrix.mult_vec_matrix(objects[0].mesh.tris[i].vertex[2].normal, objects[0].mesh.tris[i].vertex[2].normal);
							}
						}
						//Translada o objeto para a esquerda (Usar tecla A)
						else if (event.key.keysym.sym == SDLK_a) {
							for (int i = 0; i < objects.at(0).mesh.tris.size(); i++) {
								moveXLeftMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[0].pos, objects[0].mesh.tris[i].vertex[0].pos);
								moveXLeftMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[1].pos, objects[0].mesh.tris[i].vertex[1].pos);
								moveXLeftMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[2].pos, objects[0].mesh.tris[i].vertex[2].pos);
								//Normais comentadas por algum bug na interaçao com a luz apos algumas transformacoes seguidas

								//normalMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[0].normal, objects[0].mesh.tris[i].vertex[0].normal);
								//normalMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[1].normal, objects[0].mesh.tris[i].vertex[1].normal);
								//normalMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[2].normal, objects[0].mesh.tris[i].vertex[2].normal);
							}
						}
						//Translada o objeto para cima (Usar tecla W)
						else if (event.key.keysym.sym == SDLK_w) {
							for (int i = 0; i < objects.at(0).mesh.tris.size(); i++) {
								moveYUpMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[0].pos, objects[0].mesh.tris[i].vertex[0].pos);
								moveYUpMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[1].pos, objects[0].mesh.tris[i].vertex[1].pos);
								moveYUpMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[2].pos, objects[0].mesh.tris[i].vertex[2].pos);
								//Normais comentadas por algum bug na interaçao com a luz apos algumas transformacoes seguidas

								//normalMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[0].normal, objects[0].mesh.tris[i].vertex[0].normal);
								//normalMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[1].normal, objects[0].mesh.tris[i].vertex[1].normal);
								//normalMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[2].normal, objects[0].mesh.tris[i].vertex[2].normal);
							}
						}
						//Translada o objeto para baixo (Usar tecla S)
						else if (event.key.keysym.sym == SDLK_s) {
							//matrix44 normalMatrix = moveYDownMatrix.inverse().transpose();
							for (int i = 0; i < objects.at(0).mesh.tris.size(); i++) {
								moveYDownMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[0].pos, objects[0].mesh.tris[i].vertex[0].pos);
								moveYDownMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[1].pos, objects[0].mesh.tris[i].vertex[1].pos);
								moveYDownMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[2].pos, objects[0].mesh.tris[i].vertex[2].pos);
								//Normais comentadas por algum bug na interaçao com a luz apos algumas transformacoes seguidas

								//normalMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[0].normal, objects[0].mesh.tris[i].vertex[0].normal);
								//normalMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[1].normal, objects[0].mesh.tris[i].vertex[1].normal);
								//normalMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[2].normal, objects[0].mesh.tris[i].vertex[2].normal);
							}
						}
						//Aumentando escala do objeto (Usar tecla shift do lado esquerdo)
						else if (event.key.keysym.sym == SDLK_LSHIFT) {
							for (int i = 0; i < objects.at(0).mesh.tris.size(); i++) {
								scaleUpMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[0].pos, objects[0].mesh.tris[i].vertex[0].pos);
								scaleUpMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[1].pos, objects[0].mesh.tris[i].vertex[1].pos);
								scaleUpMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[2].pos, objects[0].mesh.tris[i].vertex[2].pos);
								//Normais comentadas por algum bug na interaçao com a luz apos algumas transformacoes seguidas

								//normalMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[0].normal, objects[0].mesh.tris[i].vertex[0].normal);
								//normalMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[1].normal, objects[0].mesh.tris[i].vertex[1].normal);
								//normalMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[2].normal, objects[0].mesh.tris[i].vertex[2].normal);
							}
						}
						//Diminuindo escala do objeto (Usar tecla CTRL do lado esquerdo)
						else if (event.key.keysym.sym == SDLK_LCTRL) {
							for (int i = 0; i < objects.at(0).mesh.tris.size(); i++) {
								scaleDownMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[0].pos, objects[0].mesh.tris[i].vertex[0].pos);
								scaleDownMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[1].pos, objects[0].mesh.tris[i].vertex[1].pos);
								scaleDownMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[2].pos, objects[0].mesh.tris[i].vertex[2].pos);
								//Normais comentadas por algum bug na interaçao com a luz apos algumas transformacoes seguidas

								//normalMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[0].normal, objects[0].mesh.tris[i].vertex[0].normal);
								//normalMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[1].normal, objects[0].mesh.tris[i].vertex[1].normal);
								//normalMatrix.mult_point_matrix(objects[0].mesh.tris[i].vertex[2].normal, objects[0].mesh.tris[i].vertex[2].normal);
							}
						}
					}

					// mouse pressed event
                    if( event.type == SDL_MOUSEBUTTONDOWN )
                    {
                        //If the left mouse button was pressed
                        if( event.button.button == SDL_BUTTON_RIGHT )
                        {
                        }
						if( event.button.button == SDL_BUTTON_LEFT)
						{
							//Declaracao de variaveis
							int x, y, objIndex = -1;
							float Px, Py, distanceToOrigin = FLT_MAX, temporaryDistance;
							bool tilt = true;
							vec3 rayDirection;
							//Relative mouse mode move o mouse para o centro da tela a cada frame
							//Util se quiser mover o mouse e nao se preocupar com ele sair da tela
							SDL_SetRelativeMouseMode(SDL_TRUE);

							SDL_GetMouseState(&x, &y);

							//Inicializacao de Px, Py e rD;
							Px = -(2 * (x / cam.imgWidth) - 1) * cam.right / cam._near;
							Py = (1 - 2 * (y / cam.imgHeight)) * cam.top / cam._near;
							rayDirection = vec3(Px, Py, -1);
							//Transformacao das coordenadadas de rD da camera para mundo
							//E normalizacao
							cam.camToWorld.mult_vec_matrix(rayDirection, rayDirection);
							rayDirection.make_unit_vector();

							//Determinar quantos objetos existem no array de objetos
							int range = objects.size();

							for (int i = 0; i < range; ++i) {
								//Para cada objeto iterar sobre sua malha de triangulos
								for (Triangle triangle : objects[i].mesh.tris) {
									//Checar se o triangulo foi intersectado pelo raio
									if (cam.check_triangle_intersection(triangle, rayDirection, temporaryDistance)) {
										//Checar se a distancia para o triangulo do objeto atual e menor do que a distancia que tinhamos anteriormente
										//Caso seja armazenar nova menor distancia e o indice do objeto que iremos manipular
										if (temporaryDistance < distanceToOrigin) {
											distanceToOrigin = temporaryDistance;
											objIndex = i;
										}
									}
								}
							}

							if (objIndex != -1) {
								//Fazer algo
							}
						}
                    }
                    if( event.type == SDL_MOUSEBUTTONUP )
                    {
                        //Se o botao esquerdo do mouse nao estiver mais sendo apertado
                        if( event.button.button == SDL_BUTTON_LEFT )
                        {
							SDL_SetRelativeMouseMode(SDL_FALSE);
                        }
                    }

					//aproxima/afasta a camera
					if (event.type == SDL_MOUSEWHEEL) {
						if (event.wheel.y < 0) {
							cam._from += cam.axisZ * (0.1f);
							cam._at += cam.axisZ * (0.1f);
						}
						else if (event.wheel.y > 0) {
							cam._from -= cam.axisZ * (0.1f);
							cam._at -= cam.axisZ * (0.1f);
						}
					}

                    if (event.type == SDL_QUIT)
                        done = SDL_TRUE;

                }
            }
        }

        if (renderer) {
            SDL_DestroyRenderer(renderer);
        }
        if (window) {
            SDL_DestroyWindow(window);
        }
    }
    SDL_Quit();
    return 0;
}
