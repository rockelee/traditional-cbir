#include <map>
#include "ImageMatch.h"

//������ͼ����ļ�·��  �� ���ݿ�ͼ�������
void __stdcall MatchImg(const char *imgPath, int imgNum, int featureDim, const int *database, char** labels, int* matchNum, char*** results){

	int *labelsIntArray = (int*)malloc(imgNum*sizeof(int));
	for (int i = 0; i < imgNum; i++){
		labelsIntArray[i] = i;
	}
	Mat Img = imread(imgPath);
	int *feature = ImageFeatureExtract(Img.data, Img.cols, Img.rows, Img.step, featureDim);//,fp);
	int *matchresult = NULL;
	int matchRepeatNum = Matching(feature, database, labelsIntArray, featureDim, imgNum, &matchresult);//�����ظ��̼�ID������ɸѡ
	
	/* ͶƱƥ�䣬ѡȡ������С��3��ͼƬ��Ȼ������3��ͼƬ�� label ͶƱ��3 ��label���ҵ����ִ�������label*/
	vector<string> labels_3;
	*matchNum = matchRepeatNum >= 3 ? 3 : matchRepeatNum;
	if(matchRepeatNum>0){    //���ƥ���ͼƬ�������� 0 �����ܽ���ͶƱ�����ȡ���� featureMatch.cpp �� threshold
		for(int i = 0; i < *matchNum; i++){
			labels_3.push_back(labels[matchresult[i]]);
		}
		int max=0;
		string most;
		map<string,int> freq;
		for(int i=0;i<labels_3.size();i++){    //ͳ��ÿ�� label ���ֵĴ���
			freq[labels_3[i]]++;
		}
		map<string,int>::iterator it;
		for(it = freq.begin();it != freq.end();it++){    //��� 3 ��label����ͬ�����������ͬ������Ž���ͶƱ��
			cout << it->first << ' ' << it->second << endl;
			if(it->second > max){
				max=it->second;
				most=it->first;
			}
		}
		if(freq.size()==3){      //��� 3 ��label ������ͬ�������ѡ�������С��ͼƬ
			most=labels_3[0];
		}

		ReleaseFeature(labelsIntArray);
		ReleaseFeature(feature);
		(*results)=(char**)malloc(1*sizeof(char*));

		int len =most.size();
		(*results)[0]=(char*)malloc(sizeof(char)*(len+1));
		strcpy((*results)[0],most.c_str());
		return;
	}
	else{    //û��ƥ���ͼƬ
		printf("no matching!");
		ReleaseFeature(labelsIntArray);
		ReleaseFeature(feature);
		system("pause");
		return ;
	}
}

