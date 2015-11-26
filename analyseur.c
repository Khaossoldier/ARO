#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/* N´ecessaire pour la macro WEXITSTATUS */
#include "sys/wait.h"
#include "graphe.h"

void graphe2visu(tGraphe graphe, char *outfile) {
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


int main(int argc, char *argv[])
{
	tGraphe graphe;
	tNomSommet nom;
	int i, j, max;

	if (argc < 3)
		halt("C.U : %s <fichier graphe> <nom fichier>", argv[0]);

	graphe = grapheAlloue();

	grapheChargeFichier(graphe, argv[1]);

	max = 0;

	printf("Sommets sans voisins :");
	for (i = 0; i < grapheNbSommets(graphe); i++){
		if (grapheNbVoisinsSommet(graphe, i) == 0){
			grapheRecupNomSommet(graphe, i, nom);
			printf(" %s", nom);
		}
		if (grapheNbVoisinsSommet(graphe, i) > max)
			max = grapheNbVoisinsSommet(graphe, i);
	}
	printf("\n\nSommets avec le maximum de voisins :");

	for (j = 0; j < grapheNbSommets(graphe); j++){
		if (grapheNbVoisinsSommet(graphe, j) == max){
			grapheRecupNomSommet(graphe, j, nom);
			printf(" %s", nom);
		}
	}
	printf("\n");
	graphe2visu(graphe, argv[2]);
	grapheLibere(graphe);
	return 0;
}