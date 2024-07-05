#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include<sstream>
#include <iostream>
#include <algorithm>
#include <limits.h>
#include "Bigraph.h"
#include "BreakBicluster.h"

Bigraph::Bigraph() : ev(0), e0(0), edit(0)
{
	this->edges = new int*[Bigraph::V];
	for(int i=0; i < Bigraph::V; ++i){
		this->edges[i] = new int[Bigraph::U];
		memset(this->edges[i], 0, sizeof(int)*U);
	}
}

Bigraph::Bigraph(std::string clustersAsStr) : ev(0), e0(0), edit(0)
{
	unsigned int i,j,k;

	this->edges = new int*[Bigraph::V];
	for(i=0; i < Bigraph::V; ++i){
		this->edges[i] = new int[Bigraph::U];
	}
        //comeca o processamento do string no indice 1,pois ja ignora o { e o primeiro (.
        i=2;
        vector<int> partV;
        vector<int> partU;
        while(1){

           partU.clear();   
           partV.clear();
           int v,u;

           //lendo os vertices da particao V
           while(1){
              //monta o numero do vertice em um string
              std::string vAsStr="";
              while(isdigit(clustersAsStr[i])){
                 vAsStr.push_back(clustersAsStr[i]);
                 i++;
              }
              if(vAsStr!=""){
                 v=atoi(vAsStr.c_str());
                 partV.push_back(v);
              }
              if(clustersAsStr[i++]==')')break; //fim da particao V
           }

           //lendo os vertices da particao U
           i++; //pula o (
           while(1){
              //monta o numero do vertice em um string
              std::string uAsStr="";
              while(isdigit(clustersAsStr[i])){
                 uAsStr.push_back(clustersAsStr[i]);
                 i++;
              }
              if(uAsStr!=""){
                 u=atoi(uAsStr.c_str());
                 partU.push_back(u);
              }

              if(clustersAsStr[i++]==')')break; //fim da particao U
           }
          
           //preenche edge para o cluster lido
           for(j=0;j<partV.size();j++){
              for(k=0;k<partU.size();k++){
                // cout<<"liga "<<partV[j]<<" a "<<partU[k]<<endl;
                 this->edges[partV[j]][partU[k]]=1;
              }
           }
          
           if(clustersAsStr[i]=='}')break;
           i++; //pula o (
 
        }//while de fora
}//fim do construtor

Bigraph::Bigraph(Bigraph &b) : ev(b.ev), e0(b.e0), edit(b.edit)   
{    
   this->edges = new int*[Bigraph::V];
   for(int i=0; i < Bigraph::V; ++i){
      this->edges[i] = new int[Bigraph::U];
      for(int j=0; j < Bigraph::U; ++j)
	 this->edges[i][j] = b.edges[i][j];
   }

   for(Bicluster *iB : b.C){
      this->C.push_back(new Bicluster(*iB));
   }
}

int Bigraph::f(bool updateDIFF)
{
	this->e0 = 0;
	this->ev = 0;
	this->edit = 0;

	//Zerando DIFF
	if(updateDIFF)
	{
	   for(unsigned int i = 0; i < this->C.size(); i++)
	   {
	      Bicluster *c = this->C[i];
	      for(int v = 0; v< Bigraph::V; ++v)
		 c->diffV[v] = 0;
	      for(int u = 0; u< Bigraph::U; ++u)
		 c->diffU[u] = 0;

	   }
	}

	//calculando o diff de cada v�rtice
	for(unsigned int i = 0; i < this->C.size(); i++)
	{
	   Bicluster *c1 = this->C[i];
	   for(int v : c1->V)
	   {
	      int *wl = Bigraph::w[v];
	      for(int u : c1->U)
	      {
		 if(updateDIFF)
		 {
		    c1->diffV[v] -= wl[u];
		    c1->diffU[u] -= wl[u];
		 }

		 //if((wl[u] < 0)&&(edges[v][u] > 0))
		 if(wl[u] < 0)
		    this->ev += abs(wl[u]);
	      }
	      //calcula diff para os outros clusters
	      for(int j = 0; j < this->C.size(); j++)
	      {
		 if(i == j)
		    continue;
		 Bicluster *c2 = this->C[j];
		 for(int u : c2->U)
		 {
		    if(updateDIFF)
		    {
		       c1->diffU[u] -= wl[u];
		       c2->diffV[v] -= wl[u];
		    }
		    //if((wl[u] > 0) && (edges[v][u] < 0))
		    if(wl[u] > 0)
		       this->e0 += abs(wl[u]);
		 }
	      }

	   }
	}

	this->edit = this->ev + this->e0;    
	return this->edit;
}


Bigraph *Bigraph::bl1Random()
{
	if(this->C.size() <= 1)
	   return NULL;

        int bestOri = -1;
        int bestDest = -1;
        int bestVertice = -1;
        int bestPartition = 0; //(0 : V - 1 : U)
        
	//Escolhendo Cluster aleatorio, escolhendo um vertice aleatorio de uma particao
        int idOri = rand() % this->C.size();
	int idDes;
	if (idOri > 0)
		idDes = idOri - 1;
	else	
		idDes = idOri + 1;
		

	bestOri = idOri;
        bestDest = idDes;

	//Escolhendo a particao aleat�ria
	if(rand() % 2 == 0){ //V
		//Correndo os vertices da particao V de ori
        	if(this->C[idOri]->V.size() <= 1)
        		return NULL;
		int c = (rand() % this->C[idOri]->V.size());
		int i = 0; 
		//Pegando o elemneto da posicao c na lista V do cluster
		for(int v : this->C[idOri]->V)
		{
		   if(c == i)
		   {
		      bestVertice = v;
		      break;
		   }
		   ++i;
		}
                bestPartition = 0;
        }else{ //U
                if(this->C[idOri]->U.size() <= 1)
                	return NULL;
                //Correndo os vertices da particao U de ori
		int c = (rand() % this->C[idOri]->U.size());
		int i = 0; 
		//Pegando o elemneto da posicao c na lista U na lista do cluster
		for(int u : this->C[idOri]->U) 
		{
		   if(c == i)
		   {
		      bestVertice = u;
		      break;
		   }
		   ++i;
		}
                bestPartition = 1;
	}

	Bigraph *bg = new Bigraph(*this);
	//Removendo o n� da origem e adicionando no destino, atualizando a matriz edges.
	if(bestPartition == 0){ // particao V
	   bg->C[bestOri]->removeV(bg, bestVertice);
	   bg->C[bestDest]->addV(bg, bestVertice);

	   for(int i=0; i < Bigraph::U; ++i)
	      bg->edges[bestVertice][i] = Bigraph::FORBIDDEN;
	   for(int u : bg->C[bestDest]->U)
	      bg->edges[bestVertice][u] = Bigraph::PERMANENT;
	}else{ //particao U
	   bg->C[bestOri]->removeU(bg, bestVertice);
	   bg->C[bestDest]->addU(bg, bestVertice);

	   for(int i=0; i < Bigraph::V; ++i)
	      bg->edges[i][bestVertice] = Bigraph::FORBIDDEN;

	   for(int v : bg->C[bestDest]->V)
	      bg->edges[v][bestVertice] = Bigraph::PERMANENT;  
	}
        return bg;
}

Bigraph *Bigraph::bl1()
{
   if(this->C.size() <= 1)
      return NULL;
   int bestE = INT_MAX;
   int bestOri = -1;
   int bestDest = -1;
   int bestVertice = -1;
   int bestPartition = 0; //(0 : V - 1 : U)
   Bicluster *single = NULL;
   //Correndo cada Cluster, percorrendo seus vertices de cada particao
   for(unsigned int idOri = 0; idOri < this->C.size(); ++idOri){
      if(this->C[idOri]->V.size() > 1){
	 //Correndo os vertices da particao V de ori
	 for(int v : this->C[idOri]->V){
	    float edAntes = this->edit - this->C[idOri]->diffV[v];

	    //Corre todos os outros cluster != ori testando a melhor insercao dos vertices de ori.
	    for(unsigned int idDes = 0; idDes < this->C.size(); ++idDes)
	    {
	       if((idOri != idDes) && (this->C[idDes]->U.size() > 0)){
		  float edDepois = edAntes + this->C[idDes]->diffV[v];
		  if(bestE > edDepois){
		     bestE = edDepois;
		     bestOri = idOri;
		     bestDest = idDes;
		     bestVertice = v;
		     bestPartition = 0;
		     if(single)
			delete single;
		     single = NULL;
		  }
	       }
	    }
	    //Tenta adicionar iV em um Bicluster singleton.
	    Bicluster *destS = new Bicluster();

	    //Para criar singleton basta computar a edicao de saida do cluster origem
	    if(bestE > edAntes){
	       bestE = edAntes;
	       bestOri = idOri;
	       bestVertice = v;
	       bestPartition = 0;
	       if(single)
		  delete single;
	       single = destS;

	    }else
	       delete destS;

	 }
      }             
      if(this->C[idOri]->U.size() == 1)
	 break;
      //Correndo os vertices da particao U de ori
      for(int u : this->C[idOri]->U){
	 float edAntes = this->edit - this->C[idOri]->diffU[u];

	 //Procura o melhor cluster para adicionar u
	 for(unsigned int idDes = 0; idDes < this->C.size(); ++idDes){
	    if((idOri != idDes) && (this->C[idDes]->V.size() > 0)){
	       float edDepois = edAntes + this->C[idDes]->diffU[u];

	       if(bestE > edDepois){
		  bestE = edDepois;
		  bestOri = idOri;
		  bestDest = idDes;
		  bestVertice = u;
		  bestPartition = 1;
		  if(single)
		     delete single;
		  single = NULL;
	       }
	    }
	 }

	 //Tenta adicionar iU em um Bicluster singleton.
	 Bicluster *destS = new Bicluster();

	 //Para cirar singleton basta a edicao de saida do cluster origem    	
	 if(bestE > edAntes){
	    bestE = edAntes;
	    bestOri = idOri;
	    bestVertice = u;
	    bestPartition = 1;
	    if(single)
	       delete single;
	    single = destS;
	 }else
	    delete destS;

      }
   }

   if(bestE == INT_MAX)
      return NULL;

   Bigraph *bg = new Bigraph(*this);
   //Removendo o n� da origem e adicionando no destino, atualizando a matriz edges.
   Bicluster *dest=  NULL;
   if(single != NULL){
      //cout << "Single" << endl;
      dest = single;
      bg->C.push_back(dest);
   }else{
      dest = bg->C[bestDest];
   }

   //bg->C[bestOri]->print();
   //dest->print();
   if(bestPartition == 0){ // particao V
      bg->C[bestOri]->removeV(bg, bestVertice);
      dest->addV(bg, bestVertice);

      for(int i=0; i < Bigraph::U; ++i)
	 bg->edges[bestVertice][i] = Bigraph::FORBIDDEN;
      for(int u : dest->U)
	 bg->edges[bestVertice][u] = Bigraph::PERMANENT;
   }else{ //particao U
      bg->C[bestOri]->removeU(bg, bestVertice);
      dest->addU(bg, bestVertice);

      for(int i=0; i < Bigraph::V; ++i)
	 bg->edges[i][bestVertice] = Bigraph::FORBIDDEN;

      for(int v : dest->V)
	 bg->edges[v][bestVertice] = Bigraph::PERMANENT;  
   }

   return bg;
}

Bigraph *Bigraph::bl2(bool f)
{
	int bestOri = -1;
	int bestDes = -1;
	int bestEdit = INT_MAX;
	//Correndo cada Cluster, percorrendo seus vertices de cada particao
        for(unsigned int idOri = 0; idOri < this->C.size(); ++idOri){
              Bicluster &ori = *this->C[idOri];
              //Corre todos os outros cluster != ori testando a melhor insercao dos vertices de ori.
              for(unsigned int idDes = idOri + 1; idDes < this->C.size(); ++idDes){
                    Bicluster &dest = *this->C[idDes];

		    //Nao juntar dois clusters sem elementos na mesma particao
		    if(((ori.V.size()==0)&&(dest.V.size()==0)) || ((ori.U.size()==0)&&(dest.U.size()==0)))
		       continue;

		    int e = this->calculateJoin(ori, dest);

		    if(f){
			if(rand() % 10 < 2){
				bestEdit = e;
				bestOri = idOri;
				bestDes = idDes;
				break;
			}

		    }else{

		    	if(e < bestEdit){
				bestEdit = e;
				bestOri = idOri;
				bestDes = idDes;
		    	}
		    }
	      }
	}
       	if(bestOri == -1)
		return NULL; 
	Bigraph *bg = new Bigraph(*this);
	bg->join(bestOri, bestDes);
	return bg;
}

//Break Bicluster
Bigraph *Bigraph::bl3(bool f)
{
	int bestEdit = INT_MAX;
	int bestOri = -1;
	
	if(f)
	{
	   //tenta quebra um bicluster aleatorio
	   for(int i=0; i<2*C.size(); ++i)
	   {
	      int oriID = rand() % C.size();
	      Bicluster *ori = this->C[oriID];
	      pair<bool, int> result = ori->evaluateBreak(true);
	      if(!result.first)
		 continue;

	      Bigraph *bg = new Bigraph(*this);
	      bg->edit += result.second;
	      ori = bg->C[oriID];
	      Bicluster *bclu = ori->breakBicluster(bg, true);
	      if(bclu == NULL)
	      {
		 delete bg;
		 continue;
	      }
	      bg->breakBicluster(ori, bclu);
	      return bg;
	   }
	   return NULL;

	}else
	{
	   for(unsigned int idOri = 0; idOri < this->C.size(); ++idOri){
	      Bicluster &ori = *this->C[idOri];
	      if((ori.U.size() == 0) || (ori.V.size() == 0))
		 continue;

	      pair<bool, int> result = ori.evaluateBreak(false);
	      if(result.first)
	      {
		 if(result.second < bestEdit){
		    bestEdit = result.second;
		    bestOri = idOri;
		 }
	      }
	   }
	}

	if(bestOri < 0)
		return NULL;

	Bigraph *bg = new Bigraph(*this);
	bg->edit += bestEdit;
	Bicluster *bclu = bg->C[bestOri]->breakBicluster(bg, false);
	if(bclu == NULL)
	{
	   delete bg;
	   return NULL;
	}
	bg->breakBicluster(bg->C[bestOri], bclu);
	//cout << "REAl F: " << bg->edit << endl;
	
	/*if((bg->e0 < 0) || (bg->ev < 0)){
		this->print();
		bg->print();
		getchar();
	}*/
	return bg;
}

int Bigraph::breakBicluster(Bicluster *b1, Bicluster *b2)
{
	//int ae0 = 0;
        //int aev = 0;

	//Calculando as edicoes apos a separacao dos clusters.
	for(int  v : b1->V){
	   for(int u : b2->U){
	      this->edges[v][u] = Bigraph::FORBIDDEN;  
	      /*if(w[v][u] > 0)
		 ae0 += w[v][u];
	      else
		 aev -= abs(w[v][u]);*/
	   }
	}

	for(int u : b1->U){
	   for(int v : b2->V){
	      this->edges[v][u] = Bigraph::FORBIDDEN;  
	      /*if(w[v][u] > 0)
		 ae0 += w[v][u];
	      else
		 aev -= abs(w[v][u]);*/
	   }
	}
	
	
	this->C.push_back(b2);

	return this->C.size();
}


int Bigraph::calculateJoin(Bicluster &b1, Bicluster &b2)
{
        int aedit = 0;

	for(int v : b1.V)
	{
	 aedit += b2.diffV[v]; // target

	}
	
	for(int u : b1.U){
	   aedit += b2.diffU[u]; // target
	}

	return this->edit + aedit;
}

void Bigraph::join(int idOri, int idDes)
{
   Bicluster &b1 = *this->C[idDes];
   Bicluster &b2 = *this->C[idOri];

   //Juntando os clusters
   //Atualizando a matriz de edicoes apos a juncao dos clusters.
   for(int v : b1.V)
      b1.diffV[v] += b2.diffV[v];
   for(int u : b1.U)
      b1.diffU[u] += b2.diffU[u];
   
   for(int u : b2.U)
   {
      b1.U.push_back(u);
      for(int v : b1.V)
	 this->edges[v][u] = Bigraph::PERMANENT;

      //Atualizando Diff
      edit += b1.diffU[u];
      b1.diffU[u] = b2.diffU[u] + b1.diffU[u];
   }
   for(int v : b2.V)
   {
      b1.V.push_back(v);
      for(int u : b1.U)
	 this->edges[v][u] = Bigraph::PERMANENT;  
      
      //Atualizando Diff
      edit += b1.diffV[v];
      b1.diffV[v] = b2.diffV[v] + b1.diffV[v];
   }

   //calculando o Diff dos vertices fora dos clusters juntados
   for(int bc=0; bc<C.size(); ++bc)
   {
      if((bc != idOri) && (bc != idDes))
      {
	 for(int v : C[bc]->V)
	    b1.diffV[v] += b2.diffV[v]; 
	 for(int u : C[bc]->U)
	    b1.diffU[u] += b2.diffU[u]; 
      }
   }

   //Remove o cluster idDes.
   this->C.erase(this->C.begin() + idOri);
   delete &b2;//limpando a memoria do bilcuster removido


/*if(wrongDiff())
{
      cout << "Loucura";
      getchar();
}*/
}


void Bigraph::extractClusters()
{
    
     bool visitV[V];
     for(int i=0; i < V; ++i)
         visitV[i] = false;
     
     bool visitU[U];
     for(int i=0; i < U; ++i)
         visitU[i] = false;

     C.clear();      
     
     for(int v=0; v < V; ++v){
         
         if(!visitV[v]){
             Bicluster *bc = new Bicluster();
             for(int u=0; u < U; ++u){
                 if(this->edges[v][u] > 0){
                    visitU[u] = true;
                    bc->U.push_back(u);
		}
             }
             list<int>::iterator iU = bc->U.begin();
             if(iU != bc->U.end()){
	     	int u = *iU;
             	for(int vv=0; vv < V; ++vv){
                     if(this->edges[vv][u] > 0){
                     	visitV[vv] = true;
                     	bc->V.push_back(vv);
                     }
             	}
	     }else{
             	visitV[v] = true;
		bc->V.push_back(v);
		//Isola o v na matriz edges (singleton)
		for(int u=0; u<Bigraph::U; ++u)
		   this->edges[v][u] = Bigraph::FORBIDDEN;

	     }
             bc->id = Bicluster::numBicluster++;
             C.push_back(bc);
         }
     }
     //Os elementos de U n visitados forma cada um, bicluster singleton.
     for(int u=0; u < U; ++u){
        if(!visitU[u]){
             Bicluster *bc = new Bicluster();
             visitU[u] = true;
	     bc->U.push_back(u);
	     for(int v=0; v<Bigraph::V; ++v)
		this->edges[v][u] = Bigraph::FORBIDDEN;
             bc->id = Bicluster::numBicluster++;
             C.push_back(bc);
	}
     }
}

void Bigraph::createN1()
{
   for (int i = 0; i < Bigraph::V; i++) {
      for (int j = 0; j < Bigraph::U; j++) {
	 if(w[i][j] > 0){
	    Bigraph::laV[i].push_back(j);
	    Bigraph::laU[j].push_back(i);

	 }
      }
   }

}

void Bigraph::createN2()
{
   for(int i=0 ; i < Bigraph::V; ++i){
      if(!laV[i].empty()){
	 for(int u : laV[i]){

	    for(int  v : laU[u]){
	       laV2[i].insert(v);
	    }
	 }
      }
   }

   for(int j=0 ; j < Bigraph::U; ++j){
      if(!laU[j].empty()){
	 for(int v : laU[j]){
	    for(int u : laV[v]){
	       laU2[j].insert(u);
	    }
	 }
      }
   }
}

void Bigraph::makeBicluster(int i,int j){

   if(Bigraph::constr_type == Bigraph::N2_N2) this->makeBicluster_n2(i,j);
   else this->makeBicluster_n1(i,j);
}

//Junta o N2(i) e N2(j) para formar bicluster.
void Bigraph::makeBicluster_n2(int i, int j){

   set<int> &N2i = Bigraph::laV2[i];
   set<int> &N2j = Bigraph::laU2[j];

   //Ligacao ente N2(i) e N2(j)
   for(int v : N2i){
      for(int u : N2j){
	 if(this->edges[v][u] == Bigraph::FREE)
	    this->edges[v][u] = Bigraph::PERMANENT;
      }
   }

   //Ligacoes fora de N2(i)
   for(int v : N2i){
      for(int jj=0; jj<Bigraph::U; ++jj){
	 if(this->edges[v][jj] != Bigraph::PERMANENT)
	    this->edges[v][jj] = Bigraph::FORBIDDEN;
      }
   }

   //Ligacoes fora de N2(j)
   for(int u : N2j){
      for(int ii=0; ii<Bigraph::V; ++ii){
	 if(this->edges[ii][u] != Bigraph::PERMANENT)
	    this->edges[ii][u] = Bigraph::FORBIDDEN;
      }	    
   }

   //Caso a aresta n exista, passa a existir
   this->edges[i][j] = Bigraph::PERMANENT;
}

void Bigraph::makeBicluster_n1(int i, int j){

   list<int> &Ni = Bigraph::laV[i];
   list<int> &Nj = Bigraph::laU[j];
   
   //Caso a aresta n exista, passa a existir
   int *edI = this->edges[i];
   edI[j] = Bigraph::PERMANENT;

   //j e N(j)
   for(int v : Nj)
      if(this->edges[v][j] == Bigraph::FREE)
	 this->edges[v][j] = Bigraph::PERMANENT;
   //i e N(i)
   for(int u : Ni)
      if(edI[u] == Bigraph::FREE)
	 edI[u] = Bigraph::PERMANENT;

   //Ligacao ente N(j) e N(i)
   for(int v : Nj){
      int *edV = this->edges[v];
      for(int u : Ni){
	 if(edV[u] == Bigraph::FREE)
	    edV[u] = Bigraph::PERMANENT;
      }
   }

   //Ligacoes fora de N(j)
   for(int v : Nj){
      int *edV = this->edges[v];
      for(int jj=0; jj<Bigraph::U; ++jj){
	 if(edV[jj] != Bigraph::PERMANENT)
	    edV[jj] = Bigraph::FORBIDDEN;
      }
   }

   //i com V2/N(i)
   for(int jj=0; jj<Bigraph::U; ++jj){
      if(edI[jj] != Bigraph::PERMANENT)
	 edI[jj] = Bigraph::FORBIDDEN;
   }

   //Ligacoes fora de N(i)
   for(int u : Ni){
      for(int ii=0; ii<Bigraph::V; ++ii){
	 if(this->edges[ii][u] != Bigraph::PERMANENT)
	    this->edges[ii][u] = Bigraph::FORBIDDEN;
      }
   }

   //j com V1/N(j)
   for(int ii=0; ii<Bigraph::V; ++ii){
      if(this->edges[ii][j] != Bigraph::PERMANENT)
	 this->edges[ii][j] = Bigraph::FORBIDDEN;
   }

}


void Bigraph::add(Bicluster *bc){
   list<int> &Ni = bc->U;
   list<int> &Nj = bc->V;

   //int itN2 = i;
   //Ligacao ente N(j) e N(i)
   for(int v : Nj){
      for(int u : Ni){
	 if(this->edges[v][u] == Bigraph::FREE)
	    this->edges[v][u] = Bigraph::PERMANENT;
      }
   }

   //Ligacoes fora de N(j)
   for(int v : Nj){
      for(int jj=0; jj<Bigraph::U; ++jj)
	 if(this->edges[v][jj] != Bigraph::PERMANENT)
	    this->edges[v][jj] = Bigraph::FORBIDDEN;
   }
   //Ligacoes fora de N(i)
   for(int u : Ni){
      for(int ii=0; ii<Bigraph::V; ++ii)
	 if(this->edges[ii][u] != Bigraph::PERMANENT)
	    this->edges[ii][u] = Bigraph::FORBIDDEN;

   }

   //Adicionndo o Bicluster no Bigraph.	
   //bc->calculateEdit(this);
   //bc->fElementsVU(this);
   bc->id = Bicluster::numBicluster++;
   C.push_back(bc);
}


float Bigraph::g_n1(int i, int j)
{
   list<int> &N1i = Bigraph::laV[i];
   list<int> &N1j = Bigraph::laU[j];

   //Custo da ligacao de i com j
   int cij = Bigraph::w[i][j];
   //soma das ligacoes entre os elementos de N1(i) com os elementos N1(j) 
   int inij = 0;
   //soma das ligacoes dos elementos de N1(i) e N1(j) que n se ligam entre si
   int outij = 0;
   //Soma das ligacoes que faltam entre N1(i) e N1(j) para fechar o bicluster.
   int diffij = 0;

   //////////////////////////////
   //computa in e diff
   for(int u : laV[i]){ //itera em N1(i)
      for(int v : laU[j]){ //itera em N1(j)
	 if(Bigraph::w[v][u]>0)
	    inij += w[v][u];
	 else diffij += w[v][u];
      }
   }
   //////////////////////////////

   //////////////////////////////	
   //calcula outij

   //soma dos pesos das arestas que comecam N1(j) e n�o terminam em N1(i)
   for(int v : laU[j]){
      //vizinhos do elemento *itNj, que esta em N1(j). Esses vizinhos devem estar em N1(i)
      for(int u : laV[v]){
	 //if(std::find(N1i.begin(), N1i.end(), u) == N1i.end())
	 if(Bigraph::w[i][u] < 0)
	    outij += w[v][u];
      }
   }

   //soma dos pesos das arestas que comecam N1(i) e n�o terminam em N1(j)
   for(int u : laV[i]){
      //vizinhos do elemento *itNi, que esta em N1(i). Esses vizinhos devem estar em N1(j)
      for(int v : laU[u]){ 
	 //if(std::find(N1j.begin(), N1j.end(), v) == N1j.end())
	 if(Bigraph::w[v][j] < 0)
	    outij += w[v][u];
      }
   }	
   //////////////////////////////


   return inij + cij +  diffij - outij;
}

float Bigraph::g_n2(int i, int j)
{
   set<int> N2i = Bigraph::laV2[i];
   set<int> N2j = Bigraph::laU2[j];
   //Custo da ligacao de i com j
   int cij = Bigraph::w[i][j];
   //soma das ligacoes entre os elementos de N2(i) com os elementos N2(j) 
   int inij = 0;
   //soma das ligacoes dos elementos de N2(i) e N2(j) que n se ligam entre si
   int outij = 0;
   //Soma das ligacoes que faltam entre N2(i) e N2(j) para fechar o bicluster.
   int diffij = 0;


   int v = i;
   //Ligacoes fora de N2(j)
   for(int  u : laV[v]){
      if(std::find(N2j.begin(), N2j.end(), u) == N2j.end())
	 outij += w[v][u];
   }

   //Calcula os pesos das ligacoes entre o i e N2(j)
   for(int u : N2j){
      if(Bigraph::w[v][u] > 0)
	 inij += w[v][u];
      else
	 diffij += w[v][u];
   }

   int u = j;
   for(int v : laU[u]){
      if(std::find(N2i.begin(), N2i.end(), v) == N2i.end())
	 outij += w[v][u];
   }

   //Calcula os pesos das ligacoes entre o j e N2(i)
   for(int v : N2i){
      if(Bigraph::w[v][u] > 0)
	 inij += w[v][u];
      else
	 diffij += w[v][u];
   }

   //Avaliando o bicluster a ser formado
   /*list<int> Ni = Bigraph::laV[i];
   list<int> Nj = Bigraph::laU[j];
   //int e = Ni.size() * Nj.size();
   int ev = 0;

   for(int v : Nj){
      for(int u : Ni){
	 if(w[v][u] < 0)
	    ev++;
      }
   }*/

   return inij + cij +  diffij - outij;
} 

//retorna a representacao em string dos clusters do bigrafo
std::string Bigraph::clustersAsStr()
{

   char v[20];
   sprintf(v, "Edit: %d\n", this->edit);
   std::string bgStr=v;
   //bgStr+=v;
   bgStr+="{";

   for(unsigned int idOri = 0; idOri < this->C.size(); ++idOri)
   {
      bgStr+="[";
      Bicluster &ori = *this->C[idOri];
      bgStr+=ori.getText();
      if(idOri < this->C.size()-1)
      	bgStr += "]\n";
      else
      	bgStr += "]";
   }
   bgStr+="}";

   return bgStr;
}

Bigraph::~Bigraph(){
   
   vector<Bicluster *>::iterator ib = this->C.begin();
   for(; ib != this->C.end(); ++ib)
      delete *ib;

   for(int i=0; i < V; ++i){
      delete this->edges[i];
   }
   delete this->edges;
}

int Bigraph::V = 0;
int Bigraph::U = 0;

int Bigraph::e = 0;

vector<list<int>> Bigraph::laV;
vector<list<int>> Bigraph::laU;

vector<set<int>> Bigraph::laV2;
vector<set<int>> Bigraph::laU2;

list<Edge> Bigraph::candidatos;

int **Bigraph::w = NULL;

const int Bigraph::FORBIDDEN = -1;
const int Bigraph::FREE = 0;
const int Bigraph::PERMANENT = 1;
const int Bigraph::N2_N2 = 1;
const int Bigraph::N2_N1 = 2;
const int Bigraph::N1_N1 = 3;
int Bigraph::constr_type = 1;


