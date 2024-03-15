////////////////////////////////////////////////////////
//
//      	graphiX functions of G2D
//
///////////////////////////////////////////////////////

#include "G2D.h"

#ifdef __linux__ 
#include "GL/glut.h"
#endif

#ifdef  _WIN32
#include "glut.h" 
#endif

#ifdef __APPLE__
#include "GLUT/glut.h"
#endif

#include <string>
#include <map>
#include <vector>
#include <chrono>

using namespace std;


/////////////////////////////////////////////////////////////
//
//	    Window Management
//
/////////////////////////////////////////////////////////////
 

void G2D::ClearScreen(Color c)
{
	glClearColor(c.R, c.G, c.B, c.A);
	glClear(GL_COLOR_BUFFER_BIT);
}


void G2D::Show()
{
	glFlush(); // single buffer
	glutSwapBuffers();  // double buffer
}


/////////////////////////////////////////////////////////////
//
//	    RectWithTexture
//
/////////////////////////////////////////////////////////////



void G2D::DrawRectWithTexture(int texture, V2 pos, V2 size, float angleDeg)
{
	glLineWidth(0);

	float w = size.x;
	float h = size.y;
	const double MPI = 3.14159265358979323846;
	glColor4ub(0, 0, 0, 128);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_TEXTURE_2D);


	//angleDeg = 45;
	float angleRad = angleDeg / 180 * MPI;

	float rX = cos(angleRad) * w - sin(angleRad) * h;
	float rY = sin(angleRad) * w + cos(angleRad) * h;

	float cx = pos.x + w / 2 - rX / 2;
	float cY = pos.y + h / 2 - rY / 2;

	glPushMatrix();
	glTranslatef(cx, cY, 0.0);
	glRotatef(angleDeg, 0.0, 0.0, 1.0);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(0, 0, 0.0f);
	glTexCoord2f(0.0, 1.0); glVertex3f(0, h, 0.0f);
	glTexCoord2f(1.0, 1.0); glVertex3f(w, h, 0.0f);
	glTexCoord2f(1.0, 0.0); glVertex3f(w, 0, 0.0f);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}



/////////////////////////////////////////////////////////////
//
//	    Geometry
//
/////////////////////////////////////////////////////////////


void G2D::DrawRectangle(V2 P1, V2 Size, Color c, bool fill)
{
	glLineWidth(1);
	glColor4d(c.R, c.G, c.B, c.A);
	glDisable(GL_TEXTURE_2D);

	if (fill)   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else    	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glRecti((int)P1.x, (int)P1.y, (int)(P1.x + Size.x), (int)(P1.y + Size.y));

}

void G2D::DrawCircle(V2 C, float r, Color c, bool fill)
{
	vector<V2> LPoints;

	int lineAmount = r / 4; //  nb of triangles used to draw circle
	if (lineAmount < 20) lineAmount = 20;

	const double PI = 3.14159265358;

	//GLfloat radius = 0.8f; //radius
	double step = 2 * PI / lineAmount;

	for (int i = 0; i <= lineAmount; i++)
		LPoints.push_back(V2(C.x + r * cos(i *  step), C.y + r * sin(i * step)));

	G2D::DrawPolygon(LPoints, c, fill);
}

void G2D::DrawLine(V2 P1, V2 P2, Color c)
{
	glLineWidth(1);
	glColor4d(c.R, c.G, c.B, c.A);
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);
	glVertex2f(P1.x, P1.y);
	glVertex2f(P2.x, P2.y);
	glEnd();
}

void G2D::DrawPolygon(vector<V2> & PointList, Color c, bool fill)
{

	if (fill) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_TEXTURE_2D);

	glLineWidth(1);
	glColor4d(c.R, c.G, c.B, c.A);

	glBegin(GL_POLYGON);
	for (V2 P : PointList)
		glVertex2f(P.x, P.y);
	glEnd();
}

/////////////////////////////////////////////////////////////
//
//	    Font
//
/////////////////////////////////////////////////////////////


void DrawString(V2 pos, string text, float fontSize, float thickness, Color c, bool FontMono)
{
	glColor4f(c.R, c.G, c.B, c.A);
	 

	// EPAISSEUR de la font
	glLineWidth(thickness);

	glPushMatrix();
	glTranslatef(pos.x, pos.y, 0);
	glScalef(1 / 152.38, 1 / 152.38, 1 / 152.38);
	glScalef(fontSize, fontSize, fontSize);
	const char * cc = text.c_str();
	for (char* p = (char*)cc; *p; p++)
	{
		if (FontMono) glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *p);
		else          glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
	}
	glPopMatrix();
	glLineWidth(1);
}


void G2D::DrawStringFontMono(V2 pos, string text, float fontSize, float thickness, Color c)
{
	glDisable(GL_TEXTURE_2D);
	DrawString(pos, text, fontSize, thickness, c, true);
}

void G2D::DrawStringFontRoman(V2 pos, string text, float fontSize, float thickness, Color c)
{
	glDisable(GL_TEXTURE_2D);
	DrawString(pos, text, fontSize, thickness, c, false);
}

/////////////////////////////////////////////////////////////
//
//	    Create Texture From String
//
/////////////////////////////////////////////////////////////


typedef unsigned char u8;

u8 LetterToColor[256][4];

void AssignColor(int code, u8 R, u8 G, u8 B, u8 A = 255)
{

	LetterToColor[code][0] = R;
	LetterToColor[code][1] = G;
	LetterToColor[code][2] = B;
	LetterToColor[code][3] = A;
}

void InitColors()
{
	// https://www.rapidtables.org/fr/web/color/RGB_Color.html

	AssignColor('R', 255, 0, 0);
	AssignColor('G', 0, 255, 0);
	AssignColor('B', 0, 0, 255);
	AssignColor('K', 0, 0, 0); // noir
	AssignColor('W', 255, 255, 255); // white
	AssignColor('M', 255, 0, 255); // magenta
	AssignColor('C', 0, 255, 255); // cyan
	AssignColor('Y', 255, 255, 0); // yellow
	AssignColor('S', 192, 192, 192); // silver
	AssignColor('G', 128, 128, 128); // gray
	AssignColor('O', 255, 215, 0); // or
	AssignColor(' ', 255, 255, 255, 0); // transparent
	AssignColor('[', 255, 255, 255, 0); // transparent
	AssignColor(']', 255, 255, 255, 0); // transparent
}

int CORECreateTextureFromRGBA(unsigned char * data, int width, int height)
{
	GLuint t = 0;
	glGenTextures(1, &t);
	glBindTexture(GL_TEXTURE_2D, t);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	return t;
}



int G2D::InitTextureFromString(V2 & Size, const string & Sprite)
{
	int LargPix = Sprite.find(']') + 1;
	Size.x = LargPix;
	int nb = Sprite.length();
	if (nb %  LargPix != 0)
		cout << "Erreur taille texture/string incorrecte";

	int HautPix = nb / LargPix;
	Size.y = HautPix;


	int pos = 0;
	unsigned char data[128 * 128];

	for (int y = HautPix - 1; y >= 0; y--)
		for (int x = 0; x < LargPix; x++)
		{
			int  p = x + y * LargPix;
			char c = Sprite[p];
			for (int t = 0; t < 4; t++)
				data[pos++] = LetterToColor[c][t];
		}

	int IdTexture = CORECreateTextureFromRGBA(data, LargPix, HautPix);
	return IdTexture;
}

