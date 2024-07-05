#include "Bicluster.h"
#include "BreakBicluster.h"
#include "Bigraph.h"

#include <algorithm>
#include <iostream>
#include <cstdlib>
#include<stdio.h>
#include <limits.h>

using namespace std;
      
Bicluster::Bicluster() : diffV(Bigraph::V, 0), diffU(Bigraph::U, 0), edit(0), ev(0), e0(0)
{
}
      
Bicluster::Bicluster(Bicluster &bc) : diffV(bc.diffV), diffU(bc.diffU), edit(bc.edit), ev(bc.ev), e0(bc.e0), V(bc.V), U(bc.U)
{
}

pair<bool, int> Bicluster::evaluateBreak(bool shake)
{
   pair<bool, int> result = std::make_pair(true, 0);
   double filtroV = 0;
   double filtroU = 0;	
   
   if(shake){
      for(int v : V)
	 filtroV += this->diffV[v];

      if(V.size() > 0)
	 filtroV /= V.size();

      for(int u : U)
	 filtroU += this->diffU[u];

      if(U.size() > 0)
	 filtroU /= U.size();
   }

   //this->print();
   vector<int> V1, V2, U1, U2;
   //Particao V
   for(int v : V){
      if(this->diffV[v] >= filtroV)
	 V2.push_back(v);
      else
	 V1.push_back(v);
   }

   //Particao U
   for(int u : U){
      if(this->diffU[u] >= filtroU)
	 U2.push_back(u);
      else
	 U1.push_back(u);
   }

   if((V1.size() + U1.size() == 0) || (U1.size() + U2.size() == 0))
      result.first = false;

   int ed = 0;
   for(int v1 : V1)
   {
      for(int u2 : U2)
	 ed += Bigraph::w[v1][u2];
   }
   for(int v2 : V2)
   {
      for(int u1 : U1)
	 ed += Bigraph::w[v2][u1];
   }
   result.second = ed;

   return result;
}
      
Bicluster *Bicluster::breakBicluster(Bigraph *bg, bool shake)
{
   Bicluster *bclu = new Bicluster();
   bclu->id = Bicluster::numBicluster++;
   
   double filtroV = 0;
   double filtroU = 0;	
   
   if(shake){
      for(int v : V)
	 filtroV += this->diffV[v];

      if(V.size() > 0)
	 filtroV /= V.size();

      for(int u : U)
	 filtroU += this->diffU[u];

      if(U.size() > 0)
	 filtroU /= U.size();
   }

   //Particao V
   list<int>::iterator itV = this->V.begin();
   for(; itV != this->V.end(); )
   {
      int v = *itV;
      if(this->diffV[v] >= filtroV)
      {
	 bclu->V.push_back(v);
	 itV = this->V.erase(itV);
      }else
	 ++itV;
   }

   //Particao U
   list<int>::iterator itU = this->U.begin();
   for(; itU != this->U.end(); )
   {
      int u = *itU;
      if(this->diffU[u] >= filtroU)
      {
	 bclu->U.push_back(u);
	 itU = this->U.erase(itU);
      }else
	 ++itU;
   }
   
   //Testa se criou bicluster vazio
   if(this->V.size() + this->U.size() == 0)
   {
      this->V = bclu->V;
      this->U = bclu->U;
      delete bclu;
      return NULL;
   }
   if(bclu->V.size() + bclu->U.size() == 0)
   {
      delete bclu;
      return NULL;
   }


   //Atualizando o Diff
   bclu->diffV = this->diffV;
   bclu->diffU = this->diffU;
   //Vertices contidos no cluster a ser quebrado
   for(int v : this->V)
   {
      for(int u : bclu->U)
      {
	 this->diffV[v] += Bigraph::w[v][u];
	 bclu->diffU[u] += Bigraph::w[v][u];
      }
      for(int u : this->U)
      {
	 bclu->diffV[v] += Bigraph::w[v][u];
	 bclu->diffU[u] += Bigraph::w[v][u];
      }
   }
   for(int v : bclu->V)
   {
      for(int u : bclu->U)
      {
	 this->diffV[v] += Bigraph::w[v][u];
	 this->diffU[u] += Bigraph::w[v][u];
      }
      for(int u : this->U)
      {
	 bclu->diffV[v] += Bigraph::w[v][u];
	 this->diffU[u] += Bigraph::w[v][u];
      }
   }
   
   //Vertices não contidos no bicluster a ser quebrado
   for(Bicluster *bc : bg->C)
   {
      if(bc != this)
      {
	 for(int v : bc->V)
	 {
	    for(int u : this->U)
	    {
	       bclu->diffV[v] += Bigraph::w[v][u]; 
	    }
	    for(int u : bclu->U)
	       this->diffV[v] += Bigraph::w[v][u]; 
	 }
	 
	 for(int u : bc->U)
	 {
	    for(int v : this->V)
	       bclu->diffU[u] += Bigraph::w[v][u]; 
	    for(int v : bclu->V)
	       this->diffU[u] += Bigraph::w[v][u]; 
	 }
      }
   }

   return bclu;
}

void Bicluster::print(){
   cout << getText() << endl;


   cout << "\nDIFF: ";
   for(int v=0; v < Bigraph::V; ++v)
      cout << this->diffV[v] << ",";
   for(int u=0; u < Bigraph::U; ++u)
      cout << this->diffU[u] << ",";
   cout << endl;
           
     cout << "\nE:" << this->edit << endl;
     cout << "E0:" << this->e0 << endl;
     cout << "Ev:" << this->ev << endl;
}

void Bicluster::removeV(Bigraph *g, int v)
{
   this->V.remove(v);

   int *wl = Bigraph::w[v];
   for(int u=0; u < g->U; ++u)
   {
      //atualiza Diff
      this->diffU[u] += wl[u];
   }
   g->edit -= this->diffV[v];
}

void Bicluster::removeU(Bigraph *g, int u)
{
   this->U.remove(u);

   for(int v=0; v < g->V; ++v)
   {
      //atualiza diff
      this->diffV[v] += Bigraph::w[v][u];
   }
   g->edit -= this->diffU[u];
}

void Bicluster::addV(Bigraph *g, int v)
{
   int *wl = Bigraph::w[v];
   for(int u=0; u < g->U; ++u)
   {
      //atualiza Diff
      this->diffU[u] -= wl[u];

   }
   this->V.push_back(v);
   
   g->edit += this->diffV[v];
}

void Bicluster::addU(Bigraph *g, int u)
{
   for(int v=0; v < g->V; ++v)
   {
      //atualiza diff
      this->diffV[v] -= Bigraph::w[v][u];

   }
   this->U.push_back(u);

   g->edit += this->diffU[u];
}

string Bicluster::getText()
{
     char v[10];
     string s = "(";
     list<int>::iterator it = V.begin();
     for(;it != V.end();){
	 sprintf(v, "%d", *it);
         s += v;
	 ++it;
	 if(it != V.end())
         	s += ",";
     }
     s += ")(";
     it = U.begin();
     for(;it != U.end();){
	 sprintf(v, "%d", *it);
         s += v;
	 ++it;
	 if(it != U.end())
         	s += ",";
     }
     s += ")";
     char ss[10];
     //this->calculateEdit();
     //sprintf(ss, "%lf", this->edit);
     //s += ss;
     //cout << s << endl; 
     return s;
}

int Bicluster::numBicluster = 0;
