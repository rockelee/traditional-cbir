
#include "ImageMatch.h"


float distance(const int *dstA, const int *dstB, int num)
{
	float dist = 0.0;
	for (int i = 0; i < num; i++){
		dist += abs(dstA[i] - dstB[i]);
	}
	return dist / num;
}


bool comp(const Dst& Dist1, const Dst& Dist2)
{
	return Dist2.dist < Dist1.dist;    //最小的放在第一个
}


/*----------------------------------------------------------
function: 特征匹配
input:    feature              待匹配图像的特征指针；
database             数据库中图像特征的数组指针；
label                数据库中每张图像对应店铺ID的数组指针；
featuredim			 数据库中图像特征的维度;
imgnum               数据库中图像的个数；
pMatchresult          存储匹配上的店铺ID数组指针；

return:   真正匹配上的图像个数，0表示匹配失败；
----------------------------------------------------------*/
int Matching(const int *feature, const int *database, int *label, int featuredim, int imgnum, int **pMatchresult)  //传测试图像的特征 和 数据库图像的特征 
{
	float min = 1000000;
	float threshold = 300000;
	int num = 0;
	Dst* results = (Dst*)malloc(sizeof(Dst)*imgnum);
	for (int i = 0; i < imgnum; i++){
		float dist = distance(feature, database + i*featuredim, featuredim);
		if (dist < threshold){                               //小于此阈值的才算匹配，
				min = dist;
				results[num].ID = label[i];
				results[num].dist = dist;
				num++;
		}
	}
	cout << "num : " << num << endl;
	make_heap(results, results + num, comp);
	(*pMatchresult) = (int*)malloc(sizeof(int)*num);
	for (int i = 0; i < num; i++){
		pop_heap(results, results + num - i, comp);
		(*pMatchresult)[i] = results[num - i - 1].ID;     //按 距离的从小到大排序
		if(i<10)
			cout << results[num-i-1].ID << '\t' << results[num-i-1].dist << endl;
	}
	free(results);
	return num;
}


