/*
Semestre 2024-1
Animación:
Sesión 1:
Simple o básica:Por banderas y condicionales (más de 1 transforomación geométrica se ve modificada
Sesión 2
Compleja: Por medio de funciones y algoritmos.
Adicional.- ,Textura Animada
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//variables para animación
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanza;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;

Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;

//	Practica

Model Dado;

Model Helicopero;

// Variables declaradas
float movDadoX;
float movDadoY;
float movDadoZ;
float offsetMovDado;

float rotDadoX;
float rotDadoY;
float rotDadoZ;
float offsetRotDado;

int estado;
int animacion;

bool lanzar;
bool rKeyPressed;

float movHelX;
float movHelY;
float movHelZ;
float offsetMovHel;

float rotHelX;
float rotHelY;
float rotHelZ;
float offsetRotHel;

int estado2;


Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};
	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");

	//	Practica

	Dado = Model();
	Dado.LoadModel("Models/Dado10Caras.obj");

	Helicopero = Model();
	Helicopero.LoadModel("Models/MRX22 Recon Flyer.obj");
	

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(0.1f, 0.1f, 0.1f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f,
		1.0f, 2.0f,
		5.0f, 40.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.001f,
		45.0f);
	spotLightCount++;
	
	//se crean mas luces puntuales y spotlight 

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	movCoche = 0.0f;
	movOffset = 0.3f;	//Velocidad
	rotllanta = 0.0f;
	rotllantaOffset = 5.0f;

	avanza = true;		//Direccion

	glfwSetTime(0);		//Contador
	float contadorPasos = 0;	

	//	Practica
	movDadoX = 0.0f;
	movDadoY = 0.0f;
	movDadoZ = 0.0f;
	offsetMovDado = 0.08f;		//Velocidad Movimiento

	rotDadoX = 0.0f;
	rotDadoY = 0.0f;
	rotDadoZ = 0.0f;
	offsetRotDado = 1.0f;	//Velocidad Rotacion

	estado = 0;
	animacion = 0;

	lanzar = false;
	rKeyPressed = false;

	movHelX = 0.0f;
	movHelY = 0.0f;
	movHelZ = 0.0f;
	offsetMovHel = 10.0f;

	rotHelX = 0.0f;
	rotHelY = 0.0f;
	rotHelZ = 0.0f;
	offsetRotHel = 0.5f;

	estado2 = 0;


	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//printf("Get Time: %f \n", glfwGetTime());

		if (glfwGetTime() > 5) {

			if (avanza == true) {
				if (movCoche > -305.0f) {
					movCoche -= movOffset * deltaTime;
					rotllanta += rotllantaOffset * deltaTime;
				}
				else {
					avanza = !avanza;
				}
			}

			if (avanza == false) {
				if (movCoche < 295.0f) {
					movCoche += movOffset * deltaTime;
					rotllanta -= rotllantaOffset * deltaTime;
				}
				else {
					avanza = !avanza;
				}
			}

		}
		

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		
		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
			glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);



		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		//Instancia del coche 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movCoche, 0.5f, -3.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Kitt_M.RenderModel();

		//Llanta delantera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(7.0f, -0.5f, 8.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);//llanta con color gris
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta trasera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(15.5f, -0.5f, 8.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta delantera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(7.0f, -0.5f, 1.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta trasera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(15.5f, -0.5f, 1.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();
	

		//	Practica	//////////////

		//	Modelo

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-150.0f, 10.0f, -150.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));

		model = glm::translate(model, glm::vec3(movHelX * 10, 0.0f, movHelZ * 10));

		model = glm::rotate(model, rotHelY * toRadians, glm::vec3(0.0f, -1.0f, 0.0f));


		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Helicopero.RenderModel();

		///////////////////////////////

		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));


		model = glm::translate(model, glm::vec3(movDadoX, movDadoY + 12.0f, movDadoZ));

		model = glm::rotate(model, rotDadoX * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, rotDadoY * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotDadoZ * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Dado.RenderModel();


		// Animaciones

		if (glfwGetTime() > 5) {

			//	Animacion #0
			if (animacion == 0) {

				if (lanzar == false) {
					animacion = rand() % 4 + 1;
					printf("Lanzando dado ... \n");
					printf("Animacion # %d \n", animacion);
					printf("------------------ \n\n");
				}

			}

			// Animacion #1
			if (animacion == 1) {

				if (estado == 0) {
					if (movDadoY > -10.0f) {
						movDadoY -= offsetMovDado * deltaTime;
					}
					else {
						estado = 1;
					}
				}

				if (estado == 1) {
					if (movDadoX <= 20.0f && rotDadoY <= 45.0f && rotDadoZ <= 150.0f) {
						movDadoX -= offsetMovDado * deltaTime;
						rotDadoY += offsetRotDado * deltaTime;
						rotDadoZ += offsetRotDado * deltaTime;
					}
					else {
						estado = 2;
					}
				}

				// Espera la tecla
				if (estado == 2) {
					if (lanzar == true) {
						animacion = 0;
						estado = 0;

						movDadoX = 0.0f;
						movDadoY = 0.0f;
						movDadoZ = 0.0f;

						rotDadoX = 0.0f;
						rotDadoY = 0.0f;
						rotDadoZ = 0.0f;
					}
				}
			}

			// Animacion #2
			if (animacion == 2) {

				if (estado == 0) {
					if (movDadoY > -10.0f) {
						movDadoY -= offsetMovDado * deltaTime;
						rotDadoX = 70.0f;
					}
					else {
						estado = 1;
					}
				}

				if (estado == 1) {
					if (movDadoX <= 15.0f && rotDadoY <= 600.0f && rotDadoZ <= 720.0f && movDadoZ <= 25.0f) {
						movDadoX += offsetMovDado * deltaTime;
						movDadoZ += offsetMovDado * deltaTime;
						rotDadoY += offsetRotDado * deltaTime;
						rotDadoZ += offsetRotDado * deltaTime;
					}
					else {
						estado = 2;
					}
				}

				// Espera la tecla
				if (estado == 2) {
					if (lanzar == true) {
						animacion = 0;
						estado = 0;

						movDadoX = 0.0f;
						movDadoY = 0.0f;
						movDadoZ = 0.0f;

						rotDadoX = 0.0f;
						rotDadoY = 0.0f;
						rotDadoZ = 0.0f;
					}
				}
			}

			// Animacion #3
			if (animacion == 3) {

				if (estado == 0) {
					if (movDadoY > -10.0f) {
						movDadoY -= offsetMovDado * deltaTime;
						rotDadoZ = -30.0f;
					}
					else {
						estado = 1;
					}
				}

				if (estado == 1) {
					if (movDadoZ <= 10.0f && rotDadoY <= 60.0f && rotDadoX >= -180.0f && rotDadoZ <= 90.0f && movDadoX <= 18.0f) {
						movDadoZ += offsetMovDado * deltaTime;
						movDadoX -= offsetMovDado * deltaTime;
						rotDadoX -= offsetRotDado * deltaTime;
						rotDadoY += offsetRotDado * deltaTime;
						rotDadoZ += offsetRotDado * deltaTime;
					}
					else {
						estado = 2;
					}
				}

				// Espera la tecla
				if (estado == 2) {
					if (lanzar == true) {
						animacion = 0;
						estado = 0;

						movDadoX = 0.0f;
						movDadoY = 0.0f;
						movDadoZ = 0.0f;

						rotDadoX = 0.0f;
						rotDadoY = 0.0f;
						rotDadoZ = 0.0f;
					}
				}
			}

			// Animacion #4
			if (animacion == 4) {

				if (estado == 0) {
					if (movDadoY > -10.0f) {
						movDadoY -= offsetMovDado * deltaTime;
						rotDadoZ = 180.0f;
					}
					else {
						estado = 1;
					}
				}

				if (estado == 1) {
					if (movDadoZ >= -18.0f && rotDadoY <= 300.0f && rotDadoX <= 450.0f && rotDadoZ >= -900.0f && movDadoX <= 10.0f) {
						movDadoZ -= offsetMovDado * deltaTime;
						movDadoX += offsetMovDado * deltaTime;
						rotDadoX += offsetRotDado * deltaTime;
						rotDadoY += offsetRotDado * deltaTime;
						rotDadoZ -= offsetRotDado * deltaTime;
					}
					else {
						estado = 2;
					}
				}

				// Espera la tecla
				if (estado == 2) {
					if (lanzar == true) {
						animacion = 0;
						estado = 0;

						movDadoX = 0.0f;
						movDadoY = 0.0f;
						movDadoZ = 0.0f;

						rotDadoX = 0.0f;
						rotDadoY = 0.0f;
						rotDadoZ = 0.0f;
					}
				}
			}
		}

		if (glfwGetKey(mainWindow.getMainWindow(), GLFW_KEY_R) == GLFW_PRESS && !rKeyPressed) {
			// Alternar el booleano de la luz 2
			lanzar = true;
			rKeyPressed = true; // Marcar que la tecla se presionó
		}

		if (glfwGetKey(mainWindow.getMainWindow(), GLFW_KEY_R) == GLFW_RELEASE) {
			rKeyPressed = false; // Permitir que la tecla pueda alternar nuevamente al soltarse
			lanzar = false;
		}

		////////////////////////////////

		if (glfwGetTime() > 3) {

			if (estado2 == 0) {

				rotHelX = 0.0f;
				rotHelY = -90.0f;
				rotHelZ = 0.0f;

				estado2 += 1;
			}

			if (estado2 == 1) {
				if (movHelX <= 300.0f) {
					movHelX += offsetMovHel * deltaTime;
				}
				else {
					estado2 += 1;
				}
			}

			if (estado2 == 2) {
				if (rotHelY <= 0.0f) {
					rotHelY += offsetRotHel * deltaTime;
				}
				else {
					estado2 += 1;
				}
			}

			if (estado2 == 3) {
				if (movHelZ <= 300.0f) {
					movHelZ += offsetMovHel * deltaTime;
				}
				else {
					estado2 += 1;
				}
			}

			if (estado2 == 4) {
				if (rotHelY <= 90.0f) {
					rotHelY += offsetRotHel * deltaTime;
				}
				else {
					estado2 += 1;
				}
			}

			if (estado2 == 5) {
				if (movHelX >= -300.0f) {
					movHelX -= offsetMovHel * deltaTime;
				}
				else {
					estado2 += 1;
				}
			}

			if (estado2 == 6) {
				if (rotHelY <= 180.0f) {
					rotHelY += offsetRotHel * deltaTime;
				}
				else {
					estado2 += 1;
				}
			}

			if (estado2 == 7) {
				if (movHelZ >= -300.0f) {
					movHelZ -= offsetMovHel * deltaTime;
				}
				else {
					estado2 += 1;
				}
			}

			if (estado2 == 8) {
				if (rotHelY <= 270.0f) {
					rotHelY += offsetRotHel * deltaTime;
				}
				else {
					estado2 = 0;
				}
			}

		}



		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
