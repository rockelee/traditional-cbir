
#include "utils.h"

extern int nClusters;
extern int dimension;
extern int kmeansClusters;

VlGMM* readGMM()
{
	//读取gmm参数
	float* means=(float*)malloc(sizeof(float)*nClusters*dimension);
	float* covariances=(float*)malloc(sizeof(float)*nClusters*dimension);
	float* priors=(float*)malloc(sizeof(float)*nClusters);
	FILE * fp1=fopen("gmm_means_covariances_priors_kmeans.cb","rb");
	fread(&nClusters,sizeof(int),1,fp1);
	fread(&dimension,sizeof(int),1,fp1);
	fread(means,sizeof(float),nClusters*dimension,fp1);
	fread(covariances,sizeof(float),nClusters*dimension,fp1);
	fread(priors,sizeof(float),nClusters,fp1);
	fclose(fp1);
	//创建gmm实例
	VlGMM* gmm=vl_gmm_new(VL_TYPE_FLOAT, dimension, nClusters);
	vl_gmm_set_means(gmm,means);
	vl_gmm_set_covariances(gmm,covariances);
	vl_gmm_set_priors(gmm,priors);
	free(means);
	free(covariances);
	free(priors);
	return gmm;
}

//void readCodebook(Mat& centres)
//{
//	FileStorage codebook("codebook.yml", FileStorage::READ);
//	codebook["codebook"] >> centres;
//}

string genrateLabel(string lable, vector<string> list)
{
	int comma_n;
	do{
		string tmp_s = "";
		comma_n = lable.find("\\");
		if(-1==comma_n){
			tmp_s = lable.substr(0,lable.length());
			list.push_back(tmp_s);
			break;
		}
		tmp_s = lable.substr(0,comma_n);
		lable.erase(0,comma_n+1);
		list.push_back(tmp_s);

	}while(true);
	string labell;
	labell+=list[3];
	labell+=list[4];
	labell+=list[5];
	labell+=list[6];
	labell+=list[7];
	return labell;
}

string genrateTestLabel(string lable, vector<string> list)
{
	int comma_n;
	do{
		string tmp_s = "";
		comma_n = lable.find("\\");
		if(-1==comma_n){
			tmp_s = lable.substr(0,lable.length());
			list.push_back(tmp_s);
			break;
		}
		tmp_s = lable.substr(0,comma_n);
		lable.erase(0,comma_n+1);
		list.push_back(tmp_s);

	}while(true);
	string labell;
	labell+=list[3];
	return labell;
}