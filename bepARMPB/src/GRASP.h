#ifndef GRASP_H
#define GRASP_H

#include <stdlib.h>
#include <time.h>

#include "ReducerBigraph.h"
#include "VNS.h"
#include "Util.h"

#define NUM_ITER 40
#define FILTRO 1

class GRASP{

	private:
		double alpha;
		Bigraph *buscaLocal(Bigraph *sol);
		VNS perturbacao;
		Bigraph* iteracao();
	public:
		GRASP(double a);
		Bigraph* run(int iteracoes);
		Bigraph* construcao();
		Bigraph* grasp_vns(float maxTime);
		Bigraph* grasp_ils(float maxTime);
		Bigraph* grasp_vns(int n_iter);
		Bigraph* grasp_ils(int n_iter);

};

#endif
