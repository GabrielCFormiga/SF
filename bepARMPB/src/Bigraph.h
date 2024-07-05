#ifndef BIG_H
#define BIG_H

#include <list>
#include <vector>
#include <set>

#include "Edge.h"
#include "Bicluster.h"


using namespace std; 

//Estrutura que representa um Bigrafo
class Bigraph{
   public:
	
      //dimensoes
	static int V, U;

	//numero de 1's do bigrafo original
	static int e;
 
	//tipo de construcao a ser utilizada pelo GRASP (avaliar n1, fechar n1; avaliar n2, fechar n2; avaliar n2, fechar n1)
	static int constr_type;
 	//constantes que representam os tipos de construcoes possiveis
        static const int N1_N1;
        static const int N2_N2;
        static const int N2_N1;

	//Contador de edicoes ao bigrafo
	int ev, e0, edit;

	//Listas de Adjacencia de cada particao de vertices (N1)
	static vector<list<int> > laV;
	static vector<list<int> > laU;
	
	static void createN1();
		
	//Listas de Adjacencia dos vizinhos de cada vertices de cada particao(N2)
	static vector<set<int> > laV2;
	static vector<set<int> > laU2;
	
	static void createN2();
	 
	static list<Edge> candidatos;

	//Matriz contendo os pesos das arestas
	static int **w;

	static const int FORBIDDEN;
    	static const int FREE;
    	static const int PERMANENT;
        
	//Matriz usada para fixacao das aretas:
	//-1 : forbidden
	//0 : free
	//+1 : permanent
	int **edges;
	
        //conjunto de biclusters da solução
	vector<Bicluster *> C;
    	void extractClusters();

	int calculateJoin(Bicluster &b1, Bicluster &b2);
	void join(int idOri, int idDes);
	int breakBicluster(Bicluster *b1, Bicluster *b2);

	Bigraph();
        Bigraph(Bigraph &b);
        Bigraph(std::string);
	~Bigraph();
       

	//Calcula o numero de edicoes da solucao e atualiza o vetor DIFF caso updateDIFF == true
	int f(bool updateDIFF=true);
	
	//Implementada a busca local de Resende para o Cell Formation (vizinhanca de Resende adaptadoao Bilcuster Editing)
	Bigraph *bl1();
	Bigraph *bl1Random();
	
	//Implementada a junção de dois clusters (adaptado ao cell formation)
	Bigraph *bl2(bool f);

	//Implementada a quebra de um custer em dois clusters
	Bigraph *bl3(bool f);

    
	//funcoes gulosas usadas nas construcoes
	static float g_n2(int i, int j); 
	static float g_n1(int i,int j);

	void makeBicluster(int i, int j);
	void add(Bicluster *bc);

	void printBigraph();
        std::string clustersAsStr();
	
	static bool compare(Bigraph *b1, Bigraph *b2){
    		if(b1->ev + 1.05*b1->e0 == b2->ev + 1.05*b2->e0)
			return false;
    		else
    			return (b1->ev + 1.05*b1->e0 < b2->ev + 1.05*b2->e0);
    	}

   private:
	void makeBicluster_n1(int i,int j);
	void makeBicluster_n2(int i,int j);
	


};

struct BigraphCompare
{
  bool operator()(Bigraph *b1, Bigraph *b2) const
  {
    if(b1->ev + 1.05*b1->e0 == b2->ev + 1.05*b2->e0)
	return false;
    else
    	return (b1->ev + 1.05*b1->e0 < b2->ev + 1.05*b2->e0);
  }
};

#endif
