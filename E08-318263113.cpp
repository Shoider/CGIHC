/*
Práctica 8: Iluminación 2
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

// Lampara
Model Lampara;
// Dado
Model dadoModel;

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
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		1.0f, 0.0f, -1.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		1.0f, 0.0f, -1.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		1.0f, 0.0f, -1.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		1.0f, 0.0f, -1.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};

	//////// DADO


	unsigned int dadoIndices[] = {
	   0, 1, 2,
	   3, 4, 5,
	   6, 7, 8,
	   9, 10, 11,
	   12, 13, 14,
	   15, 16, 17,
	   18, 19, 20,
	   21, 22, 23,
	   24, 25, 26,
	   27, 28, 29
	};

	GLfloat dadoVertices[] = {
		// Posiciones           // Coordenadas UV  // Normales
		// Cara 1
		0.0f, 0.0f, 6.0f,       0.0f, 0.0f,       -0.3890f, -0.7129f, 0.5835f,
		-6.0f, 0.0f, 2.0f,      1.0f, 0.0f,       -0.3890f, -0.7129f, 0.5835f,
		0.0f, -5.0f, -0.1094f,  0.5f, 1.0f,       -0.3890f, -0.7129f, 0.5835f,

		// Cara 2
		0.0f, -5.0f, -0.1094f,  0.0f, 0.0f,        0.3890f, -0.7129f, 0.5835f,
		6.0f, 0.0f, 2.0f,       1.0f, 0.0f,        0.3890f, -0.7129f, 0.5835f,
		0.0f, 0.0f, 6.0f,       0.5f, 1.0f,        0.3890f, -0.7129f, 0.5835f,

		// Cara 3
		-6.0f, 0.0f, 2.0f,      0.0f, 0.0f,       -0.6671f, -0.7201f, -0.1906f,
		-4.0f, 0.0f, -5.0f,     1.0f, 0.0f,       -0.6671f, -0.7201f, -0.1906f,
		0.0f, -5.0f, -0.1094f,  0.5f, 1.0f,       -0.6671f, -0.7201f, -0.1906f,

		// Cara 4
		-4.0f, 0.0f, -5.0f,     0.0f, 0.0f,       -0.0000f, -0.6992f, -0.7149f,
		3.0f, 0.0f, -5.0f,      1.0f, 0.0f,       -0.0000f, -0.6992f, -0.7149f,
		0.0f, -5.0f, -0.1094f,  0.5f, 1.0f,       -0.0000f, -0.6992f, -0.7149f,

		// Cara 5
		3.0f, 0.0f, -5.0f,      0.0f, 0.0f,        0.6708f, -0.6837f, -0.2875f,
		6.0f, 0.0f, 2.0f,       1.0f, 0.0f,        0.6708f, -0.6837f, -0.2875f,
		0.0f, -5.0f, -0.1094f,  0.5f, 1.0f,        0.6708f, -0.6837f, -0.2875f,

		// Cara 6
		-6.0f, 0.0f, 2.0f,   0.0f, 0.0f,       -0.6642f, 0.7231f, -0.1898f,
		-10.648956f, 10.590763f, -9.168627f, 1.0f, 0.0f,  -0.6642f, 0.7231f, -0.1898f,
		-4.0f, 0.0f, -5.0f,    0.5f, 1.0f,      -0.6642f, 0.7231f, -0.1898f,

		// Cara 7
		0.0f, -5.0f, -0.109369f,   0.0f, 0.0f,   0.0f, 0.7044f, -0.7098f,
		3.0f, 0.0f, -5.0f,          1.0f, 0.0f,   0.0f, 0.7044f, -0.7098f,
		0.0f, 5.0f, -0.037398f,     0.5f, 1.0f,   0.0f, 0.7044f, -0.7098f,

		// Cara 8
		0.025894f, 5.0f, -0.037398f, 0.0f, 0.0f,  -0.3908f, 0.7098f, 0.5861f,
		-3.512371f, -5.100750f, 13.801956f, 1.0f, 0.0f,  -0.3908f, 0.7098f, 0.5861f,
		-4.0f, 0.0f, -5.0f,          0.5f, 1.0f,  -0.3908f, 0.7098f, 0.5861f,

		// Cara 9
		0.025894f, 5.0f, -0.037398f, 0.0f, 0.0f,   0.3919f, 0.7077f, 0.5878f,
		3.0f, 0.0f, -5.0f,            1.0f, 0.0f,   0.3919f, 0.7077f, 0.5878f,
		6.0f, 0.0f, 2.0f,             0.5f, 1.0f,   0.3919f, 0.7077f, 0.5878f,

		// Cara 10
		6.0f, 0.0f, 2.0f,             0.0f, 0.0f,   0.6705f, 0.6840f, -0.2873f,
		-10.648956f, 10.590763f, -9.168627f, 1.0f, 0.0f,  0.6705f, 0.6840f, -0.2873f,
		3.0f, 0.0f, -5.0f,            0.5f, 1.0f,   0.6705f, 0.6840f, -0.2873f,

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

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(dadoVertices, dadoIndices, 38, 10);
	meshList.push_back(obj5);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

	// Calcular las normales promedio
	calcAverageNormals(dadoIndices, 30, dadoVertices, 90, 8, 5);

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

	//Textura y Modelo Dado

	Texture dadoTexture;
	dadoTexture = Texture("Textures/Dado10Caras.tga");	// Cargar la textura del dado
	dadoTexture.LoadTextureA();							// Cargar con soporte de alfa

	dadoModel = Model();
	dadoModel.LoadModel("Models/Dado10Caras.obj");		// Cargar el archivo .obj del cubo



	// Modelos
	/*Kitt_M = Model();
	Kitt_M.LoadModel("Models/Carro.obj");

	Llanta_M = Model();
	Llanta_M.LoadModel("Models/Llanta.obj");

	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");*/

	Lampara = Model();
	Lampara.LoadModel("Models/Lampara.obj");
	

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
		//Intensidad
		0.3f, 0.3f,
		//Direccion de la luz
		0.0f, -1.0f, 0.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.4f, 1.0f,
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;


	/// EJERCICIOS
	//Declaración de luz de mi lampara	
	pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,	// Color blanco
		1.0f, 3.0f,					// Intensidad alta para que se note
		40.0f, 10.0f, 0.0f,			// Posicion centrada en la lampara
		0.1f, 0.1f, 0.02f);			// Atenuacion
	pointLightCount++;

	

	////Declaración de luz	
	//pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,	// Color blanco
	//	1.0f, 3.0f,					// Intensidad alta para que se note
	//	40.0f, 10.0f, 0.0f,			// Posicion centrada en la lampara
	//	0.0f, 0.0f, 0.1f);			// Atenuacion
	//pointLightCount++;



	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		//Tamaño cono
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(1.0f, 0.0f, 0.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;


	/*
	//luz Faro
	spotLights[2] = SpotLight(0.0f, 0.0f, 1.0f, //Color Azul
		1.0f, 2.0f,
		15.0f, 2.0f, 0.0f,		//Posicion inicial
		-5.0f, 0.0f, 0.0f,		//Direccion en -X
		1.0f, 0.0f, 0.0f,
		25.0f);
	spotLightCount++;

	//luz Helicoptero
	
	spotLights[3] = SpotLight(1.0f, 1.0f, 0.0f, //Color Amarillo
		1.0f, 2.0f,
		15.0f, 2.0f, 0.0f,		//Posicion inicial
		-2.0f, -5.0f, 0.0f,		//Direccion Ligeramente hacia adelante para parecer realista
		1.0f, 0.0f, 0.0f,
		25.0f);
	spotLightCount++;
	
	*/

	//se crean mas luces puntuales y spotlight 

	// Definir dos arreglos de colores de las luces
	glm::vec3 arreglo3[4] = {
		glm::vec3(0.0f, 1.0f, 0.0f),  // Verde
		glm::vec3(0.0f, 0.0f, 1.0f),  // Azul
		glm::vec3(1.0f, 0.0f, 0.0f),  // Rojo
		glm::vec3(1.0f, 1.0f, 1.0f)   // Blanco
	};

	glm::vec3 arreglo4[4] = {
		glm::vec3(1.0f, 1.0f, 1.0f),  // Blanco
		glm::vec3(0.0f, 1.0f, 0.0f),  // Verde
		glm::vec3(0.0f, 0.0f, 1.0f),  // Azul
		glm::vec3(1.0f, 0.0f, 0.0f)   // Rojo
	};

	int arregloSeleccionado = 1; // Para alternar entre los arreglos




	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

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

		
		//Agave ¿qué sucede si lo renderizan antes del coche y el helicóptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		//Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);

		

		// En el bucle de renderizado o lógica
		/*for (int i = 0; i < 4; i++) {
			if (arregloSeleccionado == 1) {
				pointLights[1].setColor(arreglo3[i].x, arreglo3[i].y, arreglo3[i].z);
			}
			else {
				pointLights[1].setColor(arreglo4[i].x, arreglo4[i].y, arreglo4[i].z);
			}
		}*/

		//////////////////Ejercicios

		// Para cambiar el arreglo en tiempo de ejecución (ejemplo con una tecla)
		if (glfwGetKey(mainWindow.getMainWindow(), GLFW_KEY_T) == GLFW_PRESS) {
			//arregloSeleccionado = 1 - arregloSeleccionado; // Alternar entre encendido y apagadp pointLightCount
			if (pointLightCount == 2) {
				pointLightCount = 1;
			}
			else {
				pointLightCount = 2;
			}
		}

		//Declaro otra matriz para la lampara
		glm::mat4 modellamp(1.0);

		// Lampara
		modellamp = glm::mat4(1.0);

		// Ajusto posicion
		modellamp = glm::translate(modellamp, glm::vec3(40.0f, 0.0f, 0.0f));
		modellamp = glm::scale(modellamp, glm::vec3(0.25f, 0.25f, 0.25f));
		modellamp = glm::rotate(modellamp, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		// Renderizo
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modellamp));
		Lampara.RenderModel();


		// DADO

		dadoTexture.UseTexture();  // Aplicar la textura

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5.0f, 10.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//meshList[4]->RenderMesh();  // Dibujar el dado en la escena
		dadoModel.RenderModel();
		
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
