#pragma once


#include <cmath>
#include <string>
#include <iostream>
#include <vector>
#include "V2.h"
#include <list>
#include <iterator>
#include <algorithm>
 
using namespace std;


struct Color
{
	float R, G, B, A;

	Color(float r, float g, float b)          { R = r; G = g; B = b; A = 1; }
	Color(float r, float g, float b, float a) { R = r; G = g; B = b; A = a; }

	const static Color Black, White, Red, Green, Blue, Magenta, Cyan, Yellow, Gray;  // use Color::Black
};
 
enum class Key {
	Unknown, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	LEFT, RIGHT, UP, DOWN, ENTER, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12 };

// On a fait le choix de définir une classe entité dont va découler les autres classes pour éviter la répétition de code
class Entity {
public :
	string Texture;
	V2 Pos;
	V2 Size;
	int IdTex;
	Entity::Entity(std::string txt,V2 pos);
	bool DetectEntity(Entity entity);
	bool PositionValide(int x, int y);
	bool* DeplacementsValides();
	V2 DirectionAuto(int pas);
	void DeplacementAuto(V2 v);
};

class Item {
public:
	string Name;
	int Quantity;
	Item::Item(std::string name, int quantity);
	bool Shooting = false; // la condition pour que la balle/arrow bouge
	V2 Direction = V2(0, 0);  // la direction de la balle ainsi que la distance qui sera parcouru
};

class Heros : public Entity {
public:
	int health;
	string Texture2;
	Heros::Heros(std::string txt, std::string txt2, V2 pos, int hp);
	Item Key = Item("Key", 0);
	Item Diamond = Item("Diamond", 0);
	Item Colt = Item("Colt", 0);
	Item Bullet = Item("Bullet", 0); // la quantité est à -1 sinon on ne peut pas changer la position de départ du colt car il respawn à une position précise dès que le nombre de balles atteint 0
	Item Arrow = Item("Arrow", -1);
};

class Momie : public Entity {
public:
	int health;
	Momie::Momie(std::string txt, V2 pos, int hp);
	V2 firstDirection = V2(0,0); // V2.x représente une direction et V2.y un nombre de pas
	// mettre un pas de -1 permet de bloquer les momies
};

namespace G2D
{
	// key management 
	bool IsKeyPressed(Key k); 

	// Main Graphic System Function

	void InitWindow(int & argc, char** & argv, V2 WindowSize, V2 WindowStartPos, std::string name);
	void Run( void logic(), void render() );
	void ClearScreen(Color c);
	void Show();  // send all draw commands 
 
	// Mouse event
	void GetMouseButtonInfo(int idButton, bool & clicked, int & x, int & y);

	// Font
	void DrawStringFontMono (V2 pos, std::string text, float fontSize = 20, float thickness = 3, Color c = Color::Black);
	void DrawStringFontRoman(V2 pos, std::string text, float fontSize = 20, float thickness = 3, Color c = Color::Black);

	// Crée une texture à partir d'une description 1 lettre = 1 couleur / voir exemple
	int InitTextureFromString(V2 & Size, const std::string & Sprite);

	// Affiche un rectangle texturé à la position (x,y) (x+w,y+h)
	// angleDef produit une rotation du rectangle autours de son centre
	int  ExtractTextureFromPPM(const std::string & filepath, bool applyTransparency);
	void DrawRectWithTexture(int IDtexture, V2 pos, V2 size, float angleDeg = 0);
	void DrawRectangle(V2 P1, V2 Size, Color c, bool fill = false);
	void DrawCircle(V2 C, float r, Color c, bool fill = false);
 
	// Draw Geometry
	void DrawLine(V2 P1, V2 P2, Color c );
	void DrawPolygon(std::vector<V2> & PointList, Color c, bool fill = false);

	// Timing function
	double ElapsedTimeFromLastCallbackSeconds();
	double ElapsedTimeFromStartSeconds();


}