#include "FileBicluster.h"
#include "VNS.h"
#include "GRASP.h"
#include <algorithm>
#include <stdio.h>
#include <limits.h>
#include <sstream>
#include <iostream>
#define N_EXEC 5

using namespace std;

int main(int argc, char** argv)
{
	

   	// ----- Carregar Dados ----- //
   	// Abre arquivo
   	char *instancia;
    instancia = argv[1];
	int constr_type = Bigraph::N2_N1;
	struct timeval tempoAntes, ta;
    struct timeval tempoDepois, td;

	FileBicluster::loadFile(instancia, constr_type);
	//rb->nrule2();
	srand(time(NULL));	

	
	int min_grasp = INT_MAX;

	float media_grasp = 0;

	float tempo_medio_grasp = 0;
       
	GRASP g(0.5);
	

	//execucoes do GRASP
	//cout << "GRASP" << endl;
	gettimeofday(&tempoAntes,NULL);
	srand(time(NULL));	
	for(int i=0;i<N_EXEC;i++){
		cout << "i = " << i << endl;
	//while(true){
		//Bigraph* sol = g.grasp_vns(85.8f);
		Bigraph* sol = g.run(30);
		media_grasp+=sol->edit;
		if(sol->edit < min_grasp)
		   min_grasp = sol->edit;

		//cout << "Min: " << sol->edit << endl;
		//for(int i=1; i<10; ++i)
		//	FileBicluster::printInstancia("nin", instancia, sol, i);
		cout << sol->clustersAsStr() << endl << endl; 
		delete sol;
		//exit(0);
		//cout << "Min: " << min_grasp << " Med: " << media_grasp << endl;
	}
	gettimeofday(&tempoDepois, NULL);
        tempo_medio_grasp = calculaTempo(&tempoAntes, &tempoDepois)/N_EXEC;
	media_grasp /= N_EXEC;

	
	cout << "Min_grasp: " << min_grasp << " Media: " << media_grasp << " TempoMed: " << tempo_medio_grasp << endl; 
}

