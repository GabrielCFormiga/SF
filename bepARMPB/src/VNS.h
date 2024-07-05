#ifndef VNS_H
#define VNS_H

#include <stdlib.h>
#include <time.h>
#include <set>

#include "ReducerBigraph.h"

#define NUM_ITER 40
#define FILTRO 1

class VNS{
	private:
		static Bigraph *greedyConstruction();
		
	public:
		Bigraph *s;
		VNS();
		static Bigraph *vnd(Bigraph *,int kmax=2);
		static Bigraph *vns(Bigraph *solAtual, float maxTime);
		static Bigraph *vns(Bigraph *solAtual);
		void start();

		//Bigraph *ils(set<Bigraph*, BigraphCompare> elite);
		static Bigraph *ils(Bigraph *elite, float maxTime);
		static Bigraph *ils(Bigraph *elite);
};

#endif
