#include "FileBicluster.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>

using namespace std;


void FileBicluster::loadFile(char *fileName, int constr_type){
	ifstream Instancia(fileName, ios::in);
   	int rSize;

   	if (!Instancia) {
      		cerr << "Arquivo nao pode ser aberto" << endl;
		return;
   	}
	//string header;
	//std::getline(Instancia, header);

	int v, u;
	Instancia >> v; // numero de pecas
   	Instancia >> u; // numero de maquinas

	Bigraph::V = v;
	Bigraph::U = u;
	Bigraph::e = 0;
	Bigraph::constr_type = constr_type;
	Bigraph::laV.resize(Bigraph::V);
	Bigraph::laU.resize(Bigraph::U);
	Bigraph::laV2.resize(Bigraph::V);
	Bigraph::laU2.resize(Bigraph::U);

	
	Bigraph::w = new int*[Bigraph::V];
        for(int i=0; i < Bigraph::V; ++i){
                Bigraph::w[i] = new int[Bigraph::U];
        }
	
	for (int i = 0; i < Bigraph::V; i++) {
        for (int j = 0; j < Bigraph::U; j++) {
            int data;
			Instancia >> data;
			if(data == 1){
				++Bigraph::e;
				Bigraph::w[i][j] = 1;
				Bigraph::laV[i].push_back(j);
				Bigraph::laU[j].push_back(i);
				
			}else
				Bigraph::w[i][j] = -1;
		}//printf("\n");
   	}


	Instancia.close();

	Bigraph::createN1();
	Bigraph::createN2();


	//iniciando a lista de candidatos
	for(int i=0 ; i < Bigraph::V; ++i){
		for(int j=0 ; j < Bigraph::U; ++j){
			int g;
			if(constr_type == Bigraph::N1_N1){
				g = Bigraph::g_n1(i,j);
			}else{
				g = Bigraph::g_n2(i,j);
			}
			Bigraph::candidatos.push_back(Edge(i,j,g));
		}
	}

	Bigraph::candidatos.sort(Edge::compare);
}


void FileBicluster::printInstancia(char *exec, char *file, Bigraph *bg, int p)
{
   FILE *pf;
   char *nn = new char[100];
   sprintf(nn, "%s%s_%d%s", OUT_PATH, file, p, ".txt");
   if((pf = fopen(nn,"a")) ==NULL){
      printf("\nNao consigo abrir o arquivo %s!", nn);
      exit(1);
   }
   fprintf(pf, "%d %d\n", Bigraph::V, Bigraph::U);

   for(int i=0; i < Bigraph::V; ++i)
   {
      for(int j=0; j < Bigraph::U; ++j)
      {
	 if((rand()%11) < p) // Fica como esta na solucao
	 {
	    if(bg->edges[i][j] < 0)
	    	fprintf(pf, "%d ", 1);
	    else
	    	fprintf(pf, "%d ", 0);
	 }else
	 {
	    if(bg->edges[i][j] < 0)
	    	fprintf(pf, "%d ", 0);
	    else
	    	fprintf(pf, "%d ", 1);
	 }

      }
	    fprintf(pf, "\n");
   }
	fclose(pf);		
	delete nn;
}

void FileBicluster::printInstancia(char *exec, char *file, float edit_medio, int min_edit, float tempo_medio, int algorithm){
       	FILE *pf;
	char *nn = new char[100];
	sprintf(nn, "%s%s%s", OUT_PATH, exec,".txt");
        if((pf = fopen(nn,"a")) ==NULL){
               	printf("\nNao consigo abrir o arquivo %s!", nn);
                exit(1);
        }
	fprintf(pf, "%s::%f::%d::%f::%d\n", file,edit_medio,min_edit,tempo_medio,algorithm);
	fclose(pf);		
	delete nn;
}

void FileBicluster::printInstancia(char *exec, char *file, int min_edit, float tempo_medio, int algorithm){
       	FILE *pf;
	char *nn = new char[100];
	sprintf(nn, "%s%s%s", OUT_PATH, exec,".txt");
        if((pf = fopen(nn,"a")) ==NULL){
               	printf("\nNao consigo abrir o arquivo %s!", nn);
                exit(1);
        }
	fprintf(pf, "%s::%d::%f::%d\n", file, min_edit,tempo_medio,algorithm);
	fclose(pf);		
	delete nn;
}
