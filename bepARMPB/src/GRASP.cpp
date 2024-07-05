#include <list>
#include <iostream>
#include <limits.h>

#include "GRASP.h"
#include "VNS.h"
//#include "BiclusterElite.h"

GRASP::GRASP(double a)
{
	this->alpha = a;
}

Bigraph* GRASP::grasp_vns(float maxTime)
{
	struct timeval tempoAntes, tempoDepois;
	Bigraph* best = NULL;
	GRASP g(0.5);

	gettimeofday(&tempoAntes, NULL);

	while(true)
	{
		Bigraph* sol_constr = g.construcao();
		Bigraph* sol_vns = VNS::vns(sol_constr);

		if( best == NULL or (sol_vns != NULL and sol_vns->edit < best->edit))
		{
			delete best;
			best = sol_vns;
		}else
		{
			delete sol_vns;
		}

		delete sol_constr;

		//verifica se o tempo acabou
		gettimeofday(&tempoDepois, NULL);
		if( calculaTempo(&tempoAntes, &tempoDepois) >= maxTime )
			break;
	}

	return best;
}

Bigraph* GRASP::grasp_vns(int n_iter)
{
	Bigraph* best = NULL;
	GRASP g(0.5);

	for(int i = 0; i < n_iter; i++)
	{
		Bigraph* sol_constr = g.construcao();
		Bigraph* sol_vns = VNS::vns(sol_constr);

		if( best == NULL or (sol_vns != NULL and sol_vns->edit < best->edit))
		{
			delete best;
			best = sol_vns;
		}else
		{
			delete sol_vns;
		}

		delete sol_constr;

	}

	return best;
}


Bigraph* GRASP::grasp_ils(float maxTime)
{
	struct timeval tempoAntes, tempoDepois;
	Bigraph* best = NULL;
	GRASP g(0.5);

	gettimeofday(&tempoAntes, NULL);

	while(true)
	{
		Bigraph* sol_constr = g.construcao();
		Bigraph* sol_ils = VNS::ils(sol_constr);

		if( best == NULL or (sol_ils != NULL and sol_ils->edit < best->edit))
		{
			delete best;
			best = sol_ils;
		}else
		{
			delete sol_ils;
		}

		delete sol_constr;

		//verifica se o tempo acabou
		gettimeofday(&tempoDepois, NULL);
		if( calculaTempo(&tempoAntes, &tempoDepois) >= maxTime )
			break;
	}

	return best;
}


Bigraph* GRASP::grasp_ils(int n_iter)
{
	Bigraph* best = NULL;
	GRASP g(0.5);

	for(int i = 0; i < n_iter; i++)
	{
		Bigraph* sol_constr = g.construcao();
		Bigraph* sol_ils = VNS::ils(sol_constr);

		if( best == NULL or (sol_ils != NULL and sol_ils->edit < best->edit))
		{
			delete best;
			best = sol_ils;
		}else
		{
			delete sol_ils;
		}

		delete sol_constr;
	}

	return best;
}


//BL
/*Bigraph *GRASP::buscaLocal(Bigraph *solAtual){

	Bigraph *bestBL = new Bigraph(*solAtual);
	
	while(true){
        	Bigraph *bl = NULL;

           	Bigraph *bl1 = bestBL->bl2(false);
           
	   	if(bl != NULL){
			bl = bl1;
		}

                bl = bestBL->bl3(false);
                if(bl != NULL){
			if(bl1 != NULL){
				if(bl1->edit < bl->edit){
                 			delete bl;
					bl = bl1;
                        	}else{
                        		delete bl1;
                        	}
			}
                 }else
			bl = bl1;
	
	   	 bl = bestBL->bl1();
                
		if(bl != NULL){
			if(bl1 != NULL){
				if(bl1->edit < bl->edit){
                 			delete bl;
					bl = bl1;
                        	}else{
                        		delete bl1;
                        	}
			}
                 }else
			bl = bl1;

		if((bl != NULL) && (bl->edit < bestBL->edit)){
			delete bestBL;
			bestBL = bl;
		}else{
			if(bl != NULL)
				delete bl;
			break;
		}

        }
        return bestBL;
}*/

#include <algorithm>

Bigraph* GRASP::run(int iteracoes){

	Bigraph* bestSol = NULL;

	for(int i=0;i < iteracoes;i++){
		Bigraph* aux = this->iteracao();
		if(bestSol == NULL || aux->edit < bestSol->edit){
			delete bestSol;
			bestSol = aux;
		}else
		   delete aux;
	}

	//cout << "F: " << bestSol->edit << " AVAL: " << bestSol->countEdit() << endl;

	return bestSol;
}

//uma iteracao GRASP consiste em duas fases: construcao e busca local
Bigraph* GRASP::iteracao(){
	Bigraph*sol = this->construcao();
	//Bigraph*solVND=VNS::vnd(sol);
        //delete sol;
        //return solVND;
	return sol;
}

Bigraph *GRASP::construcao()
{
	Bigraph *sol = new Bigraph();
        float a=this->alpha;

	list<Edge> candidatos = Bigraph::candidatos; 

		
	while(true){ 
		//Setando o valor de filtro dos elementos do LCR
		double gmax = (*candidatos.begin()).g;
 	    	double gmin = (*candidatos.rbegin()).g;
		double filtro = gmax - a*(gmax - gmin);
		//printf("MIN: %f - MAX: %f FILTRO: %f\n", gmin, gmax, filtro);
		//Inicializando o LCR
		vector<Edge> LCR;
		list<Edge>::iterator it = candidatos.begin();
		for(; it != candidatos.end(); ++it){
			Edge e = *it;
			if(e.g >= filtro)
				LCR.push_back(e);
			else
				break;
		}
		if(LCR.empty())
			break;

		//cout << "SIZE LCR: " << LCR.size() << endl;
		
		//Escolha aleatoria do elemento na LCR
		int pos = rand()%LCR.size();
		Edge e = LCR[pos];
				
		//cout << "I: " << e.i <<  " J: " << e.j << endl;
		
		sol->makeBicluster(e.i, e.j);
		//sol->print();

		//Remove os candidatos já tratados
		candidatos.clear();
		list<Edge>::iterator ite = Bigraph::candidatos.begin();
		for(; ite != Bigraph::candidatos.end(); ++ite){
			Edge e = *ite;
			if(sol->edges[e.i][e.j] == Bigraph::FREE){
				//cout << "ADD I: " << e.i <<  " J: " << e.j << endl;
				candidatos.push_back(*ite);
			}
		}
		if(candidatos.empty())
			break;
	}
	sol->extractClusters();
        sol->f();
	//cout << "F(): " << sol->edit << endl;
	//sol->fCellFormation();
	
	return sol;
}

/*
void GRASP::startApproxBC(int GRASPmax){
	srand(time(NULL));
    
   	float finicial = INT_MAX;
	Bigraph *sol;
	int iter, a;
	for(iter =0; iter < GRASPmax; iter++){
		sol = this->approxBicluster();
		
		if(sol == NULL)
			continue;
		
		if(sol->edit < finicial){
		//if(sol->T > finicial){
			if(this->s != NULL){
                   		delete this->s;
               		}
			this->s = sol;
			finicial = sol->edit;
			//finicial = sol->T;
		}else
			delete sol;
			
	}

}

*/
