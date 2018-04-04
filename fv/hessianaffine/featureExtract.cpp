
#include "utils.h"
#include "hesaff.h"
extern int nClusters;
extern int dimension;

void siftExtractOpenCV(Mat& image, Mat& feature)
{
	int maxImSize = 300;
	int maxEdge = image.cols > image.rows ? image.cols : image.rows;    
	if (maxEdge > maxImSize){
		resize(image,image,Size(image.cols/(maxEdge/(float)maxImSize),image.rows/(maxEdge/(float)maxImSize)));
	}
	initModule_nonfree();
	Ptr<FeatureDetector> detector = FeatureDetector::create("SIFT");//创建SIFT特征检测器
	Ptr<DescriptorExtractor> descriptor_extractor = DescriptorExtractor::create("SIFT");//创建特征向量生成器
	vector<KeyPoint> keypoints1;
	detector->detect(image, keypoints1);
	descriptor_extractor->compute(image, keypoints1, feature);
	//cout << feature.depth() << ' ' << feature.channels() << endl;   //5  1

	for(int i=0;i<feature.rows;i++){    //改动的部分
		//normalize(feature.row(i),feature.row(i),1.0,0.0,NORM_L1);    //对每一行进行了归一化,缺省是L2归一化,rootsift
		//sqrt(feature.row(i),feature.row(i));
		normalize(feature.row(i),feature.row(i),1.0,0.0,NORM_L2);
	}
}
void hesaffsiftExtractForTrain(Mat& image, Mat& feature)
{
	int maxImSize = 300;
	int maxEdge = image.cols > image.rows ? image.cols : image.rows;    
	if (maxEdge > maxImSize){
		resize(image,image,Size(image.cols/(maxEdge/(float)maxImSize),image.rows/(maxEdge/(float)maxImSize)));
	}

	HessianAffineParams par;
	PyramidParams p;
	p.threshold = par.threshold;
         
	AffineShapeParams ap;
	ap.maxIterations = par.max_iter;
	ap.patchSize = par.patch_size;
	ap.mrSize = par.desc_factor;
         
	SIFTDescriptorParams sp;
	sp.patchSize = par.patch_size;
                
	AffineHessianDetector detector(image, p, ap, sp);
	detector.detectPyramidKeypoints(image);
	for(unsigned int i=0;i<detector.keys.size();i++){
		Mat tmp(1,128,CV_8UC1,detector.keys[i].desc,0);
		tmp.convertTo(tmp,CV_32FC1);
		normalize(tmp.row(0),tmp.row(0),1.0,0.0,NORM_L2);
		feature.push_back(tmp);
	}
}

void hesaffsiftExtract(Mat& image, Mat& feature, Mat& centres, Mat& keypt)
{
	int maxImSize = 300;
	int maxEdge = image.cols > image.rows ? image.cols : image.rows;    
	if (maxEdge > maxImSize){
		resize(image,image,Size(image.cols/(maxEdge/(float)maxImSize),image.rows/(maxEdge/(float)maxImSize)));
	}

	HessianAffineParams par;
	PyramidParams p;
	p.threshold = par.threshold;
         
	AffineShapeParams ap;
	ap.maxIterations = par.max_iter;
	ap.patchSize = par.patch_size;
	ap.mrSize = par.desc_factor;
         
	SIFTDescriptorParams sp;
	sp.patchSize = par.patch_size;
                
	AffineHessianDetector detector(image, p, ap, sp);
	detector.detectPyramidKeypoints(image);
	for(unsigned int i=0;i<detector.keys.size();i++){
		Mat tmp(1,128,CV_8UC1,detector.keys[i].desc,0);
		tmp.convertTo(tmp,CV_32FC1);
		normalize(tmp.row(0),tmp.row(0),1.0,0.0,NORM_L2);
		feature.push_back(tmp);
	}

	BFMatcher matcher;
	vector<DMatch> matches;
	matcher.match(feature,centres,matches);
	keypt.create(4,matches.size(),CV_32F);
	for(unsigned int i=0;i<detector.keys.size();i++){
		keypt.at<float>(0,i)=detector.keys[i].x;
		keypt.at<float>(1,i)=detector.keys[i].y;
		keypt.at<float>(2,i)=detector.keys[i].s;
		keypt.at<float>(3,i)=float(matches[i].trainIdx);
	}
}

void fvExtract(VlGMM *gmm,Mat& feature,float* enc)
{
	int numData = feature.rows;
	float *vl_feature=(float*)(feature.data);
	vl_fisher_encode(enc,VL_TYPE_FLOAT,
					vl_gmm_get_means(gmm), dimension, nClusters,
					vl_gmm_get_covariances(gmm),
					vl_gmm_get_priors(gmm),
					vl_feature, numData,
					VL_FISHER_FLAG_IMPROVED);

}