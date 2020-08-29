// Fichero principal 
////////////////////////////////////////////////////

#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Utils.h"
#include "Vector3.h"
#include "TextureManager.h"
#include "SoundManager.h"
#include "ModelManager.h"

int w_width = 500; // Tamano incial de la ventana
int w_height = 500;

//Clipping planes
const float zNear = 1.0f;
float zFar = 48.0f;

const float axisSize = zFar;

Vector3 ba; //Bloc actual (posició)

int btipus = 1; //Bloc actual (tipus)

#include "KeyboardManager.h"

#include "Camera.h"
Camera camera = Camera(Vector3(64, 66, 64), Vector3(64, 66, 60));

#include "World.h"
World* world;

int darrerIdle = 0, darrerDisplay = 0; //Control del temps

int lastX = 0, lastY = 0; //Darreres posicions del ratolí

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

void onWindowResize(int width, int height);
void onKeyboardUp(unsigned char key, int x, int y);
void onKeyboardDown(unsigned char key, int x, int y);
void toggleAxis();
void changeScene(unsigned char sceneNumber);
void scaleListener(int wheel, int direction, int x, int y);
void mouseListener(int button, int state, int x, int y);
void lookAround(int x, int y);
void movement(unsigned char key);
void centerPointer();
void updatePlayerBlock();


// Funcion que visualiza la escena OpenGL
void Display(void)
{ 
	int temps = glutGet(GLUT_ELAPSED_TIME);
	int delta = temps - darrerDisplay;
	float fps = 1.0f / ((float)delta / 1000.0f);
	printf("%f\n", fps);
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
	glDisable(GL_LIGHTING); //El volem veure sempre
	glLineWidth(5.0f);
	world->drawBloc(ba, Bloc::CUB, true);
	glLineWidth(1.0f);
	glEnable(GL_LIGHTING); //El volem veure sempre

	world->drawSol(camera.getPos(), zFar); //Dibuixam el sol

	// dibuixar els 3 eixos
	if (axisVisible) {
		world->drawAxis(Vector3((float)(world->sizex*16)/2, (float)(world->sizey*16) / 2 + 1, (float)(world->sizez*16) / 2), 50.0f);
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
		for (int i = 0; i < 23; i++) { //Objectes de l'inventari
			glPushMatrix();
			glScalef(0.1f, 0.1f, 0.1f);
			Block bsel = Block(NULL, static_cast<Bloc>(i+1), NULL);
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

	// intercanviar els buffers
	glutSwapBuffers();
}

// Funcion que se ejecuta cuando el sistema no esta ocupado
void Idle(void)
{
	//Gestió del temps
	int temps = glutGet(GLUT_ELAPSED_TIME);
	int delta = temps - darrerIdle;
	darrerIdle = temps;

	//Movem la càmera en la direcció que toqui
	if (KeyboardManager::isPressed('w')) {
		camera.move(Camera::direction::ENVANT, delta);
		updatePlayerBlock();
	}
	else if (KeyboardManager::isPressed('s')) {
		camera.move(Camera::direction::ENRERE, delta);
		updatePlayerBlock();
	}
	if (KeyboardManager::isPressed('d')) {
		camera.move(Camera::direction::DRETA, delta);
		updatePlayerBlock();
	}
	else if (KeyboardManager::isPressed('a')) {
		camera.move(Camera::direction::ESQUERRA, delta);
		updatePlayerBlock();
	}

	//Actualitzam el món
	world->update(delta, camera.getPos());

	//Si tenim una entitat controlada
	if (ent != 0) {
		ent->control(delta); //Actualitzam el seu estat intern
		Vector3 pos = ent->getPos();
		alListener3f(AL_POSITION, pos.x, pos.y, pos.z); //Actualitzam la posició de l'escoltador a l'entitat
	}


	// Indicamos que es necesario repintar la pantalla
	glutPostRedisplay();
}

// Funcion principal
int main(int argc, char** argv)
{

	// necessari inicialitzar el món dins el main (físiques)
	world = new World(12,12,12, &camera);
	//cotxe = Car(world, Vector3(66, 65, 66));

	// Inicializamos la libreria GLUT
	glutInit(&argc, argv);

	// Indicamos como ha de ser la nueva ventana
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(w_width, w_height);
	glutSetOption(GLUT_MULTISAMPLE, 8);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

	// Creamos la nueva ventana
	glutCreateWindow("Etapa 6");
	glutSetCursor(GLUT_CURSOR_NONE);

	//Inicialització Glew
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		printf("glewInit error: %s \n", glewGetErrorString(err));
		exit(1);
	}

	// indicar l'escoltador del canvi de mida
	// és important cridar aquesta funció després de crear la finestra
	glutReshapeFunc(&onWindowResize);

	// processar les entrades per reclat
	glutKeyboardFunc(&onKeyboardDown);
	glutKeyboardUpFunc(&onKeyboardUp);

	//I per ratoli
	glutPassiveMotionFunc(&lookAround);
	glutMouseWheelFunc(&scaleListener);
	glutMouseFunc(&mouseListener);


	// Indicamos cuales son las funciones de redibujado e idle
	glutDisplayFunc(Display);
	glutIdleFunc(Idle);

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

	gluPerspective(camera.getFov() , 1, 1, zFar);
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
	world->setBlock(Bloc::CONO, Vector3(66, 65, 64));

	//Entitats
	//world->addEntity(Entitat::COTXE, Vector3(70, 65, 66));
	//world->addEntity(Entitat::COTXE2, Vector3(70, 65, 60));
	//world->setBlock(Bloc::GRUA, Vector3(60, 65, 66));

	glEnable(GL_CULL_FACE); //Cull face, no renderitza les cares no visibles
	glCullFace(GL_BACK);

	//Textures
	glEnable(GL_TEXTURE_2D); //Activació
	TextureManager::LoadTexture("Textures/terra.png", Textura::TERRA); //Càrrega
	TextureManager::LoadTexture("Textures/fusta.jpg", Textura::FUSTA);
	TextureManager::LoadTexture("Textures/pedra.png", Textura::PEDRA);
	TextureManager::LoadTexture("Textures/altaveu.jpg", Textura::ALTAVEU);
	TextureManager::LoadTexture("Textures/estalagmita.jfif", Textura::ESTALAGMITA);

	//Sons
	SoundManager::initialize();
	SoundManager::loadSound("Sons/acc.wav", So::ACCELERA); //Càrrega
	SoundManager::loadSound("Sons/jukebox.wav", So::MUSICA);
	SoundManager::loadSound("Sons/switch.wav", So::ONOFF);
	SoundManager::loadSound("Sons/break.wav", So::DESTRUEIX);
	SoundManager::loadSound("Sons/place.wav", So::COLOCA);

	//Models
	ModelManager::initialize();

	//Antialising
	glEnable(GL_MULTISAMPLE);
	glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);

	//Framebuffer
	glEnable(GL_FRAMEBUFFER);
	glEnable(GL_FRAMEBUFFER_SRGB);

	changeScene('1');

	// Comienza la ejecucion del core de GLUT
	glutMainLoop();
	return 0;
}

//Control amb botons del ratolí
void mouseListener(int button, int state, int x, int y) {
	if (inv) { //Interacció amb l'inventari
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) { 
			float maxy = (float)w_height * 0.85f;
			float miny = (float)w_height * 0.15f;
			float maxx = (float)w_width / 2.0f + ((float)w_height/2.5f)*0.90f;
			float minx = (float)w_width / 2.0f - ((float)w_height / 2.5f) * 0.90f;

			float yi = ((float)y - miny) / (maxy-miny) * 6;// *7.02f;
			float xi = ((float)x - minx) / (maxx - minx) * 6;// *7.1f;

			btipus = (int)floor(yi) * 6 +  (int)floor(xi) + 1;
			if (btipus > 23) { //Si no se selecciona cap objecte, no n'hi haurà cap de seleccionat
				btipus = 0;
			}
		}
	}
	else {
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) { //Botó dret, eliminar blocs
			if (world->deleteBlock(ba, true)) {
				SoundManager::playSound(So::DESTRUEIX, ba, true);
			}
		}
		else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) { //Botó de la rodeta, canviar d'objecte
			btipus = (btipus + 1) % 22;
		}
		else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) { //Botó dret, col·locar bloc / interactuar
			Vector3 front = camera.getFront();
			Bloc tipusbloc = static_cast<Bloc>(btipus);
			if (world->getBlock(ba - front * 0.1f) == Bloc::RES && tipusbloc != Bloc::AIRE) { //Només deixam posar un bloc si no n'hi ha un ja
				world->setBlock(tipusbloc, ba - front * 0.1f);
				SoundManager::playSound(So::COLOCA, ba, true);
			}
			else {
				world->interact(ba); //Si no tenim cap bloc seleccionat, interactuam
			}
		}
	}
}

//Control amb teclat
void movement(unsigned char key) {
	if (ent != 0) {
		ent->control(key);
	}
	else if (key == 'f' || key == 'F') {
		llanterna = !llanterna;
		if (llanterna) {
			glEnable(GL_LIGHT0);
		}
		else {
			glDisable(GL_LIGHT0);
		}
	}
	else if (key == '\t') { //Tab: obrir inventari
		inv = !inv;
		if (inv) {
			glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
		}
		else {
			glutSetCursor(GLUT_CURSOR_NONE);
		}
	}
	else if (key == ' ') { //Canvi de model d'ombrejat
		smoothlight = !smoothlight;
		if (smoothlight) {
			glShadeModel(GL_FLAT);
		}
		else {
			glShadeModel(GL_SMOOTH);
		}
	}

	if (key == 'E' || key == 'e') { //Attach a una entitat propera
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
}

//Control amb la rodeta del ratolí
void scaleListener(int wheel, int direction, int x, int y) {
	if (KeyboardManager::isPressed('q')) { //Permetem fer la llanterna més grossa o petita
		float cutoff;
		glGetLightfv(GL_LIGHT0, GL_SPOT_CUTOFF, &cutoff);
		cutoff += direction;
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, cutoff);
	}
	else { //Feim zoom
		camera.zoom(direction * 0.5f);
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
void onWindowResize(int width, int height) {
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
			glFogf(GL_FOG_DENSITY, 0.03f);
			glFogf(GL_FOG_MODE, GL_EXP2);
			glFogf(GL_FOG_END, zFar - 1);

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

			//Càmera
			camera.setPos(Vector3(64, 66, 64));
			camera.setFreeLook(true);
			camera.setFreeMove(true);
			camera.setDrawMove(true);
		}
	}
}

//Hook pel moviment del ratolí
void lookAround(int x, int y) {
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
	int posx = w_width / 2;
	int posy = w_height / 2;
	glutWarpPointer(posx, posy);
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