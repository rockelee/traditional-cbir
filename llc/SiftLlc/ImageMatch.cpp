#include <map>
#include "ImageMatch.h"

//传测试图像的文件路径  和 数据库图像的特征
void __stdcall MatchImg(const char *imgPath, int imgNum, int featureDim, const int *database, char** labels, int* matchNum, char*** results){

	int *labelsIntArray = (int*)malloc(imgNum*sizeof(int));
	for (int i = 0; i < imgNum; i++){
		labelsIntArray[i] = i;
	}
	Mat Img = imread(imgPath);
	int *feature = ImageFeatureExtract(Img.data, Img.cols, Img.rows, Img.step, featureDim);//,fp);
	int *matchresult = NULL;
	int matchRepeatNum = Matching(feature, database, labelsIntArray, featureDim, imgNum, &matchresult);//会有重复商家ID，进行筛选
	
	/* 投票匹配，选取距离最小的3张图片，然后用这3张图片的 label 投票，3 个label，找到出现次数最多的label*/
	vector<string> labels_3;
	*matchNum = matchRepeatNum >= 3 ? 3 : matchRepeatNum;
	if(matchRepeatNum>0){    //如果匹配的图片数量大于 0 ，才能进行投票，这个取决于 featureMatch.cpp 的 threshold
		for(int i = 0; i < *matchNum; i++){
			labels_3.push_back(labels[matchresult[i]]);
		}
		int max=0;
		string most;
		map<string,int> freq;
		for(int i=0;i<labels_3.size();i++){    //统计每个 label 出现的次数
			freq[labels_3[i]]++;
		}
		map<string,int>::iterator it;
		for(it = freq.begin();it != freq.end();it++){    //针对 3 个label有相同的情况，有相同的情况才进行投票；
			cout << it->first << ' ' << it->second << endl;
			if(it->second > max){
				max=it->second;
				most=it->first;
			}
		}
		if(freq.size()==3){      //针对 3 个label 互不相同的情况，选择距离最小的图片
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
	else{    //没有匹配的图片
		printf("no matching!");
		ReleaseFeature(labelsIntArray);
		ReleaseFeature(feature);
		system("pause");
		return ;
	}
}

