#ifndef BCLU_H
#define BCLU_H

#include <vector>
#include <list>
#include <string>

class Bigraph;
class BreakBicluster;

using namespace std;

class Bicluster
{
   public:
      static int numBicluster;
      int id;
      list<int> V;
      list<int> U;
      
      float edit, ev, e0;
     
       //indica o quanto o vertice esta ligado ao seu cluster atual
       //diff = (soma das ligacoes positivas - soma das ligacoes negativas) com o cluster atual
       vector<int> diffV, diffU;

      Bicluster();
      
      Bicluster(Bicluster &bc);
      void print();
      string getText();

       //Atualiza o edit do Bigraph e o diff dos vertices com relacao ao cluster.

       void removeV(Bigraph *g, int v);
       void removeU(Bigraph *g, int u); 
       void addV(Bigraph *g, int v);
       void addU(Bigraph *g, int u);

      pair<bool, int> evaluateBreak(bool shake);
      Bicluster *breakBicluster(Bigraph *bg, bool shake);

};

#endif
