#ifndef RB_H
#define RB_H

#include <list>
#include <vector>

#include "Bigraph.h"

//Estrutura que implementa as regras de reducao para o Bicluster Editing
class ReducerBigraph{
    public:
	//dimensoes
	int V, U;
	int Voriginal, Uoriginal;
	
	//numero de 1's do bigrafo original
	int e;

	//Listas de Adjacencia de cada particao de vertices
	vector<list<int>> laV;
	vector<list<int>> laU;

	//Vetores contendo os indices das linhas e colunas dos vertices  
	vector<int> indRow;
	vector<int> indCol;

	//Matriz contendo os pesos das arestas
	int **w;

	//Matriz usada para fixacao das arestas:
	//-1 : forbidden
	//0 : free
	//+1 : permanent
	int **edges;

	ReducerBigraph(int v, int u);
	~ReducerBigraph();

	static ReducerBigraph *loadFile(char *fileName);

	void print();
	void initBigraph(int constr_type);
	static Bigraph *getBigraph();
	void f();
	
	//##Regras de reducao##//

	//Procura Elemento em cada particao que tenham a mesma vizinhanca. Caso verdade junta os vertices em um so.	
	void nrule2();
	void nrule2V();
	void nrule2U();
	
	//Procura o conjunto idenpendente R e remove elementos dele enquanto |T| < |R|, considere T = N2(R)\R 
	void rule2();
	void rule2V();
	void rule2U();
	
	//Testa se ha linhas ou colunas sem arestas associadas (-1). As mesmas serao eliminadas da entrada.
	void rule3();

	//Para toda aresta w(i,j) < 0, se |w(i,j)| < |N(l) - N(j)| (onde l é o vizinho de i com maior grau)
	void rule4();
	void rule4VU(int i,int j, list<int> &Ni, list<int> &Nj);
        void rule4UV(int i,int j, list<int> &Ni, list<int> &Nj);
	
	static const int FORBIDDEN;
	static const int FREE;
	static const int PERMANENT;

	static ReducerBigraph *getInstance();
	static ReducerBigraph *rb;

};

#endif
