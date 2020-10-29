// Fichero principal 
////////////////////////////////////////////////////

#include <iostream>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
//#include <GL/freeglut.h>
#include "Utils.h"
#include "Vector3.h"
#include "TextureManager.h"
#include "SoundManager.h"
#include "ModelManager.h"
#include "Entities/Player.h"
#include <sys/stat.h>

int w_width = 500; // Tamano incial de la ventana
int w_height = 500;
GLFWwindow* window;

//Clipping planes
const float zNear = 0.001f;
float zFar = 64.0f;

const float axisSize = zFar;

Vector3 ba; //Bloc actual (posició)

int btipus = 1; //Bloc actual (tipus)

#include "KeyboardManager.h"

#include "Camera.h"
Camera camera = Camera(Vector3(64, 66, 64), Vector3(64, 66, 60));

#include "World.h"
World* world;

int darrerIdle = 0, darrerDisplay = 0; //Control del temps

double lastX = 250, lastY = 250; //Darreres posicions del ratolí

//luces
bool llanterna = false;
bool smoothlight = true;

//Indica si l'inventari està obert
bool inv = false;

//Entitats
#include "Entities/Entity.h"
Entity* ent; //Entitat controlada

bool axisVisible = true; //Eixos ON/OFF
int numScene = 0; //Escena per defecte

//Nom del món
std::string wname;
int updTimer = 500; //500 ms = 20tps

void onWindowResize(GLFWwindow* window, int width, int height);
void onKeyboardUp(unsigned char key, int x, int y);
void onKeyboardDown(unsigned char key, int x, int y);
void toggleAxis();
void changeScene(unsigned char sceneNumber);
void scaleListener(GLFWwindow* window, double xoffset, double yoffset);
void mouseListener(GLFWwindow* window, int button, int action, int mods);
void lookAround(GLFWwindow* window, double x, double y);
void movement(int key);
void centerPointer();
void updatePlayerBlock();
void onKey(GLFWwindow* window, int key, int scancode, int action, int mods);

int fpsc = 0;

// Funcion que visualiza la escena OpenGL
void Display(GLFWwindow* window)
{ 
	int temps = glfwGetTime() * 1000;
	//printf("%d\n", temps);
	int delta = temps - darrerDisplay;
	float fps = 1.0f / ((float)delta / 1000.0f);

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

	glBindFramebuffer(GL_FRAMEBUFFER, 0); //Ens asseguram que no esteim dibuixant a cap framebuffer
	glBindTexture(GL_TEXTURE_2D, 0); //I que no tenim cap textura seleccionada
	glViewport(0, 0, w_width, w_height); //Establim el viewport

	// borrar l'escena pintada
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// guardar la matriu abans d'aplicar rotacions i translacions
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(camera.getFov(), camera.getAspect() , zNear, zFar);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

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
	}
	
	//Transformam la matriu de visualització per mirar on toqui
	camera.display();

	glEnable(GL_LIGHTING); //Ens asseguram que l'il·luminació està activada

	world->draw(camera.getPos(), zFar); //Dibuixam el món

	//Actualitzam les llums del món
	world->updateLights(camera.getPos(), Vector3::normalize(camera.getFront()), camera.getFov(), camera.getAspect());

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

	world->drawSol(camera.getPos(), zFar-1); //Dibuixam el sol

	// dibuixar els 3 eixos
	if (axisVisible) {
		world->drawAxis(Vector3((float)(world->sizex*CHUNKSIZE)/2, (float)(world->sizey*CHUNKSIZE) / 2 + 1, (float)(world->sizez*CHUNKSIZE) / 2), 50.0f);
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

	glPushMatrix();

	//Dibuixam el bloc seleccionat al cantó esquerra inferior
	Block bsel = Block(NULL, static_cast<Bloc>(btipus), NULL);
	glTranslatef(0.9f * camera.getAspect(), 0.1f, -2);
	glScalef(0.1f, 0.1f, 0.1f);
	bsel.draw();

	glPopMatrix();

	if (inv) { //Dibuixam inventari
		glPushMatrix();
		float aspect = camera.getAspect();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);

		glColor3f(1,1,1);
		glBegin(GL_QUADS); //Quadrat blanc exterior
		glVertex3f(0.5f * aspect - 0.4f, 0.9f, -1);
		glVertex3f(0.5f * aspect - 0.4f, 0.1f, -1);
		glVertex3f(0.5f * aspect + 0.4f, 0.1f, -1);
		glVertex3f(0.5f * aspect + 0.4f, 0.9f, -1);
		glEnd();

		glColor4f(0,0,0,1);
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
		glTranslatef(0.5f * aspect - 0.3f, 0.8f, -1);
		for (int i = 0; i < 27; i++) { //Objectes de l'inventari
			glPushMatrix();
			glScalef(0.1f, 0.1f, 0.1f);
			Block bsel = Block(NULL, static_cast<Bloc>(i+2), NULL); //+2 perque botam aire i null
			glDisable(GL_LIGHTING);
			bsel.draw();
			glPopMatrix();

			glTranslatef(0.12f, 0, 0); //Passam a la següent columna
			if ((i+1) % 6 == 0) { //I, si cal, a la següent fila
				glTranslatef(0, -0.12f, 0);
				glTranslatef(-0.12f*6, 0, 0);
			}
		}
		glPopMatrix();
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

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	//Fi dibuix 2D

	// recuperar la matriu original pel pròxim display
	glPopMatrix();

	glFlush();
}

// Funcion que se ejecuta cuando el sistema no esta ocupado
void Idle(void)
{
	//Solució temporal shift i ctrl, fins que empri glfw o algo així
	if (GetKeyState(VK_SHIFT) & 0x8000) {
		KeyboardManager::onKeyDown('{');
	}
	else {
		KeyboardManager::onKeyUp('{');
	}
	if (GetKeyState(VK_CONTROL) & 0x8000) {
		KeyboardManager::onKeyDown('}');
	}
	else {
		KeyboardManager::onKeyUp('}');
	}

	//Gestió del temps
	//int temps = glutGet(GLUT_ELAPSED_TIME);
	int temps = glfwGetTime() * 1000;
	int delta = temps - darrerIdle;
	darrerIdle = temps;

	updatePlayerBlock();


	//Actualitzam el món
	world->update(delta, camera.getPos());

	if (ent != nullptr) { //TEMPORAL: se n'ha d'encarregar el món
		ent->update(delta);
	}

	//Si tenim una entitat controlada
	if (ent != 0) {
		ent->control(delta, &camera); //Actualitzam el seu estat intern
		Vector3 pos = ent->getPos();
		alListener3f(AL_POSITION, pos.x, pos.y, pos.z); //Actualitzam la posició de l'escoltador a l'entitat
	}
}

// Funcion principal
int main(int argc, char** argv)
{

	// necessari inicialitzar el món dins el main (físiques)
	//Càmera
	camera.setPos(Vector3(64, 66, 64));
	camera.setFreeLook(true);
	camera.setFreeMove(true);
	camera.setDrawMove(true);
	//world = new World(16,16,16, &camera);
	printf("World name: ");
	std::cin >> wname;
	std::string path = "worlds/" + wname;
	struct stat buffer;
	if (stat(path.c_str(), &buffer) == 0) {
		//Si existeix
		printf("Loading world %s... \n", wname);
		world = new World(wname, &camera);
		ent = new Player(world, world->getSpawn() + Vector3(0, 2, 0));
		//printf("with spawn at %f %f %f\n", world->getSpawn().x, world->getSpawn().y, world->getSpawn().z);
		//cotxe = Car(world, Vector3(66, 65, 66));
	}
	else {
		printf("Creating world %s... \n", wname);
		printf("Seed: ");
		std::string sseed;
		std::cin >> sseed;
		int seed = 0;
		for (int i = 0; i < sseed.length(); i++) {
			seed += sseed[i];
		}
		//int seed = std::atoi(sseed.c_str());
		printf("Seed: %d\n", seed);
		world = new World(seed, 24, 16, 24, &camera);
		ent = new Player(world, world->getSpawn() + Vector3(0, 10, 0));
		world->save(wname);
	}
	// Inicializamos la libreria GLUT
	//glutInit(&argc, argv);
	glfwInit();

	// Indicamos como ha de ser la nueva ventana
	//glutInitWindowPosition(100, 100);
	//glutInitWindowSize(w_width, w_height);
	//glutSetOption(GLUT_MULTISAMPLE, 8);
	//glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

	// Creamos la nueva ventana
	//glutInitContextVersion(3, 1);
	//glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
	//glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	//glutCreateWindow("Etapa 6");
	//glutSetCursor(GLUT_CURSOR_NONE);

	window = glfwCreateWindow(w_width, w_height, "AlphaCraft", NULL, NULL);
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

	// indicar l'escoltador del canvi de mida
	// és important cridar aquesta funció després de crear la finestra
	//glutReshapeFunc(&onWindowResize);

	// processar les entrades per reclat
	//glutKeyboardFunc(&onKeyboardDown);
	//glutKeyboardUpFunc(&onKeyboardUp);

	//I per ratoli
	//glutPassiveMotionFunc(&lookAround);
	//glutMouseWheelFunc(&scaleListener);
	//glutMouseFunc(&mouseListener);
	glfwSetScrollCallback(window, scaleListener);
	glfwSetCursorPosCallback(window, lookAround);
	glfwSetMouseButtonCallback(window, mouseListener);


	// Indicamos cuales son las funciones de redibujado e idle
	//glutDisplayFunc(Display);
	//glutIdleFunc(Idle);

	glEnable(GL_DEPTH_TEST); //Activam la profunditat

	glEnable(GL_LIGHTING); //Il·luminació
	glEnable(GL_COLOR_MATERIAL); //Perquè els objectes reflecteixin el color
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	float specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular); //Especular per defecte
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 1); //Brillantor
	glShadeModel(GL_SMOOTH); //Model d'ombrejat

	glEnable(GL_BLEND); //Activam transparència
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// El color de fondo sera el negro (RGBA, RGB + Alpha channel)
	glClearColor(0,0,0, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(camera.getFov() , 1, zNear, zFar);
	camera.setViewDist(zFar); //Establim la distància de visió de la càmera

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Activam la boira
	glEnable(GL_FOG);
	float fogcolor[4] = { 0,0,0.1f,0.1f};
	glFogfv(GL_FOG_COLOR, fogcolor);

	// altres configuracions
	KeyboardManager::addKeyHandler('x', &toggleAxis);
	KeyboardManager::addKeyHandler(&movement);

	//Blocs món per defecte:
	world->setBlock(Bloc::CUB, Vector3(64, 65, 64));
	world->setBlock(Bloc::CUB, Vector3(65, 65, 64));

	//Entitats
	//world->addEntity(Entitat::COTXE, Vector3(70, 65, 66));
	//world->addEntity(Entitat::COTXE2, Vector3(70, 65, 60));
	//world->setBlock(Bloc::GRUA, Vector3(60, 65, 66));

	glEnable(GL_CULL_FACE); //Cull face, no renderitza les cares no visibles
	glCullFace(GL_BACK);

	//Textures
	glEnable(GL_TEXTURE_2D); //Activació
	//TextureManager::LoadTexture("Textures/64pTest/terra.png", Textura::TERRA); //Càrrega
	//TextureManager::LoadTexture("Textures/64pTest/fusta.jpg", Textura::FUSTA);
	//TextureManager::LoadTexture("Textures/64pTest/pedra.png", Textura::PEDRA);
	//TextureManager::LoadTexture("Textures/64pTest/altaveu.jpg", Textura::ALTAVEU);
	//TextureManager::LoadTexture("Textures/64pTest/estalagmita.jfif", Textura::ESTALAGMITA);
	//TextureManager::LoadTexture("Textures/64pTest/fustaarbre.jpg", Textura::FUSTAARBRE);
	//TextureManager::LoadTexture("Textures/64pTest/fullaarbre.png", Textura::FULLAARBRE);
	TextureManager::LoadTexture("Textures/texture.jpg", Textura::BLOC);

	//Sons
	SoundManager::initialize();
	SoundManager::loadSound("Sons/acc.wav", So::ACCELERA); //Càrrega
	SoundManager::loadSound("Sons/jukebox.wav", So::MUSICA);
	SoundManager::loadSound("Sons/switch.wav", So::ONOFF);
	SoundManager::loadSound("Sons/break.wav", So::DESTRUEIX);
	SoundManager::loadSound("Sons/place.wav", So::COLOCA);
	SoundManager::loadSound("Sons/passes/mud02.wav", So::CAMINA);

	//Models
	ModelManager::initialize();

	//Antialising
	glfwWindowHint(GLFW_SAMPLES, 4);
	glEnable(GL_MULTISAMPLE);

	//Framebuffer
	glEnable(GL_FRAMEBUFFER);
	glEnable(GL_FRAMEBUFFER_SRGB);

	changeScene('1');

	// Comienza la ejecucion del core de GLUT
	while (!glfwWindowShouldClose(window))
	{
		Display(window);
		Idle();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

//Control amb botons del ratolí
void mouseListener(GLFWwindow* window, int button, int action, int mods) {
	if (inv) { //Interacció amb l'inventari
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

			btipus = (int)floor(yi) * 6 +  (int)floor(xi) + 2; //+2 per botar aire i res
			if (btipus > 27) { //Si no se selecciona cap objecte, no n'hi haurà cap de seleccionat
				btipus = 0;
			}
		}
	}
	else {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) { //Botó dret, eliminar blocs
			if (world->deleteBlock(ba, true)) {
				SoundManager::playSound(So::DESTRUEIX, ba, true);
			}
		}
		else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) { //Botó de la rodeta, canviar d'objecte
			btipus = (btipus + 1) % 22;
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) { //Botó dret, col·locar bloc / interactuar
			Vector3 front = camera.getFront();
			Bloc tipusbloc = static_cast<Bloc>(btipus);
			if (world->getBlock(ba - front * 0.1f) == Bloc::RES && tipusbloc != Bloc::RES) { //Només deixam posar un bloc si no n'hi ha un ja
				Vector3 bpos = ba - front * 0.1f;
				Vector3 epos;
				if (ent != nullptr) {
					epos = ent->getPos() - Vector3(0,1,0);
					epos.floor();
				}
				bpos.floor();
				if (ent == nullptr || epos != bpos) {
					world->setBlock(tipusbloc, bpos);
					SoundManager::playSound(So::COLOCA, ba, true);
				}
			}
			else {
				world->interact(ba); //Si no tenim cap bloc seleccionat, interactuam
			}
		}
	}
}

//Control amb teclat
void movement(int key) {
	if (ent != 0) {
		ent->control(key);
	}
	if (key == GLFW_KEY_F) {
		llanterna = !llanterna;
		if (llanterna) {
			glEnable(GL_LIGHT0);
		}
		else {
			glDisable(GL_LIGHT0);
		}
	}
	if (key == GLFW_KEY_P) {
		printf("%f %f %f \n", ent->getPos().x, ent->getPos().y, ent->getPos().z);
	}
	if (key == GLFW_KEY_TAB) { //Tab: obrir inventari
		inv = !inv;
		if (inv) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}

	if (key == GLFW_KEY_E) { //Attach a una entitat propera
		if (ent != NULL) { //Si controlam una entitat, la deixam
			ent->onDeattach();
			ent = NULL;
			camera.setFreeLook(true);
			camera.setFreeMove(true);
		}
		else { //Si no, en cercam una de propera i la controlam
			ent = world->getNearestEntity(camera.getPos(), 4.0f, true);
			if (ent != 0) {
				camera.setFreeLook(false);
				camera.setFreeMove(false);
				ent->onAttach();
			}
		}
	}

	if (key == GLFW_KEY_T) {
		world->save(wname);
	}
}

//Control amb la rodeta del ratolí
void scaleListener(GLFWwindow* window, double xoffset, double yoffset) {
	if (KeyboardManager::isPressed('q')) { //Permetem fer la llanterna més grossa o petita
		float cutoff;
		glGetLightfv(GL_LIGHT0, GL_SPOT_CUTOFF, &cutoff);
		cutoff += yoffset;
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, cutoff);
	}
	else { //Feim zoom
		camera.zoom(yoffset * 0.5f);
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
	KeyboardManager::onKeyDown(key);
}

void onKeyboardUp(unsigned char key, int x, int y) {
	KeyboardManager::onKeyUp(key);
}

void toggleAxis() { //Axis i trajectòria càmera ON/OFF
	axisVisible = !axisVisible;
	camera.setDrawMove(!camera.getDrawMove());
}

/**
  * Canvia l'escena en funció de la tecla pitjada
  */
void changeScene(unsigned char sceneNumber) {
	int scene = (int)(sceneNumber - '0');
	scene = scene - 1;

	if (scene >= 0) {
		for (int i = GL_LIGHT0; i <= GL_LIGHT7; i++) {
			glDisable(i); //Desactivam totes les llums
		}

		numScene = scene;
		if (numScene == 0) {
			//Establim la boira
			glFogf(GL_FOG_DENSITY, 0.02f);
			glFogf(GL_FOG_MODE, GL_EXP2);
			glFogf(GL_FOG_END, zFar);
			glFogf(GL_FOG_START, zFar-4);

			//LLum ambient
			GLfloat lluma[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lluma);

			//Llanterna
			glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 1.0f);
			glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.0f);
			glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1f);
			glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.1f);

			//Sol
			world->setSol(GL_LIGHT1); //Triam quina llum farà de sol (llum direccional)

			std::cout << "Scene is " << scene << std::endl;
		}
	}
}

//Hook pel moviment del ratolí
void lookAround(GLFWwindow* window, double x, double y) {
	if (!inv) {
		camera.lookAround(x, y, lastX, lastY);

		updatePlayerBlock();

		//Intentam evitar que el ratoli surti de la finestra
		if (x != (w_width / 2) || y != (w_height / 2)) {
			centerPointer();
		}
	}
}

//Centra el ratolí enmig de la finestra, per evitar que s'escapi
void centerPointer() {
	double posx = w_width / 2;
	double posy = w_height / 2;
	glfwGetCursorPos(window, &posx, &posy);
	//glutWarpPointer(posx, posy);
	lastX = posx;
	lastY = posy;
}

//Actualitzam el bloc seleccionat (posició) segons on miri el jugador
void updatePlayerBlock() {
	ba = camera.getPos() + camera.getFront();
	int i = 0;
	while (world->getBlock(ba) == Bloc::RES && i < 100) { //Traçam una línia cap a la direcció del front de la càmera
		ba = ba + camera.getFront() * 0.1f;
		i++;
	}
	if (i == 100) { //Si no hem trobat cap bloc, no es veurà la selecció
		ba = Vector3(-1, -1, -1);
	}
}

void onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (action == GLFW_PRESS) {
		KeyboardManager::onKeyDown(key);
	}
	else if (action == GLFW_RELEASE) {
		KeyboardManager::onKeyUp(key);
	}
}