#include<gl/glew.h>
#include<GL/freeglut.h>
#include<iostream>
#include<string>
#include<vector>
#include "stb_image/stb_image.h"

bool* keyStates = new bool[2];
bool started = false;
unsigned int gunTex, bulletTex, balloonText;
unsigned int balloonTex[6];
GLfloat posx = 0.0f, posy = 0.0f;
GLfloat gunx = 210.0f, guny = 0.0f;
unsigned int maxBullet = 10;
unsigned int maxBalloon = 30;
GLfloat width = 800.0, height = 700.0;
unsigned int missedBalloon;

// declaration for the baalloonBurt function
void balloonBurst(int i);

template<typename tVal> tVal map_value(std::pair<tVal, tVal> a, std::pair<tVal, tVal> b, tVal inVal) { 
	tVal inValNorm = inVal - a.first;  
	tVal aUpperNorm = a.second - a.first;  
	tVal normPosition = inValNorm / aUpperNorm;  
	tVal bUpperNorm = b.second - b.first;  
	tVal bValNorm = normPosition * bUpperNorm;   
	tVal outVal = b.first + bValNorm;     
	return outVal;
}

// texture mapping
unsigned int texMapping(const std::string& path)
{
	unsigned int texName;
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* image = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);
	if (image) {
		glGenTextures(1, &texName); // specifies the no. of texture names stored in the following texture array

		glBindTexture(GL_TEXTURE_2D, texName); // specifying the actual binding of the texture should be in 2D

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // the texture wraps around the edges repeatedly along the x direction
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // the texture wraps around the edges repeatedly along y direction
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // when the texture area is large, repeat texel nearest center
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //when the texture area is small, repeat texel nearest center

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(image);

	}
	else {
		std::cout << "failed to load texture";
	}
	return texName;
}

// texture binding the gun
void gun()
{
	glTranslatef(posx, posy, 0.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gunTex);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0);     glVertex3f(210.0, -20.0, 0.0);
	glTexCoord2f(0.0, 1.0);     glVertex3f(210.0, 20.0, 0.0);
	glTexCoord2f(1.0, 1.0);     glVertex3f(250.0, 20.0, 0.0);
	glTexCoord2f(1.0, 0.0);     glVertex3f(250.0, -20.0, 0.0);
	glEnd();
	glDisable(GL_TEXTURE_2D);

}


// balloon function
class Balloon {
public:float bx, by;
	  bool hit;
	  bool animated;
	  bool animCalled;
	  unsigned int z;
public: Balloon(float x = -230.0, float y = -250.0) {
	bx = x;
	by = y;
	hit = false;
	animCalled = false;
	animated = false;
	z = 0;
}
public:
	void draw() {
		if (!hit && by < 250) { 
			// texture binding the balloon
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, balloonText);
			glBegin(GL_POLYGON);
			glTexCoord2f(0.0, 0.0);	glVertex3f(bx, by, 0.0);
			glTexCoord2f(0.0, 1.0);	glVertex3f(bx, by + 30.0, 0.0);
			glTexCoord2f(1.0, 1.0);	glVertex3f(bx + 30.0, by + 30.0, 0.0);
			glTexCoord2f(1.0, 0.0);	glVertex3f(bx + 30.0, by, 0.0);
			glEnd();
			glDisable(GL_TEXTURE_2D);
			by += 0.25;
		}
		else if (hit && !animated) {
			if (!animCalled) {
				balloonBurst(1);
				//std::cout << "test";
				animCalled = true;
			}
			// texture binding for bursting the balloon
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, balloonTex[z]);
			glBegin(GL_POLYGON);
			glTexCoord2f(0.0, 0.0);	glVertex3f(bx, by, 0.0);
			glTexCoord2f(0.0, 1.0);	glVertex3f(bx, by + 30.0, 0.0);
			glTexCoord2f(1.0, 1.0);	glVertex3f(bx + 30.0, by + 30.0, 0.0);
			glTexCoord2f(1.0, 0.0);	glVertex3f(bx + 30.0, by, 0.0);
			glEnd();
			glDisable(GL_TEXTURE_2D);
		}
		if (!hit && by == 250.0) {
			missedBalloon++;
			by++;
		}
	}
};
std::vector<Balloon> B;

// generating and setting the balloon coordinates
void genBalloon(int x) {
	std::pair<float, float> a(0, 100), b(-230.0, 180.0);
	//std::cout<< map_value(a, b, (float)(rand() % 1000)<<std::endl;
	if (B.size() < maxBalloon && missedBalloon <= 2) {
		B.push_back(Balloon(map_value(a, b, (float)(rand() % 100)), -250.0));
	}
	else if (missedBalloon == 2) {
		B.clear();
	}
	glutTimerFunc(2000, genBalloon, 0);
}

// function to burst the balloon 
void balloonBurst(int i)
{
	for (auto& i : B) {
		if (i.hit == true && i.z < 6 && i.animated == false) {
			i.z++;
			if (i.z == 6) {
				i.animated = true;
			}
			glutTimerFunc(150, balloonBurst, 0);
		}
		else if (i.hit == false) {
			i.z = 0;
		}
	}
}

// bullet function
class Bullet {
public: float bulletx, bullety;
private: bool fired;
public:
	Bullet() {
		bulletx = gunx - 5.0;
		bullety = guny + 3.0;
		fired = true;
	}
public:
	void draw() {
		if (fired == true && bulletx >= -250) {
			// texture binding the bullet
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, bulletTex);
			glBegin(GL_POLYGON);
			glTexCoord2f(0.0, 0.0);	glVertex3f(bulletx, bullety, 0.0);
			glTexCoord2f(0.0, 1.0);	glVertex3f(bulletx, bullety + 3.0, 0.0);
			glTexCoord2f(1.0, 1.0);	glVertex3f(bulletx + 5.0, bullety + 3.0, 0.0);
			glTexCoord2f(1.0, 0.0);	glVertex3f(bulletx + 5.0, bullety, 0.0);
			glEnd();
			glDisable(GL_TEXTURE_2D);
			bulletx += -1.5;
		}
		if (bulletx <= -250) {
			fired = false;
		}
	}
};

std::vector<Bullet> b;

// function to add text to the scene
void drawText(float x, float y, std::string text)
{
	glClearColor(13.0 / 255.0, 27.0 / 255.0, 39.0 / 255.0, 0.8 / 255.0);
	glRasterPos2f(x, y);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)text.c_str());
}

// display function
void display() {
	if (started) {
	glClear(GL_COLOR_BUFFER_BIT );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
		glClearColor(13.0/255.0, 27.0/255.0, 39.0/255.0, 0.8/255.0);
		if (missedBalloon <= 2) {
			if (B.size() == maxBalloon && (B[maxBalloon-1].by > 250.0 ||  B[maxBalloon-1].hit == true)) {
				B.clear();
			}
			for (int i = 0; i < B.size(); i++) {
				B[i].draw();
			}
			for (int x = 0; x < b.size(); x++) {
				if (b[x].bulletx <= -250) {
					b[x].~Bullet();
				}
				else {
					for (int i = 0; i < B.size(); i++) { 
						if (B[i].bx<b[x].bulletx + 5 && B[i].bx + 30>b[x].bulletx&& B[i].by<b[x].bullety + 5 && B[i].by + 30>b[x].bullety) { // condition to detect if every balloon is hit or not
							B[i].hit = true;
						}
					}
					b[x].draw();
				}
			}
			gun();
		}
		else {
			glClear(GL_COLOR_BUFFER_BIT);
			drawText(-25, 0, "GAME OVER");
		}
		glutSwapBuffers();
		glFlush();

	}

	else {
		glClear(GL_COLOR_BUFFER_BIT);
		drawText(-55,0, "PRESS 's' TO START");
		drawText(-85, -20, "PRESS LEFT ARROW KEY TO SHOOT,");
		drawText(-145,-40,"UPWARD AND DOWNWARD ARROW KEYS TO MOVE THE GUN");
		glutSwapBuffers();
	}

	glutPostRedisplay();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-250.0, 250.0, -250.0, 250.0, -1.0, 1.0);

}

// special keys(arrow keys)
void keyPressed(int key, int x, int y)
{

	switch (key) {
	case GLUT_KEY_UP:
		// move upwards
		if (guny < 240) {
			guny += 10.0f;
			keyStates[key] = true;
			posy += 10.0f;
		}
		break;
	case GLUT_KEY_DOWN:
		// move downwards
		if (guny > -240) {
			guny += -10.0f;
			keyStates[key] = true;
			posy += -10.0f;
		}
		break;
	case GLUT_KEY_LEFT:
		if (b.size() < maxBullet)
			b.push_back(Bullet());
		else {
			//std::cout << "reload";
			b.clear();
		}
		break;
	}
}

// normal keys
void keyPress(unsigned char key,int x,int y)
{
	if (key == 115) {
		started = true;
		missedBalloon = 0;
		glutTimerFunc(3000, genBalloon, 0);
	}
	
}

void keyUp(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_UP:
		// when the upwards arrow key is released
		keyStates[key] = false;
		break;
	case GLUT_KEY_DOWN:
		// when the down arrow key is released
		keyStates[key] = false;
		break;
	case GLUT_KEY_LEFT:
		// when the left arrow key is released
		break;
	case 's': break;
	}
}

/*
 *  Request double buffer display mode.
 *  Register keyboard input callback functions
 */
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
	glEnable(GL_MULTISAMPLE);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 20);
	glutCreateWindow("BALLOON SHOOT");
	gunTex = texMapping("Textures/gun.png");
	balloonText = texMapping("Textures/f1balloon.png");
	balloonTex[0] = texMapping("Textures/f2balloon.png");
	balloonTex[1] = texMapping("Textures/f3balloon.png");
	balloonTex[2] = texMapping("Textures/f4balloon.png");
	balloonTex[3] = texMapping("Textures/f5balloon.png");
	balloonTex[4] = texMapping("Textures/f6balloon.png");
	balloonTex[5] = texMapping("Textures/f7balloon.png");
	bulletTex = texMapping("Textures/bullet.png");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(keyPressed);
	glutKeyboardFunc(keyPress);
	glutMainLoop();
	return 0;
}
