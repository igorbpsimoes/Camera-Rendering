#ifndef CAMERAH
#define CAMERAH

#include "vec3.h"
#include "vec2.h"
#include "matrix44.h"
#include "object.h"

#include <SDL2/SDL.h>

const int WIDTH = 600;
const int HEIGHT = 400;
const int LEFT = 1, RIGHT = 2, BOTTOM = 4, TOP = 8;

/*//Constantes do Macaco
const vec3 Kd = vec3(170, 170, 170);
const vec3 Ks = vec3(180, 180, 180);
const float n = 4;
const vec3 enviromentConstant = vec3(40, 40, 40);*/

//Constantes do Elfo
const vec3 Kd = vec3(30, 30, 30);
const vec3 Ks = vec3(50, 50, 50);
const float n = 2;
const vec3 enviromentConstant = vec3(40, 40, 40);


class camera
{
public:
    int imgWidth, imgHeight;
    float fov, _near, _far;
    float bottom, left, top, right;
    matrix44 camToWorld;
    matrix44 worldToCamera;

	vec3 _from, _at, _up;
    vec3 axisX, axisY, axisZ;

public:
    camera();
    camera(const vec3 &from, const vec3 &at, const vec3 &up, const float &f, const float &n, const float &far, const int &iwidth, const int &iheight): 
           fov(f), _near(n), imgWidth(iwidth), imgHeight(iheight), _from(from), _at(at), _up(up), _far(far) {
                look_at(from, at, up);
				camToWorld = matrix44(axisX[0], axisX[1], axisX[2], 0.f,
									axisY[0], axisY[1], axisY[2], 0.f,
									axisZ[0], axisZ[1], axisZ[2], 0.f,
									_from[0], _from[1], _from[2], 1.f);
				worldToCamera = camToWorld.inverse();
				top = tan(fov / 2 * (M_PI/180));
				right = top * ((float)imgWidth / (float)imgHeight);
				left = -right;
				bottom = -top;
           }
	//dot retorna produto interno 2 vetores; cross retorna produto vetorial;
    void look_at(const vec3 &from, const vec3 &at, const vec3 &up) {
		axisZ = from - at;
		axisZ.make_unit_vector();
		axisY = up - ((dot(up, axisZ) / dot(axisZ, axisZ)) * axisZ);
		axisY.make_unit_vector();
		axisX = cross(axisY, axisZ);
		axisX.make_unit_vector();
    }

	void update_pixel_coordinates() {
		look_at(_from, _at, _up);
		camToWorld = matrix44(axisX[0], axisX[1], axisX[2], 0.f,
			axisY[0], axisY[1], axisY[2], 0.f,
			axisZ[0], axisZ[1], axisZ[2], 0.f,
			_from[0], _from[1], _from[2], 1.f);
		worldToCamera = camToWorld.inverse();
	}

	void rotateY(int positive) {
		matrix44 m = matrix44(0.98480f, 0.f, -0.17365 * (positive), 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.17365 * (positive), 0.f, 0.98480f, 0.f,
			0.f, 0.f, 0.f, 1.f);
		m.mult_vec_matrix(axisZ, axisZ);
		_at = _from - axisZ;
		m.mult_vec_matrix(_up, _up);
		update_pixel_coordinates();
	}

	void rotateX(int positive) {
		matrix44 m = matrix44(1.f, 0.f, 0.f, 0.f,
			0.f, 0.98480f, -0.17365 * (positive), 0.f,
			0.f, 0.17365 * (positive), 0.98480f, 0.f,
			0.f, 0.f, 0.f, 1.f);
		m.mult_vec_matrix(axisZ, axisZ);
		_at = _from - axisZ;
		m.mult_vec_matrix(_up, _up);
		update_pixel_coordinates();
	}

	void rotateZ(int positive) {
		matrix44 m = matrix44(0.98480f, -0.17365 * (positive), 0.f, 0.f,
			0.17365 * (positive), 0.98480f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f);
		m.mult_vec_matrix(axisZ, axisZ);
		_at = _from - axisZ;
		m.mult_vec_matrix(_up, _up);
		update_pixel_coordinates();
	}

    bool compute_pixel_coordinates(const vec3 pWorld, vec2 &pRaster, std::vector<float> &zCoordinates) {
		vec3 pCamera, pNDC, pScreen;
		matrix44 projectionMatrix, normalizationMatrix;
		projectionMatrix = matrix44(1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 1.f / _near,
			0.f, 0.f, 0.f, 1.f);
		normalizationMatrix = matrix44((2.f * _near) / (right - left), 0.f, 0.f, 0.f,
			0.f, (2.f * _near) / (bottom - top), 0.f, 0.f,
			(-(right + left)) / (right - left), (-(bottom + top)) / (bottom - top), (_far + _near) / (_far - _near), 1.f,
			0.f, 0.f, (-(2.f * _near)) / (_far - _near), 0.f);
		//Conversao do World Space para o Camera Space
		worldToCamera.mult_point_matrix(pWorld, pCamera);
		//Calculando inverso da coordenada Z e armazenando para posterior interpolacao na fill triangle
		zCoordinates.push_back(1.f/-pCamera.z());
		//Projecao do ponto do Camera Space para Screen Space
		projectionMatrix.mult_point_matrix(pCamera, pScreen);
		//Normalizacao do ponto para o NDC Space
		normalizationMatrix.mult_point_matrix(pScreen, pNDC);
		if (abs(pNDC[0]) > right || abs(pNDC[1]) > top || pCamera[2] >= 0)
			return false;
		//Conversao da NDC Space para o Raster Space 
		pRaster[0] = ((1+pNDC[0]) / 2) * imgWidth;
		pRaster[1] = ((1-pNDC[1]) / 2) * imgHeight;
		//Retornar verdadeiro caso o ponto esteja no mesmo plano da imagem
		return true;
    }
	
	int get_outcode(vec2 point) {
		int outcode = 0;
		//Se o y do ponto for maior que o Ymax (imgHeight) setar bit mais significativo para 1
		if (point.y() > imgHeight)
			outcode |= TOP;
		else if (point.y() < 0) //Caso seja menor que o Ymin, setar 2 bit mais significativo para 1
			outcode |= BOTTOM;
		//Se o x do ponto for maior que o Xmax (imgWidth) setar 3 bit mais significativo para 1
		if (point.x() > imgWidth)
			outcode |= RIGHT;
		else if (point.x() < 0) //Caso seja menor que o Xmin, setar 4 bit mais significativo para 1
			outcode |= LEFT;
		
		return outcode;
	}

	bool clip_line(vec2 &p0, vec2 &p1) {
		//Determinando outcode de p0 e p1 e inicializacao de x_max e y_max
		int outcode_p0 = get_outcode(p0), outcode_p1 = get_outcode(p1), outcode_external;
		float x_max = (float)imgWidth, y_max = (float)imgHeight;
		bool accepted = false;
		
		while (!accepted) {
			//Caso a reta esteja dentro da tela
			if (outcode_p0 == 0 || outcode_p1 == 0) {
				accepted = true;
				break;
			//Caso a reta nao passe pela tela
			} else if ((outcode_p0 & outcode_p1) != 0) {
				break;
			} else { //Casos em que a reta passa pela tela mas precisa ser cortada
				float slope, tempX = 0, tempY = 0;
				//Inclinacao da reta calculada
				slope = (p1.y() - p0.y()) / (p1.x() - p0.x());
				outcode_external = (outcode_p0 != 0 ? outcode_p0 : outcode_p1);
				//Se estiver no quadrante TOP
				if (outcode_external & TOP) {
					tempX = p0.x() + (y_max - p0.y())/slope;
					tempY = y_max;
				//Se estiver no quadrante BOTTOM
				} else if (outcode_external & BOTTOM) {
					tempX = p0.x() + (-p0.y()) / slope;
					tempY = 0;
				//Se estiver no quadrante RIGHT
				} else if (outcode_external & RIGHT) {
					tempX = x_max;
					tempY = p0.y() + (x_max - p0.x()) * slope;
				//Se estiver no quadrante LEFT
				} else if (outcode_external & LEFT) {
					tempX = 0;
					tempY = p0.y() + (-p0.x()) * slope;
				}
				//Se tivermos editado o p0, atualizar seus valores e recalcular seu outcode
				if (outcode_external == outcode_p0) {
					p0[0] = tempX;
					p0[1] = tempY;
					outcode_p0 = get_outcode(p0);
				} else { //Caso contrario editamos p1 e precisamos atualiza-lo
					p1[0] = tempX;
					p1[1] = tempY;
					outcode_p1 = get_outcode(p1);
				}
			}
		}

		return accepted;
	}

	void draw_line(SDL_Renderer* renderer, vec2 &p0, vec2 &p1) {
		//Criacao do Vetor Diretor
		vec2 director = p1 - p0;
		//Determinando limite de iteracao
		int range = (int)director.length();
		//Vetor unitario para desenhar cada ponto da reta
		director.make_unit_vector();
		//Determinando ponto inicial do vetor e em seguida iteracoes para desenhar os pontos da reta
		vec2 start_point = p0;
		for (int i = 0; i < range; ++i, start_point += director)
			SDL_RenderDrawPoint(renderer, start_point.x(), start_point.y());
	}

	bool check_triangle_intersection(Triangle triangle, vec3 &rayDirection, float &distanceToOrigin) {
		//Declaracao de Variaveis
		vec3 planeNormal, AB, AC, intersectionPoint;
		float parameterT;
		//Calculo dos vetores AB e BC para obtencao do vetor Normal
		AB = triangle.vertex[1].pos - triangle.vertex[0].pos;
		AC = triangle.vertex[2].pos - triangle.vertex[0].pos;
		planeNormal = cross(AB, AC);

		//Checar se rD e perpendicular a normal do plano
		//Se for, temos que e paralelo ao plano, portanto nao pode intersecta-lo
		if (dot(planeNormal, rayDirection) < FLT_EPSILON)
			return false;

		//Calculando distancia do plano para origem
		distanceToOrigin = planeNormal.x() * triangle.vertex[0].pos.x() + planeNormal.y() * triangle.vertex[0].pos.y() + planeNormal.z() * triangle.vertex[0].pos.z();
		//Calculando parametro T da equacao paramatrica do Raio
		parameterT = -(dot(planeNormal, _from) - distanceToOrigin) / dot(planeNormal, rayDirection);
		//Se o parametro T for menor que 0 temos que o objeto esta atras da camera
		//portanto nao ha intersecao
		if (parameterT < 0.f)
			return false;
		//C�lculo do ponto de intersecao
		intersectionPoint = _from + parameterT * rayDirection;

		//Se o ponto de intersecao estiver fora do triangulo, nao ha intersecao com o objeto
		if (!inside_outside_triangle_test(triangle, planeNormal, intersectionPoint))
			return false;

		//Ja que o ponto esta dentro do triangulo temos que ha intersecao com o objeto
		return true;
	}

	bool inside_outside_triangle_test(Triangle triangle, vec3 normal, vec3 P) {
		//Declaracao de variaveis
		vec3 AB, BC, CA, AP, BP, CP;
		
		//Obtencao das arestas do triangulo em sentido horario
		AB = triangle.vertex[1].pos - triangle.vertex[0].pos;
		BC = triangle.vertex[2].pos - triangle.vertex[1].pos;
		CA = triangle.vertex[0].pos - triangle.vertex[2].pos;

		//Obtencao dos vetores de cada vertice ate o ponto P com sentido para P
		AP = P - triangle.vertex[0].pos;
		BP = P - triangle.vertex[1].pos;
		CP = P - triangle.vertex[2].pos;
		//Checar se o produto interno entre normal e produtos vetoriais e maior que 0
		if (dot(normal, cross(AB, AP)) > 0 && dot(normal, cross(BC, BP)) > 0 && dot(normal, cross(CA, CP)) > 0)
			return true;

		//Se algum dos testes falhar, o ponto esta fora do triangulo
		return false;
	}

	vec3 phong_shading(vec3 viewPoint, vec3 triangleNormal, vec3 lightDir, float lightIntensity, vec3 textureColor) {
		vec3 diffuseComponent, specularComponent, reflection, color;
		float angleNormalRayCosine;

		angleNormalRayCosine = std::max(0.0f, -dot(triangleNormal, lightDir));

		reflection = lightDir - (2*dot(triangleNormal, lightDir)) * triangleNormal;
		reflection.make_unit_vector();
			
		diffuseComponent = angleNormalRayCosine*(Kd + textureColor);

		specularComponent = std::pow(std::max(0.0f, dot(reflection, viewPoint)), n)*Ks;

		color = enviromentConstant + lightIntensity*(specularComponent + diffuseComponent);

		return color;
	}

	float edge_function(const vec2 edge, const vec2 vertexToP) {
		float WA = (edge.x() * vertexToP.y()) - (edge.y() * vertexToP.x());

		return WA;
	}

	void fill_triangle(SDL_Renderer *renderer, const vec2 &v0, const vec2 &v1, const vec2 &v2, const Triangle &triangle, const vec3 &lightDir, std::vector<float> &zBuffer, std::vector<float> zCoordinates, std::vector<vec3> &textureBuffer, const int textureWidth, const int textureHeight) {
		float minX, minY, maxX, maxY, EFAB, EFBC, EFCA, TriangleArea;

		//Calculando bounding box do triangulo
		minX = std::min(v0.x(), v1.x());
		minX = std::min(minX, v2.x());

		minY = std::min(v0.y(), v1.y());
		minY = std::min(minY, v2.y());
		
		maxX = std::max(v0.x(), v1.x());
		maxX = std::max(maxX, v2.x());
		
		maxY = std::max(v0.y(), v1.y());
		maxY = std::max(maxY, v2.y());

		if (minX >= 0 && minY >= 0 && maxX <= WIDTH && maxY <= HEIGHT) {
			float zCoord, v0BarycentricCoordinate, v1BarycentricCoordinate, v2BarycentricCoordinate;
			vec2 textureCoord;
			vec3 normal, viewPoint, textureColor(0, 0, 0);
			
			for (int x = minX; x <= maxX; ++x) {
				for (int y = minY; y <= maxY; ++y) {
					EFAB = edge_function(v1 - v0, vec2(x - v0.x(), y - v0.y()));
					EFBC = edge_function(v2 - v1, vec2(x - v1.x(), y - v1.y()));
					EFCA = edge_function(v0 - v2, vec2(x - v2.x(), y - v2.y()));
					if (EFAB >= 0 && EFBC >= 0 && EFCA >= 0) {
						TriangleArea = edge_function(v1 - v0, v2 - v0);

						v0BarycentricCoordinate = EFBC / TriangleArea;
						v1BarycentricCoordinate = EFCA / TriangleArea;
						v2BarycentricCoordinate = EFAB / TriangleArea;

						zCoord = 1.f / (v0BarycentricCoordinate * zCoordinates[0] + v1BarycentricCoordinate * zCoordinates[1] + v2BarycentricCoordinate * zCoordinates[2]);

						if (zCoord < zBuffer[y * WIDTH + x]) {
							zBuffer[y * WIDTH + x] = zCoord;
							normal = v0BarycentricCoordinate * triangle.vertex[0].normal + v1BarycentricCoordinate * triangle.vertex[1].normal + v2BarycentricCoordinate * triangle.vertex[2].normal;
							normal.make_unit_vector();

							if (textureBuffer.size() > 0) {
								textureCoord = v0BarycentricCoordinate * triangle.vertex[0].texture + v1BarycentricCoordinate * triangle.vertex[1].texture + v2BarycentricCoordinate * triangle.vertex[2].texture;
								textureCoord[1] = 1.f - textureCoord[1];
								textureCoord[0] *= textureWidth;
								textureCoord[1] *= textureHeight;
								textureColor = textureBuffer[int(textureCoord[1]) * textureWidth + int(textureCoord[0])];
							}
							
							vec3 color = phong_shading(axisZ, normal, lightDir, 1.0f, textureColor);

							SDL_SetRenderDrawColor(renderer, std::min(255.f, color.r()), std::min(255.f, color.g()), std::min(255.f, color.b()), 255);
							SDL_RenderDrawPoint(renderer, x, y); //Desenha o ponto
						}
					}
				}
			}
		}
	}

    void render_scene( std::vector<Obj> objs, SDL_Renderer* renderer) {
		std::vector<float> zBuffer(WIDTH * HEIGHT, FLT_MAX);
        vec3 light(0.0f, 0.0f, -1.0f);
        light.make_unit_vector();

        for (auto obj : objs){
			for (int i = 0; i < obj.mesh.tris.size(); i++)
			{
				vec2 praster1;
				vec2 praster2;
				vec2 praster3;
				
				std::vector<float> zCoordinates;

				vec3 col(255, 255, 255);
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

				bool v1, v2, v3;
				v1 = compute_pixel_coordinates(obj.mesh.tris[i].vertex[0].pos, praster1, zCoordinates);
				v2 = compute_pixel_coordinates(obj.mesh.tris[i].vertex[1].pos, praster2, zCoordinates);
				v3 = compute_pixel_coordinates(obj.mesh.tris[i].vertex[2].pos, praster3, zCoordinates);
				
				/*A fim de clareza no objeto, é melhor não exibir suas linhas*/
				/*if (v1 && v2) {
					//Se estiver na tela, desenhar a linha
					if(clip_line(praster1, praster2))
						draw_line(renderer, praster1, praster2);
				}
				if (v1 && v3) {
					//Se estiver na tela, desenhar a linha
					if (clip_line(praster1, praster3))
						draw_line(renderer, praster1, praster3);
				}
				if (v2 && v3) {
					//Se estiver na tela, desenhar a linha
					if (clip_line(praster2, praster3))
						draw_line(renderer, praster2, praster3);
				}*/

				fill_triangle(renderer, praster1, praster2, praster3, obj.mesh.tris[i], light, zBuffer, zCoordinates, obj.texture_buffer, obj.texture_width, obj.texture_height);
            }
        }
    }
};


#endif
