#include <map>
#include "ImageMatch.h"
struct Feature{
		Mat frame;
		Mat word;
};
string genrateLabel(string lable, vector<string> list);
float geometricVerification(const struct Feature &fea1, const struct Feature &fea2);
int geoVerify1(Mat& keypt, vector<Mat>& keyptMatchs, int K)
{

	vector<float> scores;
	struct Feature fea1;
	fea1.frame=keypt.rowRange(0,3).clone();
	fea1.word=keypt.row(3).clone();
	for(int i=0;i<K;i++){
		struct Feature fea2;
		fea2.frame=keyptMatchs[i].rowRange(0,3).clone();
		fea2.word=keyptMatchs[i].row(3).clone();
		float score=geometricVerification(fea1, fea2);
		scores.push_back(score);
	}
	float maxScore=scores[0];
	int maxID=0;
	for(int i=0;i<K;i++){
		if(scores[i]>maxScore){
			maxID=i;
			maxScore=scores[i];
		}
	}
	return maxID;
}

/*
ofstream geo_result("geometryVerify_result.txt");
void geoVerify(string testPath, vector<string> & imgMatchs, int * corrGeo)
{
	Mat imgTest = imread(testPath,0);

	map<string,int> labelResultMap;
	int maxMatchNum = 0, maxMatchSingleImage = 0, difFirstSecond = 1000;
	string maxLabel, maxLabelSingleImage;

	for(unsigned int i=0;i<imgMatchs.size();i++){
		Mat imgMatch = imread(imgMatchs[i],0);
		string tmp=imgMatchs[i].substr(0,imgMatchs[i].rfind("\\"));
		int goodMatchNum = geometryVerifyMatch(imgTest,imgMatch);
		if(goodMatchNum > maxMatchSingleImage){
			maxMatchSingleImage = goodMatchNum;
			maxLabelSingleImage = tmp;
		}
		labelResultMap[tmp] += goodMatchNum;
	}

	map<string,int>::iterator it = labelResultMap.begin();
	for (;it != labelResultMap.end();++it){
		cout << it->second << " " << it->first << endl;
		if (it->second > maxMatchNum){
			difFirstSecond = it->second - maxMatchNum;
			maxMatchNum = it->second;
			maxLabel = it->first;
		}
	}
	maxLabel = difFirstSecond > 20 ? maxLabel : maxLabelSingleImage; //如果第一比第二没有绝对优势，那么选单张图像匹配数最多的店家
	cout << "maxLabel : " << maxLabel << endl;

	string genrateLabel(string lable, vector<string> list);
	string genrateTestLabel(string lable, vector<string> list);
	vector<string> testPath_list;
	string label_test=testPath.substr(0,testPath.rfind("\\"));
	string labell_test=genrateTestLabel(label_test,testPath_list);
	vector<string> matchPath_list;
	string labell_match=genrateLabel(maxLabel,matchPath_list);
	cout << "geoVerify: " << labell_test << ' ' << labell_match << endl;
	geo_result << labell_test << ' ' << labell_match << endl;
	if(labell_test==labell_match){
		(*corrGeo)++;
	}
}
*/


//
//double MatchTime=0;
//double forCostTime=0;
//double ExtFeaCostTime=0;
//
//void __stdcall MatchImg(const char *imgPath, int imgNum, int featureDim, const int *database, char** labels, int* matchNum, char*** results){
//
//	int *labelsIntArray = (int*)malloc(imgNum*sizeof(int));
//	for (int i = 0; i < imgNum; i++){
//		labelsIntArray[i] = i;
//	}
//	Mat img_obj = imread(imgPath);
//	double timestamp = GetTickCount();
//	int *feature = ImageFeatureExtract(img_obj.data, img_obj.cols, img_obj.rows, img_obj.step, featureDim);//,fp);
//	double timestamp2 = GetTickCount() - timestamp;
//	ExtFeaCostTime = ExtFeaCostTime + timestamp2/1000;
//	printf("extract feature cost %f \n",timestamp2/1000);
//
//	int *matchresult = NULL;
//	int matchRepeatNum = Matching(feature, database, labelsIntArray, featureDim, imgNum, &matchresult);//会有重复商家ID，进行筛选
//	
//	if (matchresult == NULL){
//		*matchNum = 0;
//		*results = NULL;
//		return;
//	}
//	/* 投票匹配，选取距离最小的3张图片，然后用这3张图片的 label 投票，3 个label，找到出现次数最多的label*/
//	vector<string> labels_3;
//	int voteNum = 3;
//	int topTen = matchRepeatNum >= voteNum ? voteNum : matchRepeatNum;
//	map<string,int> labelReusltMap;
//	if(matchRepeatNum>0){    //如果匹配的图片数量大于 0 ，才能进行投票，这个取决于 featureMatch.cpp 的 threshold
//		int maxMatchNum = 0;
//		string maxLabel;
//		for(int i = 0; i < topTen; i++){
//			Mat img_scene = imread( labels[matchresult[i]], CV_LOAD_IMAGE_GRAYSCALE );
//			int goodMatchNum = geometryVerifyMatch(img_obj, img_scene);
//			labelReusltMap[getShopId(labels[matchresult[i]])] += goodMatchNum;
//		}
//		map<string,int>::iterator it = labelReusltMap.begin();
//		for (;it != labelReusltMap.end();++it){
//			cout << it->second << " " << it->first << endl;
//			if (it->second > maxMatchNum){
//				maxMatchNum = it->second;
//				maxLabel = it->first;
//			}
//		}
//		ReleaseFeature(labelsIntArray);
//		ReleaseFeature(feature);
//		(*results)=(char**)malloc(1*sizeof(char*));
//
//		(*results)[0]=(char*)malloc(sizeof(char)*(maxLabel.size() + 1));
//		strcpy((*results)[0],maxLabel.c_str());
//		*matchNum = 1;
//		return;
//	}
//	else{    //没有匹配的图片
//		printf("no matching!");
//		ReleaseFeature(labelsIntArray);
//		ReleaseFeature(feature);
//		system("pause");
//		return ;
//	}
//}
//
//string getShopId(string imgPath){
//	int curSplitInd = imgPath.rfind('\\');
//	if (curSplitInd == -1)
//	{
//		return string();
//	}
//	imgPath.erase(imgPath.begin() + curSplitInd,imgPath.end());
//	int findNum = 4;
//
//	for(findNum = 0;findNum < 5 && curSplitInd != -1;findNum++)
//	{
//		imgPath.erase(imgPath.begin() + curSplitInd);
//		curSplitInd = imgPath.rfind('\\', curSplitInd);
//	}
//	if (findNum < 5)
//	{
//		return string();
//	}else
//	{
//		imgPath.erase(imgPath.begin(), imgPath.begin() + curSplitInd + 1);
//	}              
//
//	return imgPath;
//}
//
//string getTestShopId(string imgPath){
//	int splitInd1 = imgPath.rfind("\\");
//	if (splitInd1 == -1)
//	{
//		return string();
//	}
//	int splitInd2 = imgPath.rfind("\\", splitInd1 - 1);
//	if (splitInd2 == -1)
//	{
//		return string();
//	}
//	return imgPath.substr(splitInd2 + 1,splitInd1 - splitInd2 - 1);
//}

//string geoVerify1(Mat& keypt, vector<Mat>& keyptMatchs, vector<string>& imgMatchs)
//{
//	map<string, float> labelResultMap;
//	string maxLabel;
//	float maxScore;
//	struct Feature fea1;
//	fea1.frame=keypt.rowRange(0,3).clone();
//	fea1.word=keypt.row(3).clone();
//	for(unsigned int i=0;i<keyptMatchs.size();i++){
//		struct Feature fea2;
//		fea2.frame=keyptMatchs[i].rowRange(0,3).clone();
//		fea2.word=keyptMatchs[i].row(3).clone();
//		float score=geometricVerification(fea1, fea2);
//		string tmp=imgMatchs[i].substr(0,imgMatchs[i].rfind("\\"));
//		labelResultMap[tmp] += score;
//	}
//	map<string,float>::iterator it = labelResultMap.begin();
//	maxLabel = it->first;
//	maxScore = it->second;
//	for(;it!=labelResultMap.end();it++){
//		if(it->second > maxScore){
//			maxLabel = it->first;
//			maxScore = it->second;
//		}
//	}
//	vector<string> tmp_list;
//	string ShopID = genrateLabel(maxLabel,tmp_list);
//	return ShopID;
//}