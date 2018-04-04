#include <map>
#include "ImageMatch.h"

ofstream geo_result("geometryVerify_result.txt");
ofstream out("debug.txt");
//void geoVerifiy1(Mat &test, Mat &match, int * corrGeo)
void geoVerify1(Mat &testKeypt, vector<Mat> &matchKeypts, int *corrGeo)
{
	for(unsigned int i=0;i<matchKeypts.size();i++){
		Mat matchKeypt = matchKeypts[i];
		//matchWords
		vector< vector<int> > matches(2);
		for(int i=0;i<testKeypt.rows;i++){
			for(int j=0;j<matchKeypt.rows;j++){
				if(testKeypt.at<float>(i,3)==matchKeypt.at<float>(j,3)){
					matches[0].push_back(i);
					matches[1].push_back(j);
					break;
				}
			}
		}
		//geometricVerification
		Mat tmp=testKeypt.colRange(0,3);
		Mat f1=tmp.clone();
		f1=f1.t();
		tmp=matchKeypt.colRange(0,3);
		Mat f2=tmp.clone();
		f2=f2.t();
		out << "f1:" << endl << f1 << endl << "f2:" << endl << f2 << endl;
		out << f1.size() << ' ' << f2.size() << endl;
		/*vector<int>*/ void geometricVerification(Mat &f1, Mat &f2, vector< vector<int> > & matches);
		geometricVerification(f1,f2,matches);
	}
}
/*vector<int>*/ void geometricVerification(Mat &f1, Mat &f2, vector< vector<int> > & matches)
{
	struct options{
		int tolerance1;
		int tolerance2;
		int tolerance3;
		int minInliers;
		int numRefinementIterations;
	};
	struct options opts={20, 20, 10, 6, 3};
	int numMatches = matches[0].size();
	vector<Mat> inliers;
	inliers.resize(numMatches);

	Mat x1(2,numMatches,CV_64F),x2(2,numMatches,CV_64F),tmp;
	for(int i=0;i<numMatches;i++){
		x1.at<double>(0,i)=f1.at<float>(0,matches[0][i]);
		x1.at<double>(1,i)=f1.at<float>(1,matches[0][i]);

		x2.at<double>(0,i)=f2.at<float>(0,matches[1][i]);
		x2.at<double>(1,i)=f2.at<float>(1,matches[1][i]);
	}
	out << "x1:" << endl <<  x1 << endl << "f1: " << endl << f1 << endl << endl
		 << "x2:" <<endl <<  x2 << endl << "f2:" << endl << f2 << endl << endl;

	Mat x1hom=x1.clone();
	Mat tmp1(1,x1.cols,CV_64F,Scalar(1));
	x1hom.push_back(tmp1);
	Mat x2hom=x2.clone();
	Mat tmp2(1,x2.cols,CV_64F,Scalar(1));
	x2hom.push_back(tmp2);
	
	out << "x1hom: " << endl << x1hom << endl << endl
		 << "x2hom:"  << endl << x2hom << endl << endl;
	Mat toAffinity(Mat &f);
	for(int i=0; i<numMatches;i++){
		for(int t=0;t<opts.numRefinementIterations;t++){
			if(t==1){
				Mat A1 = toAffinity(f1.col(matches[0][i]));
				out << "A1:" << endl << A1 << endl;
				Mat A2 = toAffinity(f1.col(matches[1][i]));
				out << "A21:" << endl << A2 << endl;
				Mat A21 = A2 * A1.inv();
				out << "A21:" << endl << A21 << endl;
				Mat x1p = A21.rowRange(0,2) * x1hom;
				out << "x1p:" << endl << x1p << endl;
				Mat tmp = A21(Rect(0,0,2,2));
				out << "tmp:" << endl << tmp << endl;
				double tol = opts.tolerance1 * sqrt(determinant(tmp));
			}
			else{
			
			}
		}
	}


}

Mat toAffinity(Mat &f)
{
	Mat A(3,3,CV_32F,Scalar(0));
	A.at<float>(0,0)=A.at<float>(1,1)=f.at<float>(0,2);
	A.at<float>(0,2)=f.at<float>(0,0);
	A.at<float>(1,2)=f.at<float>(0,1);
	return A;
}
//Mat toAffinity(struct Keypoint f)
//{
//	Mat A(3,3,CV_32F,Scalar(0));
//	//cout << A << endl;
//	//cout << A.channels() << ' ' << A.depth() << ' ' << A.isContinuous() << endl;
//	float *p = (float*)A.data;
//	p[0]=p[4]=f.s;
//	p[2]=f.x;
//	p[5]=f.y;
//	p[1]=p[3]=p[6]=p[7]=0;
//	p[8]=1.0;
//	return A;
//}

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