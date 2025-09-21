
#include <windows.h>		// Header File For Windows
#include <gl.h>			// Header File For The OpenGL32 Library
#include <glu.h>			// Header File For The GLu32 Library
#include <glaux.h>		// Header File For The Glaux Library
#include <cmath>
#include "texture.h"
#include "Model_3DS.h"
#include "camera.h"
#include "Sound.h"

bool lightOn = false; 
GLfloat lightColorOn[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // لون الضوء عند التشغيل (أبيض)
GLfloat lightColorOff[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // لون الضوء عند الإيقاف (أسود)
HDC			hDC = NULL;		// Private GDI Device Context
HGLRC		hRC = NULL;		// Permanent Rendering Cntext
HWND		hWnd = NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

bool	keys[256];			// Array Used For The Keyboard Routine
bool	active = TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen = FALSE;	// Fullscreen Flag Set To Fullscreen Mode By Default

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height == 0)										// Prevent A Divide By Zero By
	{
		height = 1;										// Making Height Equal One
	}

	glViewport(0, 0, width, height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 10000.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

void initLighting() {
    // تفعيل الإضاءة
    glEnable(GL_LIGHTING);

    // تفعيل مصدر الضوء الأول (GL_LIGHT0)
    glEnable(GL_LIGHT0);

    GLfloat lightColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // أبيض
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);

    GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };  // لون خافت
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);

    // تحديد موقع الضوء
    GLfloat lightPosition[] = { 0.0f, 1.0f, 0.0f, 0.0f };  // الضوء في الأعلى
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}
GLfloat mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };  // اللون المحيط
GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // اللون المنتشر (أبيض)
GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // اللون اللامع (أبيض)
GLfloat mat_shininess[] = { 50.0f };  // اللمعان

void setupMaterial() {
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}


// متغيرات لتتبع حالة الصوت
bool soundBird = false;
bool soundElevator = false;
bool soundPeople = false;
// دالة لتشغيل أو إيقاف الصوت عند الضغط على B و P
void playsound() {
   // معالجة صوت الطيور عند الضغط على B
    if (keys['B']) {
        if (!soundBird) {
            // تشغيل صوت الطيور بشكل غير متزامن ومتكرر (Looping)
            PlaySound("FOREST_BIRDS_SOUND.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
            soundBird = true;

            // إيقاف صوت العالم إذا كان قيد التشغيل
            if (soundElevator) {
                sndPlaySound(NULL, SND_ASYNC | SND_PURGE); // إيقاف صوت العالم
                soundElevator = false;
            }
      if (soundPeople) {
                sndPlaySound(NULL, SND_ASYNC | SND_PURGE); // إيقاف صوت العالم
                soundPeople = false;
            }
        }
    } else {
        // إذا لم يتم الضغط على B، إيقاف صوت الطيور
        if (soundBird) {
            sndPlaySound(NULL, SND_ASYNC | SND_PURGE); // إيقاف صوت الطيور
            soundBird = false;
        }
    }
    // معالجة صوت المصعد عند الضغط على E
    if (keys['E']) {
        if (!soundElevator) {
            // تشغيل صوت العالم بشكل غير متزامن ومتكرر (Looping)
            PlaySound("Elevator - SOUND EFFECT.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
            soundElevator = true;

            // إيقاف صوت الطيور إذا كان قيد التشغيل
            if (soundBird) {
                sndPlaySound(NULL, SND_ASYNC | SND_PURGE); // إيقاف صوت الطيور
                soundBird = false;
            }
       // إيقاف صوت الطيور إذا كان قيد التشغيل
      if (soundPeople) {
                sndPlaySound(NULL, SND_ASYNC | SND_PURGE); // إيقاف صوت الطيور
                soundPeople = false;
            }
        }
    } else {
        // إذا لم يتم الضغط على E، إيقاف صوت المصعد
        if (soundElevator) {
            sndPlaySound(NULL, SND_ASYNC | SND_PURGE); // إيقاف صوت العالم
            soundElevator = false;
        }
    }
  // معالجة صوت العالم عند الضغط على P
    if (keys['P']) {
    if (!soundPeople) {
            // تشغيل صوت العالم بشكل غير متزامن ومتكرر (Looping)
            PlaySound("soundpeople.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
      soundPeople = true;

            // إيقاف صوت الطيور إذا كان قيد التشغيل
            if (soundBird) {
                sndPlaySound(NULL, SND_ASYNC | SND_PURGE); // إيقاف صوت الطيور
                soundBird = false;
            }
       // إيقاف صوت الطيور إذا كان قيد التشغيل
      if (soundElevator) {
                sndPlaySound(NULL, SND_ASYNC | SND_PURGE); // إيقاف صوت الطيور
                soundElevator = false;
            }
        }
    } else {
        // إذا لم يتم الضغط على P، إيقاف صوت العالم
    if (soundPeople) {
            sndPlaySound(NULL, SND_ASYNC | SND_PURGE); // إيقاف صوت العالم
      soundPeople = false;
        }
    }

   
}

void KeyboardFunc(unsigned char key, int x, int y) {
    keys[key] = true; // تعيين المفتاح على أنه مضغوط
}

// دالة لمعالجة رفع الأصابع عن المفاتيح
void KeyboardUpFunc(unsigned char key, int x, int y) {
    keys[key] = false; // تعيين المفتاح على أنه غير مضغوط
}

// دالة لتحديث الشاشة أو التفاعل مع الألعاب
void Update() {
    playsound(); // تحديث الأصوات بناءً على ضغط المفاتيح
}

int road, grass,block,building1,top,building2,
	frontbuilding2,building3,sidebuilding3,football,top2,tshirt3,tshirt4,shirt3,shirt4,tshirt5,top3,
	table33,wardrobe1,jeans1,sign2,pic2,bag1,bag2,bag3,bag4,bag5,bag6,pics5,pics6,
	parking,land,mallwall,column,floorm,roof,iron,elevatordoor,shop4wall,shop2wall,tshirt1,tshirt2,shirt1,shirt2,top1,shelve1,cocacola,restaurantwall,
  drinks,drinks1,drinks2,drinks3,drinks4,box1,box2,box3,box4,box5,box6,box7,box8,box9,
  drinks5,drinks6,drinks7,drinks8,drinks9,drinks10,ele,logo3,pics1,pics2,pics3,pics7,
  ad1,ad2,ad3,ad4,ad5,ad6,table3,building4,building5,building6,building7;

Camera MyCamera;

Model_3DS *tree;
Model_3DS *fountain;
Model_3DS *spoon;

int bottom,side,table,table2,menu,north,up,south,east,west,chair,table1,food1,food2,food3,clock,sign1,laptop,laptop2;
GLTexture Leaf,BARK,Foun,Water,spoon3;


int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	glEnable(GL_TEXTURE_2D);
	road = LoadTexture("road.bmp");
	block = LoadTexture("mallfloor2.bmp");
	building1 = LoadTexture("building1.bmp");
	top = LoadTexture("tkan.bmp");
	building2 = LoadTexture("f2.bmp");
	frontbuilding2 = LoadTexture("b3.bmp");
	building3 = LoadTexture("building11.bmp");
	sidebuilding3 = LoadTexture("building4.bmp");
	parking = LoadTexture("parking_lot.bmp");
	land = LoadTexture("land.bmp");
	grass = LoadTexture("leaf3.bmp");
	east = LoadTexture("east.bmp");
	west = LoadTexture("west.bmp");
	up = LoadTexture("up.bmp");
	north = LoadTexture("north.bmp");
	south = LoadTexture("south.bmp");
	table = LoadTexture("table.bmp");
	mallwall = LoadTexture("wallm3.bmp");
	column = LoadTexture("column.bmp");
	floorm = LoadTexture("floor5.bmp");
	roof = LoadTexture("mallroof.bmp");
	iron = LoadTexture("iron2.bmp");
	elevatordoor = LoadTexture("edoor.bmp");
	chair = LoadTexture("chair.bmp");
	table1 = LoadTexture("table1.bmp");
	food1 = LoadTexture("food2.bmp");
	food2 = LoadTexture("food4.bmp");
	food3 = LoadTexture("food5.bmp");
	menu = LoadTexture("menu.bmp");
	clock = LoadTexture("clock.bmp");
	sign1 = LoadTexture("logo2.bmp");
	laptop = LoadTexture("laptop.bmp");
	laptop2 = LoadTexture("laptop2.bmp");
	shop4wall = LoadTexture("build21.bmp");
	shop2wall = LoadTexture("shop2wall2.bmp");
	tshirt1 = LoadTexture("tshirt1.bmp");
	tshirt2 = LoadTexture("tshirt2.bmp");
	tshirt3 = LoadTexture("tshirt3.bmp");
	tshirt4 = LoadTexture("tshirt4.bmp");
	tshirt5 = LoadTexture("sthirt5.bmp");
	shirt1 = LoadTexture("shirt1.bmp");
	shirt2 = LoadTexture("shirt2.bmp");
	shirt3 = LoadTexture("shirt3.bmp");
	shirt4 = LoadTexture("shirt4.bmp");
	top1 = LoadTexture("top1.bmp");
	top2= LoadTexture("top3.bmp");
	top3= LoadTexture("top10.bmp");
	table33 =LoadTexture("table33.bmp");
	wardrobe1 =LoadTexture("wardrobe.bmp");
	jeans1 =LoadTexture("jeans.bmp");
	sign2 = LoadTexture("sign4.bmp");
	pic2 = LoadTexture("shopping2.bmp");
	bag1 = LoadTexture("bag1.bmp");
	bag2 = LoadTexture("bag2.bmp");
	bag3 = LoadTexture("bag3.bmp");
	bag4 = LoadTexture("bag4.bmp");
	bag5 = LoadTexture("bag5.bmp");
	bag6 = LoadTexture("bag6.bmp");
	mallwall = LoadTexture("wallm3.bmp");
    column = LoadTexture("column.bmp");
    floorm = LoadTexture("floor5.bmp");
    roof = LoadTexture("mallroof.bmp");
    iron = LoadTexture("iron2.bmp");
    elevatordoor = LoadTexture("edoor.bmp");
    drinks1 = LoadTexture("drinks1.bmp");
    drinks = LoadTexture("drinks.bmp");
     drinks2 = LoadTexture("drinks2.bmp");
     drinks3 = LoadTexture("drinks3.bmp");
     drinks4 = LoadTexture("drinks4.bmp");
     drinks5 = LoadTexture("drinks5.bmp");
     drinks6 = LoadTexture("drinks6.bmp");
     drinks7 = LoadTexture("drinks7.bmp");
      drinks8 = LoadTexture("drinks8.bmp");
    drinks9 = LoadTexture("drinks9.bmp");
    drinks10 = LoadTexture("drinks10.bmp");

    restaurantwall = LoadTexture("wall.bmp");

   ad1 = LoadTexture("ad1.bmp");
    ad2 = LoadTexture("ad2.bmp");
    ad3 = LoadTexture("ad3.bmp");
	  ad4 = LoadTexture("ad4.bmp");
	  ad5 = LoadTexture("ad5.bmp");
	  ad6 = LoadTexture("ad6.bmp");

    cocacola = LoadTexture("cocacola.bmp");
	table2 = LoadTexture("table2.bmp");
	 table3 = LoadTexture("table3.bmp");
	 football = LoadTexture("football2.bmp");
	 building4 =LoadTexture("bg5.bmp");
	 building5 =LoadTexture("bg6.bmp");
	 building6 =LoadTexture("bg7.bmp");
	 building7 =LoadTexture("bg8.bmp");
	 ele = LoadTexture("floor2.bmp");
	 box1 = LoadTexture("box14.bmp");
	 box2 = LoadTexture("box11.bmp");
	 box3 = LoadTexture("box3.bmp");
	 box4 = LoadTexture("box12.bmp");
	 box5 = LoadTexture("box5.bmp");
	 box6 = LoadTexture("box6.bmp");
	 box7 = LoadTexture("box15.bmp");
	 box8 = LoadTexture("box8.bmp");
	 box9 = LoadTexture("box9.bmp");
	 logo3 = LoadTexture("logo3.bmp");
	 pics1 = LoadTexture("pics1.bmp");
	 pics2= LoadTexture("pics2.bmp");
	 pics3 = LoadTexture("pic4.bmp");
	 pics5= LoadTexture("pics5.bmp");
	 pics6 = LoadTexture("pics6.bmp");
	 pics7 = LoadTexture("ad9.bmp");

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

	tree = new Model_3DS();
	tree->Load("Tree1.3DS");
	Leaf.LoadBMP("bat.bmp");
	BARK.LoadBMP("bark_loo.bmp");

	tree->Materials[0].tex=BARK;
	tree->Materials[1].tex=Leaf;
	tree->Materials[2].tex=Leaf;
	tree->Materials[3].tex=Leaf;
	tree->Materials[4].tex=Leaf;

	fountain = new Model_3DS();
	fountain->Load("Bacarra Resort Fountain.3DS");
	Foun.LoadBMP("fountain.bmp");
	Water.LoadBMP("water.bmp");
	fountain->Materials[0].tex=Foun;
	fountain->Materials[1].tex=Foun;
	fountain->Materials[2].tex=Water;
	fountain->Materials[3].tex=Water;
	fountain->Materials[4].tex=Foun;
	fountain->Materials[5].tex=Foun;
	fountain->Materials[6].tex=Foun;
	fountain->Materials[7].tex=Foun;
	fountain->Materials[8].tex=Water;
	
	spoon = new Model_3DS();
	spoon->Load("Cutlery.3DS");
	spoon3.LoadBMP("spoon1.bmp");

	spoon->Materials[0].tex=spoon3;
	spoon->Materials[1].tex=spoon3;
	spoon->Materials[2].tex=spoon3;
	spoon->pos.x=35;
	spoon->pos.y=0;
	spoon->pos.z=22;
	spoon->scale=0.04;

	MyCamera = Camera();
	MyCamera.Position.x = 0;
	MyCamera.Position.y = 45;
	MyCamera.Position.z = +45;
	
	return TRUE;										// Initialization Went OK
}

GLdouble an,al,ar,as,r=15,m=0;
double n=80,s=80;
void movedoor()
{
	if (keys['T']) { 
		if (al < 60) {
		al += 0.2; 
	} 
		else {al = 60;} 
	} else { 
		if (al > 0) {al -= 0.2;} 
		else { al = 0;} 
	}
	if (keys['R']) { 
		if (ar < 60) {
		ar += 0.2; 
	} 
		else {ar = 60;} 
	} else { 
		if (ar > 0) {ar -= 0.2;} 
		else { ar = 0;} 
	}
	if (keys['Y']) { 
		if (as < 60) {
		as += 0.2; 
	} 
		else {as = 60;} 
	} else { 
		if (as > 0) {as -= 0.2;} 
		else { as = 0;} 
	}
	
	if(keys['U']){
		while (r>15)
		{
			r=r-0.002;
		}
	}
	if(keys['I']){
		while (r<40)
		{
			r=r+0.002;
		}
	}
	if(keys['V'] && m <= 1){
    while (n<110) {
        n+=0.002;
    }
	}

	if(keys['C'] && m <= 1){
		while (n>81) {
			n-=0.002;
		}
	}

	if(keys['G'] && m >= 100){
		while (s<110) {
			s+=0.002;
		}
	}

	if(keys['F'] && m >= 100){
		while (s>81) {
			s-=0.002;
		}
	}

	if(keys['H']){
    while (m<100) {
        m+=0.00001; // زيادة صغيرة جدًا لجعل الحركة أبطأ
    }
	}
	if(keys['J']){
		while (m>1) {
			m-=0.00001; // تقليل صغير جدًا لجعل الحركة أبطأ
		}
	}

	if (keys['K']) { 
		if (an < 60) {
		an += 0.2; 
	} 
		else {an = 60;} 
	} else { 
		if (an > 0) {an -= 0.2;} 
		else { an = 0;} 
	}
	
}


GLdouble size = 500;
void drawUnitCube() {
  glPushMatrix();
  glColor3d(0.4,0.2,0);
  glEnable(GL_TEXTURE_2D);

  // down
  glBindTexture(GL_TEXTURE_2D, up);
  glBegin(GL_QUADS);
  glTexCoord2d(0.25, 0.3333); glVertex3d(-size, 0, -size); 
  glTexCoord2d(0.50, 0.3333); glVertex3d(size, 0, -size); 
  glTexCoord2d(0.50, 0); glVertex3d(size, 0, size);
  glTexCoord2d(0.25, 0); glVertex3d(-size, 0, size); 
  glEnd();

  glColor3d(1,1,1);
  glEnable(GL_TEXTURE_2D);

  // up (previously left)
  glBindTexture(GL_TEXTURE_2D, up);
  glBegin(GL_QUADS);
  glTexCoord2d(0.25, 1); glVertex3d(-size, size, -size); 
  glTexCoord2d(0.50, 1); glVertex3d(size, size, -size); 
  glTexCoord2d(0.50, 0.6666); glVertex3d(size, size, size); 
  glTexCoord2d(0.25, 0.6666); glVertex3d(-size, size, size); 
  glEnd();

  // left (previously up)
  glBindTexture(GL_TEXTURE_2D, up);
  glBegin(GL_QUADS);
  glTexCoord2d(0.25, 0.6666); glVertex3d(-size, size, size); 
  glTexCoord2d(0.50, 0.6666); glVertex3d(-size, size, -size); 
  glTexCoord2d(0.50, 0.3333); glVertex3d(-size, 0, -size);
  glTexCoord2d(0.25, 0.3333); glVertex3d(-size, 0, size); 
  glEnd();

  // right
  glBindTexture(GL_TEXTURE_2D, up);
  glBegin(GL_QUADS);
  glTexCoord2d(0, 0.6666); glVertex3d(size, size, size); 
  glTexCoord2d(0.25, 0.6666); glVertex3d(size, size, -size); 
  glTexCoord2d(0.25, 0.3333); glVertex3d(size, 0, -size); 
  glTexCoord2d(0, 0.3333); glVertex3d(size, 0, size); 
  glEnd();

  // front
  glBindTexture(GL_TEXTURE_2D, up);
  glBegin(GL_QUADS);
  glTexCoord2d(0.50, 0.6666); glVertex3d(-size, size, size);
  glTexCoord2d(0.75, 0.6666); glVertex3d(size, size, size);
  glTexCoord2d(0.75, 0.3333); glVertex3d(size, 0, size);
  glTexCoord2d(0.50, 0.3333); glVertex3d(-size, 0, size);
  glEnd();

  // back
  glBindTexture(GL_TEXTURE_2D, up);
  glBegin(GL_QUADS);
  glTexCoord2d(0.75, 0.6666); glVertex3d(-size, size, -size);
  glTexCoord2d(1, 0.6666); glVertex3d(size, size, -size);
  glTexCoord2d(1, 0.3333); glVertex3d(size, 0, -size);
  glTexCoord2d(0.75, 0.3333); glVertex3d(-size, 0, -size);
  glEnd();

  glPopMatrix();
}

void Road() {
  glPushMatrix();
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,road);
  glBegin(GL_QUADS);
  glColor3f(1, 1, 1); // لون الطريق (رمادي) 
  glTexCoord2d(0,0); glVertex3f(-30, 0.5, -500) ;
  glTexCoord2d(1,0); glVertex3f(30, 0.5,-500) ;
  glTexCoord2d(1,6); glVertex3f(30, 0.5,500) ;
  glTexCoord2d(0,6); glVertex3f(-30,0.5,500) ;
  glEnd();
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}
void Column(){
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,column);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0); glVertex3f(-230, 210, -180);
    glTexCoord2d(1, 0); glVertex3f(-230, 210, -150);
    glTexCoord2d(1, 1); glVertex3f(-230, 0.5, -150);
    glTexCoord2d(0, 1); glVertex3f(-230, 0.5, -180);

	glTexCoord2d(0, 0); glVertex3f(-230, 210, -150);
    glTexCoord2d(1, 0); glVertex3f(-203, 210, -150);
    glTexCoord2d(1, 1); glVertex3f(-203, 0.5, -150);
    glTexCoord2d(0, 1); glVertex3f(-230, 0.5, -150);

	glTexCoord2d(0, 0); glVertex3f(-230, 210, -180);
    glTexCoord2d(1, 0); glVertex3f(-203, 210, -180);
    glTexCoord2d(1, 1); glVertex3f(-203, 0.5, -180);
    glTexCoord2d(0, 1); glVertex3f(-230, 0.5, -180);

	glTexCoord2d(0, 0); glVertex3f(-203, 210, -150);
    glTexCoord2d(1, 0); glVertex3f(-203, 210, -180);
    glTexCoord2d(1, 1); glVertex3f(-230, 210, -180);
    glTexCoord2d(0, 1); glVertex3f(-230, 210, -150);
	glEnd();
	glDisable(GL_TEXTURE_2D);

}

void door(){
    // left door
    glPushMatrix();
    glScaled(2.5,2.5,2.5);
    glTranslated(-15, 0, 5);
    glRotated(-al, 0, 1, 0); 
    glTranslated(15, 0, -5);
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    glColor4f(1.0, 1.0, 1.0, 0.6);
    glBegin(GL_QUADS); 
    glVertex3f(-15, 0, 5); 
    glVertex3f(-15, 15.5, 5);
    glVertex3f(-0.1, 15.5, 5); 
    glVertex3f(-0.1, 0, 5); 
    glEnd(); 
    glDisable(GL_BLEND);
    // Left door handle
    glColor3f(0.2, 0.2, 0.2); // Darker color
    GLUquadric* quad;
    quad = gluNewQuadric();
    glPushMatrix();
    glTranslated(-1.0, 7.75, 5.1);
    gluSphere(quad, 0.5, 20, 20); // Smaller handle
    glPopMatrix();
    gluDeleteQuadric(quad);
    glPopMatrix();
    
    // right door
    glPushMatrix(); 
    glScaled(2.5,2.5,2.5);
    glTranslated(15, 0, 5);
    glRotated(ar, 0, 1, 0); 
    glTranslated(-15, 0, -5);
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    glColor4f(1.0, 1.0, 1.0, 0.6); 
    glBegin(GL_QUADS); 
    glVertex3f(15, 0, 5); 
    glVertex3f(15, 15.5, 5);
    glVertex3f(0.1, 15.5, 5); 
    glVertex3f(0.1, 0, 5); 
    glEnd(); 
    glDisable(GL_BLEND); 
    // Right door handle
    glColor3f(0.2, 0.2, 0.2); // Darker color
    quad = gluNewQuadric();
    glPushMatrix();
    glTranslated(1.0, 7.75, 5.1);
    gluSphere(quad, 0.5, 20, 20); // Smaller handle
    glPopMatrix();
    gluDeleteQuadric(quad);
    glPopMatrix();
}

void door1(){
 // left door
    glPushMatrix();
    glScaled(2.5,4,2);
    glTranslated(-15, 0, 5);
    glRotated(-as, 0, 1, 0); 
    glTranslated(15, 0, -5);
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    glColor4f(1.0, 1.0, 1.0, 0.6);
    glBegin(GL_QUADS); 
    glVertex3f(-15, 0, 5); 
    glVertex3f(-15, 15.5, 5);
    glVertex3f(-0.1, 15.5, 5); 
    glVertex3f(-0.1, 0, 5); 
    glEnd(); 
    glDisable(GL_BLEND);
    // Left door handle
    glColor3f(0.2, 0.2, 0.2); // Darker color
    GLUquadric* quad;
    quad = gluNewQuadric();
    glPushMatrix();
    glTranslated(-1.0, 7.75, 5.1);
    gluSphere(quad, 0.5, 20, 20); // Smaller handle
    glPopMatrix();
    gluDeleteQuadric(quad);
    glPopMatrix();
}

void DrawChair2(int texture) {
    glPushMatrix();
    // أرجل الكرسي
    glColor3f(0.3f, 0.3f, 0.3f); 
    glBegin(GL_QUADS);
    glVertex3f(-0.15f, 0.0f, 0.15f);
    glVertex3f(-0.1f, 0.0f, 0.15f);
    glVertex3f(-0.1f, 0.5f, 0.15f);
    glVertex3f(-0.15f, 0.5f, 0.15f);
    
    glVertex3f(0.15f, 0.0f, 0.15f);
    glVertex3f(0.1f, 0.0f, 0.15f);
    glVertex3f(0.1f, 0.5f, 0.15f);
    glVertex3f(0.15f, 0.5f, 0.15f);

    glVertex3f(-0.15f, 0.0f, -0.15f);
    glVertex3f(-0.1f, 0.0f, -0.15f);
    glVertex3f(-0.1f, 0.5f, -0.15f);
    glVertex3f(-0.15f, 0.5f, -0.15f);
    
    glVertex3f(0.15f, 0.0f, -0.15f);
    glVertex3f(0.1f, 0.0f, -0.15f);
    glVertex3f(0.1f, 0.5f, -0.15f);
    glVertex3f(0.15f, 0.5f, -0.15f);
    glEnd();

    // مقعد الكرسي
    glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texture);
    glBegin(GL_QUADS);

	glTexCoord2d(0, 0);
    glVertex3f(-0.15f, 0.5f, -0.17f);
	glTexCoord2d(1, 0);
    glVertex3f(0.15f, 0.5f, -0.17f);
	glTexCoord2d(1, 1);
    glVertex3f(0.15f, 0.5f, 0.17f);
	glTexCoord2d(0, 1);
    glVertex3f(-0.15f, 0.5f, 0.17f);

    // المسند
    glColor3f(1, 1, 1);
	glTexCoord2d(0, 0);
    glVertex3f(0.15f, 0.5f, -0.17f);
	glTexCoord2d(1, 0);
    glVertex3f(0.2f, 0.9f, -0.17f);
	glTexCoord2d(1, 1);
    glVertex3f(0.2f, 0.9f, 0.17f);
	glTexCoord2d(0, 1);
    glVertex3f(0.15f, 0.5f, 0.17f);
    glEnd();
	glDisable(GL_TEXTURE_2D);
    glPopMatrix();
	

}

void DrawPlate(float x, float y, float z, float radius, float height) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotated(90,1,0,0);
    // رسم الدائرة السفلية للصحن
    glColor3f(0.95f, 0.95f, 0.95f); // لون الصحن (أبيض باهت)
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, 0.0f); // مركز الدائرة
    for(int i = 0; i <= 360; i++) {
        float angle = i * 3.14159 / 180;
        glVertex3f(radius * cos(angle), radius * sin(angle), 0.0f);
    }
    glEnd();

    // رسم الحواف للأعلى بشكل متدرج
	glTranslated(0,0,-0.1);
    glColor3f(1.0f, 1.0f, 1.0f); // لون الحافة البيضاء
    glBegin(GL_TRIANGLE_STRIP);
    for(int i = 0; i <= 360; i++) {
        float angle = i * 3.14159 / 180;
        glVertex3f(radius * cos(angle), radius * sin(angle), height);
        glVertex3f((radius-0.02) * cos(angle), (radius-0.02) * sin(angle), height + 0.01f); // ارتفاع الحافة
    }
    glEnd();
	
	glPopMatrix();
}
void DrawGlass(float x, float y, float z, float radius, float height) {
    glPushMatrix();
    glTranslatef(x, y, z);
	glRotated(-90,1,0,0);
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	glColor4f(1.0, 1.0, 1.0, 0.6);

    GLUquadric *quad;
    quad = gluNewQuadric();

    // رسم قاعدة الكأس
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    gluDisk(quad, 0.0f, radius, 32, 32);
    glPopMatrix();

    // رسم جسم الكأس
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    gluCylinder(quad, radius, radius, height, 32, 32);
    glPopMatrix();

    // رسم الحافة العليا للكأس
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, height);
    gluDisk(quad, 0.0f, radius, 32, 32);
    glPopMatrix();

    gluDeleteQuadric(quad);
	glDisable(GL_BLEND); 

    glPopMatrix();
}
void DrawTable2() {
    glPushMatrix();
    glScalef(20,20,20);
    // أرجل الطاولة
    glColor3f(0.3f, 0.3f, 0.3f); 
    glBegin(GL_QUADS);
    glVertex3f(-0.55f, 0.0f, 0.55f);
    glVertex3f(-0.5f, 0.0f, 0.55f);
    glVertex3f(-0.5f, 0.7f, 0.55f);
    glVertex3f(-0.55f, 0.7f, 0.55f);
    
    glVertex3f(0.55f, 0.0f, 0.55f);
    glVertex3f(0.5f, 0.0f, 0.55f);
    glVertex3f(0.5f, 0.7f, 0.55f);
    glVertex3f(0.55f, 0.7f, 0.55f);

    glVertex3f(-0.55f, 0.0f, -0.55f);
    glVertex3f(-0.5f, 0.0f, -0.55f);
    glVertex3f(-0.5f, 0.7f, -0.55f);
    glVertex3f(-0.55f, 0.7f, -0.55f);
    glVertex3f(0.55f, 0.0f, -0.55f);
    glVertex3f(0.5f, 0.0f, -0.55f);
    glVertex3f(0.5f, 0.7f, -0.55f);
    glVertex3f(0.55f, 0.7f, -0.55f);
    glEnd();

    // سطح الطاولة
    //glColor3f(0.9f, 0.9f, 0.9f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,table1);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
    glVertex3f(-0.55f, 0.7f, -0.55f);
	glTexCoord2d(1, 0);
    glVertex3f(0.55f, 0.7f, -0.55f);
	glTexCoord2d(1, 1);
    glVertex3f(0.55f, 0.7f, 0.55f);
	glTexCoord2d(0, 1);
    glVertex3f(-0.55f, 0.7f, 0.55f);
    glEnd();
	glDisable(GL_TEXTURE_2D);

	DrawGlass(-0.3f, 0.7f, -0.07f, 0.05f, 0.2f);
	DrawGlass(0.3f, 0.7f, 0.07f, 0.05f, 0.2f);
	DrawPlate(-0.2f, 0.7f, 0.2f, 0.2f,0.09f);
	DrawPlate(0.2f, 0.7f, -0.2f, 0.2f,0.09f); 
	
	glPushMatrix();
	glTranslated(-35.2,0.71,-22.3);
	spoon->Draw(); // استدعاء دالة الرسم للنموذج الثلاثي الأبعاد 
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-34.7,0.71,-21.7);
	spoon->Draw(); // استدعاء دالة الرسم للنموذج الثلاثي الأبعاد 
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0.5,0,0);
	DrawChair2(chair);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-0.5,0,0);
	glRotated(180,0,1,0);
	DrawChair2(chair);
	glPopMatrix();
    glPopMatrix();
}

void pic(double x1,double x2 ,double y1 ,double y2 ,double z, int texture){
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texture);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 1);
    glVertex3f(x1, y1, z);
	glTexCoord2d(0, 0);
    glVertex3f(x1, y2, z);
	glTexCoord2d(1,0);
    glVertex3f(x2, y2, z);
	glTexCoord2d(1, 1);
    glVertex3f(x2, y1, z);
    glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}
void sign(){
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,sign1);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
    glVertex3f(-122, 170, 30);
	glTexCoord2d(0, 1);
    glVertex3f(-122, 190,30);
	glTexCoord2d(1,1);
    glVertex3f(-122, 190, 60);
	glTexCoord2d(1, 0);
    glVertex3f(-122, 170, 60);
    glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void drawlaptop(){
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,laptop);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
    glVertex3f(55, 128.5, 85);
	glTexCoord2d(0, 1);
    glVertex3f(55, 138,85);
	glTexCoord2d(1,1);
    glVertex3f(55, 138, 100);
	glTexCoord2d(1, 0);
    glVertex3f(55, 128.5, 100);
    glEnd();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,iron);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
    glVertex3f(55, 128.5, 85);
	glTexCoord2d(0, 1);
    glVertex3f(65, 128.5,85);
	glTexCoord2d(1,1);
    glVertex3f(65, 128.5, 100);
	glTexCoord2d(1, 0);
    glVertex3f(55, 128.5, 100);
    glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void drawCashierCounter2(int texure1,int texture2) {
    glPushMatrix();
	glScaled(18,18,18);
    
    glEnable(GL_TEXTURE_2D); // قم بتمكين الtextures

    // تعيين الملمس للطاولة 
	glBindTexture(GL_TEXTURE_2D, texure1);

    glBegin(GL_QUADS); 
    
    // الجدار الأمامي 
    glTexCoord2f(0,0); glVertex3f(-1.5f, 0.0f, 0.5f);
    glTexCoord2f(1,0); glVertex3f(1.5f, 0.0f, 0.5f); 
    glTexCoord2f(1,1); glVertex3f(1.5f, 1.0f, 0.5f); 
    glTexCoord2f(0,1); glVertex3f(-1.5f, 1.0f, 0.5f);
    
    // الجدار الخلفي
    glTexCoord2f(0,0); glVertex3f(-1.5f, 0.0f, -0.5f); 
    glTexCoord2f(1,0); glVertex3f(1.5f, 0.0f, -0.5f);
    glTexCoord2f(1,1); glVertex3f(1.5f, 1.0f, -0.5f); 
    glTexCoord2f(0,1); glVertex3f(-1.5f, 1.0f, -0.5f);
    
    // الجدار الأيسر
    glTexCoord2f(0,0); glVertex3f(-1.5f, 0.0f, -0.5f); 
    glTexCoord2f(1,0); glVertex3f(-1.5f, 0.0f, 0.5f); 
    glTexCoord2f(1,1); glVertex3f(-1.5f, 1.0f, 0.5f); 
    glTexCoord2f(0,1); glVertex3f(-1.5f, 1.0f, -0.5f); 
    
    // الجدار الأيمن 
    glTexCoord2f(0,0); glVertex3f(1.5f, 0.0f, -0.5f);
    glTexCoord2f(1,0); glVertex3f(1.5f, 0.0f, 0.5f); 
    glTexCoord2f(1,1); glVertex3f(1.5f, 1.0f, 0.5f); 
    glTexCoord2f(0,1); glVertex3f(1.5f, 1.0f, -0.5f); 
    
    glEnd();
    
    glDisable(GL_TEXTURE_2D); // قم بتعطيل الtextures

    // سطح الكاشير 
    glEnable(GL_TEXTURE_2D); // تمكين الtextures مرة أخرى
	glBindTexture(GL_TEXTURE_2D, texure1);
    
    glBegin(GL_QUADS); 
    glTexCoord2f(0,0); glVertex3f(-1.5f, 1.0f, 0.5f); 
    glTexCoord2f(1,0); glVertex3f(1.5f, 1.0f, 0.5f); 
    glTexCoord2f(1,1); glVertex3f(1.5f, 1.0f, -0.5f);
    glTexCoord2f(0,1); glVertex3f(-1.5f, 1.0f, -0.5f); 
    glEnd(); 
    glDisable(GL_TEXTURE_2D); // تعطيل الtextures مرة أخرى
    
    // الرف الخلفي للكاشير 
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texure1);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3f(-1.5f, 1.0f, -0.5f); 
    glTexCoord2f(1,0); glVertex3f(1.5f, 1.0f, -0.5f); 
    glTexCoord2f(1,1); glVertex3f(1.5f, 1.5f, -0.5f); 
    glTexCoord2f(0,1); glVertex3f(-1.5f, 1.5f, -0.5f); 
    glTexCoord2f(0,0); glVertex3f(-1.5f, 1.5f, -0.5f);
    glTexCoord2f(1,0); glVertex3f(-1.5f, 1.5f, 0.0f);
    glTexCoord2f(1,1); glVertex3f(1.5f, 1.5f, 0.0f);
    glTexCoord2f(0,1); glVertex3f(1.5f, 1.5f, -0.5f); 
    glEnd(); 
    glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glScaled(2,2,2);
	glTranslated(0,0,0.5);
	glRotated(-90,0,1,0);
	DrawChair2(texture2);
	glPopMatrix();
    glPopMatrix();
}

void shop4(){
	glPushMatrix();
	glTranslated(-160,101,-20);
	DrawTable2();
	glTranslated(0,0,70);
	DrawTable2();
	glPopMatrix();
	glPushMatrix();
	glTranslated(-70,101,35);
	DrawTable2();
	glPushMatrix();
	glTranslated(-0,0,60);
	DrawTable2();
	glPopMatrix();
	glTranslated(60,0,0);
	DrawTable2();
	glPushMatrix();
	glTranslated(-0,0,60);
	DrawTable2();
	glPopMatrix();
	glTranslated(60,0,0);
	DrawTable2();
	glPopMatrix();
	pic(20,-5,180,150,8,food1);
	pic(-15,-40,170,140,8,food2);
	pic(-50,-75,160,130,8,food3);
	pic(20,5,160,140,115,menu);
	pic(-20,-35,155,140,17,clock);
	sign();
	drawlaptop();
	glPushMatrix();
	glTranslated(60,101,85);
	glRotated(90,0,1,0);
	drawCashierCounter2(table1,chair);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-130,101,30);
	glRotated(90,0,1,0);
	door1();
	glPopMatrix();
}


void drawHanger(double x, double y, double z) {
	glColor3d(0,0,0);
	glBegin(GL_LINES); 
	glVertex3d(x, y, z); 
	glVertex3d(x - 4.0, y - 1.6, z); 
	glVertex3d(x - 4.0, y - 1.6, z);
	glVertex3d(x - 8.0, y, z);
	glVertex3d(x - 8.0, y, z); 
	glVertex3d(x + 8.0, y, z); 
	glVertex3d(x + 8.0, y, z); 
	glVertex3d(x + 4.0, y - 1.6, z); 
	glVertex3d(x + 4.0, y - 1.6, z); 
	glVertex3d(x, y, z); 
	glVertex3d(x, y, z);  
	glVertex3d(x, y + 2.4, z); 
	glVertex3d(x, y + 2.4, z);
	glVertex3d(x - 1.6, y + 2.4, z);
	glEnd();
	glColor3d(1,1,1);
}

void tshirt(double c1,double c2, double c3,double z,int texture){
	glPushMatrix();
	glColor3d(c1,c2,c3);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texture);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
    glVertex3f(-75, 66.5, z);
	glTexCoord2d(0, 1);
    glVertex3f(-65, 66.5,z);
	glTexCoord2d(1,1);
    glVertex3f(-65, 50, z);
	glTexCoord2d(1, 0);
    glVertex3f(-75, 50, z);

	glVertex3f(-75, 66.5, z);
	glTexCoord2d(0, 1);
    glVertex3f(-75, 63,z);
	glTexCoord2d(1,0);
    glVertex3f(-78, 60, z);
	glTexCoord2d(1, 1);
    glVertex3f(-80, 62, z);

	glVertex3f(-65, 66.5, z);
	glTexCoord2d(0, 1);
    glVertex3f(-65, 63,z);
	glTexCoord2d(1,1);
    glVertex3f(-62, 60, z);
	glTexCoord2d(1, 0);
    glVertex3f(-60, 62, z);

    glEnd();
	glDisable(GL_TEXTURE_2D);
	glColor3d(1,1,1);
	glPopMatrix();
}

void shirt(double c1,double c2, double c3,double z,int texture){
	glPushMatrix();
	glColor3d(c1,c2,c3);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texture);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
    glVertex3f(-75, 66.5, z);
	glTexCoord2d(0, 1);
    glVertex3f(-65, 66.5,z);
	glTexCoord2d(1,1);
    glVertex3f(-65, 50, z);
	glTexCoord2d(1, 0);
    glVertex3f(-75, 50, z);

	glVertex3f(-75, 66.5, z);
	glTexCoord2d(0, 1);
    glVertex3f(-75, 61,z);
	glTexCoord2d(1,0);
    glVertex3f(-78, 50, z);
	glTexCoord2d(1, 1);
    glVertex3f(-80, 53, z);

	glVertex3f(-65, 66.5, z);
	glTexCoord2d(0, 1);
    glVertex3f(-65, 61,z);
	glTexCoord2d(1,1);
    glVertex3f(-62, 50, z);
	glTexCoord2d(1, 0);
    glVertex3f(-60, 53, z);

    glEnd();
	glDisable(GL_TEXTURE_2D);
	glColor3d(1,1,1);
	glPopMatrix();
}

void topshirt(double c1,double c2, double c3,double z,int texture){
	glPushMatrix();
	glColor3d(c1,c2,c3);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texture);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 1);
    glVertex3f(-75, 65, z);
	glTexCoord2d(1, 1);
    glVertex3f(-65, 65,z);
	glTexCoord2d(1,0);
    glVertex3f(-65, 53, z);
	glTexCoord2d(0, 0);
    glVertex3f(-75, 53, z);
    glEnd();
	glDisable(GL_TEXTURE_2D);

	glColor3d(0,0,0);
	glBegin(GL_QUADS);
	glVertex3f(-73.5, 67, z);
	glTexCoord2d(0, 1);
    glVertex3f(-73, 67,z);
	glTexCoord2d(1,1);
    glVertex3f(-73, 65, z);
	glTexCoord2d(1, 0);
    glVertex3f(-73.5, 65, z);

	glVertex3f(-66.5, 67, z);
	glTexCoord2d(0, 1);
    glVertex3f(-66, 67,z);
	glTexCoord2d(1,0);
    glVertex3f(-66, 65, z);
	glTexCoord2d(1, 1);
    glVertex3f(-66.5, 65, z);
	glEnd();

	glColor3d(1,1,1);
	glPopMatrix();
}


void stick(int kind) {
    glPushMatrix();
	glColor3d(0,0,0);
    glBegin(GL_QUADS);
    glVertex3d(-70, 70, 14);
    glVertex3d(-70, 70, 29);
    glVertex3d(-69, 70, 29);
    glVertex3d(-69, 70, 14);

    glVertex3d(-70, 69, 14);
    glVertex3d(-70, 69, 29);
    glVertex3d(-69, 69, 29);
    glVertex3d(-69, 69, 14);

    glVertex3d(-69, 70, 14);
    glVertex3d(-69, 70, 29);
    glVertex3d(-69, 69, 29);
    glVertex3d(-69, 69, 14);

    glVertex3d(-70, 70, 14);
    glVertex3d(-70, 70, 29);
    glVertex3d(-70, 69, 29);
    glVertex3d(-70, 69, 14);
    glEnd();
	glColor3d(1,1,1);
    // إضافة علاقات الملابس
    drawHanger(-70, 67, 26);
    drawHanger(-70, 67, 22);
    drawHanger(-70, 67, 18);

	if(kind ==1){
		tshirt(1,1,1,26,tshirt1);
		tshirt(1,0,0,22,tshirt1);
		tshirt(0,0,1,18,tshirt1);
	}
	else if(kind ==4){
		tshirt(1,1,1,18,tshirt2);
		tshirt(0.5,0.5,0.5,22,tshirt2);
		tshirt(0,0,1,26,tshirt2);
	}
	else if(kind ==3){
		shirt(1,1,1,26,shirt1);
		shirt(0.5,0.5,0.5,22,shirt1);
		shirt(1,0,1,18,shirt1);
	}
	else if(kind ==6){
		shirt(1,1,1,18,shirt2);
		shirt(0.5,0.5,0.5,26,shirt2);
		shirt(1,0,1,22,shirt2);
	}
	else if(kind ==2){
		topshirt(1,1,1,18,top1);
		topshirt(0.5,0.5,0.5,22,top1);
		topshirt(0.3,0.3,0.3,26,top1);
	}
	else if(kind ==5){
		topshirt(1,1,1,26,top2);
		topshirt(1,0,0,22,top2);
		topshirt(0,0,1,18,top2);
	}
	else if(kind ==7){
		tshirt(1,1,1,26,tshirt3);
		tshirt(0.5,0.5,0.5,22,tshirt3);
		tshirt(0.3,0.3,0.3,18,tshirt3);
	}
	else if(kind ==10){
		tshirt(1,1,1,26,tshirt4);
		tshirt(0,1,0,22,tshirt4);
		tshirt(0.3,0.3,0.3,18,tshirt4);
	}
	else if(kind ==9){
		shirt(1,1,1,18,shirt3);
		shirt(0.5,0.5,0.5,26,shirt3);
		shirt(1,0,1,22,shirt3);
	}
	else if(kind ==8){
		shirt(1,1,1,18,shirt4);
		shirt(0.5,0.5,0.5,22,shirt4);
		shirt(1,0,1,26,shirt4);
	}
	else if(kind ==11){
		tshirt(1,1,1,26,tshirt5);
		tshirt(0.5,0.5,0.5,22,tshirt5);
		tshirt(0.3,0.3,0.3,18,tshirt5);
	}
	else if(kind ==12){
		topshirt(1,1,1,26,top3);
		topshirt(0.5,0.5,0.5,22,top3);
		topshirt(0.3,0.3,0.3,18,top3);
	}
    glPopMatrix();
}


void drawbag(int texture,double x ,double z){
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture);
    glBegin(GL_QUADS);
    glTexCoord2d(0, 1);
    glVertex3f(x, 30, z);
    glTexCoord2d(1, 1);
    glVertex3f(x, 30,z+10);
    glTexCoord2d(1,0);
    glVertex3f(x, 23, z+10);
    glTexCoord2d(0, 0);
    glVertex3f(x, 23, z);

    glTexCoord2d(0, 1);
    glVertex3f(x-5, 30, z);
    glTexCoord2d(1, 1);
    glVertex3f(x-5, 30,z+10);
    glTexCoord2d(1,0);
    glVertex3f(x-5, 23, z+10);
    glTexCoord2d(0, 0);
    glVertex3f(x-5, 23, z);

    glTexCoord2d(0, 1);
    glVertex3f(x, 30, z+10);
    glTexCoord2d(1, 1);
    glVertex3f(x-5, 30,z+10);
    glTexCoord2d(1,0);
    glVertex3f(x-5, 23, z+10);
    glTexCoord2d(0, 0);
    glVertex3f(x, 23, z+10);

    glTexCoord2d(0, 1);
    glVertex3f(x, 30, z);
    glTexCoord2d(1, 1);
    glVertex3f(x-5, 30,z);
    glTexCoord2d(1,0);
    glVertex3f(x-5, 23, z);
    glTexCoord2d(0, 0);
    glVertex3f(x, 23, z);

    glTexCoord2d(0, 0);
    glVertex3f(x, 30, z);
    glTexCoord2d(1, 0);
    glVertex3f(x, 30,z+10);
    glTexCoord2d(1,1);
    glVertex3f(x-5, 30, z+10);
    glTexCoord2d(0, 1);
    glVertex3f(x-5, 30, z);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // إضافة مسكة نصف دائرية مفرغة
    glPushMatrix();
	glColor3d(0,0,0);
    glTranslated((x - 2.5), 30, (z + 5));
	glRotated(90,1,0,0);
	glRotated(90,0,1,0);
    GLUquadric* quad = gluNewQuadric();
    gluQuadricDrawStyle(quad, GLU_LINE); // مفرغة
    gluPartialDisk(quad, 2.0, 2.5, 20, 1, 0, 180);
    gluDeleteQuadric(quad);
    glPopMatrix();
	
    glPopMatrix();
	glColor3d(1,1,1);
}

void drawtable2(){
	glPushMatrix();
    glScalef(35,30,47);
    // أرجل الطاولة
    glColor3f(0.3f, 0.3f, 0.3f); 
    glBegin(GL_QUADS);
    glVertex3f(-0.55f, 0.0f, 0.55f);
    glVertex3f(-0.5f, 0.0f, 0.55f);
    glVertex3f(-0.5f, 0.7f, 0.55f);
    glVertex3f(-0.55f, 0.7f, 0.55f);
    
    glVertex3f(0.55f, 0.0f, 0.55f);
    glVertex3f(0.5f, 0.0f, 0.55f);
    glVertex3f(0.5f, 0.7f, 0.55f);
    glVertex3f(0.55f, 0.7f, 0.55f);

    glVertex3f(-0.55f, 0.0f, -0.55f);
    glVertex3f(-0.5f, 0.0f, -0.55f);
    glVertex3f(-0.5f, 0.7f, -0.55f);
    glVertex3f(-0.55f, 0.7f, -0.55f);
    glVertex3f(0.55f, 0.0f, -0.55f);
    glVertex3f(0.5f, 0.0f, -0.55f);
    glVertex3f(0.5f, 0.7f, -0.55f);
    glVertex3f(0.55f, 0.7f, -0.55f);
    glEnd();

    // سطح الطاولة
    glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,table33);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
    glVertex3f(-0.55f, 0.7f, -0.55f);
	glTexCoord2d(1, 0);
    glVertex3f(0.55f, 0.7f, -0.55f);
	glTexCoord2d(1, 1);
    glVertex3f(0.55f, 0.7f, 0.55f);
	glTexCoord2d(0, 1);
    glVertex3f(-0.55f, 0.7f, 0.55f);
    glEnd();
	glDisable(GL_TEXTURE_2D);
	
	glPopMatrix();
	
}

void changingRoom(){
	glPushMatrix();
	glColor3d(1,1,1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,elevatordoor);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
    glVertex3f(65, 65, r);
	glTexCoord2d(1, 0);
    glVertex3f(65, 65,45);
	glTexCoord2d(1,1);
    glVertex3f(65, 2, 45);
	glTexCoord2d(0, 1);
    glVertex3f(65, 2, r);
    glEnd();
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,shop2wall);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
    glVertex3f(65, 65, 45);
	glTexCoord2d(1, 0);
    glVertex3f(115, 65,45);
	glTexCoord2d(1,1);
    glVertex3f(115, 2, 45);
	glTexCoord2d(0, 1);
    glVertex3f(65, 2, 45);

	glTexCoord2d(0, 0);
    glVertex3f(65, 65, 15);
	glTexCoord2d(1, 0);
    glVertex3f(65, 65,45);
	glTexCoord2d(1,1);
    glVertex3f(115, 65, 45);
	glTexCoord2d(0, 1);
    glVertex3f(115, 65, 15);
    glEnd();
	glDisable(GL_TEXTURE_2D);
	//علاقة
	glColor3d(0.3, 0.3, 0.3);
	glBegin(GL_QUADS);
	glVertex3d(114, 55, 18);
	glVertex3d(114, 55, 42);
	glVertex3d(114, 50, 42);
	glVertex3d(114, 50, 18);
	glEnd();
	glColor3f(0.2, 0.2, 0.2); 
	GLUquadric* quad1 = gluNewQuadric();
	GLUquadric* quad2 = gluNewQuadric();
	glPushMatrix();
	glTranslated(113, 52.5, 24); 
	gluSphere(quad1, 1.5, 20, 20); 
	glPopMatrix();
	glPushMatrix();
	glTranslated(113, 52.5, 36); 
	gluSphere(quad2, 1.5, 20, 20); 
	glPopMatrix();
	gluDeleteQuadric(quad1);
	gluDeleteQuadric(quad2);
	glColor3d(1, 1, 1);

	glPopMatrix();
}

void jeans(double c1,double c2,double c3,float y ,float z){
	glPushMatrix();
	glColor3d(c1,c2,c3);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,jeans1);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 1);
    glVertex3f(100, y+4, z);
	glTexCoord2d(1, 1);
    glVertex3f(100, y+4,z+15);
	glTexCoord2d(1,0);
    glVertex3f(100, y+0.1, z+15);
	glTexCoord2d(0, 0);
    glVertex3f(100, y+0.1, z);
    glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void wardrobe(){
	glPushMatrix();
	glColor3d(1,1,1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,wardrobe1);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
    glVertex3f(95, 65, 55);
	glTexCoord2d(1, 0);
    glVertex3f(110, 65,55);
	glTexCoord2d(1,1);
    glVertex3f(110, 2, 55);
	glTexCoord2d(0, 1);
    glVertex3f(95, 2, 55);

	glTexCoord2d(0, 0);
    glVertex3f(95, 65, 115);
	glTexCoord2d(1, 0);
    glVertex3f(110, 65,115);
	glTexCoord2d(1,1);
    glVertex3f(110, 2, 115);
	glTexCoord2d(0, 1);
    glVertex3f(95, 2, 115);

	glTexCoord2d(0, 0);
    glVertex3f(95, 65, 58);
	glTexCoord2d(1, 0);
    glVertex3f(95, 65,55);
	glTexCoord2d(1,1);
    glVertex3f(95, 2, 55);
	glTexCoord2d(0, 1);
    glVertex3f(95, 2, 58);

	glTexCoord2d(0, 0);
    glVertex3f(95, 65, 115);
	glTexCoord2d(1, 0);
    glVertex3f(95, 65,112);
	glTexCoord2d(1,1);
    glVertex3f(95, 2, 112);
	glTexCoord2d(0, 1);
    glVertex3f(95, 2, 115);

	glTexCoord2d(0, 0);
    glVertex3f(95, 65, 55);
	glTexCoord2d(1, 0);
    glVertex3f(95, 65,115);
	glTexCoord2d(1,1);
    glVertex3f(111, 65, 115);
	glTexCoord2d(0, 1);
    glVertex3f(110, 65, 55);

	glTexCoord2d(0, 0);
    glVertex3f(95, 50, 55);
	glTexCoord2d(1, 0);
    glVertex3f(95,50,115);
	glTexCoord2d(1,1);
    glVertex3f(111, 50, 115);
	glTexCoord2d(0, 1);
    glVertex3f(110, 50, 55);

	glTexCoord2d(0, 0);
    glVertex3f(95, 35, 55);
	glTexCoord2d(1, 0);
    glVertex3f(95, 35,115);
	glTexCoord2d(1,1);
    glVertex3f(111, 35, 115);
	glTexCoord2d(0, 1);
    glVertex3f(110, 35, 55);

	glTexCoord2d(0, 0);
    glVertex3f(95, 20, 55);
	glTexCoord2d(1, 0);
    glVertex3f(95, 20,115);
	glTexCoord2d(1,1);
    glVertex3f(111, 20, 115);
	glTexCoord2d(0, 1);
    glVertex3f(110, 20, 55);

	glTexCoord2d(0, 0);
    glVertex3f(95, 20, 55);
	glTexCoord2d(1, 0);
    glVertex3f(95, 20,115);
	glTexCoord2d(1,1);
    glVertex3f(95, 19, 115);
	glTexCoord2d(0, 1);
    glVertex3f(95, 19, 55);

	glTexCoord2d(0, 0);
    glVertex3f(95, 35, 55);
	glTexCoord2d(1, 0);
    glVertex3f(95, 35,115);
	glTexCoord2d(1,1);
    glVertex3f(95, 34, 115);
	glTexCoord2d(0, 1);
    glVertex3f(95, 34, 55);

	glTexCoord2d(0, 0);
    glVertex3f(95, 50, 55);
	glTexCoord2d(1, 0);
    glVertex3f(95, 50,115);
	glTexCoord2d(1,1);
    glVertex3f(95, 49, 115);
	glTexCoord2d(0, 1);
    glVertex3f(95, 49, 55);
    glEnd();
	glDisable(GL_TEXTURE_2D);
	jeans(1,1,1,20,60);
	jeans(0,0,0.3,24,60);
	jeans(0,0,0.1,28,60);

	jeans(0,0,0.5,20,77);
	jeans(1,1,1,24,77);
	jeans(0.5,0.5,0.5,28,77);

	jeans(0,0,0.7,20,94);
	jeans(0.3,0.3,0.3,24,94);
	jeans(0.8,0.8,0.8,28,94);

	jeans(0,0,0.7,35,60);
	jeans(0.3,0.3,0.3,39,60);
	jeans(0.8,0.8,0.8,43,60);

	jeans(1,1,1,35,77);
	jeans(0,0,0.3,39,77);
	jeans(0,0,0.1,43,77);

	jeans(0,0,0.5,35,94);
	jeans(1,1,1,39,94);
	jeans(0.5,0.5,0.5,43,94);

	jeans(0,0,0.2,50,60);
	jeans(0.6,0.6,0.6,54,60);
	jeans(0.8,0.8,0.8,58,60);

	jeans(1,1,1,50,77);
	jeans(0,0,0.1,54,77);
	jeans(0,0,0.5,58,77);

	jeans(0,0,0.9,50,94);
	jeans(1,1,1,54,94);
	jeans(0.6,0.6,0.6,58,94);
	glPopMatrix();
}

void drawsign2(){
	glPushMatrix();
	glColor3d(1,1,1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,sign2);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 1);
    glVertex3f(-83, 85, 9);
	glTexCoord2d(1, 1);
    glVertex3f(-118, 85,9);
	glTexCoord2d(1,0);
    glVertex3f(-118, 65, 9);
	glTexCoord2d(0, 0);
    glVertex3f(-83, 65, 9);
    glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void drawpic2(){
	glPushMatrix();
	glColor3d(1,1,1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,pic2);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 1);
    glVertex3f(95, 80, 9);
	glTexCoord2d(1, 1);
    glVertex3f(-70, 80,9);
	glTexCoord2d(1,0);
    glVertex3f(-70, 20, 9);
	glTexCoord2d(0, 0);
    glVertex3f(95, 20, 9);
    glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void shop2(){
	drawbag(bag4,-100,85);
	drawbag(bag2,-100,70);
	drawbag(bag3,-100,55);
	drawbag(bag1,-80,85);
	drawbag(bag5,-80,70);
	drawbag(bag6,-80,55);
	drawpic2();
	drawsign2();
	wardrobe();
	changingRoom();
	stick(1);
	glPushMatrix();
	glTranslated(0,0,85);
	stick(2);
	glTranslated(0,-30,0);
	stick(8);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0,-30,0);
	stick(7);
	glPopMatrix();
	glPushMatrix();
	glTranslated(40,0,0);
	stick(3);
	glPushMatrix();
	glTranslated(0,-30,0);
	stick(12);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0,0,85);
	stick(4);
	glTranslated(0,-30,0);
	stick(9);
	glPopMatrix();
	glTranslated(40,0,0);
	stick(5);
	glPushMatrix();
	glTranslated(0,-30,0);
	stick(11);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0,0,85);
	stick(6);
	glTranslated(0,-30,0);
	stick(10);
	glPopMatrix();
	glPopMatrix();
	glPushMatrix();
	glTranslated(-95,2,73);
	drawtable2();
	glPopMatrix();
	glPushMatrix();
	glTranslated(60,2,80);
	glRotated(90,0,1,0);
	drawCashierCounter2(table33,table1);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-82.5,2,0);
	door1();
	glPopMatrix();
}

void glass(double z1,double z2){
	glPushMatrix();
	glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    glColor4f(1.0, 1.0, 1.0, 0.6);
    glBegin(GL_QUADS); 
    glVertex3f(-200, 180, z1); 
    glVertex3f(-200, 180, z2);
    glVertex3f(-200, 100, z2); 
    glVertex3f(-200, 100, z1); 
    glEnd(); 
    glDisable(GL_BLEND);
	glPopMatrix();
}

void fence1(double x,double z1,double z2){
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,iron);
    glBegin(GL_QUADS);
    glTexCoord2d(0, 1); glVertex3f(x, 121, z1);
    glTexCoord2d(0, 0); glVertex3f(x, 121, z2);
    glTexCoord2d(1, 0); glVertex3f(x, 120, z2);
    glTexCoord2d(1, 1); glVertex3f(x, 120, z1);

	glTexCoord2d(0, 1); glVertex3f(x, 116, z1);
    glTexCoord2d(0, 0); glVertex3f(x, 116, z2);
    glTexCoord2d(1, 0); glVertex3f(x, 115, z2);
    glTexCoord2d(1, 1); glVertex3f(x, 115, z1);

	glTexCoord2d(0, 0); glVertex3f(x, 122, z1-1);
    glTexCoord2d(1, 0); glVertex3f(x, 122, z1);
    glTexCoord2d(1, 1); glVertex3f(x, 102, z1);
    glTexCoord2d(0, 1); glVertex3f(x, 102, z1-1);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void fence2(double x1,double x2,double z) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,iron);
    glBegin(GL_QUADS);
    glTexCoord2d(0, 1); glVertex3f(x1, 121, z);
    glTexCoord2d(0, 0); glVertex3f(x2, 121, z);
    glTexCoord2d(1, 0); glVertex3f(x2, 120, z);
    glTexCoord2d(1, 1); glVertex3f(x1, 120, z);

	glTexCoord2d(0, 1); glVertex3f(x1, 116, z);
    glTexCoord2d(0, 0); glVertex3f(x2, 116, z);
    glTexCoord2d(1, 0); glVertex3f(x2, 115, z);
    glTexCoord2d(1, 1); glVertex3f(x1, 115, z);

	glTexCoord2d(0, 0); glVertex3f(x1-1, 122, z);
    glTexCoord2d(1, 0); glVertex3f(x1, 122, z);
    glTexCoord2d(1, 1); glVertex3f(x1, 102, z);
    glTexCoord2d(0, 1); glVertex3f(x1-1, 102, z);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void eleveter(){
    glPushMatrix();
    glColor3d(0.7,0.7,0.7);
    glTranslated(0,m,0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,ele);
    glBegin(GL_QUADS);
    glTexCoord2d(0, 0); glVertex3f(85, 2, -200);
    glTexCoord2d(1, 0); glVertex3f(85, 2, -142);
    glTexCoord2d(1, 1); glVertex3f(85, 75, -142);
    glTexCoord2d(0, 1); glVertex3f(85, 75, -200);
    glTexCoord2d(0, 0); glVertex3f(116, 2, -200);
    glTexCoord2d(1, 0); glVertex3f(116, 2, -142);
    glTexCoord2d(1, 1); glVertex3f(116, 75, -142);
    glTexCoord2d(0, 1); glVertex3f(116, 75, -200);
    glTexCoord2d(0, 0); glVertex3f(85, 2, -200);
    glTexCoord2d(1, 0); glVertex3f(85, 75, -200);
    glTexCoord2d(1, 1); glVertex3f(116, 75, -200);
    glTexCoord2d(0, 1); glVertex3f(116, 2, -200);
    glTexCoord2d(0, 0); glVertex3f(85, 75, -200);
    glTexCoord2d(1, 0); glVertex3f(85 ,75, -142);
    glTexCoord2d(1, 1); glVertex3f(116, 75, -142);
    glTexCoord2d(0, 1); glVertex3f(116, 75 ,-200);
    glTexCoord2d(0, 0); glVertex3f(85, 2, -200);
    glTexCoord2d(1, 0); glVertex3f(85, 2, -142);
    glTexCoord2d(1, 1); glVertex3f(116, 2, -142);
    glTexCoord2d(0, 1); glVertex3f(116, 2 ,-200);
    glEnd();
    glColor3d(1,1,1);
    glPopMatrix();
}


void drawStairs(float x, float y, float z) {
    glScaled(3, 3, 3);
    float stepWidth = 14.0f; // عرض الدرجة
    float stepHeight = 3.0f; // ارتفاع الدرجة
    float stepDepth = 2.2f; // عمق الدرجة
    int numSteps = 10;
    float railHeight = 6.0f; // قيمة جديدة لزيادة طول الممسكات
    for (int i = 0; i < numSteps; ++i) {
        glPushMatrix();
        glTranslatef(x, y + i * stepHeight, z - i * stepDepth);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, column);
        glBegin(GL_QUADS);

        // رسم الوجه العلوي
        glBegin(GL_QUADS);
        glTexCoord2d(0, 0);
        glVertex3f(0.0f, stepHeight, 0.0f);
        glTexCoord2d(1, 0);
        glVertex3f(stepWidth, stepHeight, 0.0f);
        glTexCoord2d(1, 1);
        glVertex3f(stepWidth, stepHeight, -stepDepth);
        glTexCoord2d(0, 1);
        glVertex3f(0.0f, stepHeight, -stepDepth);
        glEnd();

        // رسم الوجه الأمامي
        glBegin(GL_QUADS);
        glTexCoord2d(0, 0);
        glVertex3f(0.0f, 0.0f, -stepDepth);
        glTexCoord2d(1, 0);
        glVertex3f(stepWidth, 0.0f, -stepDepth);
        glTexCoord2d(1, 1);
        glVertex3f(stepWidth, stepHeight, -stepDepth);
        glTexCoord2d(0, 1);
        glVertex3f(0.0f, stepHeight, -stepDepth);
        glEnd();

        // رسم الوجه الجانبي
        glBegin(GL_QUADS);
        glTexCoord2d(0, 0);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glTexCoord2d(1, 0);
        glVertex3f(0.0f, 0.0f, -stepDepth);
        glTexCoord2d(1, 1);
        glVertex3f(0.0f, stepHeight, -stepDepth);
        glTexCoord2d(0, 1);
        glVertex3f(0.0f, stepHeight, 0.0f);
        glEnd();

        // رسم الوجه الجانبي للطرف الثاني
        glBegin(GL_QUADS);
        glTexCoord2d(0, 0);
        glVertex3f(stepWidth, 0.0f, 0.0f);
        glTexCoord2d(1, 0);
        glVertex3f(stepWidth, 0.0f, -stepDepth);
        glTexCoord2d(1, 1);
        glVertex3f(stepWidth, stepHeight, -stepDepth);
        glTexCoord2d(0, 1);
        glVertex3f(stepWidth, stepHeight, 0.0f);
        glEnd();
        glDisable(GL_TEXTURE_2D);

        // رسم الممسكات
        // عمود الممسكة الأمامي الأيسر
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, iron);
        glBegin(GL_QUADS);
        glTexCoord2d(0, 0);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glTexCoord2d(1, 0);
        glVertex3f(0.3f, 0.0f, 0.0f);
        glTexCoord2d(1, 1);
        glVertex3f(0.3f, railHeight, 0.0f);
        glTexCoord2d(0, 1);
        glVertex3f(0.0f, railHeight, 0.0f);

        // عمود الممسكة الأمامي الأيمن
        glTexCoord2d(0, 0);
        glVertex3f(stepWidth - 0.3f, 0.0f, 0.0f);
        glTexCoord2d(1, 0);
        glVertex3f(stepWidth, 0.0f, 0.0f);
        glTexCoord2d(1, 1);
        glVertex3f(stepWidth, railHeight, 0.0f);
        glTexCoord2d(0, 1);
        glVertex3f(stepWidth - 0.3f, railHeight, 0.0f);

        // عمود الممسكة الخلفي الأيسر
        glTexCoord2d(0, 0);
        glVertex3f(0.0f, 0.0f, -stepDepth);
        glTexCoord2d(1, 0);
        glVertex3f(0.3f, 0.0f, -stepDepth);
        glTexCoord2d(1, 1);
        glVertex3f(0.3f, railHeight, -stepDepth);
        glTexCoord2d(0, 1);
        glVertex3f(0.0f, railHeight, -stepDepth);

        // عمود الممسكة الخلفي الأيمن
        glTexCoord2d(0, 0);
        glVertex3f(stepWidth - 0.3f, 0.0f, -stepDepth);
        glTexCoord2d(1, 0);
        glVertex3f(stepWidth, 0.0f, -stepDepth);
        glTexCoord2d(1, 1);
        glVertex3f(stepWidth, railHeight, -stepDepth);
        glTexCoord2d(0, 1);
        glVertex3f(stepWidth - 0.3f, railHeight, -stepDepth);
        glEnd();

        glPopMatrix();
    }

    // رسم عمود مائل يجمع الممسكات على مستوى الممسكات
    float startHeight = railHeight; // تحديث قيمة startHeight
    glPushMatrix();
    glTranslatef(x, y, z);
    glBegin(GL_QUADS);
    glTexCoord2d(0, 0);
    glVertex3f(0.0f, startHeight, 0.0f);
    glTexCoord2d(1, 0);
    glVertex3f(0.3f, startHeight, 0.0f);
    glTexCoord2d(1, 1);
    glVertex3f(0.3f, startHeight + (numSteps - 1) * stepHeight, -stepDepth * (numSteps - 1));
    glTexCoord2d(0, 1);
    glVertex3f(0.0f, startHeight + (numSteps - 1) * stepHeight, -stepDepth * (numSteps - 1));
    glTexCoord2d(0, 0);
    glVertex3f(stepWidth - 0.3f, startHeight, 0.0f);
    glTexCoord2d(1, 0);
    glVertex3f(stepWidth, startHeight, 0.0f);
    glTexCoord2d(1, 1);
    glVertex3f(stepWidth, startHeight + (numSteps - 1) * stepHeight, -stepDepth * (numSteps - 1));
    glTexCoord2d(0, 1);
    glVertex3f(stepWidth - 0.3f, startHeight + (numSteps - 1) * stepHeight, -stepDepth * (numSteps - 1));
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}


// shop1 left

//door
void Door1() {

  // right door
    glPushMatrix(); 
    glScaled(2.5,2.5,2.5);
    glTranslated(15, 0, 5);
    glRotated(-an, 0, 1, 0); 
    glTranslated(-15, 0, -5);
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    glColor4f(1.0, 1.0, 1.0, 0.6); 
    glBegin(GL_QUADS); 
    glVertex3f(15, 0, 5); 
    glVertex3f(15, 15.5, 5);
    glVertex3f(0.1, 15.5, 5); 
    glVertex3f(0.1, 0, 5); 
    glEnd(); 
    glDisable(GL_BLEND); 
    // Right door handle
    glColor3f(0.2, 0.2, 0.2); // Darker color
  GLUquadric* quad;
    quad = gluNewQuadric();
    glPushMatrix();
    glTranslated(1.0, 7.75, 5.1);
    gluSphere(quad, 0.5, 20, 20); // Smaller handle
    glPopMatrix();
    gluDeleteQuadric(quad);
  glColor3d(1,1,1);
  glPopMatrix();
}

void drawLShapedTable() {
    glPushMatrix(); 
    glColor3d(1,1,1);
    glEnable(GL_TEXTURE_2D); // قم بتمكين الtextures

    // تعيين الملمس للطاولة 
    glBindTexture(GL_TEXTURE_2D, table2);

    glBegin(GL_QUADS); 
    
    // سطح الطاولة
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.0f, 0.0f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(2.0f, 0.0f, 0.5f); 
    glTexCoord2f(1.0f, 1.0f); glVertex3f(2.0f, 1.0f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-2.0f, 1.0f, 0.5f);
    
    // الجانب السفلي للطاولة
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.0f, 0.0f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(2.0f, 0.0f, -0.5f); 
    glTexCoord2f(1.0f, 1.0f); glVertex3f(2.0f, 1.0f, -0.5f); 
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-2.0f, 1.0f, -0.5f);
    
    // الجوانب للطاولة
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.0f, 0.0f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-2.0f, 1.0f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-2.0f, 1.0f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-2.0f, 0.0f, -0.5f);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(2.0f, 0.0f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(2.0f, 1.0f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(2.0f, 1.0f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(2.0f, 0.0f, -0.5f);

    // إضافة سطح علوي للطاولة
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.0f, 1.0f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(2.0f, 1.0f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(2.0f, 1.0f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-2.0f, 1.0f, -0.5f);

    glEnd(); 

    glDisable(GL_TEXTURE_2D); // قم بتعطيل الtextures

    glPopMatrix();
}

void drawCubeWithTexture(float width, float height, float depth) { glPushMatrix();
 glScalef(width, height, depth);
 glEnable(GL_TEXTURE_2D);
 glBindTexture(GL_TEXTURE_2D, table); // ربط الإكساء
 glBegin(GL_QUADS);
 // الوجه الأمامي
 glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
 glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f); 
 glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
 glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
 // الوجه الخلفي
 glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
 glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f); 
 glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
 glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
 // الوجه العلوي 
 glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
 glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.5f, 0.5f); 
 glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
 glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
 // الوجه السفلي 
 glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
 glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
 glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, -0.5f, 0.5f);
 glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
 // الوجه الأيمن
 glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
 glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f); 
 glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
 glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
 // الوجه الأيسر 
 glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
 glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
 glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);
 glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
 glEnd();
 glDisable(GL_TEXTURE_2D);
 glColor3d(1,1,1);

 glPopMatrix();
}

void DrawChair() {
 
float legHeight = 1.0f;


glPushMatrix();
glTranslatef(0.0f, legHeight + 0.05f, 0.0f);
drawCubeWithTexture(1.0f, 0.1f, 1.0f);
glPopMatrix();


glPushMatrix();
glTranslatef(0.4f, legHeight / 2, 0.4f);
drawCubeWithTexture(0.1f, legHeight, 0.1f);
glPopMatrix();


glPushMatrix();
glTranslatef(-0.4f, legHeight / 2, 0.4f);
drawCubeWithTexture(0.1f, legHeight, 0.1f);
glPopMatrix();

glPushMatrix();
glTranslatef(0.4f, legHeight / 2, -0.4f);
drawCubeWithTexture(0.1f, legHeight, 0.1f);
glPopMatrix();


glPushMatrix();
glTranslatef(-0.4f, legHeight / 2, -0.4f);
drawCubeWithTexture(0.1f, legHeight, 0.1f);
glPopMatrix();

glPushMatrix();
glTranslatef(0.0f, legHeight + 0.5f, -0.4f);
drawCubeWithTexture(1.0f, 0.8f, 0.1f);
glPopMatrix();


}

void drawCashierCounter() {
    glPushMatrix();
    
    glEnable(GL_TEXTURE_2D); // قم بتمكين الtextures

    // تعيين الملمس للطاولة 
    glBindTexture(GL_TEXTURE_2D, table3);

    glBegin(GL_QUADS); 
    
    // الجدار الأمامي 
    glTexCoord2f(0,0); glVertex3f(-1.5f, 0.0f, 0.5f);
    glTexCoord2f(1,0); glVertex3f(1.5f, 0.0f, 0.5f); 
    glTexCoord2f(1,1); glVertex3f(1.5f, 1.0f, 0.5f); 
    glTexCoord2f(0,1); glVertex3f(-1.5f, 1.0f, 0.5f);
    
    // الجدار الخلفي
    glTexCoord2f(0,0); glVertex3f(-1.5f, 0.0f, -0.5f); 
    glTexCoord2f(1,0); glVertex3f(1.5f, 0.0f, -0.5f);
    glTexCoord2f(1,1); glVertex3f(1.5f, 1.0f, -0.5f); 
    glTexCoord2f(0,1); glVertex3f(-1.5f, 1.0f, -0.5f);
    
    // الجدار الأيسر
    glTexCoord2f(0,0); glVertex3f(-1.5f, 0.0f, -0.5f); 
    glTexCoord2f(1,0); glVertex3f(-1.5f, 0.0f, 0.5f); 
    glTexCoord2f(1,1); glVertex3f(-1.5f, 1.0f, 0.5f); 
    glTexCoord2f(0,1); glVertex3f(-1.5f, 1.0f, -0.5f); 
    
    // الجدار الأيمن 
    glTexCoord2f(0,0); glVertex3f(1.5f, 0.0f, -0.5f);
    glTexCoord2f(1,0); glVertex3f(1.5f, 0.0f, 0.5f); 
    glTexCoord2f(1,1); glVertex3f(1.5f, 1.0f, 0.5f); 
    glTexCoord2f(0,1); glVertex3f(1.5f, 1.0f, -0.5f); 
    
    glEnd();
    
    glDisable(GL_TEXTURE_2D); // قم بتعطيل الtextures
	// سطح الكاشير 
    glEnable(GL_TEXTURE_2D); // تمكين الtextures مرة أخرى
    glBindTexture(GL_TEXTURE_2D, table3);
    
    glBegin(GL_QUADS); 
    glTexCoord2f(0,0); glVertex3f(-1.5f, 1.0f, 0.5f); 
    glTexCoord2f(1,0); glVertex3f(1.5f, 1.0f, 0.5f); 
    glTexCoord2f(1,1); glVertex3f(1.5f, 1.0f, -0.5f);
    glTexCoord2f(0,1); glVertex3f(-1.5f, 1.0f, -0.5f); 
    glEnd(); 
    glDisable(GL_TEXTURE_2D); // تعطيل الtextures مرة أخرى
    
    // الرف الخلفي للكاشير 
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,table3);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3f(-1.5f, 1.0f, -0.5f); 
    glTexCoord2f(1,0); glVertex3f(1.5f, 1.0f, -0.5f); 
    glTexCoord2f(1,1); glVertex3f(1.5f, 1.5f, -0.5f); 
    glTexCoord2f(0,1); glVertex3f(-1.5f, 1.5f, -0.5f); 
    glTexCoord2f(0,0); glVertex3f(-1.5f, 1.5f, -0.5f);
    glTexCoord2f(1,0); glVertex3f(-1.5f, 1.5f, 0.0f);
    glTexCoord2f(1,1); glVertex3f(1.5f, 1.5f, 0.0f);
    glTexCoord2f(0,1); glVertex3f(1.5f, 1.5f, -0.5f); 
    glEnd(); 
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void drawCylinder(float radius, float height) {
  const int NUM_SEGMENTS = 36;
  glPushMatrix(); 
  glBindTexture(GL_TEXTURE_2D, table2);
  // رسم الغطاء العلوي
  glBegin(GL_TRIANGLE_FAN);
  glTexCoord2f(0.5f, 0.5f);
  glVertex3f(0.0f, height / 2.0f, 0.0f); 
  // مركز الغطاء العلوي 
  for (int i = 0; i <= NUM_SEGMENTS; ++i) { 
    float angle = 2.0f * 3.14* float(i) / float(NUM_SEGMENTS); 
    float x = radius * cosf(angle); 
    float z = radius * sinf(angle); glVertex3f(x, height / 2.0f, z); 
  }  glEnd(); 
  // رسم الغطاء السفلي 
  glBegin(GL_TRIANGLE_FAN);
  glTexCoord2f(0.5f, 0.5f);
  glVertex3f(0.0f, -height / 2.0f, 0.0f);
  // مركز الغطاء السفلي 
  for (int i = 0; i <= NUM_SEGMENTS; ++i) { 
    float angle = 2.0f * 3.14 * float(i) / float(NUM_SEGMENTS); 
    float x = radius * cosf(angle);
    float z = radius * sinf(angle); 
    glTexCoord2f(0.5f + 0.5f * cosf(angle), 0.5f + 0.5f * sinf(angle));
    glVertex3f(x, -height / 2.0f, z); 
  }   glEnd(); 
  // رسم الجوانب 
  glBegin(GL_QUAD_STRIP); 
  for (int i = 0; i <= NUM_SEGMENTS; ++i) {
    float angle = 2.0f * 3.14 * float(i) / float(NUM_SEGMENTS); 
    float x = radius * cosf(angle); 
    float z = radius * sinf(angle);
    glTexCoord2f(float(i) / float(NUM_SEGMENTS), 1.0f);
    glVertex3f(x, height / 2.0f, z);
    glTexCoord2f(float(i) / float(NUM_SEGMENTS), 0.0f);
    glVertex3f(x, -height / 2.0f, z); }
      glEnd();
    glPopMatrix(); 

}


void DrawTable() {
    glPushMatrix();
    
    glEnable(GL_TEXTURE_2D); // تمكين الtextures
    glBindTexture(GL_TEXTURE_2D, table2);
	// أرجل الطاولة 
    glColor3f(0.3f, 0.3f, 0.3f); 
    glBegin(GL_QUADS);
    // رجل أمامية يسار
    glVertex3f(-0.55f, 0.0f, 0.55f);
    glVertex3f(-0.5f, 0.0f, 0.55f);
    glVertex3f(-0.5f, 0.7f, 0.55f);
    glVertex3f(-0.55f, 0.7f, 0.55f);
    // رجل أمامية يمين
    glVertex3f(0.55f, 0.0f, 0.55f);
    glVertex3f(0.5f, 0.0f, 0.55f);
    glVertex3f(0.5f, 0.7f, 0.55f);
    glVertex3f(0.55f, 0.7f, 0.55f);
    // أرجل خلفية
    glVertex3f(-0.55f, 0.0f, -0.55f);
    glVertex3f(-0.5f, 0.0f, -0.55f);
    glVertex3f(-0.5f, 0.7f, -0.55f);
    glVertex3f(-0.55f, 0.7f, -0.55f);
    glVertex3f(0.55f, 0.0f, -0.55f);
    glVertex3f(0.5f, 0.0f, -0.55f);
    glVertex3f(0.5f, 0.7f, -0.55f);
    glVertex3f(0.55f, 0.7f, -0.55f);
    glEnd();

    // سطح الطاولة
    glBegin(GL_QUADS);
    glTexCoord2d(0,0); glVertex3f(-0.55f, 0.7f, -0.55f);
    glTexCoord2d(1,0); glVertex3f(0.55f, 0.7f, -0.55f);
    glTexCoord2d(1,1); glVertex3f(0.55f, 0.7f, 0.55f);
    glTexCoord2d(0,1); glVertex3f(-0.55f, 0.7f, 0.55f);
    glEnd();

    glDisable(GL_TEXTURE_2D); // تعطيل الtextures

    // إضافة ظل نصف شفاف 
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    glColor4f(0.6f, 0.6f, 0.6f, 0.5f); // لون رمادي شفاف 
    glBegin(GL_QUADS);
    glVertex3f(-0.6f, 0.0f, -0.6f);
    glVertex3f(0.6f, 0.0f, -0.6f); 
    glVertex3f(0.6f, 0.0f, 0.6f);
    glVertex3f(-0.6f, 0.0f, 0.6f); 
    glEnd(); 
    glDisable(GL_BLEND);
  glColor3d(1,1,1);
    glPopMatrix();
}


void JuiceMachine() {
    // Bind the texture
  glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, drinks1);

    glPushMatrix();
    glBegin(GL_QUADS);

    // Front Face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);

    // Back Face
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);

    // Top Face
     glVertex3f(-1.0f,  1.0f, -1.0f);
     glVertex3f(-1.0f,  1.0f,  1.0f);
     glVertex3f( 1.0f,  1.0f,  1.0f);
     glVertex3f( 1.0f,  1.0f, -1.0f);

    // Bottom Face
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);

    // Right face
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);

    // Left Face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);

    glEnd();
  glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}
void ColaMachine() {
    // Bind the texture
  glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, drinks);

    glPushMatrix();
    glBegin(GL_QUADS);

    // Front Face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);

    // Back Face
  glColor3d(0.4,0.4,0.4);
     glVertex3f(-1.0f, -1.0f, -1.0f);
     glVertex3f(-1.0f,  1.0f, -1.0f);
     glVertex3f( 1.0f,  1.0f, -1.0f);
     glVertex3f( 1.0f, -1.0f, -1.0f);

    // Top Face
     glVertex3f(-1.0f,  1.0f, -1.0f);
     glVertex3f(-1.0f,  1.0f,  1.0f);
     glVertex3f( 1.0f,  1.0f,  1.0f);
     glVertex3f( 1.0f,  1.0f, -1.0f);
	 // Bottom Face
     glVertex3f(-1.0f, -1.0f, -1.0f);
     glVertex3f( 1.0f, -1.0f, -1.0f);
     glVertex3f( 1.0f, -1.0f,  1.0f);
     glVertex3f(-1.0f, -1.0f,  1.0f);

    // Right face
     glVertex3f( 1.0f, -1.0f, -1.0f);
     glVertex3f( 1.0f,  1.0f, -1.0f);
     glVertex3f( 1.0f,  1.0f,  1.0f);
     glVertex3f( 1.0f, -1.0f,  1.0f);

    // Left Face
     glVertex3f(-1.0f, -1.0f, -1.0f);
     glVertex3f(-1.0f, -1.0f,  1.0f);
     glVertex3f(-1.0f,  1.0f,  1.0f);
     glVertex3f(-1.0f,  1.0f, -1.0f);

    glEnd();
  glColor3d(1,1,1);
  glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}


void drawRestaurant1()  {
  glPushMatrix();

  glPushMatrix();
  glTranslated(-160,2,70);
  glScaled(10,10,10);
  glRotated(90,0,1,0);
  drawCashierCounter();
  glPopMatrix();

  glPushMatrix();
  glTranslated(-160,2,10);
  glScaled(10,10,10);
  glRotated(90,0,1,0);
  drawLShapedTable();
  glPushMatrix();
  glTranslated(2,0,0);
  glRotated(180,0,1,0);
  drawLShapedTable();
  glPopMatrix();
  glPopMatrix();

  glPushMatrix();
  float tablePositions[6][3] = {
    {-220, 2, -50},
    {-200, 2, -50},
    {-220, 2, 50},
    {-200, 2, 50},
    {-220, 2, 0},
    {-200, 2, 0}
  };
  for (int i = 0; i < 6; i++) {
    double x = tablePositions[i][0]; 
    double z = tablePositions[i][2];
    // رسم الطاولة 
    glPushMatrix(); 
    glTranslated(x, 2, z);
    glScaled(10,10,10);
    DrawTable();
    DrawGlass(0.0f, 0.7f, 0.2f, 0.05f, 0.2f);
    DrawGlass(0.0f, 0.7f, -0.2f, 0.05f, 0.2f);
    glPopMatrix();

    // رسم الكرسي الأول
    glPushMatrix();
    glTranslated(x, 2, z-15); 
    glScaled(5, 5, 5);
    DrawChair(); 
    glPopMatrix();

    // رسم الكرسي الثاني 
    glPushMatrix(); 
    glTranslated(x, 2, z+15);
    glRotated(180, 0, 1, 0);
    glScaled(5, 5, 5);
    DrawChair(); 
    glPopMatrix();
  }

    glPushMatrix();
    glScaled(2,2,2);
    glTranslated(10,5,-50);
    DrawGlass(0.0f, 0.0f, 0.0f, 0.05f, 0.2f);
    glPopMatrix();

  glPopMatrix();

  glPushMatrix();
  glTranslated(-215,20,110);
  glScaled(8,20,8);
  JuiceMachine();
  glPopMatrix();

  glPushMatrix();
  glTranslated(-160,15,-50);
  glScaled(10,15,10);
  glRotated(-90,0,1,0);
  ColaMachine();
  glPopMatrix();

  for (int i = 0; i < 5; ++i) {
    glPushMatrix();
    glColor3f(0.5f, 0.2f, 0.5f);
    glTranslated(-170, 2,  i * 10 - 20);
    glScaled(3, 4, 3); 
    drawCylinder(1.0f, 3.0f); 
    glPopMatrix();
  }
   glPushMatrix();
   glTranslated(-200, 2, 108);
    Door1();
  glPopMatrix();
  
  glColor3d(1,1,1);

  glPopMatrix();
}
void machine1(int image) {
  // Bind the texture
  glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, image);

    glPushMatrix();
    glBegin(GL_QUADS);

    // Front Face
  glColor3d(0.4,0.4,0.4);
     glVertex3f(-1.0f, -1.0f,  1.0f);
     glVertex3f( 1.0f, -1.0f,  1.0f);
     glVertex3f( 1.0f,  1.0f,  1.0f);
     glVertex3f(-1.0f,  1.0f,  1.0f);

    // Back Face
   glColor3d(0.4,0.4,0.4);
     glVertex3f(-1.0f, -1.0f, -1.0f);
     glVertex3f(-1.0f,  1.0f, -1.0f);
     glVertex3f( 1.0f,  1.0f, -1.0f);
     glVertex3f( 1.0f, -1.0f, -1.0f);

    // Top Face
   glColor3d(0.4,0.4,0.4);
     glVertex3f(-1.0f,  1.0f, -1.0f);
     glVertex3f(-1.0f,  1.0f,  1.0f);
     glVertex3f( 1.0f,  1.0f,  1.0f);
     glVertex3f( 1.0f,  1.0f, -1.0f);

    // Bottom Face
   glColor3d(0.4,0.4,0.4);
     glVertex3f(-1.0f, -1.0f, -1.0f);
     glVertex3f( 1.0f, -1.0f, -1.0f);
     glVertex3f( 1.0f, -1.0f,  1.0f);
     glVertex3f(-1.0f, -1.0f,  1.0f);

    // Right face
   glColor3d(1,1,1);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);

    // Left Face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);

    glEnd();
  glColor3d(1,1,1);
  glDisable(GL_TEXTURE_2D);
    glPopMatrix();
};
void smallbox(double c1,double c2,double c3,int texture,double x,double y){
	glPushMatrix();
	glColor3d(c1,c2,c3);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
    glVertex3f(x+7, y+10, -70);
	glTexCoord2d(1, 0);
    glVertex3f(x+7, y+10,-65);
	glTexCoord2d(1,1);
    glVertex3f(x, y+10, -65);
	glTexCoord2d(0, 1);
    glVertex3f(x, y+10, -70);

	glVertex3f(x, y+10, -70);
	glTexCoord2d(1, 0);
    glVertex3f(x+7, y+10,-70);
	glTexCoord2d(1,1);
    glVertex3f(x+7, y, -70);
	glTexCoord2d(0, 1);
    glVertex3f(x, y, -70);

	glTexCoord2d(0, 0);
    glVertex3f(x, y+10, -65);
	glTexCoord2d(1, 0);
    glVertex3f(x, y+10,-70);
	glTexCoord2d(1,1);
    glVertex3f(x, y, -70);
	glTexCoord2d(0, 1);
    glVertex3f(x, y, -65);
	

	glTexCoord2d(0, 0);
    glVertex3f(x+7, y+10, -65);
	glTexCoord2d(1, 0);
    glVertex3f(x+7, y+10,-70);
	glTexCoord2d(1,1);
    glVertex3f(x+7, y, -70);
	glTexCoord2d(0, 1);
    glVertex3f(x+7, y, -65);
	glEnd();
	
	glColor3d(1,1,1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 1);
	glVertex3f(x, y+10, -65);
	glTexCoord2d(1, 1);
    glVertex3f(x+7, y+10,-65);
	glTexCoord2d(1,0);
    glVertex3f(x+7, y, -65);
	glTexCoord2d(0, 0);
    glVertex3f(x, y, -65);
	glEnd();
	glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}
void shelf(double y){
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wardrobe1);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
    glVertex3f(-162, y, -78);
	glTexCoord2d(1, 0);
    glVertex3f(-162, y,-58);
	glTexCoord2d(1,1);
    glVertex3f(-215, y, -58);
	glTexCoord2d(0, 1);
    glVertex3f(-215, y, -78);

	glVertex3f(-215, y, -58);
	glTexCoord2d(1, 0);
    glVertex3f(-162, y,-58);
	glTexCoord2d(1,1);
    glVertex3f(-162, y-2, -58);
	glTexCoord2d(0, 1);
    glVertex3f(-215, y-2, -58);
	glEnd();
	glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}
//void sign3(){
//	glPushMatrix();
//	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D,logo3);
//    glBegin(GL_QUADS);
//	glTexCoord2d(0, 0);
//    glVertex3f(-200, 170, 30);
//	glTexCoord2d(0, 1);
//    glVertex3f(-200, 190,30);
//	glTexCoord2d(1,1);
//    glVertex3f(-200, 190, 60);
//	glTexCoord2d(1, 0);
//    glVertex3f(-200, 170, 60);
//    glEnd();
//	glDisable(GL_TEXTURE_2D);
//	glPopMatrix();
//}
void drawRestaurant2(){
	shelf(170);
	shelf(155);
	shelf(140);
	smallbox(0,0,1,box1,-205,170);
	smallbox(1,0,0,box2,-190,170);
	smallbox(1,1,0,box3,-175,170);
	smallbox(0,0,0,box4,-205,155);
	smallbox(1,0,1,box5,-190,155);
	smallbox(0,0,1,box6,-175,155);
	smallbox(1,1,0,box7,-205,140);
	smallbox(0,0,1,box8,-190,140);
	smallbox(1,0,1,box9,-175,140);
	glPushMatrix();
	glTranslated(-190,101,-40);
	glRotated(180,0,1,0);
	drawCashierCounter2(table,chair);
	//sign3();
	glPopMatrix();
  glPushMatrix();

  //left part
  glPushMatrix();

  glTranslated(-232,130,18);
  glScaled(3,30,20);
  machine1(drinks);
  glPopMatrix();

  glPushMatrix();
  glTranslated(-232,130,58);
  glScaled(3,30,20);
  machine1(drinks2);
  glPopMatrix();

  glPushMatrix();
  glTranslated(-232,130,98);
  glScaled(3,30,20);
  machine1(drinks3);
  glPopMatrix();

  glPushMatrix();
  glTranslated(-232,130,-22);
  glScaled(3,30,20);
  machine1(drinks5);
  glPopMatrix();

  glPushMatrix();
  glTranslated(-232,130,-52);
  glScaled(3,30,15);
  machine1(drinks4);
  glPopMatrix();

  //right part
  glPushMatrix();
  glTranslated(-150,130,10);
  glScaled(3,30,20);
  machine1(drinks6);
  glPopMatrix();

  glPushMatrix();
  glTranslated(-150,130,50);
  glScaled(3,30,20);
  machine1(drinks7);
  glPopMatrix();

  glPushMatrix();
  glTranslated(-150,130,90);
  glScaled(3,30,20);
  machine1(drinks8);
  glPopMatrix();

  glPushMatrix();
  glTranslated(-150,130,-30);
  glScaled(3,30,20);
  machine1(drinks9);
  glPopMatrix();

  glPushMatrix();
  glTranslated(-150,130,-60);
  glScaled(3,30,15);
  machine1(drinks10);
  glPopMatrix();
  
   glPushMatrix();
   glTranslated(-200, 101, 108);
    Door1();
  glPopMatrix();
  glPopMatrix();
}

void lights(){
	glPushMatrix();
	glColor3d(1,1,1);
	glTranslated(-100, 90, -120);
	glScaled(5,2,5);
	drawCylinder(1.0f,3.0f);
	glEnd();
	glPopMatrix();
}

void drawMall() {
    glPushMatrix();
	glTranslated(350,0.5,340);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,mallwall);
    glBegin(GL_QUADS);
	//left
    glTexCoord2d(0, 0); glVertex3f(-200, 0.5, -240);
    glTexCoord2d(1, 0); glVertex3f(117, 0.5, -240);
    glTexCoord2d(1, 1); glVertex3f(117, 200, -240);
    glTexCoord2d(0, 1); glVertex3f(-200, 200, -240);
	//right
    glTexCoord2d(0, 0); glVertex3f(-200, 0.5, 117);
    glTexCoord2d(1, 0); glVertex3f(117, 0.5, 117);
    glTexCoord2d(1, 1); glVertex3f(117, 200, 117);
    glTexCoord2d(0, 1); glVertex3f(-200, 200, 117);
	//front
    glTexCoord2d(0, 0); glVertex3f(-200, 180, -240);
    glTexCoord2d(1, 0); glVertex3f(-200, 180, 117);
    glTexCoord2d(1, 1); glVertex3f(-200, 200, 117);
    glTexCoord2d(0, 1); glVertex3f(-200, 200, -240);

	glTexCoord2d(0, 0); glVertex3f(-200, 100, -240);
    glTexCoord2d(1, 0); glVertex3f(-200, 100, -160);
    glTexCoord2d(1, 1); glVertex3f(-200, 180, -160);
    glTexCoord2d(0, 1); glVertex3f(-200, 180, -240);

	glTexCoord2d(0, 0); glVertex3f(-200, 100, 40);
    glTexCoord2d(1, 0); glVertex3f(-200, 100, 117);
    glTexCoord2d(1, 1); glVertex3f(-200, 180, 117);
    glTexCoord2d(0, 1); glVertex3f(-200, 180, 40);

	glTexCoord2d(0, 0); glVertex3f(-200, 100, -100);
    glTexCoord2d(1, 0); glVertex3f(-200, 100, -90);
    glTexCoord2d(1, 1); glVertex3f(-200, 180, -90);
    glTexCoord2d(0, 1); glVertex3f(-200, 180, -100);

	glTexCoord2d(0, 0); glVertex3f(-200, 100, -30);
    glTexCoord2d(1, 0); glVertex3f(-200, 100, -20);
    glTexCoord2d(1, 1); glVertex3f(-200, 180, -20);
    glTexCoord2d(0, 1); glVertex3f(-200, 180, -30);

	glTexCoord2d(0, 0); glVertex3f(-200, 40, -240);
    glTexCoord2d(1, 0); glVertex3f(-200, 40, 117);
    glTexCoord2d(1, 1); glVertex3f(-200, 100, 117);
    glTexCoord2d(0, 1); glVertex3f(-200, 100, -240);

	glTexCoord2d(0, 0); glVertex3f(-200, 0.5, -240);
    glTexCoord2d(1, 0); glVertex3f(-200, 0.5, -97.8);
    glTexCoord2d(1, 1); glVertex3f(-200, 50, -97.8);
    glTexCoord2d(0, 1); glVertex3f(-200, 50, -240);

	glTexCoord2d(0, 0); glVertex3f(-200, 0.5, -22.8);
    glTexCoord2d(1, 0); glVertex3f(-200, 0.5, 117);
    glTexCoord2d(1, 1); glVertex3f(-200, 50, 117);
    glTexCoord2d(0, 1); glVertex3f(-200, 50, -22.8);
	//back
    glTexCoord2d(0, 0); glVertex3f(117, 0.5, -240);
    glTexCoord2d(1, 0); glVertex3f(117, 0.5, 117);
    glTexCoord2d(1, 1); glVertex3f(117, 200, 117);
    glTexCoord2d(0, 1); glVertex3f(117, 200, -240);
	//top
    glTexCoord2d(0, 0); glVertex3f(-200, 200, -240);
    glTexCoord2d(1, 0); glVertex3f(117, 200, -240);
    glTexCoord2d(1, 1); glVertex3f(117, 200, 117);
    glTexCoord2d(0, 1); glVertex3f(-200, 200, 117);
	//shop 1
	glTexCoord2d(0, 0); glVertex3f(-120, 2, -140);
    glTexCoord2d(1, 0); glVertex3f(80, 2, -140);
    glTexCoord2d(1, 1); glVertex3f(80, 97, -140);
    glTexCoord2d(0, 1); glVertex3f(-120, 97, -140);
	
	glTexCoord2d(0, 0); glVertex3f(-120, 41, -240);
    glTexCoord2d(1, 0); glVertex3f(-120, 41, -140);
    glTexCoord2d(1, 1); glVertex3f(-120, 99, -140);
    glTexCoord2d(0, 1); glVertex3f(-120, 99, -240);

	glTexCoord2d(0, 0); glVertex3f(-120, 2, -240);
    glTexCoord2d(1, 0); glVertex3f(-120, 2, -200);
    glTexCoord2d(1, 1); glVertex3f(-120, 41, -200);
    glTexCoord2d(0, 1); glVertex3f(-120, 41, -240);

	glTexCoord2d(0, 0); glVertex3f(-120, 2, -163);
    glTexCoord2d(1, 0); glVertex3f(-120, 2, -140);
    glTexCoord2d(1, 1); glVertex3f(-120, 41, -140);
    glTexCoord2d(0, 1); glVertex3f(-120, 41, -163);
	//shop 2
	glTexCoord2d(0, 0); glVertex3f(-120, 64, 10);
    glTexCoord2d(1, 0); glVertex3f(117, 64, 10);
    glTexCoord2d(1, 1); glVertex3f(117, 97, 10);
    glTexCoord2d(0, 1); glVertex3f(-120, 97, 10);

	glTexCoord2d(0, 0); glVertex3f(-82, 2, 10);
    glTexCoord2d(1, 0); glVertex3f(117, 2, 10);
    glTexCoord2d(1, 1); glVertex3f(117, 64, 10);
    glTexCoord2d(0, 1); glVertex3f(-82, 64, 10);

	glTexCoord2d(0, 0); glVertex3f(-120, 2, 10);
    glTexCoord2d(1, 0); glVertex3f(-120, 2, 117);
    glTexCoord2d(1, 1); glVertex3f(-120, 97, 117);
    glTexCoord2d(0, 1); glVertex3f(-120, 97, 10);
	//shop 3
	glTexCoord2d(0, 0); glVertex3f(-120, 101, -140);
    glTexCoord2d(1, 0); glVertex3f(80, 101, -140);
    glTexCoord2d(1, 1); glVertex3f(80, 199, -140);
    glTexCoord2d(0, 1); glVertex3f(-120, 199, -140);

	glTexCoord2d(0, 0); glVertex3f(-120, 140, -240);
    glTexCoord2d(1, 0); glVertex3f(-120, 140, -140);
    glTexCoord2d(1, 1); glVertex3f(-120, 199, -140);
    glTexCoord2d(0, 1); glVertex3f(-120, 199, -240);

	glTexCoord2d(0, 0); glVertex3f(-120, 102, -240);
    glTexCoord2d(1, 0); glVertex3f(-120, 102, -200);
    glTexCoord2d(1, 1); glVertex3f(-120, 140, -200);
    glTexCoord2d(0, 1); glVertex3f(-120, 140, -240);

	glTexCoord2d(0, 0); glVertex3f(-120, 102, -163);
    glTexCoord2d(1, 0); glVertex3f(-120, 102, -140);
    glTexCoord2d(1, 1); glVertex3f(-120, 140, -140);
    glTexCoord2d(0, 1); glVertex3f(-120, 140, -163);
	//shop 4
	glTexCoord2d(0, 0); glVertex3f(-120, 101, 10);
    glTexCoord2d(1, 0); glVertex3f(117, 101, 10);
    glTexCoord2d(1, 1); glVertex3f(117, 199, 10);
    glTexCoord2d(0, 1); glVertex3f(-120, 199, 10);


	glTexCoord2d(0, 0); glVertex3f(-120, 163, 10);
    glTexCoord2d(1, 0); glVertex3f(-120, 163, 117);
    glTexCoord2d(1, 1); glVertex3f(-120, 199, 117);
    glTexCoord2d(0, 1); glVertex3f(-120, 199, 10);

	glTexCoord2d(0, 0); glVertex3f(-120, 101, 10);
    glTexCoord2d(1, 0); glVertex3f(-120, 101, 30);
    glTexCoord2d(1, 1); glVertex3f(-120, 163, 30);
    glTexCoord2d(0, 1); glVertex3f(-120, 163, 10);

	glTexCoord2d(0, 0); glVertex3f(-120, 101, 67);
    glTexCoord2d(1, 0); glVertex3f(-120, 101, 117);
    glTexCoord2d(1, 1); glVertex3f(-120, 163, 117);
    glTexCoord2d(0, 1); glVertex3f(-120, 163, 67);

	//eleveter
	glTexCoord2d(0, 0); glVertex3f(80, 2, -240);
    glTexCoord2d(1, 0); glVertex3f(80, 2, -140);
    glTexCoord2d(1, 1); glVertex3f(80, 199, -140);
    glTexCoord2d(0, 1); glVertex3f(80, 199, -240);

	glTexCoord2d(0, 0); glVertex3f(80, 98, -140);
    glTexCoord2d(1, 0); glVertex3f(115, 98, -140);
    glTexCoord2d(1, 1); glVertex3f(115, 75, -140);
    glTexCoord2d(0, 1); glVertex3f(80, 75, -140);

	glTexCoord2d(0, 0); glVertex3f(80, 198, -140);
    glTexCoord2d(1, 0); glVertex3f(115, 198, -140);
    glTexCoord2d(1, 1); glVertex3f(115, 175, -140);
    glTexCoord2d(0, 1); glVertex3f(80, 175, -140);
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,elevatordoor);
    glBegin(GL_QUADS);
	glTexCoord2d(1, 0); glVertex3f(s, 101, -140);
    glTexCoord2d(1, 1); glVertex3f(s, 175, -140);
    glTexCoord2d(0, 1); glVertex3f(116, 175, -140);
    glTexCoord2d(0, 0); glVertex3f(116, 101, -140);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,elevatordoor);
    glBegin(GL_QUADS);
	glTexCoord2d(1, 0); glVertex3f(n, 2, -140);
    glTexCoord2d(1, 1); glVertex3f(n, 75, -140);
    glTexCoord2d(0, 1); glVertex3f(116, 75, -140);
    glTexCoord2d(0, 0); glVertex3f(116, 2, -140);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	eleveter();
	
	//Separator
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,floorm);
    glBegin(GL_QUADS);
    glTexCoord2d(0, 0); glVertex3f(-199, 100, -239);
    glTexCoord2d(1, 0); glVertex3f(-110, 100, -239);
    glTexCoord2d(1, 3); glVertex3f(-110, 100, 116);
    glTexCoord2d(0, 3); glVertex3f(-199, 100, 116);

	glTexCoord2d(0, 0); glVertex3f(95, 100, -140);
    glTexCoord2d(1, 0); glVertex3f(116, 100, -140);
    glTexCoord2d(1, 2); glVertex3f(116, 100, 116);
    glTexCoord2d(0, 2); glVertex3f(95, 100, 116);

	glTexCoord2d(0, 0); glVertex3f(44, 100, -100);
    glTexCoord2d(2, 0); glVertex3f(65, 100, -100);
    glTexCoord2d(2, 2); glVertex3f(65, 100, -20);
    glTexCoord2d(0, 2); glVertex3f(44, 100, -20);

	glTexCoord2d(0, 0); glVertex3f(-110, 100, -239);
    glTexCoord2d(2, 0); glVertex3f(79, 100, -239);
    glTexCoord2d(2, 1); glVertex3f(79, 100, -100);
    glTexCoord2d(0, 1); glVertex3f(-110, 100, -100);

	glTexCoord2d(0, 0); glVertex3f(79, 100, -140);
    glTexCoord2d(1, 0); glVertex3f(95, 100, -140);
    glTexCoord2d(1, 1); glVertex3f(95, 100, -99);
    glTexCoord2d(0, 1); glVertex3f(79, 100, -99);

	glTexCoord2d(0, 0); glVertex3f(-110, 100, -20);
    glTexCoord2d(2, 0); glVertex3f(95, 100, -20);
    glTexCoord2d(2, 1); glVertex3f(95, 100, 116);
    glTexCoord2d(0, 1); glVertex3f(-110, 100, 116);

	glTexCoord2d(0, 0); glVertex3f(-197, 1, -238);
    glTexCoord2d(5, 0); glVertex3f(116, 1, -238);
    glTexCoord2d(5, 5); glVertex3f(116, 1, 115);
    glTexCoord2d(0, 5); glVertex3f(-197, 1, 115);
	glEnd();
	//roof
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,roof);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 0); glVertex3f(-199, 99, -239);
    glTexCoord2d(1, 0); glVertex3f(-111, 99, -239);
    glTexCoord2d(1, 1); glVertex3f(-111, 99, 116);
    glTexCoord2d(0, 1); glVertex3f(-199, 99, 116);

	glTexCoord2d(0, 0); glVertex3f(94, 99, -140);
    glTexCoord2d(1, 0); glVertex3f(116, 99, -140);
    glTexCoord2d(1, 3); glVertex3f(116, 99, 116);
    glTexCoord2d(0, 3); glVertex3f(94, 99, 116);

	glTexCoord2d(0, 0); glVertex3f(44, 99, -100);
    glTexCoord2d(2, 0); glVertex3f(65, 99, -100);
    glTexCoord2d(2, 2); glVertex3f(65, 99, -20);
    glTexCoord2d(0, 2); glVertex3f(44, 99, -20);

	glTexCoord2d(0, 0); glVertex3f(-111, 99, -239);
    glTexCoord2d(2, 0); glVertex3f(79, 99, -239);
    glTexCoord2d(2, 1); glVertex3f(79, 99, -99);
    glTexCoord2d(0, 1); glVertex3f(-111, 99, -99);

	glTexCoord2d(0, 0); glVertex3f(79, 99, -140);
    glTexCoord2d(2, 0); glVertex3f(95, 99, -140);
    glTexCoord2d(2, 1); glVertex3f(95, 99, -99);
    glTexCoord2d(0, 1); glVertex3f(79, 99, -99);

	glTexCoord2d(0, 0); glVertex3f(-111, 99, -21);
    glTexCoord2d(2, 0); glVertex3f(95, 99, -21);
    glTexCoord2d(2, 1); glVertex3f(95, 99, 116);
    glTexCoord2d(0, 1); glVertex3f(-111, 99, 116);
	glEnd();
	glPushMatrix();
	
	glDisable(GL_TEXTURE_2D);
	//shop4 wall
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,shop4wall);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 0); glVertex3f(-118, 101, 16);
    glTexCoord2d(1, 0); glVertex3f(116, 101, 16);
    glTexCoord2d(1, 1); glVertex3f(116, 199, 16);
    glTexCoord2d(0, 1); glVertex3f(-118, 199, 16);

	glTexCoord2d(0, 0); glVertex3f(-118, 101, 115);
    glTexCoord2d(1, 0); glVertex3f(116, 101, 115);
    glTexCoord2d(1, 1); glVertex3f(116, 199, 115);
    glTexCoord2d(0, 1); glVertex3f(-118, 199, 115);

	glTexCoord2d(0, 0); glVertex3f(116, 101, 16);
    glTexCoord2d(1, 0); glVertex3f(116, 101, 115);
    glTexCoord2d(1, 1); glVertex3f(116, 199, 115);
    glTexCoord2d(0, 1); glVertex3f(116, 199, 16);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	//shop2 wall
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,shop2wall);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 1); glVertex3f(-120, 64, 14);
    glTexCoord2d(1, 1); glVertex3f(116, 64, 14);
    glTexCoord2d(1, 0); glVertex3f(116, 97, 14);
    glTexCoord2d(0, 0); glVertex3f(-120, 97, 14);

	glTexCoord2d(0, 0); glVertex3f(-80, 2, 14);
    glTexCoord2d(1, 0); glVertex3f(116, 2, 14);
    glTexCoord2d(1, 1); glVertex3f(116, 64, 14);
    glTexCoord2d(0, 1); glVertex3f(-80, 64, 14);

	glTexCoord2d(0, 0); glVertex3f(-118, 2, 15);
    glTexCoord2d(1, 0); glVertex3f(-118, 2, 116);
    glTexCoord2d(1, 1); glVertex3f(-118, 97, 116);
    glTexCoord2d(0, 1); glVertex3f(-118, 97, 15);

	glTexCoord2d(0, 0); glVertex3f(-118, 2, 115);
    glTexCoord2d(1, 0); glVertex3f(116, 2, 115);
    glTexCoord2d(1, 1); glVertex3f(116, 97, 115);
    glTexCoord2d(0, 1); glVertex3f(-118, 97, 115);

	glTexCoord2d(0, 0); glVertex3f(116, 2, 14);
    glTexCoord2d(1, 0); glVertex3f(116, 2, 115);
    glTexCoord2d(1, 1); glVertex3f(116, 97, 115);
    glTexCoord2d(0, 1); glVertex3f(116, 97, 14);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	//shop2 pic
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,logo3);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 0); glVertex3f(-123, 150, -200);
    glTexCoord2d(1, 0); glVertex3f(-123, 150, -150);
    glTexCoord2d(1, 1); glVertex3f(-123, 169, -150);
    glTexCoord2d(0, 1); glVertex3f(-123, 169, -200);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,pics3);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 1); glVertex3f(-170, 170, -237);
    glTexCoord2d(1, 1); glVertex3f(-140, 170, -237);
    glTexCoord2d(1, 0); glVertex3f(-140, 125, -237);
    glTexCoord2d(0, 0); glVertex3f(-170, 125, -237);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,pics2);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 1); glVertex3f(-100, 170, -139);
    glTexCoord2d(1, 1); glVertex3f(-60, 170, -139);
    glTexCoord2d(1, 0); glVertex3f(-60, 135, -139);
    glTexCoord2d(0, 0); glVertex3f(-100, 135, -139);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,pics1);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 1); glVertex3f(-30, 170, -139);
    glTexCoord2d(1, 1); glVertex3f(10, 170, -139);
    glTexCoord2d(1, 0); glVertex3f(10, 125, -139);
    glTexCoord2d(0, 0); glVertex3f(-30, 125, -139);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,pics5);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 1); glVertex3f(-170, 170, 114);
    glTexCoord2d(1, 1); glVertex3f(-145, 170, 114);
    glTexCoord2d(1, 0); glVertex3f(-145, 145, 114);
    glTexCoord2d(0, 0); glVertex3f(-170, 145, 114);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,ad2);
    glBegin(GL_QUADS);
	glTexCoord2d(0, 0); glVertex3f(115, 120, -100);
    glTexCoord2d(1, 0); glVertex3f(115, 120, -25);
    glTexCoord2d(1, 1); glVertex3f(115, 170, -25);
    glTexCoord2d(0, 1); glVertex3f(115, 170, -100);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	fountain->pos.x = -70 ;
	fountain->pos.y = 2 ;
	fountain->pos.z = -50;
	fountain->scale=0.05;
	fountain->Draw();
	glTranslated(0,0,-35);
    Column();
	glPopMatrix();
	glPushMatrix();
	glTranslated(0,0,245);
	Column();
	glPopMatrix();

	glPushMatrix();
	lights();
	glPopMatrix();
	
	fence1(44,-20,-100);
	fence1(-112,-100,-20);
	fence2(-112,44,-20);
	fence2(44,-112,-100);
	glPushMatrix();
	glRotated(-90,0,1,0);
	drawStairs(-25,0,-5);
	glPopMatrix();
	drawLShapedTable();
	shop4();
	shop2();
	glPushMatrix();
	glRotated(-90,0,1,0);
	drawRestaurant1();
	glPopMatrix();
	glPushMatrix();
	glRotated(-90,0,1,0);
	drawRestaurant2();
	glPopMatrix();
	glPushMatrix();
	glTranslated(-187,1.5,-60);
	glRotated(-90,0,1,0);
	door();
	glPopMatrix();
	glass(-160,-100);
	glass(-90,-30);
	glass(-20,40);
	glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void mallFloor() {
  glPushMatrix();
  glTranslated(132,0.5,132);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,block);
  glBegin(GL_QUADS);
  glTexCoord2d(0,0); glVertex3f(-100,  0.5, -100) ;
  glTexCoord2d(15,0); glVertex3f(368,  0.5, -100) ;
  glTexCoord2d(15,15); glVertex3f(368,  0.5, 368) ;
  glTexCoord2d(0,15); glVertex3f(-100, 0.5, 368) ;
  glEnd();

  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}

void Building1(int texture) {

    glPushMatrix();
    glScaled(4,20,4);
    glTranslated(-20,5.1,-20);
  // Q1 Back
    glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texture);
    glBegin(GL_QUADS);
    glTexCoord2d(0,0); glVertex3d(5,-5,-5);
    glTexCoord2d(1,0); glVertex3d(-5,-5,-5);
    glTexCoord2d(1,1); glVertex3d(-5,5,-5);
    glTexCoord2d(0,1); glVertex3d(5,5,-5);
    glEnd();
     glDisable(GL_TEXTURE_2D);
    //.....

    // Q6 Front
    glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texture);
    glBegin(GL_QUADS);
    glTexCoord2d(1,1); glVertex3d(-5,5,5);
    glTexCoord2d(0,1); glVertex3d(5,5,5);
    glTexCoord2d(0,0); glVertex3d(5,-5,5);
    glTexCoord2d(1,0); glVertex3d(-5,-5,5);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    //...........

  // Q2 Left
    glColor3d(0.9,0.9,0.9);
    glBegin(GL_QUADS);
    glTexCoord2d(0,0); glVertex3d(-5,-5,-5);
    glTexCoord2d(0,1); glVertex3d(-5,-5,5);
    glTexCoord2d(1,1); glVertex3d(-5,5,5);
    glTexCoord2d(1,0); glVertex3d(-5,5,-5);
    glEnd();
    //.....
  // Q3 Right
    glBegin(GL_QUADS);
    glTexCoord2d(0,0); glVertex3d(5,-5,5);
    glTexCoord2d(0,1); glVertex3d(5,-5,-5);
    glTexCoord2d(1,1); glVertex3d(5,5,-5);
    glTexCoord2d(1,0); glVertex3d(5,5,5);
    glEnd();
    //.....
  // Q4 Top
     glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,top);
    glBegin(GL_QUADS);
    glTexCoord2d(0,0); glVertex3d(-5,5,5);
    glTexCoord2d(0,1); glVertex3d(5,5,5);
    glTexCoord2d(1,1); glVertex3d(5,5,-5);
    glTexCoord2d(1,0); glVertex3d(-5,5,-5);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    //.....
  // Q5 Bottom
    glBegin(GL_QUADS);
    glTexCoord2d(0,0); glVertex3d(-5,-5,-5);
    glTexCoord2d(1,0); glVertex3d(5,-5,-5);
    glTexCoord2d(1,1); glVertex3d(5,-5,5);
    glTexCoord2d(0,1); glVertex3d(-5,-5,5);
    
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    //.....
}
void Building2(int texture) {

    glPushMatrix();
    glScaled(10,14,10);
    glTranslated(-20,5.1,-10);
  // Q1 Back
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture);
    glBegin(GL_QUADS);
    glTexCoord2d(0,0); glVertex3d(5,-5,-5);
    glTexCoord2d(1,0); glVertex3d(-5,-5,-5);
    glTexCoord2d(1,1); glVertex3d(-5,5,-5);
    glTexCoord2d(0,1); glVertex3d(5,5,-5);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    //.....

    // Q6 Front
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture);
    glBegin(GL_QUADS);
    glTexCoord2d(1,1); glVertex3d(-5,5,5);
    glTexCoord2d(0,1); glVertex3d(5,5,5);
    glTexCoord2d(0,0); glVertex3d(5,-5,5);
    glTexCoord2d(1,0); glVertex3d(-5,-5,5);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    //...........

  // Q2 Left
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture);
    glBegin(GL_QUADS);
    glTexCoord2d(1,0); glVertex3d(-5,-5,-5);
    glTexCoord2d(0,0); glVertex3d(-5,-5,5);
    glTexCoord2d(0,1); glVertex3d(-5,5,5);
    glTexCoord2d(1,1); glVertex3d(-5,5,-5);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    //.....
  // Q3 Right
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture);
    glBegin(GL_QUADS);
    glTexCoord2d(1,0); glVertex3d(5,-5,5);
    glTexCoord2d(0,0); glVertex3d(5,-5,-5);
    glTexCoord2d(0,1); glVertex3d(5,5,-5);
    glTexCoord2d(1,1); glVertex3d(5,5,5);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    //.....
  // Q4 Top
     glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,top);
    glBegin(GL_QUADS);
    glTexCoord2d(0,0); glVertex3d(-5,5,5);
    glTexCoord2d(0,1); glVertex3d(5,5,5);
    glTexCoord2d(1,1); glVertex3d(5,5,-5);
    glTexCoord2d(1,0); glVertex3d(-5,5,-5);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    //.....
  // Q5 Bottom
    glBegin(GL_QUADS);
    glTexCoord2d(0,0); glVertex3d(-5,-5,-5);
    glTexCoord2d(1,0); glVertex3d(5,-5,-5);
    glTexCoord2d(1,1); glVertex3d(5,-5,5);
    glTexCoord2d(0,1); glVertex3d(-5,-5,5);
    
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    //.....
}
void Building3(int texture) {

    glPushMatrix();
    glScaled(15,15,15);
    glTranslated(-10,5.1,-20);
    glRotated(90,0,1,0);
  // Q1 Back
    glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texture);
    glBegin(GL_QUADS);
    glTexCoord2d(0,0); glVertex3d(5,-5,-5);
    glTexCoord2d(1,0); glVertex3d(-5,-5,-5);
    glTexCoord2d(1,1); glVertex3d(-5,5,-5);
    glTexCoord2d(0,1); glVertex3d(5,5,-5);
    glEnd();
    glColor3d(1,1,1);
    //.....

    // Q6 Front
    glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texture);
	glBegin(GL_QUADS);
    glTexCoord2d(1,1); glVertex3d(-5,5,5);
    glTexCoord2d(0,1); glVertex3d(5,5,5);
    glTexCoord2d(0,0); glVertex3d(5,-5,5);
    glTexCoord2d(1,0); glVertex3d(-5,-5,5);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    //...........

  // Q2 Left
    glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texture);
    glBegin(GL_QUADS);
    glTexCoord2d(1,0); glVertex3d(-5,-5,-5);
    glTexCoord2d(0,0); glVertex3d(-5,-5,5);
    glTexCoord2d(0,1); glVertex3d(-5,5,5);
    glTexCoord2d(1,1); glVertex3d(-5,5,-5);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    //.....
  // Q3 Right
    glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texture);
    glBegin(GL_QUADS);
    glTexCoord2d(1,0); glVertex3d(5,-5,5);
    glTexCoord2d(0,0); glVertex3d(5,-5,-5);
    glTexCoord2d(0,1); glVertex3d(5,5,-5);
    glTexCoord2d(1,1); glVertex3d(5,5,5);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    //.....
  // Q4 Top
     glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,top);
    glBegin(GL_QUADS);
    glTexCoord2d(0,0); glVertex3d(-5,5,5);
    glTexCoord2d(0,1); glVertex3d(5,5,5);
    glTexCoord2d(1,1); glVertex3d(5,5,-5);
    glTexCoord2d(1,0); glVertex3d(-5,5,-5);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    //.....
  // Q5 Bottom
    glBegin(GL_QUADS);
    glTexCoord2d(0,0); glVertex3d(-5,-5,-5);
    glTexCoord2d(1,0); glVertex3d(5,-5,-5);
    glTexCoord2d(1,1); glVertex3d(5,-5,5);
    glTexCoord2d(0,1); glVertex3d(-5,-5,5);
    
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    //.....
}
void drawBus3D() { 
    glPushMatrix(); 
    glScaled(20, 10, 15); 
    glRotated(90, 0, 1, 0);
    glTranslated(-1.5, 0.5, 1.3); 
    // جسم الباص 
    glColor3f(0.7, 0.0, 0.0); // اللون الأحمر 
    glBegin(GL_QUADS); 
    // وجه الباص 
    glVertex3f(-0.5, -0.25, 0.25); 
    glVertex3f(0.5, -0.25, 0.25); 
    glVertex3f(0.5, 0.25, 0.25); 
    glVertex3f(-0.5, 0.25, 0.25); 
    // خلف الباص 
    glVertex3f(-0.5, -0.25, -0.25); 
    glVertex3f(0.5, -0.25, -0.25); 
    glVertex3f(0.5, 0.25, -0.25); 
    glVertex3f(-0.5, 0.25, -0.25); 
    // الجوانب 
    glVertex3f(-0.5, -0.25, 0.25); 
    glVertex3f(-0.5, -0.25, -0.25); 
    glVertex3f(-0.5, 0.25, -0.25); 
    glVertex3f(-0.5, 0.25, 0.25); 
    glVertex3f(0.5, -0.25, 0.25); 
    glVertex3f(0.5, -0.25, -0.25); 
    glVertex3f(0.5, 0.25, -0.25); 
    glVertex3f(0.5, 0.25, 0.25); 
    // السطح والأرضية 
    glVertex3f(-0.5, 0.25, 0.25); 
    glVertex3f(0.5, 0.25, 0.25); 
    glVertex3f(0.5, 0.25, -0.25); 
    glVertex3f(-0.5, 0.25, -0.25); 
    glVertex3f(-0.5, -0.25, 0.25); 
    glVertex3f(0.5, -0.25, 0.25); 
    glVertex3f(0.5, -0.25, -0.25); 
    glVertex3f(-0.5, -0.25, -0.25); 
    glEnd(); 
    // رسم المربع العلوي
    glColor3f(0.7, 0.0, 0.0); // اللون الأزرق 
    glBegin(GL_QUADS); 
    // وجه المربع 
    glVertex3f(-0.3, 0.25, 0.15); 
    glVertex3f(0.3, 0.25, 0.15); 
    glVertex3f(0.3, 0.55, 0.15); 
    glVertex3f(-0.3, 0.55, 0.15); 
    // خلف المربع 
    glVertex3f(-0.3, 0.25, -0.15); 
    glVertex3f(0.3, 0.25, -0.15); 
    glVertex3f(0.3, 0.55, -0.15); 
    glVertex3f(-0.3, 0.55, -0.15); 
    // الجوانب 
    glVertex3f(-0.3, 0.25, 0.15); 
    glVertex3f(-0.3, 0.25, -0.15); 
    glVertex3f(-0.3, 0.55, -0.15); 
    glVertex3f(-0.3, 0.55, 0.15); 
    glVertex3f(0.3, 0.25, 0.15); 
    glVertex3f(0.3, 0.25, -0.15); 
    glVertex3f(0.3, 0.55, -0.15); 
    glVertex3f(0.3, 0.55, 0.15); 
    // السطح والأرضية 
    glVertex3f(-0.3, 0.55, 0.15); 
    glVertex3f(0.3, 0.55, 0.15); 
    glVertex3f(0.3, 0.55, -0.15); 
    glVertex3f(-0.3, 0.55, -0.15); 
    glVertex3f(-0.3, 0.25, 0.15); 
    glVertex3f(0.3, 0.25, 0.15); 
    glVertex3f(0.3, 0.25, -0.15); 
    glVertex3f(-0.3, 0.25, -0.15); 
    glEnd(); 

    // رسم النوافذ
    glColor3f(0.0, 0.0, 1.0); // اللون الأزرق
    glBegin(GL_QUADS);
    // النوافذ الجانبية
    for (float x = -0.4; x <= 0.4; x += 0.3) {
        glVertex3f(x, -0.1, 0.26);
        glVertex3f(x + 0.2, -0.1, 0.26);
        glVertex3f(x + 0.2, 0.2, 0.26);
        glVertex3f(x, 0.2, 0.26);

        glVertex3f(x, -0.1, -0.26);
        glVertex3f(x + 0.2, -0.1, -0.26);
        glVertex3f(x + 0.2, 0.2, -0.26);
        glVertex3f(x, 0.2, -0.26);
    }
    // النافذة الأمامية
    glVertex3f(-0.1, 0.0, 0.26);
    glVertex3f(0.1, 0.0, 0.26);
    glVertex3f(0.1, 0.2, 0.26);
    glVertex3f(-0.1, 0.2, 0.26);
    glEnd();

    // رسم الأبواب
    glColor3f(0.0, 0.0, 0.0); // اللون الأسود
    glBegin(GL_QUADS);
    // الباب الأمامي
    glVertex3f(-0.3, -0.25, 0.26);
    glVertex3f(-0.1, -0.25, 0.26);
    glVertex3f(-0.1, 0.1, 0.26);
    glVertex3f(-0.3, 0.1, 0.26);
    glEnd();

    // رسم المصابيح الأمامية
    glColor3f(1.0, 1.0, 0.0); // اللون الأصفر
    glBegin(GL_QUADS);
    glVertex3f(-0.4, -0.15, 0.26);
    glVertex3f(-0.3, -0.15, 0.26);
    glVertex3f(-0.3, -0.05, 0.26);
    glVertex3f(-0.4, -0.05, 0.26);

    glVertex3f(0.3, -0.15, 0.26);
    glVertex3f(0.4, -0.15, 0.26);
    glVertex3f(0.4, -0.05, 0.26);
    glVertex3f(0.3, -0.05, 0.26);
    glEnd();

    // رسم العجلات ثلاثية الأبعاد 
    for (float x = -0.35; x <= 0.35; x += 0.7) { 
        for (float z = -0.25; z <= 0.25; z += 0.5) { 
            // رسم العجلة 
            glPushMatrix(); 
            glTranslatef(x, -0.35, z); 
            glColor3f(0.0, 0.0, 0.0); // اللون الأسود 
            // رسم الأسطوانة 
			glBegin(GL_QUAD_STRIP); 
			for (float angle = 0; angle <= 2 * 3.14; angle += 3.14 / 12) { 
				float vertexX = 0.1 * cos(angle); 
				float vertexY = 0.1 * sin(angle); 
				glVertex3f(vertexX, vertexY, 0.05); 
				glVertex3f(vertexX, vertexY, -0.05); 
			} 
			glEnd(); 
			// رسم القاعدتين 
			glBegin(GL_TRIANGLE_FAN); 
			glVertex3f(0, 0, 0.05); 
			for (float angle = 0; angle <= 2 * 3.14; angle += 3.14 / 12) { 
				glVertex3f(0.1 * cos(angle), 0.1 * sin(angle), 0.05); 
			} glEnd(); 
			glBegin(GL_TRIANGLE_FAN); 
			glVertex3f(0, 0, -0.05); 
			for (float angle = 0; angle <= 2 * 3.14; angle += 3.14 / 12) { 
				glVertex3f(0.1 * cos(angle), 0.1 * sin(angle), -0.05); 
			} glEnd(); 
			glPopMatrix(); 
		} 
	} 
	glPopMatrix(); 
}
void park1() {
     
    glPushMatrix();
    glTranslated(-350,2,-275);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,parking);
    glBegin(GL_QUADS);
    glTexCoord2d(0,0); glVertex3d(-100,0,-100);
    glTexCoord2d(1,0); glVertex3d(100,0,-100);
    glTexCoord2d(1,1); glVertex3d(100,0,100);
    glTexCoord2d(0,1); glVertex3d(-100,0,100);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    drawBus3D();
    glColor3d(1,1,1);
    glPopMatrix();
}
void Trees() {

  glTranslated(-240,0.6,240);
    float treePosition[5][3] = {
    {0, 0.6, 0},
    {-100, 0.6, -100},
    {100, 0.6, 100},
    {100, 0.6, 0},
    {-100, 0.6, 0}
};

  for(int i=0;i<=4;i++) {
    tree->pos.x = treePosition[i][0]; 
    tree->pos.y = treePosition[i][1]; 
    tree->pos.z = treePosition[i][2];
    tree->scale=10;
    tree->Draw();
  }
}

void drawCube(float size) {
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, table);
    glBegin(GL_QUADS);

    // Front face
    glTexCoord2d(0, 0); glVertex3f(-size, -size, size);
    glTexCoord2d(1, 0); glVertex3f(size, -size, size);
    glTexCoord2d(1, 1); glVertex3f(size, size, size);
    glTexCoord2d(0, 1); glVertex3f(-size, size, size);

    // Back face
    glTexCoord2d(0, 0); glVertex3f(-size, -size, -size);
    glTexCoord2d(1, 0); glVertex3f(size, -size, -size);
    glTexCoord2d(1, 1); glVertex3f(size, size, -size);
    glTexCoord2d(0, 1); glVertex3f(-size, size, -size);

    // Top face
    glTexCoord2d(0, 0); glVertex3f(-size, size, -size);
    glTexCoord2d(1, 0); glVertex3f(size, size, -size);
    glTexCoord2d(1, 1); glVertex3f(size, size, size);
    glTexCoord2d(0, 1); glVertex3f(-size, size, size);

    // Bottom face
    glTexCoord2d(0, 0); glVertex3f(-size, -size, -size);
    glTexCoord2d(1, 0); glVertex3f(size, -size, -size);
    glTexCoord2d(1, 1); glVertex3f(size, -size, size);
    glTexCoord2d(0, 1); glVertex3f(-size, -size, size);

    // Right face
    glTexCoord2d(0, 0); glVertex3f(size, -size, -size);
    glTexCoord2d(1, 0); glVertex3f(size, size, -size);
    glTexCoord2d(1, 1); glVertex3f(size, size, size);
    glTexCoord2d(0, 1); glVertex3f(size, -size, size);

    // Left face
    glTexCoord2d(0, 0); glVertex3f(-size, -size, -size);
    glTexCoord2d(1, 0); glVertex3f(-size, size, -size);
    glTexCoord2d(1, 1); glVertex3f(-size, size, size);
    glTexCoord2d(0, 1); glVertex3f(-size, -size, size);

    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}
void drawChair() {
  glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, table);

    // Seat
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.0f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.0f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.0f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.0f, -0.5f);
    glEnd();

    // Legs
    glBegin(GL_QUADS);
    // Back left leg
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.0f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.4f, 0.0f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.4f, -0.5f, 0.5f);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.0f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.4f, 0.0f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.4f, -0.5f, -0.5f);

    // Back right leg
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.0f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.4f, 0.0f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.4f, -0.5f, 0.5f);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.0f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.4f, 0.0f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.4f, -0.5f, -0.5f);

    // Front left leg
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.4f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.0f, 0.4f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.4f, 0.0f, 0.4f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.4f, -0.5f, 0.4f);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.4f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.0f, -0.4f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.4f, 0.0f, -0.4f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.4f, -0.5f, -0.4f);

    // Front right leg
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.4f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.0f, 0.4f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.4f, 0.0f, 0.4f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.4f, -0.5f, 0.4f);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.4f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f, 0.0f, -0.4f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.4f, 0.0f, -0.4f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.4f, -0.5f, -0.4f);
    glEnd();

    // Backrest
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.0f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.0f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, -0.5f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}
void drawTable() {
    // Draw table top
    glPushMatrix();
    glScalef(2.0f, 0.1f, 2.0f);
    drawCube(1.0f);
    glPopMatrix();

    // Draw table legs
    glPushMatrix();
    glTranslatef(-1.8f, -1.8f, 1.8f);
    glScalef(0.1f, 2.0f, 0.1f);
    drawCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.8f, -1.8f, 1.8f);
    glScalef(0.1f, 2.0f, 0.1f);
    drawCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1.8f, -1.8f, -1.8f);
    glScalef(0.1f, 2.0f, 0.1f);
    drawCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.8f, -1.8f, -1.8f);
    glScalef(0.1f, 2.0f, 0.1f);
    drawCube(1.0f);
    glPopMatrix();
}
void garden() {

  glPushMatrix();
  //floor
  glTranslated(-380,0.5,150);
  glPushMatrix();
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,land);
  glBegin(GL_QUADS);
  glTexCoord2d(0,0); glVertex3f(-120,  0.5, -120) ;
  glTexCoord2d(6,0); glVertex3f(350,  0.5, -120) ;
  glTexCoord2d(6,6); glVertex3f(350,  0.5, 350) ;
  glTexCoord2d(0,6); glVertex3f(-120, 0.5, 350) ;
  glEnd();
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
  //grass
  glPushMatrix();
  glTranslated(120,0.6,120);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,grass);
  glBegin(GL_QUADS);
  glTexCoord2d(0,0); glVertex3f(-175,  0.5, -175) ;
  glTexCoord2d(6,0); glVertex3f(175,  0.5, -175) ;
  glTexCoord2d(6,6); glVertex3f(175,  0.5, 175) ;
  glTexCoord2d(0,6); glVertex3f(-175, 0.5, 175) ;
  glEnd();
  glDisable(GL_TEXTURE_2D);

  //table
  glPushMatrix();
  glTranslated(75,15,-75);
  glScaled(15,15,15);
  drawTable();
  glPopMatrix();

  //chair
  glPushMatrix();
  glTranslated(100,10,-80);
  glRotated(-90,0,1,0);
  glScaled(30,25,30);
  drawChair();
  glPopMatrix();

  glPopMatrix();

  glPopMatrix();

}

void Ads(int texture) {
  glPushMatrix();
  glScaled(50,50,50);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,texture);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -0.5f, 0.0f); 
  glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -0.5f, 0.0f); 
  glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 0.5f, 0.0f); 
  glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 0.5f, 0.0f);
  glEnd();
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}

void drawAds(){

    glPushMatrix();
    glTranslated(300,50,-100);
    glRotated(-90,0,1,0);
    Ads(ad1);
    glPushMatrix();
    glTranslated(20,-50,12);
    glScaled(0.1,0.4,0.1);
    glColor3d(0.4,0.4,0.4);
    Column();
    glColor3d(1,1,1);
    glPopMatrix();
    glPopMatrix();

    //
    glPushMatrix();
    glTranslated(300,50,150);
    glRotated(-90,0,1,0);
    Ads(ad2);
    glPushMatrix();
    glTranslated(20,-50,12);
    glScaled(0.1,0.4,0.1);
    glColor3d(0.4,0.4,0.4);
    Column();
    glColor3d(1,1,1);
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
     glTranslated(390,50,-140);
     glScaled(0.5,0.5,0.5);
     Ads(ad3);
    glPopMatrix();

  glPushMatrix();
  glTranslated(500,50,-44.5);
  glScaled(0.5,0.5,0.5);
  Ads(ad4);
  glPopMatrix();

  glPushMatrix();
  glTranslated(575,50,-44.5);
  glScaled(0.5,0.9,0.5);
  Ads(ad5);
  glPopMatrix();

  glPushMatrix();
  glTranslated(355,67,33);
  glScaled(0.5,0.9,0.5);
  glRotated(-90,0,1,0);
  Ads(ad6);
  glPopMatrix();

}
void sideMallDoor() {
  glPushMatrix();
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,table);
  glTranslated(140,10,230);
  glColor3f(0.5f, 0.2f, 0.0f);
  glScaled(5,5,5);
  drawCylinder(1.0f,3.0f);
  glColor3d(1,1,1);
  glPushMatrix();
  tree->pos.x=0;
  tree->pos.y=0;
  tree->pos.z=0;
  tree->scale = 0.5;
  tree->Draw();
  glPopMatrix();
  glPopMatrix();

  glPushMatrix();
  glTranslated(140,10,340);
  glColor3f(0.5f, 0.2f, 0.0f);
  glScaled(5,5,5);
  drawCylinder(1.0f,3.0f);
  glColor3d(1,1,1);
  glPushMatrix();
  tree->pos.x=0;
  tree->pos.y=0;
  tree->pos.z=0;
  tree->scale = 0.5;
  tree->Draw();
  glPopMatrix();
  glPopMatrix();
}
void Key(bool* keys, float speed)
{
  if (keys[VK_DOWN])
    MyCamera.RotateX(-1 * speed);
  if (keys[VK_UP])
    MyCamera.RotateX(1 * speed);
  if (keys[VK_RIGHT])
    MyCamera.RotateY(-1 * speed);
  if (keys['Z'])
    MyCamera.RotateZ(1 * speed);
  if (keys['X'])
    MyCamera.RotateZ(-1 * speed);
  if (keys[VK_LEFT])
    MyCamera.RotateY(1 * speed);
  if (keys['W'])
    MyCamera.MoveForward(1 * speed);
  if (keys['S'])
    MyCamera.MoveForward(-1 * speed);
  if (keys['D'])
    MyCamera.MoveRight(1 * speed);
  if (keys['A'])
    MyCamera.MoveRight(-1 * speed);
  if (keys['O'])
    MyCamera.MoveUpward(1 * speed);
  if (keys['L'])
    MyCamera.MoveUpward(-1 * speed);
  
}

int DrawGLScene(GLvoid)                  // Here's Where We Do All The Drawing
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  glTranslated(0, 0, -40);
  MyCamera.Render();
  Key(keys, 0.35);

  glPushMatrix();
  glTranslated(0,0,0);
  drawUnitCube();
  glPopMatrix();

  Road();
  glPushMatrix();
  glRotated(90,0,1,0);
  Road();
  glPopMatrix();
  
  
  mallFloor();

  
  Building1(building1);
  Building2(frontbuilding2);
  Building3(building3);
  glPushMatrix();
  glTranslated(460,0,0);
  Building1(building7);
  Building2(building5);
  Building3(building3);
  glPopMatrix();	
  glPushMatrix();
  glTranslated(300,0,-220);
  Building2(building7);
  glTranslated(0,0,200);
  Building2(building4);
  glPopMatrix();
  sideMallDoor(); 
  park1();

  garden();
  Trees();
  movedoor();
  glPushMatrix();
  glTranslated(40,0,18);
  drawAds();
  glPopMatrix();
  if (lightOn) {
        initLighting();  // تفعيل الإضاءة فقط إذا كانت الحالة "مفعل"
    }
    // إعداد المادة
    setupMaterial();
  playsound();
  glPushMatrix();
  glTranslated(250,0,-230);
  drawMall();
  glPopMatrix();
  
  return TRUE;
}
GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL, 0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL, NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		hRC = NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd, hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hDC = NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL", hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;									// Set hInstance To NULL
	}
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
*	title			- Title To Appear At The Top Of The Window				*
*	width			- Width Of The GL Window Or Fullscreen Mode				*
*	height			- Height Of The GL Window Or Fullscreen Mode			*
*	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
*	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/

BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left = (long)0;			// Set Left Value To 0
	WindowRect.right = (long)width;		// Set Right Value To Requested Width
	WindowRect.top = (long)0;				// Set Top Value To 0
	WindowRect.bottom = (long)height;		// Set Bottom Value To Requested Height

	fullscreen = fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance = GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc = (WNDPROC)WndProc;					// WndProc Handles Messages
	wc.cbClsExtra = 0;									// No Extra Window Data
	wc.cbWndExtra = 0;									// No Extra Window Data
	wc.hInstance = hInstance;							// Set The Instance
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground = NULL;									// No Background Required For GL
	wc.lpszMenuName = NULL;									// We Don't Want A Menu
	wc.lpszClassName = "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}

	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth = width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight = height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel = bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL, "The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", "GL template", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			{
				fullscreen = FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL, "Program Will Now Close.", "ERROR", MB_OK | MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle = WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle = WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle = WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd = CreateWindowEx(dwExStyle,							// Extended Style For The Window
		"OpenGL",							// Class Name
		title,								// Window Title
		dwStyle |							// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		0, 0,								// Window Position
		WindowRect.right - WindowRect.left,	// Calculate Window Width
		WindowRect.bottom - WindowRect.top,	// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		hInstance,							// Instance
		NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	if (!(hDC = GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!SetPixelFormat(hDC, PixelFormat, &pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC = wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!wglMakeCurrent(hDC, hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hWnd, SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	return TRUE;									// Success
}

LRESULT CALLBACK WndProc(HWND	hWnd,			// Handle For This Window
	UINT	uMsg,			// Message For This Window
	WPARAM	wParam,			// Additional Message Information
	LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)									// Check For Windows Messages
	{
	case WM_ACTIVATE:							// Watch For Window Activate Message
	{
													if (!HIWORD(wParam))					// Check Minimization State
													{
														active = TRUE;						// Program Is Active
													}
													else
													{
														active = FALSE;						// Program Is No Longer Active
													}

													return 0;								// Return To The Message Loop
	}

	case WM_SYSCOMMAND:							// Intercept System Commands
	{
													switch (wParam)							// Check System Calls
													{
													case SC_SCREENSAVE:					// Screensaver Trying To Start?
													case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
														return 0;							// Prevent From Happening
													}
													break;									// Exit
	}

	case WM_CLOSE:								// Did We Receive A Close Message?
	{
													PostQuitMessage(0);						// Send A Quit Message
													return 0;								// Jump Back
	}

	case WM_KEYDOWN:							// Is A Key Being Held Down?
	{
													keys[wParam] = TRUE;					// If So, Mark It As TRUE
													return 0;								// Jump Back
	}

	case WM_KEYUP:								// Has A Key Been Released?
	{
													keys[wParam] = FALSE;					// If So, Mark It As FALSE
													return 0;								// Jump Back
	}

	case WM_SIZE:								// Resize The OpenGL Window
	{
													ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));  // LoWord=Width, HiWord=Height
													return 0;								// Jump Back
	}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE	hInstance,			// Instance
	HINSTANCE	hPrevInstance,		// Previous Instance
	LPSTR		lpCmdLine,			// Command Line Parameters
	int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure
	BOOL	done = FALSE;								// Bool Variable To Exit Loop

	// Ask The User Which Screen Mode They Prefer
	//if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen = FALSE;							// Windowed Mode
	}

	// Create Our OpenGL Window
	if (!CreateGLWindow("Example", 640, 480, 16, fullscreen))
	{
		return 0;									// Quit If Window Was Not Created
	}

	while (!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message == WM_QUIT)				// Have We Received A Quit Message?
			{
				done = TRUE;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if (active)								// Program Active?
			{
				if (keys[VK_ESCAPE])				// Was ESC Pressed?
				{
					done = TRUE;						// ESC Signalled A Quit
				}
				else								// Not Time To Quit, Update Screen
				{
					DrawGLScene();					// Draw The Scene
					SwapBuffers(hDC);				// Swap Buffers (Double Buffering)
				}
			}
			if (keys['M']) {  // إذا تم الضغط على 'M'
        if (!lightOn) {
            glEnable(GL_LIGHTING);  // تفعيل الإضاءة
            glEnable(GL_LIGHT0);     // تفعيل مصدر الضوء
            glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColorOn);  // تغيير لون الضوء إلى الأبيض
            lightOn = true;         // تحديث حالة الضوء إلى "مفعل"
             
        }
    }
    else if (keys['N']) {  // إذا تم الضغط على 'N'
       if (lightOn) {
            glDisable(GL_LIGHTING);  // إيقاف الإضاءة
            glDisable(GL_LIGHT0);    // إيقاف مصدر الضوء
            glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColorOff);  // تغيير لون الضوء إلى الأسود
            lightOn = false;         // تحديث حالة الضوء إلى "مطفأ"
            
        }
    }
			if (keys[VK_F1])						// Is F1 Being Pressed?
			{
				keys[VK_F1] = FALSE;					// If So Make Key FALSE
				KillGLWindow();						// Kill Our Current Window
				fullscreen = !fullscreen;				// Toggle Fullscreen / Windowed Mode
				// Recreate Our OpenGL Window
				if (!CreateGLWindow("OpenGL template", 640, 480, 16, fullscreen))
				{
					return 0;						// Quit If Window Was Not Created
				}
			}
		}
	}

	// Shutdown
	KillGLWindow();									// Kill The Window
	return (msg.wParam);							// Exit The Program
}

