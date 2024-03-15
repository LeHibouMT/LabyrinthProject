
#pragma warning( disable : 4996 ) 


#include <cstdlib>
#include <vector>
#include <iostream>
#include <string>
#include "G2D.h"
#include <list>
#include <chrono>
#include <thread>
#ifdef  _WIN32
#include "glut.h" 
#endif
#ifdef _WIN32
#include <Windows.h>
#endif

using namespace std;
typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::duration<double> duration;

// PARTIE SUR LES CONTRUCTEURS AINSI QUE LES FONCTIONS DES CLASSES
// LES CLASSES SONT DANS G2D.h ON A FAIT CE CHOIX POUR AVOIR UN PROJET PLUS LISIBLE ET UNE MEILLEURE HIERARCHIE

Entity::Entity(std::string txt, V2 pos) {
	this->Pos = pos;
	this->IdTex = 0;
	this->Size;
	this->Texture = txt;
};

bool Entity::DetectEntity(Entity entity) {
	V2 pos = this->Pos;  // On va considérer qu'il y a collision si nous avons proximité à l'horizontale et à la verticale
	return (((pos.x <= entity.Pos.x && pos.x + this->Size.x >= entity.Pos.x) || // proximité par la gauche
		(pos.x >= entity.Pos.x && pos.x <= entity.Pos.x + entity.Size.x)) && // proximité par la droite
		((pos.y <= entity.Pos.y && pos.y + this->Size.y >= entity.Pos.y) || // proximité par le bas
			(pos.y >= entity.Pos.y && pos.y <= entity.Pos.y + entity.Size.y))); // proximité par le haut
};

Item::Item(std::string name, int quantity) {
	this->Name = name;
	this->Quantity = quantity;
};

Heros::Heros(std::string txt, std::string txt2, V2 pos, int hp) : Entity(txt, pos) {
	this->health = hp;
	this->Texture2 = txt2;
};

Momie::Momie(std::string txt, V2 pos, int hp) : Entity(txt, pos) {
	this->health = hp;
};





// PARTIE SUR LES DATA DU JEU

struct GameData
{
	// On a fait le choix d'utiliser des timers, mais utiliser un nombre de frame ou un compteur était tout à fait possible, le fonctionnement étant similaire
	double timer1 = (0.0); // permet d'avoir un timer pour plusieurs fonctions dont les déplacements des momies
	double timer2 = (0.0); // pour les pas du héros
	double timer3 = (0.0); // pour les respawns du diamant ou des momies
	double timer4 = (0.0); // pour la balle
	double timer5 = (0.0); // pour l'arrow
	int ScreenNumber = 0; // numéro de l'écran actuel
	int NumberOfDiamonds = 0; // Nombre de diamants ramassés

	// Partie Accueil // Inutile actuellement mais je le laisse vu que ça m'a prit du temps
	Entity Welcome = Entity(
		"[W       W       W  WWWWW  W      WWWWW  WWWWWWW  WW       WW  WWWWW]"
		"[ W     W W     W   W      W      W      W     W  W W     W W  W    ]"
		"[  W   W   W   W    WWWWW  W      W      W     W  W  W   W  W  WWWWW]"
		"[   W W     W W     W      W      W      W     W  W   W W   W  W    ]"
		"[    W	      W      WWWWW  WWWWW  WWWWW  WWWWWWW  W    W    W  WWWWW]",
		V2(25, 300));

	// Partie Game Over
	Entity Game = Entity(
		"[WWWWW  WWWWW  WW       WW  WWWWW]"
		"[W      W   W  W W     W W  W    ]"
		"[W  WW  WWWWW  W  W   W  W  WWWWW]"
		"[W   W  W   W  W   W W   W  W    ]"
		"[WWWWW  W   W  W    W    W  WWWWW]",
		V2(165,400));

	Entity Over = Entity(
		"[WWWWWW  W       W  WWWWW  WWWWW]"
		"[W    W   W     W   W      W   W]"
		"[W    W    W   W    WWWWW  WWWWW]"
		"[W    W     W W     W      W W  ]"
		"[WWWWWW      W      WWWWW  W  W ]",
		V2(170,300));

	Entity GG = Entity(
		"[WWWWW  WWWWW]"
		"[W      W    ]"
		"[W  WW  W  WW]"
		"[W   W  W   W]"
		"[WWWWW  WWWWW]",
		V2(225, 300));
   

	// Partie Jeu

	string Map =
		"MMMMMMMMMMMMMMM"
		"M M           M"
		"M M M MMM MMM M"
		"M   M       M M"
		"MMM M M MMM M M"
		"M   M M     M M"
		"M MMM MMM MMMMM"
		"M   M  M      M"
		"M M M  M M MM M"
		"M M M  M M M  M"
		"M M M MM M MMMM"
		"M M M    M    M"
		"M M M MMMMMMM M"
		"M M      M    M"
		"MMMMMMMMMMMMMMM";

	// indique la présence d'un mur à la case (x,y)
	bool Mur(int x, int y) { return Map[(15 - y - 1) * 15 + x] == 'M'; }

	int Lpix = 40;  // largeur en pixels des cases du labyrinthe

	// data de la clé
	Entity Key = Entity(    // Entité de la clé
		"[                      ]"
		"[     W W         WWWW ]"
		"[     W W        W    W]"
		"[   WWWWWWWWWWWWWW    W]"
		"[                W    W]"
		"[                 WWWW ]",
		//V2(45, 80)); // pour les tests
		V2(525, 375));

	// data du coffre
	Entity Chest = Entity(
		"[WWWWWWWWWWWWWWWWWWWWWW]"
		"[WWWWWWWW      WWWWWWWW]"
		"[WWWWWWWW      WWWWWWWW]"
		"[WWWWWWWWWW  WWWWWWWWWW]"
		"[WWWWWWWWWW  WWWWWWWWWW]"
		"[WWWWWWWWWWWWWWWWWWWWWW]",
		//V2(45, 125)); // pour les tests
	    V2(410,55));

	// data des diamants
	Entity Diamond = Entity(
		"[  WWWWWWWWWWWWWWWWWWW  ]"
		"[ W   W           W   W ]"
		"[WWWWWWWWWWWWWWWWWWWWWWW]"
		"[ W     W       W     W ]"
		"[  W     W     W     W  ]"
		"[   W     W   W     W   ]"
		"[    W     W W     W    ]"
		"[     WWWWWWWWWWWWW     ]",
		//V2(45,125)); // pour les tests
		V2(485, 212));

	// data du colt
	Entity Colt = Entity(
		"[WWWWWWWWWWWWWW]"
		"[WWWWWWWWWWWWWW]"
		"[           WWW]"
		"[           WWW]"
		"[           WWW]"
		"[           WWW]",
		//V2(45,125)); // pour les tests
		V2(44, 540));

	// data de la balle
	// la balle va être spéciale car elle sera toujours présente mais lorsqu'elle n'est pas utilisée,
	// elle sera cachée en dehors de l'écran
	Entity Bullet = Entity(
		"[  W  ]"
		"[  W  ]"
		"[WWWWW]"
		"[  W  ]"
		"[  W  ]",
		//V2(45,125)); // pour les tests
		V2(600,600));

	// data de la flèche
	// la flèche fonctionnera comme une balle
	Entity Arrow = Entity(
		"[  R  ]"
		"[  R  ]"
		"[RRRRR]"
		"[  R  ]"
		"[  R  ]",
		//V2(45,125)); // pour les tests
		V2(600, 600));

	// data du trapArrow
	Entity TrapArrow = Entity(
		"[YYYYYY]"
		"[Y    Y]"
		"[Y    Y]"
		"[Y    Y]"
		"[YYYYYY]",
		//V2(45,125)); // pour les tests
		V2(475, 55));

	// data des traps
	Entity Trap1 = Entity(
		"[RRRRRR]"
		"[R    R]"
		"[R    R]"
		"[R    R]"
		"[RRRRRR]",
		//V2(45,125)); // pour les tests
		V2(65, 480));

	Entity Trap2 = Entity(
		"[RRRRRR]"
		"[R    R]"
		"[R    R]"
		"[R    R]"
		"[RRRRRR]",
		//V2(45, 125)); // pour les tests
		V2(519, 245));
	
	Entity Trap3 = Entity(
		"[RRRRRR]"
		"[R    R]"
		"[R    R]"
		"[R    R]"
		"[RRRRRR]",
		//V2(45, 125)); // pour les tests
		V2(545, 65));

	// data du héros
	Heros Heros = Heros::Heros(
		"[RRR  ]"
		"[RRWR ]"
		"[RRR  ]"
		"[YY   ]"
		"[YYY  ]"
		"[YY YG]"
		"[GG   ]"
		"[CC   ]"
		"[CC   ]"
		"[C C  ]"
		"[C C  ]",
		"[RRR  ]"
		"[RRWR ]"
		"[RRR  ]"
		"[YY   ]"
		"[YYY  ]"
		"[YY YG]"
		"[GG   ]"
		"[CCC  ]"
		"[CCC  ]"
		"[C    ]"
		"[C    ]",
		//V2(500,55), // pour les tests
		V2(45, 45),
		3);
	
	// data des momies
	Momie Momie1 = Momie(
		"[CCC  ]"
		"[CCWC ]"
		"[CCC  ]"
		"[CC   ]"
		"[CCC  ]"
		"[CC CG]"
		"[GG   ]"
		"[CC   ]"
		"[CC   ]"
		"[C C  ]"
		"[C C  ]",
		V2(375,350),
		2);


	Momie Momie2 = Momie(
		"[RRR  ]"
		"[RRWR ]"
		"[RRR  ]"
		"[RR   ]"
		"[RRR  ]"
		"[RR RG]"
		"[GG   ]"
		"[RR   ]"
		"[RR   ]"
		"[R R  ]"
		"[R R  ]",
		V2(455, 450),
		2);

	Momie Momie3 = Momie(
		"[YYY  ]"
		"[YYWY ]"
		"[YYY  ]"
		"[YY   ]"
		"[YYY  ]"
		"[YY YG]"
		"[GG   ]"
		"[YY   ]"
		"[YY   ]"
		"[Y Y  ]"
		"[Y Y  ]",
		V2(250, 250),
		2);

	// data du score
	int Score = 0;

	GameData() {}

};

GameData G;






// PARTIE SUR LES DEPLACEMENTS DES ENTITES 

// indique si la position (x,y) est valide
bool Entity::PositionValide(int x, int y) {
	if ((0<=x/40<=14) && (0<=y/40<=14)) {
		return !G.Mur(x/40, y/40);
	}
	else return false;
};

// fonction pour générer les déplacements valides du héros
// retournant un pointeur
bool* Entity::DeplacementsValides() {
	static bool t[4];
	t[0] = this->PositionValide(this->Pos.x - 1, this->Pos.y) && this->PositionValide(this->Pos.x - 1, this->Pos.y + this->Size.y); // déplacement à gauche
	t[1] = this->PositionValide(this->Pos.x + 1 + this->Size.x, this->Pos.y) && this->PositionValide(this->Pos.x + 1 + this->Size.x, this->Pos.y + this->Size.y), // déplacement à droite
	t[2] = this->PositionValide(this->Pos.x, this->Pos.y + 1 + this->Size.y) && this->PositionValide(this->Pos.x + this->Size.x, this->Pos.y + 1 + this->Size.y), // déplacement en haut
	t[3] = this->PositionValide(this->Pos.x, this->Pos.y - 1) && this->PositionValide(this->Pos.x + this->Size.x, this->Pos.y - 1); // déplacement en bas
	return t;
};

// Le but est de déterminer les directions que l'entité peut prendre et il en choisit une aléatoirement
// Pour que la direction semble intéressante, il faut que l'entité puisse avancer d'au moins 'pas' cases dans la direction
V2 Entity::DirectionAuto(int pas) {
	static bool Dauto[4];
	Dauto[0] = true;
	Dauto[1] = true;
	Dauto[2] = true;
	Dauto[3] = true;
	for (int i = 1; i < pas + 1; i++) {
		Dauto[0] = Dauto[0] && this->PositionValide(this->Pos.x - i, this->Pos.y) && this->PositionValide(this->Pos.x - i, this->Pos.y + this->Size.y); // direction gauche
		Dauto[1] = Dauto[1] && this->PositionValide(this->Pos.x + i + this->Size.x, this->Pos.y) && this->PositionValide(this->Pos.x + i + this->Size.x, this->Pos.y + this->Size.y); // direction droite
		Dauto[2] = Dauto[2] && this->PositionValide(this->Pos.x, this->Pos.y + i + this->Size.y) && this->PositionValide(this->Pos.x + this->Size.x, this->Pos.y + i + this->Size.y); // direction haut
		Dauto[3] = Dauto[3] && this->PositionValide(this->Pos.x, this->Pos.y - i) && this->PositionValide(this->Pos.x + this->Size.x, this->Pos.y - i); // direction bas
	}
	int testbloque = 0;
	for (int i = 0; i < 4; i++) {
		if (*(Dauto + i)) testbloque++;
	};
	if (testbloque == 0) {   // dans le cas où aucune direction n'est bonne, on va baisser le pas
		return DirectionAuto(pas - 5);
	}
	else {
		int i = rand() % 4;
		while (!*(Dauto + i)) {
			i = rand() % 4;   // on prend un déplacement valide aléatoire
		};
		return V2(i, pas); // retourne une direction ainsi que le nombre de pas possible dans cette direction !
	}
};

// IA des entités, en particulier les momies
// Une utilité de programmer cette fonction pour toutes les entités est de pouvoir faire bouger le héros ou même la clé
// Cela va servir au déplacement de la balle
void Entity::DeplacementAuto(V2 v)
{
	int direction = v.x;
	int pas = v.y;
	if (pas > 0) {
		if (direction == 0) {
			this->Pos.x--;   // On pourrait changer la position de manière plus importante pour que la momie se déplace plus rapidement mais ça ne serait pas fluide à l'écran
		}
		if (direction == 1) {
			this->Pos.x++;
		}
		if (direction == 2) {
			this->Pos.y++;
		}
		if (direction == 3) {
			this->Pos.y--;
		}
	}
	// Correction en cas de bug et que l'entité arrive dans le mur, ce n'est pas vraiment nécessaire
	//if (this->PositionValide(this->Pos.x, this->Pos.y + this->Size.y) || !this->PositionValide(this->Pos.x + this->Size.x, this->Pos.y + this->Size.y)) this->Pos.y--;
	//if (!this->PositionValide(this->Pos.x, this->Pos.y) || !this->PositionValide(this->Pos.x + this->Size.x, this->Pos.y)) this->Pos.y++;
};




// fonction pour reset le jeu après avoir perdu/gagné
void reset()
{
	G.Heros.health = 3;
	G.Heros.Pos = V2(45, 45);
	G.Heros.Key.Quantity = 0;
	G.Key.Pos = V2(525, 375);
	G.Chest.Pos = V2(410, 55);
	G.Diamond.Pos = V2(485, 212);
	G.Colt.Pos = V2(44, 540);
	G.Bullet.Pos = V2(600, 600);
	G.Arrow.Pos = V2(600, 600);
	G.TrapArrow.Pos = V2(475, 55);
	G.Momie1.Pos = V2(375, 350);
	G.Momie2.Pos = V2(455, 450);
	G.Momie3.Pos = V2(250, 250);
	G.Momie1.health = G.Momie2.health = G.Momie3.health = 2;
	G.Heros.Key.Quantity = G.Heros.Colt.Quantity = G.Heros.Bullet.Quantity = G.Score = 0;
	G.Heros.Arrow.Quantity = -1;
	G.Heros.Arrow.Shooting = false;
	G.Heros.Bullet.Shooting = false;
	G.Momie1.firstDirection.y = G.Momie2.firstDirection.y = G.Momie3.firstDirection.y = 0;
};





// PARTIE SUR L'AFFICHAGE

// écran d'accueil
void renderWelcome() // ScreenNumber 0
{
	G2D::ClearScreen(Color::Black);
	// On a en commentaire une ancienne version d'affichage "plus bricolage"
	// Affichage du Welcome avec un zoom x 8
	// G2D::DrawRectWithTexture(G.Welcome.IdTex, G.Welcome.Pos, G.Welcome.Size); // Ancien affichage de bienvenue
	G2D::DrawStringFontRoman(V2(100, 500), "WELCOME", 100, 1, Color::White);
	G2D::DrawStringFontRoman(V2(135, 400), "TO THE", 100, 1, Color::White);
	G2D::DrawStringFontRoman(V2(85, 300), "LABYRINTH", 100, 1, Color::White);
	G2D::DrawStringFontRoman(V2(195, 200), "Score : " + to_string(G.Score), 50, 1, Color::White);
	G2D::Show();
};

// écran de jeu
void renderGame() // ScreenNumber 1
{
	// Affichage de l'écran de jeu

	// Affichage de l'écran de fond noir
	G2D::ClearScreen(Color::Black);

	// Affichage de la carte
	for (int x = 0; x < 15; x++)
		for (int y = 0; y < 15; y++)
		{
			int xx = x * G.Lpix;
			int yy = y * G.Lpix;
			if (G.Mur(x, y))
				G2D::DrawRectangle(V2(xx, yy), V2(G.Lpix, G.Lpix), Color::Blue, true);
		}

	// affichage du héro avec boite englobante et zoom x 2
	G2D::DrawRectangle(G.Heros.Pos, G.Heros.Size, Color::Red);
	G2D::DrawRectWithTexture(G.Heros.IdTex, G.Heros.Pos, G.Heros.Size);

	// affichage de la clef et zoom x 1.25
	G2D::DrawRectWithTexture(G.Key.IdTex, G.Key.Pos, G.Key.Size);

	// affichage du coffre et zoom x 2
	G2D::DrawRectWithTexture(G.Chest.IdTex, G.Chest.Pos, G.Chest.Size);

	// affichage du diamant et zoom x 2
	G2D::DrawRectWithTexture(G.Diamond.IdTex, G.Diamond.Pos, G.Diamond.Size);

	// affichage du colt et zoom x 2
	G2D::DrawRectWithTexture(G.Colt.IdTex, G.Colt.Pos, G.Colt.Size);

	// affichage de la balle et zoom x 2
	G2D::DrawRectWithTexture(G.Bullet.IdTex, G.Bullet.Pos, G.Bullet.Size);

	// affichage de la flèche et zoom x 2
	G2D::DrawRectWithTexture(G.Arrow.IdTex, G.Arrow.Pos, G.Arrow.Size);

	// afficgage de la TrapArrow et zoom x 2
	G2D::DrawRectWithTexture(G.TrapArrow.IdTex, G.TrapArrow.Pos, G.TrapArrow.Size);

	// affichage des traps et zoom x 2
	G2D::DrawRectWithTexture(G.Trap1.IdTex, G.Trap1.Pos, G.Trap1.Size);
	G2D::DrawRectWithTexture(G.Trap2.IdTex, G.Trap2.Pos, G.Trap2.Size);
	G2D::DrawRectWithTexture(G.Trap3.IdTex, G.Trap3.Pos, G.Trap3.Size);

	// affichage des momies et zoom x 2
	G2D::DrawRectWithTexture(G.Momie1.IdTex, G.Momie1.Pos, G.Momie1.Size);
	G2D::DrawRectWithTexture(G.Momie2.IdTex, G.Momie2.Pos, G.Momie2.Size);
	G2D::DrawRectWithTexture(G.Momie3.IdTex, G.Momie3.Pos, G.Momie3.Size);

	// affichage du score 
	G2D::DrawStringFontRoman(V2(400, 573), "Score : " + to_string(G.Score), 20, 1, Color::White);

	// affichage du nombre de diamants
	G2D::DrawStringFontRoman(V2(250, 573), "Diamants : " + to_string(G.NumberOfDiamonds), 20, 1, Color::White);

	G2D::Show();
};

// écran de mort
void renderGameOver() // ScreenNumber 2
{
	G2D::ClearScreen(Color::Black);
	// On a en commentaire une ancienne version d'affichage "plus bricolage"
	// Affichage du Welcome avec un zoom x 8
	// G2D::DrawRectWithTexture(G.Game.IdTex, G.Game.Pos, G.Game.Size);
	// G2D::DrawRectWithTexture(G.Over.IdTex, G.Over.Pos, G.Over.Size);
	G2D::DrawStringFontRoman(V2(215, 335), "YOU", 100, 1, Color::White);
	G2D::DrawStringFontRoman(V2(215, 235), "DIED", 100, 1, Color::White);
	G2D::DrawStringFontRoman(V2(0, 0), "HAHA NOOB - probably Michel", 10, 1, Color::White);
	G2D::Show();
};

// écran de victoire
void renderWin() // ScreenNumber 3
{
	G2D::ClearScreen(Color::Black);
	// Affichage de l'écran de win avec un zoom x 10
	G2D::DrawRectWithTexture(G.GG.IdTex, G.GG.Pos, G.GG.Size);
	G2D::DrawStringFontRoman(V2(195, 200), "Score : " + to_string(G.Score), 50, 1, Color::White);
	G2D::Show();
}

// écran des règles
void renderRules() // ScreenNumber 4
{
	G2D::ClearScreen(Color::Black);
	G2D::DrawStringFontRoman(V2(25, 550), "Pour gagner, il faut ouvrir le coffre apres avoir ramasse la cle.", 17, 1, Color::White);
	G2D::DrawStringFontRoman(V2(25, 500), "Vous avez 3 vies, perdre une vie vous ramene au point de depart.", 17, 1, Color::White);
	G2D::DrawStringFontRoman(V2(25, 450), "Vous pouvez ramasser une arme qui a 6 munitions et tirer avec z,q,s,d.", 17, 1, Color::White);
	G2D::DrawStringFontRoman(V2(25, 400), "Les zombies ont chacun 2 vies, 6 munitions suffisent.", 17, 1, Color::White);
	G2D::DrawStringFontRoman(V2(25, 350), "L'arme respawn apres avoir tire toutes les munitions.", 17, 1, Color::White);
	G2D::DrawStringFontRoman(V2(25, 300), "Toucher un zombie ou une trappe rouge fait perdre une vie.", 17, 1, Color::White);
	G2D::DrawStringFontRoman(V2(25, 250), "Toucher une trappe jaune tire une fleche dans le couloir.", 17, 1, Color::White);
	G2D::DrawStringFontRoman(V2(25, 200), "Vous pouvez augmenter votre score en accomplissant des objectifs.", 17, 1, Color::White);
	G2D::DrawStringFontRoman(V2(25, 150), "Le diamant respawn toutes les 5 secondes.", 17, 1, Color::White);
	G2D::DrawStringFontRoman(V2(25, 100), "Vous pouvez mettre le jeu en pause avec la touche entree.", 17, 1, Color::White);
	G2D::DrawStringFontRoman(V2(25, 50), "Jeu realise par Michel le boss, Lesly et Oussama.", 17, 1, Color::White);

	G2D::Show();
};

void render()
{
	if (G.ScreenNumber == 0) renderWelcome();
	if (G.ScreenNumber == 1) renderGame();
	if (G.ScreenNumber == 2) renderGameOver();
	if (G.ScreenNumber == 3) renderWin();
	if (G.ScreenNumber == 4) renderRules();
};


// PARTIE SUR LA LOGIQUE DU JEU

void ScreenControl() // gestion des écrans
{
	// affichage des écrans
	// changement d'écran vers l'écran de jeu
	if (G2D::IsKeyPressed(Key::ENTER)) {
		Sleep(250);
		if (G.ScreenNumber == 0) {    // passage de l'écran d'accueil vers l'écran de jeu
			G.ScreenNumber = 4;
			glutDisplayFunc(render);
		}
		else {
			if (G.ScreenNumber == 1) { // passage de l'écran de jeu vers l'écran d'accueil
				G.ScreenNumber = 0;
				glutDisplayFunc(render);
			}
			else {  // passage de l'écran GameOver vers l'écran de jeu 
				if (G.ScreenNumber == 2) {
					G.ScreenNumber = 1;
					glutDisplayFunc(render);
				}
				else {
					if (G.ScreenNumber == 3) {
						G.ScreenNumber = 0;
						glutDisplayFunc(render);
					}
					else {
						if (G.ScreenNumber == 4) {
							G.ScreenNumber = 1;
							glutDisplayFunc(render);
						}
					}
				}
			}
		}
	}
}

void PlayerMovements() // gestion des déplacements
{
	// Le héros 
	// Les déplacements
	bool* t; // pointeur vers le tableau des déplacements autorisés
	t = G.Heros.DeplacementsValides();
	if (G2D::IsKeyPressed(Key::LEFT) && *(t) && G.ScreenNumber == 1) {
		G.Heros.Pos.x--;
	}
	if (G2D::IsKeyPressed(Key::RIGHT) && *(t + 1) && G.ScreenNumber == 1) {
		G.Heros.Pos.x++;
	}
	if (G2D::IsKeyPressed(Key::UP) && *(t + 2) && G.ScreenNumber == 1) {
		G.Heros.Pos.y++;
	}
	if (G2D::IsKeyPressed(Key::DOWN) && *(t + 3) && G.ScreenNumber == 1) {
		G.Heros.Pos.y--;
	}

	// changement de texture du héros pour qu'on ait l'impression qu'il marche
	if (G.timer2 < 0.25) {
		G.timer2 += G2D::ElapsedTimeFromLastCallbackSeconds();
		G.Heros.IdTex = G2D::InitTextureFromString(G.Heros.Size, G.Heros.Texture2); // On met la texture de base du héros pendant une seconde
		G.Heros.Size = G.Heros.Size * 2;
		G2D::DrawRectWithTexture(G.Heros.IdTex, G.Heros.Pos, G.Heros.Size);
	}
	if (G.timer2 >= 0.25 && G.timer2 <= 0.5) {
		G.timer2 += G2D::ElapsedTimeFromLastCallbackSeconds();
		G.Heros.IdTex = G2D::InitTextureFromString(G.Heros.Size, G.Heros.Texture); // On change la texture du héros pour une seconde
		G.Heros.Size = G.Heros.Size * 2;
		G2D::DrawRectWithTexture(G.Heros.IdTex, G.Heros.Pos, G.Heros.Size);
	}
	if (G.timer2 > 0.5) G.timer2 = 0;
};

void PlayerInteractions()  // gestion des interactions entre le joueur et les éléments du jeu
{
	// Les collisions avec des entités telles que les momies ou la clé par le héros
	// avec les momies
	if (G.Heros.DetectEntity(G.Momie1) || G.Heros.DetectEntity(G.Momie2) || G.Heros.DetectEntity(G.Momie3)) { // détection des momies
		G.Heros.health--;
		if (G.Heros.health > 0) G.Heros.Pos = V2(45, 45); // retour au départ
		else {
			reset();
			G.ScreenNumber = 2;
			glutDisplayFunc(renderGameOver);
		};
	};
	// avec la clé
	if (G.Heros.DetectEntity(G.Key)) { // détection de la clé
		G.Key.Pos = V2(550, 575);
		G.Heros.Key.Quantity++;
		G.Score += 500;
	};
	// avec le coffre
	if (G.Heros.DetectEntity(G.Chest)) { // détection du coffre
		if (G.Heros.Key.Quantity > 0) {
			G.Score += 10000;
			G.Heros.Pos = V2(45, 45); // on le remet au point de départ car sinon le héros gagne des points à l'infini
			glutDisplayFunc(renderWin);
			G.ScreenNumber = 3;
			G.timer1 = 0; // On reset le timer
		};
	}
	// avec la fin du jeu
	if (G.ScreenNumber == 3 && G.timer1 > 3.0) {
		reset();
		G.ScreenNumber = 0;
		glutDisplayFunc(render); // 3 secondes après le reset du timer, on affiche l'écran d'accueil
	}
	// avec le diamant
	if (G.Heros.DetectEntity(G.Diamond)) { // détection du diamant
		G.Diamond.Pos = V2(600, 600);
		G.Heros.Diamond.Quantity++;
		G.timer3 = 0; // On reset le timer
		G.Score += 1000;
		G.NumberOfDiamonds++;
	}
	// respawn du diamant
	if ((G.Diamond.Pos.x == 600 && G.Diamond.Pos.y == 600) && G.timer3 > 5.0) { // timing du respawn du diamant
		G.Diamond.Pos = V2(485, 212);   // respawn du diamant 
	}
	else G.timer3 += G2D::ElapsedTimeFromLastCallbackSeconds();
	// avec le colt
	if (G.Heros.DetectEntity(G.Colt)) {  // détection du colt
		G.Colt.Pos = V2(203, 573);
		G.Heros.Colt.Quantity++;
		G.Heros.Bullet.Quantity = 6;
	}
	// avec les traps
	if (G.Heros.DetectEntity(G.Trap1) || G.Heros.DetectEntity(G.Trap2) || G.Heros.DetectEntity(G.Trap3)) {
		G.Heros.Pos = V2(45, 45);
		G.Heros.health--;
		if (G.Heros.health > 0) G.Heros.Pos = V2(45, 45); // retour au départ
		else {
			reset();
			G.ScreenNumber = 2;
			glutDisplayFunc(renderGameOver);
		};
	}
	// avec la TrapArrow
	// activation
	if (G.Heros.DetectEntity(G.TrapArrow)) {
		G.TrapArrow.Pos = V2(600, 600); // on déplace le piège pour qu'il ne s'active qu'une seule fois
		G.Arrow.Pos = V2(400, 55);
		G.Heros.Arrow.Shooting = true;
		G.Heros.Arrow.Quantity++;
		G.Heros.Arrow.Direction = V2(1, 150);
		G.timer5 = 0; // on reset le timer
	}
	// tir de la flèche
	if (G.Heros.Arrow.Quantity > 0) {
		Sleep(100); // pour qu'on voit qu'il se passe quelque chose et pouvoir voir la flèche arriver
	}
	if (G.timer5 > 0.0005 && G.ScreenNumber == 1) {
		if (G.Heros.Arrow.Shooting && G.Heros.Arrow.Direction.y > 0) {
			G.Arrow.DeplacementAuto(G.Heros.Arrow.Direction); // déplacement de l'arrow
			G.Heros.Arrow.Direction.y--;
			if (G.Heros.Arrow.Direction.y == 0) G.Arrow.Pos = V2(600, 600);
		}
		G.timer5 = 0;
	}
	else {
		G.timer5 += G2D::ElapsedTimeFromLastCallbackSeconds();
	}
	// avec la flèche
	if (G.Heros.DetectEntity(G.Arrow)) {
		G.Heros.health--;
		if (G.Heros.health > 0) G.Heros.Pos = V2(45, 45); // retour au départ
		else {
			reset();
			G.ScreenNumber = 2;
			glutDisplayFunc(renderGameOver);
		};
	}
};

void ColtLogic() // gestion du Colt
{
	// déplacement de la balle
	// tir des balles
	if (G2D::IsKeyPressed(Key::Q) && G.Heros.Bullet.Quantity > 0) { // tir vers la gauche
		Sleep(250); // pour avoir un temps entre les tirs et éviter de tirer plusieurs balles d'un coup
		G.Heros.Bullet.Quantity--;
		if (G.Heros.Bullet.Quantity == 0) { // respawn du colt après avoir tiré toutes les balles
			G.Colt.Pos = V2(44, 540);
			G.Heros.Colt.Quantity--;
		}
		G.Bullet.Pos = G.Heros.Pos;
		G.timer1 = 0; // On reset le timer
		G.Heros.Bullet.Shooting = true; // La condition pour que la balle bouge est activée
		G.Heros.Bullet.Direction = V2(0, 100);
	}
	if (G2D::IsKeyPressed(Key::D) && G.Heros.Bullet.Quantity > 0) { // tir vers la droite
		Sleep(100); // pour avoir un temps entre les tirs
		G.Heros.Bullet.Quantity--;
		if (G.Heros.Bullet.Quantity == 0) { // respawn du colt après avoir tiré toutes les balles
			G.Colt.Pos = V2(44, 540);
			G.Heros.Colt.Quantity--;
		}
		G.Bullet.Pos = G.Heros.Pos;
		G.timer1 = 0; // On reset le timer
		G.Heros.Bullet.Shooting = true; // La condition pour que la balle bouge est activée
		G.Heros.Bullet.Direction = V2(1, 100);
	}
	if (G2D::IsKeyPressed(Key::Z) && G.Heros.Bullet.Quantity > 0) { // tir vers le haut
		Sleep(100); // pour avoir un temps entre les tirs
		G.Heros.Bullet.Quantity--;
		if (G.Heros.Bullet.Quantity == 0) { // respawn du colt après avoir tiré toutes les balles
			G.Colt.Pos = V2(44, 540);
			G.Heros.Colt.Quantity--;
		}
		G.Bullet.Pos = G.Heros.Pos;
		G.timer1 = 0; // On reset le timer
		G.Heros.Bullet.Shooting = true; // La condition pour que la balle bouge est activée
		G.Heros.Bullet.Direction = V2(2, 100);
	}
	if (G2D::IsKeyPressed(Key::S) && G.Heros.Bullet.Quantity > 0) { // tir vers le bas
		Sleep(100); // pour avoir un temps entre les tirs
		G.Heros.Bullet.Quantity--;
		if (G.Heros.Bullet.Quantity == 0) { // respawn du colt après avoir tiré toutes les balles
			G.Colt.Pos = V2(44, 540);
			G.Heros.Colt.Quantity--;
		}
		G.Bullet.Pos = G.Heros.Pos;
		G.timer1 = 0; // On reset le timer
		G.Heros.Bullet.Shooting = true; // La condition pour que la balle bouge est activée
		G.Heros.Bullet.Direction = V2(3, 100);
	}
	// Déplacement de la balle
	if (G.timer4 > 0.005 && G.ScreenNumber == 1) {
		if (G.Heros.Bullet.Shooting && G.Heros.Bullet.Direction.y > 0) {
			G.Bullet.DeplacementAuto(G.Heros.Bullet.Direction); // déplacement de la balle
			G.Heros.Bullet.Direction.y--;
			if (G.Heros.Bullet.Direction.y == 0) G.Bullet.Pos = V2(600, 600);
		}
		G.timer4 = 0;
	}
	else {
		G.timer4 += G2D::ElapsedTimeFromLastCallbackSeconds() * 3;  // On peut multiplier le timer pour que la balle aille plus vite
		// Cependant la vitesse de la balle à une limite à cause de la vitesse de rafraichissement entre les calls
	}
};

void MomiesMovements(int pas, double time) // gestion de déplacements des momies
{
	// Déplacements réguliers des momies
	if (G.timer1 > time && G.ScreenNumber == 1) { // ce timer définit en fait la vitesse de déplacement des momies
		// bien que les momies ont tous la même vitesse, on peut leur assigner des mouvements différents grâce au pas
		// par exemple si on met un pas de 600, la momie ne s'arrêtera que lorsqu'il y a un mur (car la map fait 600)
		// si on met 25 par exemple comme lors du test de la vidéo, les momies vont privilégier les mouvements de 25. 
		if (G.Momie1.firstDirection.y == 0) G.Momie1.firstDirection = G.Momie1.DirectionAuto(pas); // le pas ici définit l'amplitude des mouvements de la momie
		else {
			G.Momie1.DeplacementAuto(G.Momie1.firstDirection);
			G.Momie1.firstDirection.y--;
		}
		if (G.Momie2.firstDirection.y == 0) G.Momie2.firstDirection = G.Momie2.DirectionAuto(pas); // le pas ici définit l'amplitude des mouvements de la momie
		else {
			G.Momie2.DeplacementAuto(G.Momie2.firstDirection);
			G.Momie2.firstDirection.y--;
		}
		if (G.Momie3.firstDirection.y == 0) G.Momie3.firstDirection = G.Momie3.DirectionAuto(pas); // le pas ici définit l'amplitude des mouvements de la momie
		else {
			G.Momie3.DeplacementAuto(G.Momie3.firstDirection);
			G.Momie3.firstDirection.y--;
		}
		G.timer1 = 0;
	}
	else {
		G.timer1 += G2D::ElapsedTimeFromLastCallbackSeconds() * 1;  // On peut içi modifier le facteur d'actualisation du timer et ça augmenterait la vitesse de déplacement de la momie
	}
};

void MomiesInteractions()
{
	// Détection des momies par la balle
	if (G.Bullet.DetectEntity(G.Momie1)) { // détection des momies
		G.Momie1.health -= 1;
		G.Bullet.Pos = V2(600, 600); // on fait disparaître la balle
		Sleep(100);
		if (G.Momie1.health == 0) {
			G.Momie1.Pos = V2(800, 800); // déplacement de la momie quand elle meurt
			G.Momie1.health = 1;
			G.Score += 500;
			G.Momie1.firstDirection.y = -1;
		}
	}
	if (G.Bullet.DetectEntity(G.Momie2)) { // détection des momies
		G.Momie2.health -= 1;
		G.Bullet.Pos = V2(600, 600); // on fait disparaître la balle
		Sleep(100);
		if (G.Momie2.health == 0) {
			G.Momie2.Pos = V2(800, 800); // déplacement de la momie quand elle meurt
			G.Momie2.health = 1;
			G.Score += 500;
			G.Momie2.firstDirection.y = -1;
		}
	}
	if (G.Bullet.DetectEntity(G.Momie3)) { // détection des momies
		G.Momie3.health -= 1;
		G.Bullet.Pos = V2(600, 600); // on fait disparaître la balle
		Sleep(100);
		if (G.Momie3.health == 0) {
			G.Momie3.Pos = V2(800, 800); // déplacement de la momie quand elle meurt
			G.Momie3.health = 1;
			G.Score += 500;
			G.Momie3.firstDirection.y = -1;
		}
	}
}

void GameLogic() // logique du jeu reliant les fonctions précédentes
{
	// controle des écrans
	ScreenControl();

	// déplacements du joueur
	PlayerMovements();

	// intéractions du joueur
	PlayerInteractions();

	// gestion du colt
	ColtLogic();

	// déplacements des momies
	MomiesMovements(600, 0.025);

	// intéractions des momies
	MomiesInteractions();
};





// PARTIE SUR LE LANCEMENT DU JEU 
void AssetsInit()
{
	// Size passé en ref et texture en param
	
	//Welcome
	G.Welcome.IdTex = G2D::InitTextureFromString(G.Welcome.Size, G.Welcome.Texture);
	G.Welcome.Size = G.Welcome.Size * 8; // on peut zoomer la taille du sprite
	//Game
	G.Game.IdTex = G2D::InitTextureFromString(G.Game.Size, G.Game.Texture);
	G.Game.Size = G.Game.Size * 8; // on peut zoomer la taille du sprite
	//Over
	G.Over.IdTex = G2D::InitTextureFromString(G.Over.Size, G.Over.Texture);
	G.Over.Size = G.Over.Size * 8; // on peut zoomer la taille du sprite
	//GG
	G.GG.IdTex = G2D::InitTextureFromString(G.GG.Size, G.GG.Texture);
	G.GG.Size = G.GG.Size * 10; // on peut zoomer la taille du sprite

	//Heros
	G.Heros.IdTex = G2D::InitTextureFromString(G.Heros.Size, G.Heros.Texture);
	G.Heros.Size = G.Heros.Size * 2; // on peut zoomer la taille du sprite

	//Key
	G.Key.IdTex = G2D::InitTextureFromString(G.Key.Size, G.Key.Texture);
	G.Key.Size = G.Key.Size * 1.25; // on peut zoomer la taille du sprite

	//Chest
	G.Chest.IdTex = G2D::InitTextureFromString(G.Chest.Size, G.Chest.Texture);
	G.Chest.Size = G.Chest.Size * 2; // on peut zoomer la taille du sprite

	//Diamond
	G.Diamond.IdTex = G2D::InitTextureFromString(G.Diamond.Size, G.Diamond.Texture);
	G.Diamond.Size = G.Diamond.Size * 2; // on peut zoomer la taille du sprite

	//Colt
	G.Colt.IdTex = G2D::InitTextureFromString(G.Colt.Size, G.Colt.Texture);
	G.Colt.Size = G.Colt.Size * 2; // on peut zoomer la taille du sprite

	//Bullet
	G.Bullet.IdTex = G2D::InitTextureFromString(G.Bullet.Size, G.Bullet.Texture);
	G.Bullet.Size = G.Bullet.Size * 2; // on peut zoomer la taille du sprite

	//Arrow
	G.Arrow.IdTex = G2D::InitTextureFromString(G.Arrow.Size, G.Arrow.Texture);
	G.Arrow.Size = G.Arrow.Size * 2; // on peut zoomber la taille du sprite

	//TrapArrow
	G.TrapArrow.IdTex = G2D::InitTextureFromString(G.TrapArrow.Size, G.TrapArrow.Texture);
	G.TrapArrow.Size = G.TrapArrow.Size * 2; // on peut zoomber la taille du sprite
	

	//Trap
	G.Trap1.IdTex = G2D::InitTextureFromString(G.Trap1.Size, G.Trap1.Texture);
	G.Trap1.Size = G.Trap1.Size * 2; // on peut zoomer la taille du sprite
	G.Trap2.IdTex = G2D::InitTextureFromString(G.Trap2.Size, G.Trap2.Texture);
	G.Trap2.Size = G.Trap2.Size * 2; // on peut zoomer la taille du sprite
	G.Trap3.IdTex = G2D::InitTextureFromString(G.Trap3.Size, G.Trap3.Texture);
	G.Trap3.Size = G.Trap3.Size * 2; // on peut zoomer la taille du sprite

	//Momies
	G.Momie1.IdTex = G2D::InitTextureFromString(G.Momie1.Size, G.Momie1.Texture);
	G.Momie2.IdTex = G2D::InitTextureFromString(G.Momie2.Size, G.Momie2.Texture);
	G.Momie3.IdTex = G2D::InitTextureFromString(G.Momie3.Size, G.Momie3.Texture);
	G.Momie1.Size = G.Momie1.Size * 2; // on peut zoomer la taille du sprite
	G.Momie2.Size = G.Momie2.Size * 2;
	G.Momie3.Size = G.Momie3.Size * 2;
};

int main(int argc, char* argv[])
{
	G2D::InitWindow(argc, argv, V2(G.Lpix * 15, G.Lpix * 15), V2(300, 100), string("Labyrinthe"));

	AssetsInit();

	G2D::Run(GameLogic, renderWelcome);
};


