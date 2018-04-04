
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
	return Dist2.dist < Dist1.dist;    //��С�ķ��ڵ�һ��
}


/*----------------------------------------------------------
function: ����ƥ��
input:    feature              ��ƥ��ͼ�������ָ�룻
database             ���ݿ���ͼ������������ָ�룻
label                ���ݿ���ÿ��ͼ���Ӧ����ID������ָ�룻
featuredim			 ���ݿ���ͼ��������ά��;
imgnum               ���ݿ���ͼ��ĸ�����
pMatchresult          �洢ƥ���ϵĵ���ID����ָ�룻

return:   ����ƥ���ϵ�ͼ�������0��ʾƥ��ʧ�ܣ�
----------------------------------------------------------*/
int Matching(const int *feature, const int *database, int *label, int featuredim, int imgnum, int **pMatchresult)  //������ͼ������� �� ���ݿ�ͼ������� 
{
	float min = 1000000;
	float threshold = 300000;
	int num = 0;
	Dst* results = (Dst*)malloc(sizeof(Dst)*imgnum);
	for (int i = 0; i < imgnum; i++){
		float dist = distance(feature, database + i*featuredim, featuredim);
		if (dist < threshold){                               //С�ڴ���ֵ�Ĳ���ƥ�䣬
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
		(*pMatchresult)[i] = results[num - i - 1].ID;     //�� ����Ĵ�С��������
		if(i<10)
			cout << results[num-i-1].ID << '\t' << results[num-i-1].dist << endl;
	}
	free(results);
	return num;
}


