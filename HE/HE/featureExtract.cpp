#include "utils.h"



void siftExtract(Mat &img, Mat &feature)
{
	initModule_nonfree();//��ʼ��ģ�飬ʹ��SIFT��SURFʱ�õ�
    Ptr<FeatureDetector> detector = FeatureDetector::create( "SIFT" );//����SIFT���������
    Ptr<DescriptorExtractor> extractor = DescriptorExtractor::create( "SIFT" );//������������������
    if( detector.empty() || extractor.empty() ){
        cout<<"fail to create detector!";
		return;
	}

	int maxImSize = 300;
	int maxEdge = img.cols > img.rows ? img.cols : img.rows;
	if (maxEdge > maxImSize){
		resize(img, img, Size(img.cols / (maxEdge / (float)maxImSize), img.rows / (maxEdge / (float)maxImSize)));
	}
	vector<KeyPoint> keypoints1;
	detector->detect( img, keypoints1);
	extractor->compute(img, keypoints1, feature);

	for(int i=0;i<feature.rows;i++){
		normalize(feature.row(i),feature.row(i));
	}
}

void heExtract(const Mat &centers, const Mat &P, const Mat &T, const Mat &feature, vector< bitset<64> > &HE, vector<int> &NN)
{
	BFMatcher matcher;
	vector<DMatch> matches;
	matcher.match(feature, centers, matches);
	for(int i = 0; i < matches.size(); i++){
		int queryIdx = matches[i].queryIdx;
		int trainIdx = matches[i].trainIdx; 
		CV_Assert( queryIdx == i );
		NN.push_back(trainIdx);
		Mat Px = P * ( feature.row(i).t() );
		bitset<64> tmp;
		for(int j = 0; j < 64; j++){
			if(Px.at<float>(j, 0) > T.at<float>(trainIdx, j))
				tmp[j] = 1;
			else
				tmp[j] = 0;
		}
		HE.push_back(tmp);
	}
}

