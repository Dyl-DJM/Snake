/*=================================================================*/
/*= Dylan DE JESUS                                                =*/
/*= Université Gustave Eiffel                                     =*/    
/*=================================================================*/


/*=================================================================*/
/*= E.Incerti - eric.incerti@univ-eiffel.fr                       =*/
/*= Université Gustave Eiffel                                     =*/
/*= Code "squelette" pour prototypage avec libg2x.6c              =*/
/*=================================================================*/


#include "Projet_Snake.h"




/*=========================================================================================================*/
/*=                                Fonctions sur la gestion des obstacles                                 =*/
/*=========================================================================================================*/



/*Definit les 'taille' coordonnées des obstacles 'tab'*/
static void init_obstacles(Obstacle * tab, int taille){

    Obstacle obs;

    float x;

    for(int i = 0; i < (taille - 1); i+= 2){

        if(i < (taille - 1) / 2)
            x = wxmin + (i * ((wxmax / (taille / 2)) + 0.5)) + 2; 

        if(i > (taille - 1) / 2)
            x = ((i - ((taille - 1) / 2)) * ((wxmax / (taille / 2)) + 0.5)) + 2 - ((taille - 1) / 2); 


        tab[i].p = (G2Xpoint){x, -5}; 
        tab[i].rayon = r; 

        tab[i + 1].p = (G2Xpoint) {x, 5};
        tab[i + 1].rayon = r; 
    }

    tab[taille - 1].p = (G2Xpoint){0, 0}; 
    tab[taille - 1].rayon = r; 
}


 
/*Fais eviter l'obstacle de point 'C', de rayon 'r' à la particule 'a' à l'aide du coefficient 'coeff'*/
static bool g2x_PartByCircle(G2Xpart *a, G2Xpoint C, double r, double coeff){

    G2Xpart ac = g2x_Part_2p(a->p, C);

    if(ac.d < r){
      a->u = g2x_mapscal2(ac.u, -1);

      return true;
    }

    double t = ac.d * g2x_ProdScal(a->u, ac.u);
    double d = ac.d * g2x_ProdVect(a->u, ac.u);

    if(t < 0. || fabs(d) > r){
        return false;
    }

    G2Xpart cp = g2x_Part_2p(C, (G2Xpoint) {a->p.x + t * a->u.x,
                                            a->p.y + t * a->u.y});

    coeff = CLIP(1., coeff, 25.);
    t = pow(r / t, coeff);
    a->u.x += t * cp.u.x;
    a->u.y += t * cp.u.y;

    g2x_Normalize(&a->u);

    return true;

}










/*=========================================================================================================*/
/*=                                Fonctions sur les particules proies                                    =*/
/*=========================================================================================================*/



/*Effectue les collisions des particules qui se percutent dans le tableau de particules 'tab' de taille 'taille'*/
static void collisions_parts(G2Xpart * tab, bool * tab_bool, int taille){

  G2Xpart p;

  int i, j;

  /*Pour chaque particule proie on regarde si elle touche une autre particule*/
  for(i = 0; i < taille; i++){

    for(j = 0; j < taille; j++){

      if(tab_bool[i] && tab_bool[j]){

        g2x_PartCollPart(&tab[i], &tab[j], 0.3);  /*Si leur point est distant de 0.3 alors elles sont en collisions*/

      }
    }
  }
}




/*Remet les particules du tableau 'tab' de taille 'taille' en jeu dans le coin haut droit*/
static void put_new_parts(G2Xpart * tab, bool * tab_bool, int taille){

  G2Xpart p;

  int i;

  for(i = 0; i < taille; i++){

    if(tab_bool[i] == false){

      p.u = g2x_RandomVector(1.);

      /*on définit un point aléatoire dans un carré de valeurs
      se situant dans le coin haut droit de la fenêtre*/
      p.p.x = (rand() % 2) + (wxmax - 1.5);
      p.p.y = (rand() % 1) + (wxmax - 1.5);

      p.d = vitesse_particule;

      tab_bool[i] = true; /*elles sont remises en jeu*/
      tab[i] = p;
    }
  }
}



/*Renvoie l'indice de la particule la plus proche de 'snake_head' et -1 si aucune particule n'est en jeu*/
static int closest_part(G2Xpart snake_head){

  int i, indice;
  G2Xpart best;

  bool almost_one_left = false;

  indice = 0;
  best = particules[0];

  /*pour chaque proie dans le tableau*/
  for(i = 0; i < NBP_MAX; i++){

    /*On trouve une distance plus proche*/
    if(is_particule[i] && g2x_Dist(particules[i].p, snake_head.p) < g2x_Dist(best.p, snake_head.p)){

      best = particules[i];
      indice = i;

    }

    /*il y a au moins une particule proie en jeu*/
    if(is_particule[i]){
        almost_one_left = true;
    }

  }

  /*il n'y a aucune proie en jeu*/
  if(almost_one_left == false){

    chasse = false; /*on desactive la chasse*/
    return -1;
  }


  return indice; 
}




/*Détermine le déplacement d'une particule 'b' en fuite par rapport à une autre particule 'a'*/
static void fuite_particule(G2Xpart a, G2Xpart * b){


  G2Xvector vec;

  /*On prend un vecteur aléatoire*/
  vec = g2x_RandomVector(1.);


  /*Si en x du vecteur unitaire on va à la rencontre de 'a'*/
  if((b->u.x > 0 && a.u.x < 0) || (b->u.x < 0 && a.u.x > 0)){

    /*le vecteur aléatoire va dans la mauvaise direction on prend l'opposé*/
    if ((vec.x > 0 && b->u.x < 0) || (vec.x < 0 && b->u.x > 0))
      vec.x = vec.y * (-1);

    b->u = vec;


  }

  /*Si en y du vecteur unitaire on va à la rencontre de 'a'*/
  if((b->u.y > 0 && a.u.y < 0) || (b->u.y < 0 && a.u.y > 0)){

    /*le vecteur aléatoire va dans la mauvaise direction on prend l'opposé*/
    if ((vec.y > 0 && b->u.y < 0) || (vec.y < 0 && b->u.y > 0))
      vec.y = vec.y * (-1);

    b->u = vec;

  }

  
  /*On fait accélérer la particule en fuite*/
  b->d = 0.1;

  

  /*On verifie qu'elle n'aille pas dans les obstacles*/
  for(int i = 0; i < NB_OBSTACLES; i++){
          g2x_PartByCircle(b, obstacles[i].p, obstacles[i].rayon + 0.25, r * 2);
  }


  /*On vérifie qu'elle n'est pas au bord*/
  if(b->p.x >= wxmax || b->p.x <= wxmin){
      b->u.x = (-1) * b->u.x;
    }

    if(b->p.y >= wymax || b->p.y <= wymin){
        b->u.y = (-1) * b->u.y;
    }


  g2x_PartMove(b);

  /*la particule a fuit, on redonne une valeur de vitesse normale*/
  b->d = vitesse_particule;
}



/*Remet toutes les proires sorties de la fenre dans la fenêtre*/
static void put_in_window(Obstacle * tab, int taille){

  int i;

  for(i = 0; i < taille; i++){

    if(tab[i].p.x <= wxmin){
      tab[i].p.x = (wxmax - 0.1);
    }

    if(tab[i].p.x >= wxmax){
      tab[i].p.x = (wxmin + 0.1);
    }

    if(tab[i].p.y <= wymin){
      tab[i].p.y = (wymax - 0.1);
    }

    if(tab[i].p.y >= wymax){
      tab[i].p.y = (wymin + 0.1);
    } 

  }

}










/*=========================================================================================================*/
/*=                                Fonctions sur le serpent du jeu                                        =*/
/*=========================================================================================================*/



/*Initialise la structure du serpent en modifiant 'snake'*/
static void init_snake(Serpent * snake){

    snake->size = 1;

    snake->premier = (Corps *) malloc(sizeof(Corps));

    (snake->premier)->particule = g2x_Part_pud(g2x_RandomPoint(5.),g2x_RandomVector(1.), vitesse_snake);

    (snake->premier)->particule.p = g2x_Point(wxmin + 0.5, wymin + 0.5);

    (snake->premier)->color.is_colored = true;
    (snake->premier)->color.value = G2Xw;


    snake->premier->suivant = NULL;

}



/*Affiche graphiquement le serpent 'snake' dans la fenêtre*/
static void show_snake(Serpent snake){

    Corps * ptr = snake.premier;
    Corps * last_ptr;

    for(int i = 0; i  <  snake.size ; i++){

          last_ptr = ptr;

          /*On affiche le fragment coloré*/
          if(ptr->color.is_colored == true){
              g2x_FillCircle(ptr->particule.p.x, ptr->particule.p.y, TAILLE_SERPENT - 0.1, ptr->color.value);
          }

          /*Dessine le contour du fragment*/
          g2x_Circle(ptr->particule.p.x, ptr->particule.p.y, TAILLE_SERPENT - 0.05, G2Xw, 1);
          
         ptr = ptr->suivant;
    }
}





/*Ajoute un fragment de corps  de serpent 'particule' (de couleur 'couleur') en fin de serpent 'list'*/
static void add_case_in_list(Serpent * list, G2Xpart particule, G2Xcolor couleur){

  /*On vérifie que le serpent n'est pas à sa taille maximale sinon on n'ajoute pas*/
  if(list->size >= LONG_SERPENT_MAX){
      return;
  }

  Corps * elem = (Corps *) malloc(sizeof(Corps));
  Corps * premier = list->premier; 
  Corps * ptr = premier;


  if(elem == NULL){
    exit(EXIT_FAILURE);
  }

  elem->particule = particule;
  elem->suivant = NULL;

  for(int i = 0; i < list->size - 1; i++){
    ptr = ptr->suivant;
  }

  /*On donne les valeur à lélément à ajouter*/
  elem->particule.u = ptr->particule.u;
  elem->particule.p = ptr->particule.p;
  elem->particule.p.y += TAILLE_SERPENT + DECALAGE;
  elem->color.is_colored = true;
  elem->color.value = couleur;


  ptr->suivant = elem;

  list->premier = premier;
  list->size += 1;
}



/*Détermine le déplacement du serpent 'snake' dans son environnement*/
static void anim_snake(Serpent snake){

  Corps * ptr = snake.premier;
  G2Xpart old_part = snake.premier->particule;
  G2Xpart ptr_part;

  int closest;


  /*La chasse est activée*/
  if(chasse){

    /*On prend la proie la plus proche du serpent*/
    closest = closest_part(ptr->particule);

    /*Au moins une proie est en jeu*/
    if(closest != -1)
      g2x_PartPosTrack(&(ptr->particule), (particules[closest]).p, 1, 0.5);

  }
  
  /*On regarde mes obstacles*/
  for(int i = 0; i < NB_OBSTACLES; i++){

        if(g2x_PartByCircle(&old_part, obstacles[i].p, obstacles[i].rayon + 0.25, r * 2)){
          ptr->particule = old_part;
        }
  }

  if(old_part.p.x >= wxmax || old_part.p.x <= wxmin){
      ptr->particule.u.x = (-1) * old_part.u.x;
  }

  if(old_part.p.y >= wymax || old_part.p.y <= wymin){
        ptr->particule.u.y = (-1) * old_part.u.y;
  }


  g2x_PartMove(&(ptr->particule));



  ptr = ptr->suivant;

  /*On parcourt tout le serpent*/
  for(int i = 0; i  <  snake.size - 1 ; i++){
      
      /*Le fragment (i + 1) devient le fragment (i)*/   
      ptr_part = ptr->particule;
      ptr->particule = old_part;
      old_part = ptr_part;

      ptr = ptr->suivant;
    }
}








/*=========================================================================================================*/
/*=                                Fonctions sur la gestion du programme                                  =*/
/*=========================================================================================================*/




/*La fonction d'initialisation : appelée 1 seule fois, au début */
static void init(void){


  /* zone graphique reelle associee a la fenetre                 */
  /* si cette fonction n'est pas appelée, les valeurs par défaut */
  /* sont (-1.,-1)->(+1.,+1.)                                    */

  srand(getpid());

  g2x_SetWindowCoord(wxmin,wymin,wxmax,wymax);
  g2x_SetBkgCol(0.);

  /*rayon d'un obstacle*/
  r = 2;


  /*On initialise les obstacles*/
  init_obstacles(obstacles, NB_OBSTACLES);

  /*Le vecteur unitaire*/
  u = (G2Xvector){0.2,0.1};
  


  /*On donnes des valeurs de couleurs aléatoires pour les proies*/
  g2x_ColorMapHSV360(particules_colors, NBP_MAX);

  /*On détermine les vitesses des éléments*/
  vitesse_particule = .03 ;
  vitesse_snake = .1;

  /*On initialise toutes les particules proies du jeu*/
  for(int i = 0; i < NBP_MAX; i++){

      particules[i] = g2x_Part_pud(g2x_RandomPoint(2.),g2x_RandomVector(1.),vitesse_particule);
      is_particule[i] = true;   /*toutes les particules sont en jeu*/
  }


  /*Options*/  
  FILLDISK = true;
  chasse = false;
  fuite = false;
  launch_parts = false;
  part_colls = true;

  /*Initialisation du serpent*/
  init_snake(&snake);

}

/* la fonction de contrôle : appelée 1 seule fois, juste après <init> */
static void ctrl(void){

  /*Scrollbars*/
  g2x_CreateScrollv_d("R",&r,0.5,2.,.5,"rayon de la balle"); // un scrollbar pour le rayon
  g2x_CreateScrollv_d("VP",&vitesse_particule, 0.,0.2,.5,"vitesse des particules"); // un scrollbar pour le rayon

  /*Switch buttons*/
  g2x_CreateSwitch("Col",&FILLDISK,"Disque plein ou cercle");
  g2x_CreateSwitch("Chss",&chasse,"Acite/Desactive la chasse du serpent"); 
  g2x_CreateSwitch("Fuite",&fuite,"Active/Desactive la fuite des proies"); 
  g2x_CreateSwitch("Part",&launch_parts,"Active/Desactive la remise en jeu des particules mangées"); 
  g2x_CreateSwitch("PartColls",&part_colls,"Active/Desactive les collisions entre particules"); 

}


/* la fonction de dessin : appelée en boucle */
static void draw(void){

  /*On rédéfinit les obastacles (au cas où le rayon ait été modifié)*/
  init_obstacles(obstacles, NB_OBSTACLES);

  /*On dessine les obstacles*/
  for(int i = 0; i < NB_OBSTACLES; i++){


      if (FILLDISK) 
        g2x_FillCircle(obstacles[i].p.x, obstacles[i].p.y, obstacles[i].rayon, G2Xy);

      g2x_Circle(obstacles[i].p.x, obstacles[i].p.y, obstacles[i].rayon, G2Xk, 2);
  }


  /*On dessine les particules qui n'ont pas été mangées*/
  for(int i = 0; i < NBP_MAX; i++){

      if(is_particule[i] == false){
        continue;
      }

      g2x_FillCircle(particules[i].p.x, particules[i].p.y, .15, particules_colors[i]); 
  }


  /*On dessine le serpent*/
  show_snake(snake);

}

/* la fonction d'animation (facultatif) */
static void anim(void){

  G2Xpart p;

  
  /*gère les collisions inter particules proies*/
  if(part_colls){
    collisions_parts(particules, is_particule, NBP_MAX);
  }


  /*lâche les particules mangées dans le coin haut droit*/
  if(launch_parts){
    put_new_parts(particules, is_particule, NBP_MAX);
  }



  /*Modifie la vitesse des particules*/
  for(int i = 0; i < NBP_MAX; i++){

      if(is_particule[i] == false){
        continue;
      }
      particules[i] = g2x_Part_pud(particules[i].p, particules[i].u, vitesse_particule);
  }


  /*Gère la fuite des particules proies*/
  if(fuite){
        fuite_particule(snake.premier->particule, &(particules[closest_part(snake.premier->particule)]));
  }


  /*Animation particules*/
  for(int i = 0; i < NBP_MAX; i++){

      if(is_particule[i] == false){
        continue;
      }

      /*modifie la position de la particule*/
      p = particules[i];
      g2x_PartMove(&p);
      particules[i] = p;


      
      /*Gère les rebonds sur tous les obstales*/
      for(int i = 0; i < NB_OBSTACLES; i++){

          g2x_PartByCircle(&p, obstacles[i].p, obstacles[i].rayon + 0.25, r * 2);
      }

      particules[i] = p;


      /*Le serpent rencontre une particule proie et donc la mange pour s'agrandir*/
      if(p.p.x >= snake.premier->particule.p.x - TAILLE_SERPENT && p.p.x <= snake.premier->particule.p.x + TAILLE_SERPENT && p.p.y >= snake.premier->particule.p.y - TAILLE_SERPENT && p.p.y <= snake.premier->particule.p.y + TAILLE_SERPENT){
          add_case_in_list(&snake, p, particules_colors[i]);
          is_particule[i] = false;
      }


      /*Gère les rebonds sur les bors de la fenêtre*/
      if(p.p.x >= wxmax || p.p.x <= wxmin){
        particules[i].u.x = (-1) * particules[i].u.x;

        g2x_PartMove(&particules[i]);
      }

      if(p.p.y >= wymax || p.p.y <= wymin){
        particules[i].u.y = (-1) * particules[i].u.y;

        g2x_PartMove(&particules[i]);
      }
  }

  /*On déplace le serpent*/
  anim_snake(snake);


  /*On ajoute les particules hors de la fenêtre dans la fenêtre*/
  put_in_window(obstacles, NBP_MAX);

}

/* la fonction de sortie  (facultatif) -- atexit() */
static void quit(void){

    fprintf(stderr,"Bye !\n\n");
}


/***************************************************************************/
/* La fonction principale : NE CHANGE JAMAIS                               */
/***************************************************************************/
int main(int argc, char **argv){
  /* creation de la fenetre - titre et tailles (pixels) */
  g2x_InitWindow(*argv,WWIDTH,WHEIGHT);

  g2x_SetInitFunction(init); /* fonction d'initialisation */
  g2x_SetCtrlFunction(ctrl); /* fonction de contrôle      */
  g2x_SetDrawFunction(draw); /* fonction de dessin        */
  g2x_SetAnimFunction(anim); /* fonction d'animation      */
  g2x_SetExitFunction(quit); /* fonction de sortie        */

  /* lancement de la boucle principale */
  return g2x_MainStart();
  /* RIEN APRES CA */
}
