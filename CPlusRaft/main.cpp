
#include <iostream>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Utils.h"
#include "Utils/Vector3.h"
#include "Utils/TextureManager.h"
#include "Utils/SoundManager.h"
#include "Utils/KeyboardManager.h"
#include "Utils/ThreadManager.h"
#include "Utils/ModelManager.h"
#include "Utils/ShaderManager.h"
#include "Entities/Player.h"
#include "Render/Shader.h"
#include <sys/stat.h>
#include <thread>  

#define TPS 30 //Actualitzacions del món per segon

int w_width = 1366;
int w_height = 768;
GLFWwindow* window;


#include "World.h"
World* world;

//Clipping planes
const float zNear = 0.001f;
float viewDist = 16.0f; //Distància de visió (en chunks)

float zFar = viewDist*CHUNKSIZE + CHUNKSIZE;
const float axisSize = zFar;

Vector3<float> ba; //Bloc actual (posició)
Vector3<float> bp; //Bloc a posar posició)

int btipus = 2; //Bloc actual (tipus)

#include "Camera.h"
Camera camera = Camera(Vector3<float>(64, 66, 64), Vector3<float>(64, 66, 60));

int darrerIdle = 0, darrerDisplay = 0; //Control del temps

double lastX = 250, lastY = 250; //Darreres posicions del ratolí

//llums
bool llanterna = false;
bool smoothlight = true;

//Control amb gamepad (control press)
bool press[GLFW_GAMEPAD_BUTTON_LAST+1];

//Entitats
#include "Entities/Entity.h"
Entity* ent; //Entitat controlada

bool axisVisible = false; //Eixos ON/OFF

float mapMult = 1; //Multiplicador mapa

//Nom del món
std::string wname;

void onWindowResize(GLFWwindow* window, int width, int height);
void onKeyboardUp(unsigned char key, int x, int y);
void onKeyboardDown(unsigned char key, int x, int y);
void toggleAxis();
void setLighting();
void scaleListener(GLFWwindow* window, double xoffset, double yoffset);
void mouseListener(GLFWwindow* window, int button, int action, int mods);
void lookAround(GLFWwindow* window, double x, double y);
void movement(int key);
void updateMousePos();
void updatePlayerBlock();
void onKey(GLFWwindow* window, int key, int scancode, int action, int mods);
void gamePad();
void draw2DUI();

double lastTime;
double lastTimeW;
int fpsc = 0;
float fps = 0;
bool run = false;
bool drawfps = false;

enum class Active {
	JOC,
	INVENTARI,
	MAPA
};

int mapY = 5;

Active act = Active::JOC; //Indica a quin menú / part està l'usuari

// Funcion que visualiza la escena OpenGL
void Display(GLFWwindow* window)
{
	int temps = int(glfwGetTime() * 1000);
	//printf("%d\n", temps);
	int delta = temps - darrerDisplay;
	fps = 1.0f / ((float)delta / 1000.0f);

	fpsc++;
	if (fpsc > 20) { //Contador fps
		//printf("%f\n", fps);
		fpsc = 0;
	}
	//if (fps < 26) {
	//	zFar--;
	//}
	//else if (fps > 34) {
	//	zFar++;
	//}
	darrerDisplay = temps;

	glBindTexture(GL_TEXTURE_2D, 0); //I que no tenim cap textura seleccionada
	glViewport(0, 0, w_width, w_height); //Establim el viewport

	// borrar l'escena pintada
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// guardar la matriu abans d'aplicar rotacions i translacions
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(camera.getFov(), camera.getAspect(), zNear, zFar);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (llanterna) {
		glEnable(GL_LIGHT0);
	}
	else {
		glDisable(GL_LIGHT0);
	}

	//Llanterna
	if (llanterna && ent == 0) {
		GLfloat llumpos[4] = { 0,0,0, 1 }; //Ha de sortir del viewpoint
		glLightfv(GL_LIGHT0, GL_POSITION, llumpos);
		float spotdir[3] = { 0, 0, -1 };
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotdir);
	}

	glPushMatrix();

	if (ent != 0) { //Si hi ha una entitat controlada, centram la càmera a ella
		ent->setCam(&camera);
		//printf("G %f\n", ((Player*)ent)->grav);
	}

	//Transformam la matriu de visualització per mirar on toqui
	camera.display();

	glEnable(GL_LIGHTING); //Ens asseguram que l'il·luminació està activada

	world->draw(camera.getPos(), viewDist, delta); //Dibuixam el món

	//Dibuixam el bloc seleccionat
	if (Block::isSolid(world->getBlock(ba))) {
		glDisable(GL_LIGHTING); //El volem veure sempre
		glPushMatrix();
		glLineWidth(5.0f);
		glColor3f(1, 0, 0);
		ba.floor();
		glTranslatef(ba.x, ba.y, ba.z);
		glBegin(GL_LINE_LOOP);
		glVertex3f(0, 0, 0); //Part d'abaix
		glVertex3f(0, 0, 1);
		glVertex3f(1, 0, 1);
		glVertex3f(1, 0, 0);
		glVertex3f(0, 0, 0);
		glEnd();
		glBegin(GL_LINE_LOOP);
		glVertex3f(0, 1, 0); //Part d'adalt
		glVertex3f(0, 1, 1);
		glVertex3f(1, 1, 1);
		glVertex3f(1, 1, 0);
		glVertex3f(0, 1, 0);
		glEnd();
		glBegin(GL_LINES);
		glVertex3f(0, 0, 0); //Part d'adalt
		glVertex3f(0, 1, 0);
		glVertex3f(1, 0, 0);
		glVertex3f(1, 1, 0);
		glVertex3f(0, 0, 1);
		glVertex3f(0, 1, 1);
		glVertex3f(1, 0, 1);
		glVertex3f(1, 1, 1);
		glEnd();
		glLineWidth(1.0f);
		glPopMatrix();
		glEnable(GL_LIGHTING); //El volem veure sempre
	}

	world->drawSol(camera.getPos(), zFar - 1); //Dibuixam el sol

	// dibuixar els 3 eixos
	if (axisVisible) {
		world->drawAxis(Vector3<float>((float)(SPAWNSIZE * CHUNKSIZE) / 2, (float)(SPAWNSIZE * CHUNKSIZE) / 2 + 1, (float)(SPAWNSIZE * CHUNKSIZE) / 2), 50.0f);
	}

	glPopMatrix();

	//Dibuix 2D, damunt l'escena 3D
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0.0, 1.0 * camera.getAspect(), 0, 1.0, 0.1, 10.0); //Cream una vista ortogràfica
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_CULL_FACE);

	glClear(GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	draw2DUI();

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	//Fi dibuix 2D

	// recuperar la matriu original pel pròxim display
	glPopMatrix();

	glFlush();
}

// Update joc
void Update(void)
{

	//Gestió del temps
	double time = glfwGetTime();
	double delta = time - lastTime;
	lastTime = time;

	if (ent != nullptr) { //L'entitat que controlam s'ha d'actualitzar quan l'ordinador ho permeti
		ent->update(delta);
		ent->setRot(camera.yaw);
		ent->control(delta, &camera); //Actualitzam el seu estat intern
		Vector3 pos = ent->getPos();
		alListener3f(AL_POSITION, pos.x, pos.y, pos.z); //Actualitzam la posició de l'escoltador a l'entitat
	}

	updatePlayerBlock();


	//Actualitzam el món
	//std::async(&World::update, world, delta, camera.getPos());
	time = glfwGetTime();
	double deltaW = time - lastTimeW;
	if (deltaW > 1.0f / (float)TPS) {
		lastTimeW = time;
		world->update(deltaW, camera.getPos());
	}
}
void Idle() {
	while (run) {
		Update();
	}
}

void Draw() {
	glfwMakeContextCurrent(window);
	onWindowResize(window, w_width, w_height);
	//glfwSwapInterval(1); //Activar VSYNC
	while (!glfwWindowShouldClose(window))
	{
		Display(window);
		glfwSwapBuffers(window);
	}
	run = false;
}

// Funcion principal
int main(int argc, char** argv)
{

	//Càmera
	camera.setPos(Vector3<float>(64, 66, 64));
	camera.setFreeLook(true);
	camera.setFreeMove(true);

	//Fils (detecció num)
	ThreadManager::initialize();
	TextureManager::initialize();

	//Carpetes
	std::filesystem::create_directory("shaders");
	std::filesystem::create_directory("worlds");

	printf("World name: ");
	std::cin >> wname;
	std::string path = "worlds/" + wname;
	struct stat buffer;
	if (stat(path.c_str(), &buffer) == 0) {
		//Si el món ja existeix, el carregam
		printf("Loading world %s... \n", wname.c_str());
		world = new World(wname, &camera);
		//ent = new Player(world, Vector3<float>(400, 120, 400) + Vector3<float>(0, 2.0f, 0));
		ent = new Player(world, Vector3<float>(0, 128, 0) + Vector3<float>(0, 2.0f, 0));
		//printf("with spawn at %f %f %f\n", world->getSpawn().x, world->getSpawn().y, world->getSpawn().z);
	}
	else {
		//Si no, el cream
		float f = 0.125f;
		printf("float %f\n", f);
		unsigned short s = toHFloat(f);
		float f2 = toFloat(s);
		printf("float2 %f\n", f2);
		printf("Creating world %s... \n", wname.c_str());
		printf("Seed: ");
		std::string sseed;
		std::cin >> sseed;
		int seed = 0;
		for (int i = 0; i < sseed.length(); i++) {
			seed += sseed[i];
		}
		//int seed = std::atoi(sseed.c_str());
		printf("Seed: %d\n", seed);
		world = new World(wname, seed, &camera);
		world->setRandomSpawn();
		world->save();
		ent = new Player(world, Vector3<float>(0, 128, 0) + Vector3<float>(0, 2.0f, 0));
		//ent = new Player(world, Vector3<float>((float)world->getSpawn().x, (float)world->getSpawn().y, (float)world->getSpawn().z) + Vector3<float>(0, 2.0f, 0));
	}
	// Inicialitzam el GLFW
	glfwInit();

	window = glfwCreateWindow(w_width, w_height, "C++Craft", NULL, NULL);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	glfwSetWindowSizeCallback(window, onWindowResize);

	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, onKey);

	//Inicialització Glew
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		printf("glewInit error: %s \n", glewGetErrorString(err));
		exit(1);
	}

	// Escoltadors de ratolí
	glfwSetScrollCallback(window, scaleListener);
	glfwSetCursorPosCallback(window, lookAround);
	glfwSetMouseButtonCallback(window, mouseListener);

	glEnable(GL_DEPTH_TEST); //Activam la profunditat

	glEnable(GL_LIGHTING); //Il·luminació
	glEnable(GL_COLOR_MATERIAL); //Perquè els objectes reflecteixin el color
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	float specular[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular); //Especular per defecte
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 1); //Brillantor
	glShadeModel(GL_SMOOTH); //Model d'ombrejat

	glEnable(GL_BLEND); //Activam transparència
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Color de fons
	glClearColor(0,0,0, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(camera.getFov() , 1, zNear, zFar);
	camera.setViewDist(viewDist * CHUNKSIZE); //Establim la distància de visió de la càmera

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Activam la boira
	glEnable(GL_FOG);
	float fogcolor[4] = { 0,0,0.1f,0.1f};
	glFogfv(GL_FOG_COLOR, fogcolor);

	// altres configuracions
	KeyboardManager::addKeyHandler('x', &toggleAxis);
	KeyboardManager::addKeyHandler(&movement);

	glEnable(GL_CULL_FACE); //Cull face, no renderitza les cares no visibles
	glCullFace(GL_BACK);

	//Textures
	glEnable(GL_TEXTURE_2D); //Activació
	TextureManager::LoadTexture("Textures/texture.png", "Bloc");
	TextureManager::LoadTexture("Textures/font.png", "Font");
	TextureManager::LoadTexture("Models/Sheep/sheep_pallete.png", "Ovella");
	TextureManager::LoadTexture("Models/Ostrich/Ostrich.png", "Estruç");

	//Models
	ModelManager::addModel("Estruç", 231, "Models/Ostrich/Ostrich");
	ModelManager::addModel("OvellaCos", 1, "Models/Sheep/sheepCentrat.obj");
	ModelManager::addModel("OvellaCamaDavantDreta", 1, "Models/Sheep/SheepDavDR.obj");
	ModelManager::addModel("OvellaCamaDavantEsquerra", 1, "Models/Sheep/SheepDavES.obj");
	ModelManager::addModel("OvellaCamaDarreraDreta", 1, "Models/Sheep/SheepDarDR.obj");
	ModelManager::addModel("OvellaCamaDarreraEsquerra", 1, "Models/Sheep/SheepDarES.obj");

	//Sons
	SoundManager::initialize();
	SoundManager::loadSound("Sons/jukebox.wav", "Musica");
	SoundManager::loadSound("Sons/switch.wav", "OnOff");
	SoundManager::loadSound("Sons/break.wav", "Destrueix");
	SoundManager::loadSound("Sons/place.wav", "Coloca");
	SoundManager::loadSound("Sons/passes.wav", "Camina");

	//Antialising
	glfwWindowHint(GLFW_SAMPLES, 8);
	glEnable(GL_MULTISAMPLE);

	setLighting();

	std::thread drw(Draw);
	glfwMakeContextCurrent(NULL);
	run = true;
	// Comienza la ejecucion del core de GLUT
	lastTime = (float)glfwGetTime();
	lastTimeW = lastTime;
	while (run) {
		gamePad();
		glfwPollEvents();
		Update();
	}
	run = false;
	drw.join();
	world->save();
	world->destroy();
	glfwTerminate();
	return 0;
}

void draw2DUI() {
	if (drawfps) {
		glPushMatrix();
		glTranslatef(0.01f * camera.getAspect(), 0.95f, 0);
		drawString(std::to_string(fps), 0.05f);
		glTranslatef(0, 0.1f, 0);
		glPopMatrix();
	}

	//Dibuixam el bloc seleccionat al cantó esquerra inferior
	Bloc bsel = static_cast<Bloc>(btipus);
	if (bsel != Bloc::LIMIT && bsel != Bloc::RES) {
		glPushMatrix();
		glTranslatef(0.9f * camera.getAspect(), 0.1f, -2);
		glScalef(0.1f, 0.1f, 0.1f);
		glEnable(GL_TEXTURE_2D);
		TextureManager::applyTexture("Bloc");
		world->drawBloc(bsel);
		glPopMatrix();
	}

	if (act == Active::INVENTARI) { //Dibuixam inventari
		glPushMatrix();
		float aspect = camera.getAspect();
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);

		glColor3f(1, 1, 1);
		glBegin(GL_QUADS); //Quadrat blanc exterior
		glVertex3f(0.5f * aspect - 0.4f, 0.9f, -1);
		glVertex3f(0.5f * aspect - 0.4f, 0.1f, -1);
		glVertex3f(0.5f * aspect + 0.4f, 0.1f, -1);
		glVertex3f(0.5f * aspect + 0.4f, 0.9f, -1);
		glEnd();

		glColor4f(0, 0, 0, 1);
		glLineWidth(3.0f);
		glBegin(GL_LINES); //Línies que indiquen els límits del dibuixat de l'inventari
		glVertex3f(0.5f * aspect - 0.4f, 0.9f, -1);
		glVertex3f(0.5f * aspect - 0.4f, 0.1f, -1);

		glVertex3f(0.5f * aspect + 0.4f, 0.1f, -1);
		glVertex3f(0.5f * aspect + 0.4f, 0.9f, -1);

		glVertex3f(0.5f * aspect - 0.4f, 0.1f, -1);
		glVertex3f(0.5f * aspect + 0.4f, 0.1f, -1);

		glVertex3f(0.5f * aspect - 0.4f, 0.9f, -1);
		glVertex3f(0.5f * aspect + 0.4f, 0.9f, -1);
		glEnd();

		glEnable(GL_TEXTURE_2D);
		TextureManager::applyTexture("Bloc");
		glTranslatef(0.5f * aspect - 0.3f, 0.8f, -1);
		for (int i = 1; i < NBLOCS - 1; i++) { //Objectes de l'inventari, botam primer (RES) i darrer (LIMIT)
			glPushMatrix();
			glScalef(0.1f, 0.1f, 0.1f);
			Block bsel = Block(static_cast<Bloc>(i));
			glDisable(GL_LIGHTING);

			if (bsel.getId() == static_cast<Bloc>(btipus)) { //L'hem de dibuixar com a seleccionat
				glPushMatrix();

				glColor4f(0, 0, 0, 1);
				glLineWidth(5.0f);
				glBegin(GL_LINES);
				glVertex3f(-0.5f, 0.5f, -0.5f);
				glVertex3f(-0.5f, -0.5f, -0.5f);

				glVertex3f(0.5f, 0.5f, -0.5f);
				glVertex3f(0.5f, -0.5f, -0.5f);

				glVertex3f(-0.5f, 0.5f, 0.5f);
				glVertex3f(0.5f, 0.5f, -0.5f);

				glVertex3f(-0.5f, -0.5f, -0.5f);
				glVertex3f(0.5f, -0.5f, -0.5f);
				glEnd();

				glPopMatrix();
			}

			world->drawBloc(bsel.getId());
			glPopMatrix();

			glTranslatef(0.12f, 0, 0); //Passam a la següent columna
			if (i % 6 == 0) { //I, si cal, a la següent fila
				glTranslatef(0, -0.12f, 0);
				glTranslatef(-0.12f * 6, 0, 0);
			}
		}
		glPopMatrix();
	}
	else if (act == Active::MAPA) {
		world->drawMap(camera.getAspect(), ent, mapY, (int)ceil((camera.getViewDist() / CHUNKSIZE) * mapMult));
	}
	else {
		glColor3i(0, 0, 0);
		glLineWidth(1.0f);
		glBegin(GL_LINES); //Mira (creu enmig de la pantalla)
		glVertex3f((camera.getAspect() / 2) - 0.02f, 0.5f, -1);
		glVertex3f((camera.getAspect() / 2) + 0.02f, 0.5f, -1);
		glVertex3f((camera.getAspect() / 2), 0.48f, -1);
		glVertex3f((camera.getAspect() / 2), 0.52f, -1);
		glEnd();
	}
}

void gamePad() {
	if (!glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
		return;
	}
	GLFWgamepadstate state;
	if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
	{
		if (act == Active::JOC) {
			//Control càmera
			float aX = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
			float aY = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
			if (abs(aX) > 0.2f || abs(aY) > 0.2f) {
				lookAround(window, lastX + aX / 50, lastY + aY / 50);
			}

			//Bot
			if (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS && !press[GLFW_GAMEPAD_BUTTON_A])
			{
				KeyboardManager::onKeyDown(GLFW_KEY_SPACE);
			}
			else if (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_RELEASE) {
				KeyboardManager::onKeyUp(GLFW_KEY_SPACE);
			}

			//Crouch
			if (state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS)
			{
				KeyboardManager::onKeyDown(GLFW_KEY_LEFT_CONTROL);
			}
			else {
				KeyboardManager::onKeyUp(GLFW_KEY_LEFT_CONTROL);
			}

			//Item seleccionat

			//Moviment DPAD
			if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] > 0.2f)
			{
				KeyboardManager::onKeyDown(GLFW_KEY_S);
			}
			else {
				KeyboardManager::onKeyUp(GLFW_KEY_S);
			}
			if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] < -0.2f)
			{
				KeyboardManager::onKeyDown(GLFW_KEY_W);
			}
			else {
				KeyboardManager::onKeyUp(GLFW_KEY_W);
			}
			if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] < -0.2f)
			{
				KeyboardManager::onKeyDown(GLFW_KEY_A);
			}
			else {
				KeyboardManager::onKeyUp(GLFW_KEY_A);
			}
			if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS || state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] > 0.2f)
			{
				KeyboardManager::onKeyDown(GLFW_KEY_D);
			}
			else {
				KeyboardManager::onKeyUp(GLFW_KEY_D);
			}


			if (state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_PRESS && !press[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER]) //Col·locar bloc
			{
				mouseListener(window, GLFW_MOUSE_BUTTON_RIGHT, GLFW_PRESS, 0);
			}
			if (state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] == GLFW_PRESS && !press[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER]) //Llevar bloc
			{
				mouseListener(window, GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, 0);
			}
			if (state.buttons[GLFW_GAMEPAD_BUTTON_Y] && !press[GLFW_GAMEPAD_BUTTON_Y]) //INVENTARI
			{
				movement(GLFW_KEY_TAB);
			}
			if (state.buttons[GLFW_GAMEPAD_BUTTON_X] && !press[GLFW_GAMEPAD_BUTTON_X]) //INVENTARI
			{
				movement(GLFW_KEY_F);
			}
		}
		else if (act == Active::INVENTARI) {
			int nbtipus = btipus;
			if (state.buttons[GLFW_GAMEPAD_BUTTON_Y] && !press[GLFW_GAMEPAD_BUTTON_Y]) //INVENTARI
			{
				movement(GLFW_KEY_ESCAPE);
			}
			if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] && !press[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT]) //INVENTARI
			{
				nbtipus++;
			}
			if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] && !press[GLFW_GAMEPAD_BUTTON_DPAD_LEFT]) //INVENTARI
			{
				nbtipus--;
			}
			if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] && !press[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]) //INVENTARI
			{
				nbtipus+=6;
			}
			if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] && !press[GLFW_GAMEPAD_BUTTON_DPAD_UP]) //INVENTARI
			{
				nbtipus-=6;
			}
			if (nbtipus >= 2 && nbtipus <= NBLOCS) {
				btipus = nbtipus;
			}
		}

		//Actualitzam l'estat dels botons
		for (int i = 0; i <= GLFW_GAMEPAD_BUTTON_LAST; i++) {
			if (state.buttons[i] == GLFW_PRESS && !press[GLFW_GAMEPAD_BUTTON_A])
			{
				press[i] = true;
			}
			else if (state.buttons[i] == GLFW_RELEASE) {
				press[i] = false;
			}
		}
	}
}

//Control amb botons del ratolí
void mouseListener(GLFWwindow* window, int button, int action, int mods) {
	if (act == Active::INVENTARI) { //Interacció amb l'inventari
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) { 
			float maxy = (float)w_height * 0.85f;
			float miny = (float)w_height * 0.15f;
			float maxx = (float)w_width / 2.0f + ((float)w_height/2.5f)*0.90f;
			float minx = (float)w_width / 2.0f - ((float)w_height / 2.5f) * 0.90f;

			double x;
			double y;
			glfwGetCursorPos(window, &x, &y);

			float yi = ((float)y - miny) / (maxy-miny) * 6;// *7.02f;
			float xi = ((float)x - minx) / (maxx - minx) * 6;// *7.1f;
			printf("%f %f\n", xi, yi);

			btipus = (int)floor(yi) * 6 +  (int)floor(xi) + 1; //+2 per botar RES
			if (btipus > NBLOCS - 2) { //Si no se selecciona cap objecte, no n'hi haurà cap de seleccionat
				btipus = static_cast<int>(Bloc::RES);
			}
		}
	}
	else if (act == Active::MAPA) {

	}
	else {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) { //Botó dret, eliminar blocs
			if (world->deleteBlock(Vector3<int>((int)floor(ba.x), (int)floor(ba.y), (int)floor(ba.z)), true)) {
				SoundManager::playSound("Destrueix", ba, true);
			}
		}
		else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) { //Botó de la rodeta, canviar d'objecte
			btipus = (btipus + 1) % NBLOCS;
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) { //Botó dret, col·locar bloc / interactuar
			Vector3 front = Vector3<float>::normalize(camera.getFront());
			Bloc tipusbloc = static_cast<Bloc>(btipus);
			if (bp == Vector3<float>(FLT_MIN, FLT_MIN, FLT_MIN)) {
				return;
			}
			Bloc blocApuntat = world->getBlock(bp);
			if (!Block::isSolid(blocApuntat) && tipusbloc != Bloc::RES) { //Només deixam posar un bloc si no n'hi ha un ja
				Vector3<float> epos;
				if (ent != nullptr) {
					epos = ent->getPos() - Vector3<float>(0,1,0);
					//epos.floor();
				}
				if (ent == nullptr || epos != bp) {
					world->setBlock(tipusbloc, Vector3<int>((int)floor(bp.x), (int)floor(bp.y), (int)floor(bp.z)));
					SoundManager::playSound("Coloca", bp, true);
				}
			}
			else if (Block::isInteractable(world->getBlock(ba))){
				
				world->interact(Vector3<int>((int)floor(ba.x), (int)floor(ba.y), (int)floor(ba.z))); //Si no tenim cap bloc seleccionat, interactuam
			}
		}
	}
}

//Control amb teclat
void movement(int key) {
	if (ent != 0) {
		ent->control(key);
	}

	if (act == Active::MAPA) {
		if (key == GLFW_KEY_W) {
			mapY++;
			printf("mapY: %d\n", mapY);
		}
		else if (key == GLFW_KEY_S) {
			mapY--;
			printf("mapY: %d\n", mapY);
		}
	}
	
	if (key == GLFW_KEY_F) {
		llanterna = !llanterna;
	}
	else if (key == GLFW_KEY_F3) {
		drawfps = !drawfps;
	}
	else if (key == GLFW_KEY_C) { //Switch Marching Cubes ON/OFF
		Block::setMCEnabled(!Block::getMCEnabled());
		world->redrawChunks();
	}
	else if (key == GLFW_KEY_P) {
		printf("x:%f y:%f z:%f b:%d r:%f\n", ent->getPos().x, ent->getPos().y, ent->getPos().z, world->getBlock(ba), ent->getRot());
		world->updateNeighborChunks(world->getChunkPos(ent->getPos().toInt()), Vector3<int>(0,0,0));
	}
	else if (key == GLFW_KEY_TAB) { //Tab: obrir inventari
		act = Active::INVENTARI;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	else if (key == GLFW_KEY_E) { //Attach a una entitat propera
		//if (ent != NULL) { //Si controlam una entitat, la deixam
		//	ent->onDeattach();
		//	ent = NULL;
		//	camera.setFreeLook(true);
		//	camera.setFreeMove(true);
		//}
		//else { //Si no, en cercam una de propera i la controlam
		//	ent = world->getNearestEntity(camera.getPos(), 4.0f, true);
		//	if (ent != 0) {
		//		camera.setFreeLook(false);
		//		camera.setFreeMove(false);
		//		ent->onAttach();
		//	}
		//}
	}
	else if (key == GLFW_KEY_T) {
		world->save();
	}
	else if (key == GLFW_KEY_M) { //Mapa biomes
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		act = Active::MAPA;

	}
	else if (key == GLFW_KEY_KP_ADD) {
		mapMult++;
	}
	else if (key == GLFW_KEY_KP_SUBTRACT) {
		mapMult--;
	}
	else if (key == GLFW_KEY_ESCAPE) {
		act = Active::JOC;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		updateMousePos(); //Perquè la càmera no es mogui violentament si el ratoli s'ha separat molt
	}
	mapMult = max(mapMult, 0.5f);
}

//Control amb la rodeta del ratolí
void scaleListener(GLFWwindow* window, double xoffset, double yoffset) {
	if (KeyboardManager::isPressed('q')) { //Permetem fer la llanterna més grossa o petita
		float cutoff;
		glGetLightfv(GL_LIGHT0, GL_SPOT_CUTOFF, &cutoff);
		cutoff += (float)yoffset;
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, cutoff);
	}
	else { //Feim zoom
		if (act == Active::MAPA) {
			mapMult -= (float)yoffset*0.2f;
			mapMult = max(mapMult, 0.5f);
			return;
		}

		camera.zoom((float)yoffset * 0.5f);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(camera.getFov(), (float)w_width / (float)w_height, zNear, zFar);
	}	
}

/**
  * Funció que es crida quan es produeix un resize del window
  * width és la nova amplada
  * height és la nova altura
  */
void onWindowResize(GLFWwindow* window, int width, int height) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float aspect = (float)width / (float)height;
	camera.setAspect(aspect); //Actualització de l'aspecte
	camera.updateCorners();


	gluPerspective(camera.getFov(), aspect, zNear, zFar); 
	w_width = width;
	w_height = height;

	glViewport(0, 0, width, height); // indicar la nova mida de la finestra
}


void onKeyboardDown(unsigned char key, int x, int y) {
	if (key == GLFW_KEY_UNKNOWN) {
		return;
	}
	KeyboardManager::onKeyDown(key);
}

void onKeyboardUp(unsigned char key, int x, int y) {
	if (key == GLFW_KEY_UNKNOWN) {
		return;
	}
	KeyboardManager::onKeyUp(key);
}

void toggleAxis() { //Axis i trajectòria càmera ON/OFF
	axisVisible = !axisVisible;
}

/**
  * Estableix l'il·luminació i boira de l'escena
  */
void setLighting() {
	for (int i = GL_LIGHT0; i <= GL_LIGHT7; i++) {
		glDisable(i); //Desactivam totes les llums
	}

	//Establim la boira
	glFogf(GL_FOG_DENSITY, 0.01f);
	glFogf(GL_FOG_MODE, GL_EXP);
	glFogf(GL_FOG_END, zFar);
	glFogf(GL_FOG_START, zFar - 4);

	//LLum ambient
	GLfloat lluma[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lluma); //Posar a 0.1

	//Llanterna
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 1.0f);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0f);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1f);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.1f);
	float diff[4] = { 1,1,1,1 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);

	//Sol
	//world->setSol(GL_LIGHT1); //Triam quina llum farà de sol (llum direccional) DESACTIVAT: Ara llum ambient
}

//Hook pel moviment del ratolí
void lookAround(GLFWwindow* window, double x, double y) {
	if (act == Active::JOC) {
		camera.lookAround(x, y, lastX, lastY);

		updatePlayerBlock();

		updateMousePos();
	}
}

//Centra el ratolí enmig de la finestra, per evitar que s'escapi
void updateMousePos() {
	glfwGetCursorPos(window, &lastX, &lastY);
}

//Actualitzam el bloc seleccionat (posició) segons on miri el jugador
void updatePlayerBlock() {
	Vector3<float> front = camera.getFront();
	Vector3<float> bpN = camera.getPos();
	Vector3<float> baN = camera.getPos() + front;
	int i = 0;
	while (!Block::isInteractable(world->getBlock(baN)) && i < 100) { //Traçam una línia cap a la direcció del front de la càmera
		bpN = baN;
		baN = baN + front * 0.1f;
		i++;
	}
	if (i == 100) { //Si no hem trobat cap bloc, no es veurà la selecció
		baN = Vector3<float>(FLT_MIN, FLT_MIN, FLT_MIN);
		bpN = Vector3<float>(FLT_MIN, FLT_MIN, FLT_MIN);
	}
	ba = baN;
	bp = bpN;
}

void onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS && act == Active::JOC) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (action == GLFW_PRESS) {
		KeyboardManager::onKeyDown(key);
	}
	else if (action == GLFW_RELEASE) {
		KeyboardManager::onKeyUp(key);
	}
}