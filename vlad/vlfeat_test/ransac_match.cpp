#include "ImageMatch.h"
//#include "hesaff.h"
int geometryVerifyMatch(Mat img_test,Mat img_match)
{
	if( !img_test.data || !img_match.data )
	{ std::cout<< " --(!) Error reading images " << std::endl; return 0; }

	int maxImSize = 300;
	int maxEdge = img_test.cols > img_test.rows ? img_test.cols : img_test.rows;    
	if (maxEdge > maxImSize){
		resize(img_test,img_test,Size(img_test.cols/(maxEdge/(float)maxImSize),img_test.rows/(maxEdge/(float)maxImSize)));
	}
	maxEdge = img_match.cols > img_match.rows ? img_match.cols : img_match.rows; 
	if(maxEdge > maxImSize){
		resize(img_match,img_match,Size(img_match.cols/(maxEdge/(float)maxImSize),img_match.rows/(maxEdge/(float)maxImSize)));
	}
	//-- Step 1: Detect the keypoints using SIFT Detector
	SiftFeatureDetector detector;
	vector<KeyPoint> keypoints_test, keypoints_match;
	detector.detect( img_test, keypoints_test );
	detector.detect( img_match, keypoints_match );
	//-- Step 2: Calculate descriptors (feature vectors)
	SiftDescriptorExtractor extractor;
	Mat probeDescriptors, frameDescriptors;
	extractor.compute( img_test, keypoints_test, probeDescriptors );
	extractor.compute( img_match, keypoints_match, frameDescriptors );
	//HessianAffineParams par;
	//PyramidParams p;
	//p.threshold = par.threshold;
 //        
	//AffineShapeParams ap;
	//ap.maxIterations = par.max_iter;
	//ap.patchSize = par.patch_size;
	//ap.mrSize = par.desc_factor;
 //        
	//SIFTDescriptorParams sp;
	//sp.patchSize = par.patch_size;
 //               
	//img_test.convertTo(img_test,CV_32FC1);
	//AffineHessianDetector detector_test(img_test, p, ap, sp);
	//detector_test.detectPyramidKeypoints(img_test);
	//img_match.convertTo(img_match,CV_32FC1);
	//AffineHessianDetector detector_match(img_match, p, ap, sp);
	//detector_match.detectPyramidKeypoints(img_match);

	//Mat feature_test;
	//for(unsigned int i=0;i<detector_test.keys.size();i++){
	//	Mat tmp(1,128,CV_8UC1,detector_test.keys[i].desc,0);
	//	tmp.convertTo(tmp,CV_32FC1);
	//	normalize(tmp.row(0),tmp.row(0),1.0,0.0,NORM_L2);
	//	feature_test.push_back(tmp);
	//	//normalize(probeDescriptors.row(i),probeDescriptors.row(i));
	//}
	//Mat feature_match;
	//for(unsigned int i=0;i<detector_match.keys.size();i++){
	//	Mat tmp(1,128,CV_8UC1,detector_match.keys[i].desc,0);
	//	tmp.convertTo(tmp,CV_32FC1);
	//	normalize(tmp.row(0),tmp.row(0),1.0,0.0,NORM_L2);
	//	feature_match.push_back(tmp);
	//	//normalize(frameDescriptors.row(i),frameDescriptors.row(i));
	//}
	//-- Step 3: Matching descriptor vectors using FLANN matcher
	//cout << feature_test.size() << ' ' << feature_test.type() << ' ' << feature_test.depth() << ' ' << feature_test.channels() << endl;
	//cout << feature_match.size() << ' ' << feature_match.type() << ' ' << feature_match.depth() << ' ' << feature_match.channels() << endl;
	
	BFMatcher matcher;
	vector<DMatch> matches;
	//matcher.match(feature_test, feature_match, matches);
	matcher.match(probeDescriptors, frameDescriptors, matches);

	std::vector< DMatch > good_matches = matches;
	if (good_matches.size() < 4){
		return 0;
	}
	//-- Localize the object
	std::vector<Point2f> test;
	std::vector<Point2f> match;

	//cout << probeDescriptors.size() << ' ' << frameDescriptors.size() << ' ' << good_matches.size() << endl;
	for( unsigned int j = 0; j < good_matches.size(); ++j ){
		//-- Get the keypoints from the good matches
	/*	Point2f tmp1(detector_test.keys[good_matches[j].queryIdx].x, detector_test.keys[good_matches[j].queryIdx].y);
		test.push_back(tmp1);
		Point2f tmp2(detector_match.keys[good_matches[j].trainIdx].x, detector_test.keys[good_matches[j].trainIdx].y);
		match.push_back(tmp2);*/
		//tmp1.x=detector_test.keys[good_matches[j].queryIdx];
		test.push_back( keypoints_test[ good_matches[j].queryIdx].pt );//保存测试图片的关键点
		match.push_back( keypoints_match[ good_matches[j].trainIdx ].pt);//保存数据库图片对应的关键点
	}
	//cout << "test and match points' size :" <<  test.size() << ' ' << match.size() << endl;
	Mat mask;
	Mat H = findHomography( test, match, mask, CV_RANSAC ,6.0);//Mask为N*1维，每一维表示匹配状态，1表示正常，0表示异常

	vector<DMatch>::iterator it = good_matches.begin();
	//把异常匹配删去
	for (int j = 0;j < mask.rows;++j){
		if (mask.at<uchar>(j) == 0){
			it = good_matches.erase(it);
		} 
		else{
			++it;
		}
	}
	//long t3 = getTickCount();
	//Mat matchResult;
	//drawMatches(img_object,keypoints_object,img_scene,keypoints_scene,good_matches,matchResult);
	//imshow("match result", matchResult);
	//waitKey(0);
	return good_matches.size();
}