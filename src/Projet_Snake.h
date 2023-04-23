#ifndef __PROJET_SNAKE__
#define __PROJET_SNAKE__

	
	/*Import de la librairie g2x*/
	#include <g2x.h>



	/*============= Macros =============*/

	#define NBP_MAX 100			/*Nombre de particules dans la partie*/
	#define TAILLE_SERPENT .2	/*Taille du serpent en "grosseur"*/
	#define DECALAGE 5.			/*Représente le décalage entre les fragments du serpent*/
	#define LONG_SERPENT_MAX 50	/*Longueur maximale que le serpent peut atteindre*/
	#define NB_OBSTACLES 8	/*Nombre d'ostacles max sur la fenêtre*/


	
	/*============= Dimensions =============*/

	/* tailles de la fenêtre (en pixel) */
	static int WWIDTH=1000, WHEIGHT=1000;
	
	/* limites de la zone reelle associee a la fenetre */
	static double wxmin=-10.,wymin=-10.,wxmax=+10.,wymax=+10.;



	/*============= Structures =============*/

	
	/*Couleur d'une des particule d'un serpent*/
	typedef struct Color_Snake{

    	G2Xcolor value;
    	bool is_colored;	/*definit si le fragment est colorié ou non*/

	} Color_Snake;



	/*Membre (portion de corps) du serpent*/
	typedef struct Corps{

    	G2Xpart particule;	/*particule représentant le membre*/
    	Color_Snake color;	/*couleur associée à la particule*/

    	struct Corps * suivant;	/*suite du corps*/

	} Corps;


	/*Représente le serpent*/
	typedef struct list {

    	int size;	/*taille du serpent (longueur)*/
    	Corps * premier;	/*tête du serpent (premier membre du serpent)*/

	} Serpent;


	/*Représente un obstacle circulaire pour le seprent*/
	typedef struct obstacle {

  		G2Xpoint p;		/*position du cercle dans la fenre graphique*/
  		double rayon;	/*rayon du cercle*/

	} Obstacle;




	/*============= Variables =============*/


	/*Représente l'ensemble des obstacles dans la fenêtre*/
	Obstacle obstacles[NB_OBSTACLES];


	/*Représente l'ensemble des particules proies*/
	G2Xpart particules[NBP_MAX];						/*particules*/
	bool is_particule[NBP_MAX];							/*si true alors n'a pas encore été mangée et false sinon*/
	G2Xcolor particules_colors[NBP_MAX];				/*couleurs de chaque particules (permet de préserver leur couleur quand on les remet en jeu)*/

	
	/*Serpent présente dans la fenêtre*/
	Serpent snake;


	double  r;
	double  r2;
	G2Xvector u;

	/*Vitesse du serpent et des particules proies*/
	double  vitesse_particule;
	double vitesse_snake;


	/*Si vaut true alors disque plein et cercle sinon*/
	bool  FILLDISK;



	/*Paramètres (options) de jeu*/
	bool fuite;							/*si vaut true les particules peuvent fuire le serpent*/
	bool chasse;						/*si vaut true le serpent est en mode chasse des proies*/
	bool launch_parts;					/*si vaut true les particules proies mangées par le serpent apparaîssent dans le coin haut droit*/
	bool part_colls;					/*si vaut true les particules s'entrechoquent entre elles*/


#endif