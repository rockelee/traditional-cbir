
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
//#include <float.h>
#include <algorithm>

using namespace std;
struct Dist{
		int ID;
		float distance;
};

bool comp(const struct Dist& Dist1, const struct Dist& Dist2)
{
	return Dist2.distance < Dist1.distance;    //��С�ķ��ڵ�һ��
}

void Match(float *feature, float *featurelib, int nClusters, int dimension, int ndbJpgs, int *matchResult, int K)
{
	struct Dist *dist=(struct Dist*)malloc(sizeof(Dist)*ndbJpgs);
	for(int i=0;i<ndbJpgs;i++){
		dist[i].ID=i;
		dist[i].distance=0.0;
		for(int j=0;j<nClusters*dimension;j++){
			dist[i].distance += abs( feature[ j ] - featurelib[ i * nClusters * dimension + j]);
		}
	}
	make_heap(dist,dist+ndbJpgs,comp);
	for(int i=0;i<K;i++){
		pop_heap(dist,dist+ndbJpgs-i,comp);
		matchResult[i]=dist[ndbJpgs-i-1].ID;
	}
	free(dist);
}
