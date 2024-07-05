#include <list>
#include <iostream>
#include <limits.h>
#include<sys/time.h>
#include "VNS.h"
#include "Util.h"

VNS::VNS()
{
        this->s = NULL;
}

Bigraph *VNS::ils(Bigraph *solAtual){
   
	Bigraph *bestBL = NULL;
	if(solAtual != NULL)
		bestBL = new Bigraph(*solAtual);
	else
		return NULL;
	
	Bigraph *sils = new Bigraph(*bestBL);
  
         
	for(int i = 0; i < 3; ++i){

                //realiza pertubacoes na solucao
  		int r = 2 + (rand() % 10);  
		for(int j=0; j < r; ++j){
			int mov = rand() % 3;
			if(mov == 1){
				Bigraph *pert = sils->bl2(true);
        			if(pert != NULL){
                			delete sils;
					sils = pert;
				}

			}else{
				if(mov == 2){
					Bigraph *pert = sils->bl3(true);
                        		if(pert != NULL){
                        	        	delete sils;
                        	        	sils = pert;
                        		}
				}else{
			 		Bigraph *pert = sils->bl1Random();
                                	if(pert != NULL){
                                       		delete sils;
                                       		sils = pert;
					}
				}
			}
		}
	
                //inicia o vnd a partir da solucao resultante da pertubacao
		Bigraph *vnd = VNS::vnd(sils);
		delete sils;

		if(vnd != NULL){
		
			if(vnd->edit < bestBL->edit){
                        	delete bestBL;
                        	bestBL = vnd;
				sils = new Bigraph(*bestBL);
				i = 0;
               		}else{
                        	sils = vnd;
               		}
        	}else
        		break;

   	}

	return bestBL;
}


Bigraph *VNS::ils(Bigraph *solAtual, float maxTime)
{
	struct timeval tempoAntes, tempoDepois;
	Bigraph *bestBL = NULL;
	if(solAtual != NULL)
		bestBL = new Bigraph(*solAtual);
	else
		return NULL;
	
	Bigraph *sils = new Bigraph(*bestBL);
  
	//tempo inical
        gettimeofday(&tempoAntes,NULL); 
         
	//for(int i=0; i < 3; ++i){
	while(true){

                //realiza pertubacoes na solucao
  		int r = 2 + (rand() % 10);  
		for(int j=0; j < r; ++j){
			int mov = rand() % 3;
			if(mov == 1){
				Bigraph *pert = sils->bl2(true);
        			if(pert != NULL){
                			delete sils;
					sils = pert;
				}

			}else{
				if(mov == 2){
					Bigraph *pert = sils->bl3(true);
                        		if(pert != NULL){
                        	        	delete sils;
                        	        	sils = pert;
                        		}
				}else{
			 		Bigraph *pert = sils->bl1Random();
                                	if(pert != NULL){
                                       		delete sils;
                                       		sils = pert;
					}
				}
			}
		}	
	
                //inicia o vnd a partir da solucao resultante da pertubacao
		Bigraph *vnd = VNS::vnd(sils);
		delete sils;

		if(vnd != NULL){
		
			if(vnd->edit < bestBL->edit){
                        	delete bestBL;
                        	bestBL = vnd;
				sils = new Bigraph(*bestBL);
               		}else{
                        	sils = vnd;
               		}
        	}else
        		break;

                //verifica se o tempo ja estourou
                gettimeofday(&tempoDepois,NULL);
                if(calculaTempo(&tempoAntes,&tempoDepois) >= maxTime)
               		break;
   	}
	return bestBL;
}




//Bigraph *VNS::ils(set<Bigraph*, BigraphCompare> elite)
//Bigraph *VNS::ils(Bigraph* sol)
//{
        /*set<Bigraph*, BigraphCompare>::iterator ielite = elite.begin();
	Bigraph *bestBL = NULL;
        if(ielite != elite.end())
                bestBL = new Bigraph(*(*ielite));
        else
                return NULL;*/

/*	Bigraph *bestBL = NULL;
        Bigraph *sils = new Bigraph(*sol);
	bestBL = new Bigraph(*sils);

//cout << "Elite: " << sils->edit << "V: " << sils->ev << "U: " << sils->e0 << endl;

	for(int i=0; i < 3; ++i){
	//int ir = 0;
	//int eliteCount = 0;
	//while((eliteCount < BiclusterElite::ELITE_SIZE) && (ielite != elite.end())){
        	int r = 2 + (rand() % 3);
        	for(int j=0; j < r; ++j){
                	//int mov = rand() % 3;
                	int mov = 1;
                	if(mov == 1){
                        	Bigraph *pert = sils->bl2(true);
                        	if(pert != NULL){
                                	delete sils;
                                	sils = pert;
                        	}

                	}else{
                        	if(mov == 2){
                                	Bigraph *pert = sils->bl3(true);
                                	if(pert != NULL){
                                        	delete sils;
                                        	sils = pert;
                                	}
                        	}else{
                                	Bigraph *pert = sils->bl1Random();
                                	if(pert != NULL){
                                        	delete sils;
                                        	sils = pert;
                                	}
                        	}
                	}
        		cout << "PERT: " << sils->edit << endl;
			//getchar();
        	}

		Bigraph *vnd = this->vnd(sils);
        	delete sils;
        	cout << "VND: " << vnd->edit << endl;

        	if(vnd != NULL){

                	if(vnd->edit < bestBL->edit){
               			//if(vnd->T - bestBL->T > 0.0001){
                        	//cout << "I: " << i << " F: " << vnd->T << endl;
                        	delete bestBL;
                        	bestBL = vnd;

                        	sils = new Bigraph(*bestBL);
                       		i = 0;
                       		//ir = 0;
                        	//continue;
               		}else{
				++ir;
				//cout << "IR: " << ir << endl;
				if(ir == 3){
					++ielite;
					++eliteCount;
					if(ielite != elite.end()){
                				sils = new Bigraph( *(*ielite));
						//cout << "Elite: " << sils->edit << "V: " << sils->ev << "U: " << sils->e0 << endl;
					}	
					ir = 0;
				
				}else{				
                        		sils = vnd;
				}
                        	//break;
               		}
        	}else
                	break;

   	}
	//cout << endl;
        return bestBL;
}*/


Bigraph *VNS::vnd(Bigraph *solAtual,int kmax){
       
       	int bestEdit = solAtual->edit;
	Bigraph *bestBL = new Bigraph(*solAtual);
	
	
        Bigraph *bl = NULL;
	while(true){
	
		while(true){
	   	 	
	//cout << "bl1 init ******" << endl;
			bl = bestBL->bl1();
           
	   		if(bl != NULL){
			   //bl->printBigraph();
				if(bl->edit < bestEdit){
	//cout << "bl1 F: " << bl->edit << " Clusters: " << bl->C.size() << endl;
                 			bestEdit = bl->edit;
                   			delete bestBL;
                   			bestBL = bl;
           			}else{
               				delete bl;
               				break;
           			}
	   		}else
				break;
		}

		if(kmax<1)break;

	//cout << "bl2 init" << endl;
           	bl = bestBL->bl2(false);
			   //bl->printBigraph();
                if(bl != NULL){
			if(bl->edit < bestEdit){
	//cout << "bl2 F: " << bl->edit << " Clusters: " << bl->C.size() << endl;
	//if(bl->edit < 7)
                 		bestEdit = bl->edit;
                                delete bestBL;
                                bestBL = bl;
                                continue;
                        }else{
                        	delete bl;
                        }
                 }
		
		//break;
		 if(kmax<2)break;

                 bl = bestBL->bl3(false);
           	 if(bl == NULL)
                	return bestBL;
           	 
 		 if(bl->edit < bestEdit){
		    //cout << "BL3 melhorou" << endl;
                 	bestEdit = bl->edit;
                 	delete bestBL;
                 	bestBL = bl;
                        continue;
           	 }else{
           		delete bl;
               		break;
           	 }

        }
	//cout << "vnd fim" << endl;

        return bestBL;
}

#include <algorithm>

Bigraph *VNS::greedyConstruction()
{
	srand(time(NULL));
	
	Bigraph *sol = ReducerBigraph::getBigraph();

	list<Edge> candidatos = Bigraph::candidatos; 

	while(true){ 
		//Escolha aleatoria.
		list<Edge>::iterator it = candidatos.begin();
		int id = rand() % candidatos.size();
		while(--id >= 0)
			++it;
		Edge e = *it;
			

		//Escolha gulosa.
		//Edge e = *candidatos.begin();
				
		//cout << "I: " << e.i <<  " J: " << e.j << endl;
		
		sol->makeBicluster(e.i, e.j);
		//sol->print();

		//Remove os candidatos j√° tratados
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
	
	//Testa se existe vertice sem aresta atribuÌda.
	//if(!sol->isCellFormation())
	//    return NULL;

	
	//cout << "F: " << sol->f() << endl;
	//cout << "FCF: " << sol->fCellFormation() << endl;
	
	sol->f();
	//sol->fCellFormation();
	sol->extractClusters();
    //getchar();
	return sol;
}

Bigraph *VNS::vns(Bigraph *sol, float maxTime){

        struct timeval tempoAntes, tempoDepois;
	Bigraph *bestBL = new Bigraph(*sol);
	Bigraph *svns = new Bigraph(*sol);
	
	int k = 0;

        //tempo inicial
        gettimeofday(&tempoAntes,NULL);

	while(true){

		//verifica se o tempo estourou
                gettimeofday(&tempoDepois,NULL);
                if(calculaTempo(&tempoAntes,&tempoDepois) >= maxTime)
                	break;

		Bigraph *pert = new Bigraph(*svns);
		int count = 2 + rand()%10; 
		for(int i=0; i < count; ++i){
			Bigraph *aux = NULL;
			switch(k){
				case 0:
					aux = pert->bl1Random();
					break;
				case 1:
					aux = pert->bl2(true);
					break;
				//case 2:
				//	aux = pert->bl3(true);
				//	break;
			}

			if(aux != NULL){
				delete pert;
				pert = aux;
			}
		}

		if(pert == NULL){
			k = k < 2 ? k+1 : 2;
			continue;
		}
		
		Bigraph *vnd = VNS::vnd(pert,k);
		if(vnd == NULL){
		        k = k < 2 ? k+1 : 2;
		        continue;
		}

		if(vnd->edit < bestBL->edit){
			delete bestBL;
			delete pert;
			delete svns;
			bestBL = vnd;
			svns = new Bigraph(*vnd);
			k = 0;
		}else{
			delete svns;
			svns = new Bigraph(*vnd);
			delete vnd;
			delete pert;
			k = k < 2 ? k+1 : 2;
		}
	}
        return bestBL;
}


Bigraph *VNS::vns(Bigraph *sol){

	Bigraph *bestBL = new Bigraph(*sol);
	Bigraph *svns = new Bigraph(*sol);
	int k = 0;

	while(k < 3){

		Bigraph *pert = new Bigraph(*svns);
		int count = 2 + rand()%10; 

		for(int i = 0; i < count; ++i){
			Bigraph *aux = NULL;
			switch(k){
				case 0:
					aux = pert->bl1();
					break;
				case 1:
					aux = pert->bl2(true);
					break;
				case 2:
					aux = pert->bl3(true);
					break;
			}

			if(aux != NULL){
				delete pert;
				pert = aux;
			}
		}

		if(pert == NULL){
			k++;
			continue;
		}
		
		Bigraph *vnd = VNS::vnd(pert,k);
		if(vnd == NULL){
		        k++;
		        continue;
		}

		if(vnd->edit < bestBL->edit){
			delete bestBL;
			delete pert;
			delete svns;
			bestBL = vnd;
			svns = new Bigraph(*vnd);
			k = 0;
		}else{
			delete svns;
			svns = new Bigraph(*vnd);
			delete vnd;
			delete pert;
			k++;
		}
	}

        return bestBL;
}
