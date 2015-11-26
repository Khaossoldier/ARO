#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
/* N´ecessaire pour la macro WEXITSTATUS */
#include "sys/wait.h"
#include "graphe.h"
/* Couleurs */
typedef enum {ROUGE=0, BLEU=1, VERT=2} tCouleur;
typedef tCouleur tTabCouleurs[MAX_SOMMETS];


char * recupNomCouleur(int i, tTabCouleurs tab){
	switch(tab[i]){
		case 0: return "red";
		case 1: return "blue";
		case 2: return "green";
		default: return "this number does not exists";
	}
}



void graphe2visu(tGraphe graphe, char *outfile, tTabCouleurs tabCouleurs) {
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
			fprintf(fic, "%s [color=%s];\n", nom1, recupNomCouleur(i, tabCouleurs));
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
			fprintf(fic, "%s [color=%s];\n", nom1, recupNomCouleur(i, tabCouleurs));
		}
		for (i = 0; i < grapheNbSommets(graphe); i++){
			for (j = 0; j < grapheNbVoisinsSommet(graphe, i); j++){
				grapheRecupNomSommet(graphe, i, nom1);
				index = grapheVoisinSommetNumero(graphe, i, j);
				if (index > i){
					grapheRecupNomSommet(graphe, index, nom2);
					fprintf(fic, "%s -- %s;\n", nom1, nom2);
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

void parcoursEnLargeur(tGraphe graphe, tNomSommet sommet, char *outfile){
	tNumeroSommet n, x;
	tFileSommets file;
	tTabCouleurs tab;
	int i, index;

	for (i = 0; i < grapheNbSommets(graphe); i++){
		tab[i] = 0;
	}
	file = fileSommetsAlloue();
	n = grapheChercheSommetParNom(graphe, sommet);


	for (i = 0; i < grapheNbSommets(graphe); i++){
		if (i != n) tab[i] = 1;
	}
	graphe2visu(graphe, outfile, tab);
	while (!fileSommetsEstVide(file)){
		fileSommetsDefile(file);
	}
	tab[n] = 2;
	fileSommetsEnfile(file, n);
	while (!fileSommetsEstVide(file)){
		x = fileSommetsDefile(file);
		for (i = 0; i < grapheNbVoisinsSommet(graphe, x); i++){
			index = grapheVoisinSommetNumero(graphe, x, i);
			if (tab[index] == 1){
				sleep(5);
				graphe2visu(graphe, outfile, tab);
				tab[index] = 2;
				fileSommetsEnfile(file, index);
			}
		}
		tab[x] = 0;
	}
	graphe2visu(graphe, outfile, tab);
}


int main(int argc, char *argv[])
{
	tGraphe graphe;

	if (argc < 4)
		halt("C.U : %s <fichier graphe> <nom du sommet de depart> <nom du fichier>", argv[0]);

	graphe = grapheAlloue();

	grapheChargeFichier(graphe, argv[1]);

	parcoursEnLargeur(graphe, argv[2], argv[3]);
	printf("\n");
	grapheLibere(graphe);
	return 0;
}