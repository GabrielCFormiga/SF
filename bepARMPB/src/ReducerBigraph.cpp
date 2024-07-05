#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <set>

#include "ReducerBigraph.h"

ReducerBigraph::ReducerBigraph(int v, int u): V(v), Voriginal(v), U(u), Uoriginal(u){
	
    this->e = 0;
    for(int i=0; i < V; ++i)
		this->indRow.push_back(i);
	
	for(int i=0; i < U; ++i)
		this->indCol.push_back(i);

	this->w = new int*[V];
	this->edges = new int*[V];
	for(int i=0; i < V; ++i){
		this->w[i] = new int[U];
		this->edges[i] = new int[U];
		memset(this->edges[i], ReducerBigraph::FREE, sizeof(int)*U);
	}
}

void ReducerBigraph::initBigraph(int constr_type)
{
	Bigraph::V = this->V;
	Bigraph::U = this->U;
	Bigraph::e = this->e;
	Bigraph::constr_type = constr_type;
	Bigraph::laV.resize(Bigraph::V);
	Bigraph::laU.resize(Bigraph::U);
	Bigraph::laV2.resize(Bigraph::V);
	Bigraph::laU2.resize(Bigraph::U);
	
	Bigraph::w = new int*[V];
        for(int i=0; i < V; ++i){
                Bigraph::w[i] = new int[U];
        }


	for(int i=0; i < this->V; ++i){
		for(int j = 0; j <this->U; ++j){
			Bigraph::w[i][j] = this->w[indRow[i]][indCol[j]];
		}
	}
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

Bigraph *ReducerBigraph::getBigraph()
{
	Bigraph *bg = new Bigraph();
	
	for(int i=0; i < rb->V; ++i)
	{
            for(int j = 0; j < rb->U; ++j)
	    {
                    bg->edges[i][j] = rb->edges[rb->indRow[i]][rb->indCol[j]];
            }
    	}
	
    	return bg;
}

void ReducerBigraph::nrule2()
{
	this->nrule2V();
	this->nrule2U();
}

void ReducerBigraph::nrule2V()
{
	//Tenta dar o merge nos vertices de V que tenham a mesma vizinhanca
	for(int i=0 ; i < this->V; ++i){
		//cout << "I: " << i << endl;
		int indi = this->indRow[i];
		vector<int> vequal;
		vector<int> vequalConteudo;
		//cout << "E: " << indRow[i] << " - ";
		//Procura os vertices com vizinhos semelhantes
		for(int j=i+1 ; j < this->V; ++j){
			//cout << "J: " << j << endl;
			int indj = this->indRow[j];
			if(this->laV[indi].size() == this->laV[indj].size()){
				if(this->laV[indi] == this->laV[indj]){
					vequal.push_back(j);
					vequalConteudo.push_back(indj);
					//cout << "indRow[j]: " << indRow[j] << " j: " << j << " - ";
				}
			}
		}
		
		vequal.push_back(i);
		vequalConteudo.push_back(this->indRow[i]);
		
		//Se todos os vizinhos de vequal tiverem os mesmos vizinhos, encontramos um bicluster. O mesmo deve ser removido do grafo de entrada
                bool isBicluster = true;
		vector<int> vequalJ;
                	
		list<int> Ni = this->laV[indRow[*vequal.begin()]];
                //cout << "E: " << indCol[i] << " - ";
                
		if(!Ni.empty()){
			//Procura os vertices com vizinhos semelhantes
			list<int>::iterator itNi = Ni.begin();
			int indii = *itNi; ++itNi;
                	vequalJ.push_back(indii);
		
			for(; itNi != Ni.end(); ++itNi){
                       		int indj = *itNi;
                        	//cout << "J: " << indj << endl;
                       		if(this->laU[indii].size() == this->laU[indj].size() && (this->laU[indii] == this->laU[indj])){
                        		vequalJ.push_back(indj);
                                	//cout << indCol[indj] << " j: " << indj <<" - ";
                        	}else{
					isBicluster = false;
					break;
				}
			}
		}else
			isBicluster = false;

		if(isBicluster){
			//Testa se a vizinhança de Ni é igual ao conteudo de vequalConteudo (ultimo teste para definir um bicluster)
			list<int> Nj = laU[*Ni.begin()];
			list<int>::iterator itNj = Nj.begin();
			for(; itNj != Nj.end(); ++itNj){
                        	int indj = *itNj;
				if(std::find(vequalConteudo.begin(), vequalConteudo.end(), indj) == vequalConteudo.end())
					isBicluster = false;
			}
			if(isBicluster){
				//cout << "Achei Bicluster" << endl;
				vector<int>::iterator it = vequalConteudo.begin();
				for(; it != vequalConteudo.end(); ++it){
				 	//cout << "I: " << *it << endl;
			 		this->indRow.erase(std::find(indRow.begin(), indRow.end(), *it));
                                	--this->V;
				}
				//vequalJ guarda o valor do vertice e n sua posicao no vetor.	
				it = vequalJ.begin();
				for(; it != vequalJ.end(); ++it){
			 		//cout << "J: " << *it << endl;
			 		this->indCol.erase(std::find(indCol.begin(), indCol.end(), *it));
                                	--this->U;
				}

				//Faz o for do i permanecer na mesma posição
				//cout << "V: " << V << " U: " << U <<endl;
				i = -1;
				continue;
			}
			//cout << "Não é bicluster" << endl;
		}

		//Adiciona na linha do primeiro vertice de vequal o peso das linhas dos outros vertices.
		if(vequal.size() > 1){
			int l1 = this->indRow[vequal.back()];
			vequal.pop_back();
			//cout << "L1: " << l1 << endl;	
			while (!vequal.empty()){
				int posl2 = vequal.back();
				int l2 = this->indRow[posl2];
				vequal.pop_back();
				//cout << "L2: " << l2 << endl;
	
				//Somando os pesoas da linha l2 em l1
				for(int j = 0; j <this->U; ++j)
					this->w[l1][indCol[j]] += this->w[l2][indCol[j]];
				//Removendo a linha l2 da matriz
				//cout << "Pos L2: " << posl2 << endl;
				this->indRow.erase(this->indRow.begin() + posl2);
				--this->V;
				//cout << "V: " << V << endl;
				//Atualizando as listas de adjacencia
				list<int> N = this->laV[l2];
				list<int>::iterator in = N.begin();
				for(; in != N.end(); ++in){
					int nn = (*in);
					//cout << "NN:" << nn << endl;
					laU[nn].remove(l2);
				}
				this->laV[l2].clear();

			}
		}
	}
}

void ReducerBigraph::nrule2U()
{
	//Tenta dar o merge nos vertices de U que tenham a mesma vizinhanca
	for(int i=0 ; i < this->U; ++i){
		//cout << "I: " << i << endl;
		int indi = this->indCol[i];
		vector<int> vequal;
		//cout << "E: " << indCol[i] << " - ";
		//Procura os vertices com vizinhos semelhantes
		for(int j=i+1 ; j < this->U; ++j){
			//cout << "J: " << j << endl;
			int indj = this->indCol[j];
			if(this->laU[indi].size() == this->laU[indj].size()){
				if(this->laU[indi] == this->laU[indj]){
					vequal.push_back(j);
					//cout << "indCol[j]: " << indCol[j] << " j: " << j <<" - ";
				}
			}
		}
		
		vequal.push_back(i);

		//Adiciona na linha do primeiro vertice de vequal o peso das linhas dos outros vertices.
		if(vequal.size() > 1){
			int l1 = this->indCol[vequal.back()];
			vequal.pop_back();
			//cout << "L1: " << l1 << endl;	
			while (!vequal.empty()){
				int posl2 = vequal.back();
				int l2 = this->indCol[posl2];
				vequal.pop_back();
				//cout << "L2: " << l2 << endl;
	
				//Somando os pesoas da coluna l2 em l1
				for(int j = 0; j <this->V; ++j)
					this->w[indRow[j]][l1] += this->w[indRow[j]][l2];
				//Removendo a linha l2 da matriz
				//cout << "Pos L2: " << posl2 << endl;
				this->indCol.erase(this->indCol.begin() + posl2);
				--this->U;
				//cout << "U: " << U << endl;
				//Atualizando as listas de adjacencia
				list<int> N = this->laU[l2];
				list<int>::iterator in = N.begin();
				for(; in != N.end(); ++in){
					int nn = (*in);
					//cout << "NN:" << nn << endl;
					laV[nn].remove(l2);
				}
				this->laU[l2].clear();

			}
			/*for(int i=0; i < U; ++i)
                		cout << " IndCol[" << i << "]: " << this->indCol[i];
        		cout << endl;*/
		}
	}
}

void ReducerBigraph::rule3()
{
	for(int i=0 ; i < V;){
        if(laV[indRow[i]].size() == 0){
			this->indRow.erase(this->indRow.begin() + i);
			--this->V;
		}else
		     ++i;
	}
	for(int j=0 ; j < U;){
		if(laU[indCol[j]].size() == 0){
			this->indCol.erase(this->indCol.begin() + j);
			--this->U;
		}else
		     ++j;
	}
}

void ReducerBigraph::rule4()
{
	for(int i=0 ; i < this->V; ++i){
		if(!laV[indRow[i]].empty()){
        		for(int j=0 ; j < this->U; ++j){
               			if(!laU[indCol[j]].empty()){
                   			list<int> Ni = laV[indRow[i]];
                   			list<int> Nj = laU[indCol[j]];				
                   			this->rule4VU(i, j, Ni, Nj);
                  			//this->rule4UV(i, j, Ni, Nj);
               			}
           		}
		}
	}
}

/*void ReducerBigraph::rule4VU(int i, int j, list<int> &Ni, list<int> &Nj)
{
	//obtendo o vizinho de j de maior grau(maior vizinhanca)
    	int maiorNj = -1;
	int grau = 0; 
	list<int>::iterator it = Nj.begin();
    	//&&&for(; it!= Nj.end(); ++it){
          if(*it == indRow[i])
                 continue;
          list<int> Nit = laV[*it];
          list<int>::iterator iNit = Nit.begin();
          
	  int g = 0;
          for(; iNit!= Nit.end(); ++iNit){
                g+= this->w[*it][*iNit];
          }
          if(g > grau){
               grau = g;
               maiorNj = *it;
          }
	}
    	if(maiorNj == -1)
          return;
	&&&/			           
	//list<int> NmaiorNj = laV[maiorNj];
	//list<int> Nj = laU[indCol[j]];
	int diff = 0;
	//it = NmaiorNj.begin();
	//for(; it!= NmaiorNj.end(); ++it){
	for(; it!= Nj.end(); ++it){
          	if(*it == indRow[i])
			continue;
		list<int>::iterator itNj = laV[*it].begin();
		for(; itNj!= laV[*it].end(); ++itNj){
			if(std::find(Ni.begin(), Ni.end(), *itNj) == Ni.end())
				diff += this->w[*it][*itNj];
		}
	}
	
	it = Ni.begin();
	for(; it!= Ni.end(); ++it){
          	if(*it == indCol[j])
			continue;
		list<int>::iterator itNi = laU[*it].begin();
		for(; itNi!= laU[*it].end(); ++itNi){
			if(std::find(Nj.begin(), Nj.end(), *itNi) == Nj.end())
				diff += this->w[*itNi][*it];
		}
	}
	
	//testa se o modulo do peso da aresta ij e menor que a diferenca de vizinhanca
	//cout << "I:" << indRow[i] << " J: " << indCol[j] << " W: " << this->w[indRow[i]][indCol[j]] << " DIFF: " << diff << endl;
	/&&if((this->w[indRow[i]][indCol[j]] < 0) && (diff - fabs(this->w[indRow[i]][indCol[j]]) > 2) && (fabs(this->w[indRow[i]][indCol[j]]) >= this->w[maiorNj][indCol[j]])){
	/&&
	if((this->w[indRow[i]][indCol[j]] < 0) && (diff - fabs(this->w[indRow[i]][indCol[j]] > 1))){
		cout << "I:" << indRow[i] << " J: " << indCol[j] << " W: " << this->w[indRow[i]][indCol[j]] << " DIFF: " << diff << endl;
		this->edges[indRow[i]][indCol[j]] = FORBIDDEN;
	}else
	if((this->w[indRow[i]][indCol[j]] > 0) && (this->w[indRow[i]][indCol[j]] - diff > 0)){
		//cout << "I:" << indRow[i] << " J: " << indCol[j] << " W: " << this->w[indRow[i]][indCol[j]] << " DIFF: " << diff << endl;
	    this->edges[indRow[i]][indCol[j]] = PERMANENT;
	}
}*/

void ReducerBigraph::rule4VU(int i, int j, list<int> &Ni, list<int> &Nj)
{
	//obtendo o vizinho de j de maior grau(maior vizinhanca)
    	int maiorNj = -1;
	int grau = 0; 
	list<int>::iterator it = Nj.begin();
	int diffinV = 0;
	int diffinU = 0;
	int diffoutV = 0;
	int diffoutU = 0;
	
	set<int> N2i;
	for(; it!= Nj.end(); ++it){
          	if(*it == indRow[i])
			continue;
		N2i.insert(*it);
		/*list<int>::iterator itNj = laV[*it].begin();
		for(; itNj!= laV[*it].end(); ++itNj){
			if(std::find(Ni.begin(), Ni.end(), *itNj) == Ni.end())
				diff += this->w[*it][*itNj];
		}*/
	}
	
	it = Ni.begin();
	set<int> N2j;
	for(; it!= Ni.end(); ++it){
          	if(*it == indCol[j])
			continue;
		
		N2j.insert(*it);
		/*list<int>::iterator itNi = laU[*it].begin();
		for(; itNi!= laU[*it].end(); ++itNi){
			if(std::find(Nj.begin(), Nj.end(), *itNi) == Nj.end())
				diff += this->w[*itNi][*it];
		}*/
	}

	set<int>::iterator itN2 = N2i.begin();
	for(; itN2!= N2i.end(); ++itN2){
		list<int>::iterator itNj = laV[*itN2].begin();
                for(; itNj!= laV[*itN2].end(); ++itNj){
                        if(std::find(N2j.begin(), N2j.end(), *itNj) == N2j.end())
                                diffoutV += this->w[*itN2][*itNj];
			else
                                diffinV += this->w[*itN2][*itNj];
                }
	}

	itN2 = N2j.begin();
        for(; itN2!= N2j.end(); ++itN2){
                list<int>::iterator itNi = laU[*itN2].begin();
                for(; itNi!= laU[*itN2].end(); ++itNi){
                        if(std::find(N2i.begin(), N2i.end(), *itNi) == N2i.end())
                                diffoutU += this->w[*itNi][*itN2];
			else
                                diffinU += this->w[*itNi][*itN2];
                }
        }
	
	if((this->w[indRow[i]][indCol[j]] > 0) && ((diffinV - diffoutV) > this->w[indRow[i]][indCol[j]])  && ((diffinU - diffoutU) > this->w[indRow[i]][indCol[j]]))
	    this->edges[indRow[i]][indCol[j]] = PERMANENT;
	//if((this->w[indRow[i]][indCol[j]] < 0) && ((diffoutV - diffinV) - fabs(this->w[indRow[i]][indCol[j]] > 0)) && ((diffoutU - diffinU) - fabs(this->w[indRow[i]][indCol[j]] > 0)))
	//if((this->w[indRow[i]][indCol[j]] < 0) && (diffinV > fabs(this->w[indRow[i]][indCol[j]])) && (diffinU > fabs(this->w[indRow[i]][indCol[j]])))
	//    this->edges[indRow[i]][indCol[j]] = FORBIDDEN;

	
	//testa se o modulo do peso da aresta ij e menor que a diferenca de vizinhanca
	//cout << "I:" << indRow[i] << " J: " << indCol[j] << " W: " << this->w[indRow[i]][indCol[j]] << " DIFF: " << diff << endl;
	/*if((this->w[indRow[i]][indCol[j]] < 0) && (diff - fabs(this->w[indRow[i]][indCol[j]]) > 2) && (fabs(this->w[indRow[i]][indCol[j]]) >= this->w[maiorNj][indCol[j]])){
	*/
	//else
	//    this->edges[indRow[i]][indCol[j]] = FORBIDDEN;
	
	/*if((this->w[indRow[i]][indCol[j]] < 0) && ((diffin - diffout) > fabs(this->w[indRow[i]][indCol[j]]))){
		cout << "I:" << indRow[i] << " J: " << indCol[j] << " W: " << this->w[indRow[i]][indCol[j]] << " DIFF: " << diff << endl;
		this->edges[indRow[i]][indCol[j]] = FORBIDDEN;
	}else
	if((this->w[indRow[i]][indCol[j]] > 0) && (this->w[indRow[i]][indCol[j]] - diff > 0)){
		//cout << "I:" << indRow[i] << " J: " << indCol[j] << " W: " << this->w[indRow[i]][indCol[j]] << " DIFF: " << diff << endl;
	    this->edges[indRow[i]][indCol[j]] = PERMANENT;
	}*/
}

void ReducerBigraph::rule4UV(int i,int j, list<int> &Ni, list<int> &Nj)
{
	//cout << "I: "<< i <<endl;
	//cout << "J: "<< j<<endl;
     //obtendo o vizinho de j de maior grau(maior vizinhanca)
    int maiorNi = -1;
	int grau = 0; 
	list<int>::iterator it = Ni.begin();
    for(; it!= Ni.end(); ++it){
          //if(*it == indCol[j])
          //       continue;
          list<int> Nit = laU[*it];
          list<int>::iterator iNit = Nit.begin();
          int g = 0;
          for(; iNit!= Nit.end(); ++iNit){
                g+= this->w[*iNit][*it];
          }
          if(g > grau){
               grau = g;
               maiorNi = *it;
          }
	}
    if(maiorNi == -1)
          return;
	//cout << "Maior Ni: "<< maiorNi<<endl; 	           
	list<int> NmaiorNi = laU[maiorNi];
	int diff = 0;
	it = NmaiorNi.begin();
	for(; it!= NmaiorNi.end(); ++it){
		if(std::find(Nj.begin(), Nj.end(), *it) == Nj.end()){
			//cout << "Achou NmaniorNi: " << *it << endl;
            diff += this->w[*it][maiorNi];
            //cout <<"DIFF: "<<diff<<endl;
        }
	}
	//cout << "Peso: " << this->w[indRow[i]][indCol[j]] <<endl;
	//testa se o modulo do peso da aresta ij e menor que a diferenca de vizinhanca 
	/*if((this->w[indRow[i]][indCol[j]] < 0) && (diff - fabs(this->w[indRow[i]][indCol[j]]) > 1))
		this->edges[indRow[i]][indCol[j]] = FORBIDDEN;
	else if((this->w[indRow[i]][indCol[j]] > 0) && (this->w[indRow[i]][indCol[j]] - diff > 1))
	    this->edges[indRow[i]][indCol[j]] = PERMANENT;*/
	if((this->w[indRow[i]][indCol[j]] > 0) && (this->w[indRow[i]][indCol[j]] - diff > 0)){
		cout << "I:" << indRow[i] << " J: " << indCol[j] << " W: " << this->w[indRow[i]][indCol[j]] << " DIFF: " << diff << endl;
	    this->edges[indRow[i]][indCol[j]] = PERMANENT;
	}
}

ReducerBigraph *ReducerBigraph::loadFile(char *fileName){
	ifstream Instancia(fileName, ios::in);
   	int rSize;

   	if (!Instancia) {
      		cerr << "Arquivo nao pode ser aberto" << endl;
		return NULL;
   	}
	//string header;
	//std::getline(Instancia, header);

	int v, u;
	Instancia >> v; // numero de pecas
   	Instancia >> u; // numero de maquinas

	ReducerBigraph::rb = new ReducerBigraph(v, u);
	rb->laV.resize(v);
	rb->laU.resize(u);

	for (int i = 0; i < v; i++) {
        	for (int j = 0; j < u; j++) {
                	int data;
			Instancia >> data;
			if(data == 1){
				++rb->e;
				rb->w[i][j] = 1;
				rb->laV[i].push_back(j);
				rb->laU[j].push_back(i);
				
			}else
				rb->w[i][j] = -1;
        	}//printf("\n");
   	}
	Instancia.close();
	return rb;
}

void ReducerBigraph::print(){

        for(int i=0; i < V; ++i)
                cout << " IndRow[" << i << "]: " << this->indRow[i]; 
	cout << endl;

        for(int i=0; i < U; ++i)
                cout << " IndCol[" << i << "]: " << this->indCol[i];
	cout << endl;

        cout << "W:" << endl;
	for(int i=0; i < V; ++i){
		for(int j=0; j < U; ++j)
                	cout << this->w[indRow[i]][indCol[j]] << " ";
		cout << endl;
        }
        
	cout << "Edges:" << endl;
	for(int i=0; i < V; ++i){
		for(int j=0; j < U; ++j)
                	cout << this->edges[indRow[i]][indCol[j]] << " ";
		cout << endl;
        }

	cout << "Lista Adjacencia V:" << endl;
	for(int i=0; i < V; ++i){
		cout << "V: " << indRow[i] << endl;
		list<int> l = this->laV[indRow[i]];
		list<int>::iterator il = l.begin();
		cout << "N(.): ";
		for(; il!= l.end(); ++il)
			cout << *il<< " ";
		cout << endl;
	}
	
	cout << "Lista Adjacencia U:" << endl;
	for(int j=0; j < U; ++j){
		cout << "U: " << indCol[j] << endl;
		list<int> l = this->laU[indCol[j]];
		list<int>::iterator il = l.begin();
		cout << "N(.): ";
		for(; il!= l.end(); ++il)
			cout << *il<< " ";
		cout << endl;
	}
	
}

void ReducerBigraph::f(){
     int e = 0;
     int e0 = 0;
     int ev = 0;
        
	for(int i=0; i < V; ++i){
		for(int j=0; j < U; ++j)
            if(this->w[indRow[i]][indCol[j]] > 0)
               e += this->w[indRow[i]][indCol[j]];
    } 
        
	for(int i=0; i < V; ++i){
		for(int j=0; j < U; ++j){
            if(this->edges[indRow[i]][indCol[j]] > 0){//PERMANENT
               if(this->w[indRow[i]][indCol[j]] < 0)
                    ev += -(this->w[indRow[i]][indCol[j]]);
            }else if(this->edges[indRow[i]][indCol[j]] < 0){
               if(this->w[indRow[i]][indCol[j]] > 0)//FORBIDDEN
                    e0 += this->w[indRow[i]][indCol[j]];   
            }
        }	
    }
    
    cout << "E: " << e << endl;
    cout << "E0: " << e0 << endl;
    cout << "Ev: " << ev << endl;
    cout << "T: " << (float)(e - e0) / (e + ev) << endl;
}

ReducerBigraph::~ReducerBigraph(){
	for(int i=0; i < Voriginal; ++i){
		delete this->w[i];
		delete this->edges[i];
	}
	delete this->w;
	delete this->edges;
}

ReducerBigraph *ReducerBigraph::getInstance(){
	return rb;
}

ReducerBigraph *ReducerBigraph::rb = NULL;

const int ReducerBigraph::FORBIDDEN = -1;
const int ReducerBigraph::FREE = 0;
const int ReducerBigraph::PERMANENT = 1;
