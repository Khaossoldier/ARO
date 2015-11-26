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

int chemin(tGraphe graphe, tNomSommet nom1, tNomSommet nom2, tTabCouleurs tab){
	int i, j;

	i = grapheChercheSommetParNom(graphe, nom1);
	j = grapheChercheSommetParNom(graphe, nom2);

	return tab[i] == 0 && tab[j] == 0;
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

void parcoursEnProf(tGraphe graphe, tNomSommet sommet, tNomSommet fin, char *outfile){
	tNumeroSommet n, x;
	tPileSommets pile, final;
	tTabCouleurs tab;
	int i, index, flag;

	for (i = 0; i < grapheNbSommets(graphe); i++){
		tab[i] = 0;
	}
	pile = pileSommetsAlloue();
	final = pileSommetsAlloue();
	n = grapheChercheSommetParNom(graphe, sommet);


	for (i = 0; i < grapheNbSommets(graphe); i++){
		if (i != n) tab[i] = 1;
	}
	while (!pileSommetsEstVide(pile)){
		pileSommetsDepile(pile);
	}
	while (!pileSommetsEstVide(final)){
		pileSommetsDepile(final);
	}
	tab[n] = 2;
	pileSommetsEmpile(pile, n);
	pileSommetsEmpile(final, n);
	while (!pileSommetsEstVide(pile)){
		flag = 0;
		x = pileSommetsTete(pile);
		for (i = 0; i < grapheNbVoisinsSommet(graphe, x); i++){
			index = grapheVoisinSommetNumero(graphe, x, i);
			if (tab[index] == 1){
				
				tab[index] = 2;
				pileSommetsEmpile(pile, index);
				if (pileSommetsTete(final) != grapheChercheSommetParNom(graphe, fin)){
					pileSommetsEmpile(final, index);
				}
				flag = 1;
				i =  grapheNbVoisinsSommet(graphe, x);
				graphe2visu(graphe, outfile, tab);
			}
		}
		if (flag == 0){
			tab[x] = 0;
			pileSommetsDepile(pile);
			if (pileSommetsTete(final) != grapheChercheSommetParNom(graphe, fin)){
				pileSommetsDepile(final);
			}
		}
	}
	
	
	if (chemin(graphe, sommet, fin, tab))
	{
		printf("\nIl existe un chemin entre %s et %s\n", sommet, fin);
		while (!pileSommetsEstVide(final)){
			tab[pileSommetsDepile(final)] = 2;
		}	
	}
	graphe2visu(graphe, outfile, tab);
}



int main(int argc, char *argv[])
{
	tGraphe graphe;

	if (argc < 4)
		halt("C.U : %s <fichier graphe> <nom du sommet de depart> <nom du sommet de fin> <nom du fichier>", argv[0]);

	graphe = grapheAlloue();

	grapheChargeFichier(graphe, argv[1]);

	parcoursEnProf(graphe, argv[2], argv[3], argv[4]);
	printf("\n");
	grapheLibere(graphe);
	return 0;
}