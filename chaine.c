#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
/* N´ecessaire pour la macro WEXITSTATUS */
#include "sys/wait.h"
#include "graphe.h"
#include <sys/time.h>

/* Couleurs */
typedef enum {ROUGE=0, BLEU=1, VERT=2} tCouleur;
typedef tCouleur tTabCouleurs[MAX_SOMMETS];

char * recupNomCouleur(tNumeroSommet t1, tNumeroSommet t2){
	if (t1==t2)
	{
		return "blue";
	}
	return "black";
}



void graphe2visu(tGraphe graphe, char *outfile, tNumeroSommet depart, tNumeroSommet pred[MAX_SOMMETS]) {
	FILE *fic;
	char commande[80];
	char dotfile[80]; /* le fichier dot pour cr´eer le ps */
	int ret, i, j, index;
	tNomSommet nom1, nom2;
	/* on va cr´eer un fichier pour graphviz, dans le fichier "outfile".dot */
	strcpy(dotfile, outfile);
	strcat(dotfile, ".dot");
	fic = fopen(dotfile, "w");
	if (fic==NULL)
		halt ("Ouverture du fichier %s en ´ecriture impossible\n", dotfile);
	

	if (grapheEstOriente(graphe)){
		fprintf(fic, "digraph {\n");
		for (i = 0; i < grapheNbSommets(graphe); i++){
			grapheRecupNomSommet(graphe, i, nom1);
			fprintf(fic, "%s [color=%s];\n", nom1, recupNomCouleur(depart, i));
		}
		for (i = 0; i < grapheNbSommets(graphe); i++){
			for (j = 0; j < grapheNbSuccesseursSommet(graphe, i); j++){
				index = grapheSuccesseurSommetNumero(graphe, i, j);
				grapheRecupNomSommet(graphe, i, nom1);
				grapheRecupNomSommet(graphe, index, nom2);
				fprintf(fic, "%s -> %s;\n", nom1, nom2);
			}
		}
		fprintf(fic, "}");
	}
	else{
		fprintf(fic, "graph {\n");
		for (i = 0; i < grapheNbSommets(graphe); i++){
			grapheRecupNomSommet(graphe, i, nom1);
			fprintf(fic, "%s [color=%s];\n", nom1, recupNomCouleur(depart, i));
		}
		for (i = 0; i < grapheNbSommets(graphe); i++){
			for (j = 0; j < grapheNbVoisinsSommet(graphe, i); j++){
				grapheRecupNomSommet(graphe, i, nom1);
				index = grapheVoisinSommetNumero(graphe, i, j);
				if (index > i){
					grapheRecupNomSommet(graphe, index, nom2);
					fprintf(fic, "%s -- %s [color=%s];\n", nom1, nom2, recupNomCouleur(depart, pred[i]));
				}
			}
		}
		fprintf(fic, "}");
	}
	fclose(fic);
	sprintf(commande, "dot -Tps %s -o %s.ps", dotfile, outfile);
	ret = system(commande);
	if (WEXITSTATUS(ret))
		halt("La commande suivante a ´echou´e\n%s\n", commande);
}


long nbMicroSecondesDepuisDebutHeure() {
struct timeval tv;
long us;
gettimeofday(&tv, NULL);
// tv.tv_sec : nbre de secondes depuis Epoch
// tv.tv_usec : compl´ement en microsecondes
tv.tv_sec = tv.tv_sec % 3600; // on fait un modulo une heure (=3600s)
us = (tv.tv_sec*1000000)+tv.tv_usec;
return us;
}

void plusCourteChaine(tGraphe graphe, tNomSommet sommet, char *outfile){
	int i, index;
	tTabCouleurs tab;
	tFileSommets file;
	tNumeroSommet n, x;
	int d[MAX_SOMMETS];
	tNumeroSommet pred[MAX_SOMMETS];

	for (i = 0; i < grapheNbSommets(graphe); i++){
		tab[i] = 0;
		d[i] = 0;
		pred[i] = 0; 
	}
	file = fileSommetsAlloue();
	n = grapheChercheSommetParNom(graphe, sommet);


	for (i = 0; i < grapheNbSommets(graphe); i++){
		if (i != n) tab[i] = 1;
	}

	while (!fileSommetsEstVide(file)){
		fileSommetsDefile(file);
	}

	d[n] = 0;
	tab[n] = 2;
	fileSommetsEnfile(file, n);
	while (!fileSommetsEstVide(file)){
		x = fileSommetsDefile(file);
		for (i = 0; i < grapheNbVoisinsSommet(graphe, x); i++){
			index = grapheVoisinSommetNumero(graphe, x, i);
			if (tab[index] == 1){
				tab[index] = 2;
				fileSommetsEnfile(file, index);
				d[index] = d[x] + 1;
				pred[index] = x;
			}
		}
		tab[x] = 0;
	}
	for (i = 0; i < grapheNbSommets(graphe); i++){
		printf("d[%d] = %d; pred[%d] = %d\n", i, d[i], i, pred[i]);
	}
	graphe2visu(graphe, outfile, n, pred);
}

int main(int argc, char *argv[])
{
	tGraphe graphe;
	long timer;

	if (argc < 0)
		halt("C.U : %s <fichier graphe> <nom du sommet de depart> <nom du fichier>", argv[0]);

	graphe = grapheAlloue();

	grapheChargeFichier(graphe, argv[1]);
	/*grapheAleatoire(graphe, 99, 0, 0.1);*/
	timer = nbMicroSecondesDepuisDebutHeure();
	plusCourteChaine(graphe, argv[2], argv[3]);
	printf("%ld \n", nbMicroSecondesDepuisDebutHeure() - timer);
	grapheLibere(graphe);
	return 0;
}