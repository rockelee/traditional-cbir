
#include "utils.h"
#include "hesaff.h"

extern int nClusters;
extern int dimension;

void siftExtract(Mat& image, Mat& feature)
{
	int noctaves = 4, nlevels = 2, o_min = 2;
	vl_sift_pix  *ImageData=new vl_sift_pix[image.rows*image.cols];
	//vl_sift_pix *ImageData = (float*)(image.data);
	int maxImSize = 300;
	int maxEdge = image.cols > image.rows ? image.cols : image.rows;    //resize 了两遍吧
	if (maxEdge > maxImSize){
		resize(image,image,Size(image.cols/(maxEdge/(float)maxImSize),image.rows/(maxEdge/(float)maxImSize)));
	}
	for (int i=0;i<image.rows;i++){  
		for (int j=0;j<image.cols;j++){  
			ImageData[i*image.cols+j]=(float)image.data[i*image.cols+j];  	
		}  
	} 
	VlSiftFilt *SiftFilt=NULL;
	SiftFilt=vl_sift_new(image.cols,image.rows,noctaves,nlevels,o_min);
	int KeyPoint=0;
	int idx=0;
	if (vl_sift_process_first_octave(SiftFilt,ImageData)!=VL_ERR_EOF){
		while (true){
			//计算每组中的关键点
			vl_sift_detect(SiftFilt);
			//遍历并绘制每个点
			KeyPoint+=SiftFilt->nkeys;
			//cout << SiftFilt->nkeys << endl;
			VlSiftKeypoint *pKeyPoint=SiftFilt->keys;     //
			for (int i=0;i<SiftFilt->nkeys;i++){
				VlSiftKeypoint TemptKeyPoint=*pKeyPoint;
				pKeyPoint++;
				idx++;
				//计算并遍历每个点的方向
				double angles[4];
				int angleCount=vl_sift_calc_keypoint_orientations(SiftFilt,angles,&TemptKeyPoint);
				for (int j=0;j<angleCount;j++){
					double TemptAngle=angles[j];
					//计算每个方向的描述
					float *Descriptors=new float[128];
					vl_sift_calc_keypoint_descriptor(SiftFilt,Descriptors,&TemptKeyPoint,TemptAngle);
					Mat tmp(1,128,CV_32FC1);
					int k=0;
					while(k<128){
						tmp.at<float>(0,k)=Descriptors[k];
						k++;
					}
					feature.push_back(tmp);
					delete []Descriptors;
					Descriptors=NULL;
				}
			}
				//下一阶
			if (vl_sift_process_next_octave(SiftFilt)==VL_ERR_EOF){
				break;
			}
		}
	}
	vl_sift_delete(SiftFilt);
	delete []ImageData;
	ImageData=NULL;
}


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
	//Mat feature;
	descriptor_extractor->compute(image, keypoints1, feature);
	//cout << feature.depth() << ' ' << feature.channels() << endl;
	for(int i=0;i<feature.rows;i++){    //改动的部分
		normalize(feature.row(i),feature.row(i));    //对每一行进行了归一化,缺省是L2归一化
	}
}

void hesaffsiftExtract(Mat& image, Mat& feature, Mat &centres, Mat &keypt)// vector<struct kpt> &ks)
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
	//cout << "Detected " << detector.g_numberOfPoints << " keypoints and " << detector.g_numberOfAffinePoints << " affine shapes in " << getTime()-t1 << " ms" << endl;
	for(unsigned int i=0;i<detector.keys.size();i++){
		Mat tmp(1,128,CV_8UC1,detector.keys[i].desc,0);
		tmp.convertTo(tmp,CV_32FC1);
		normalize(tmp.row(0),tmp.row(0),1.0,0.0,NORM_L2);
		feature.push_back(tmp);
	}
	cout << feature.size() << endl;
	BFMatcher matcher;
	vector<DMatch> matches;
	matcher.match(feature,centres,matches);
	//cout << matches.size() << ' ' << detector.keys.size() << endl;
	//ks.resize(matches.size());
	keypt.create(matches.size(),4,CV_32F);
	//cout << keypt.size() << ' ' << keypt.channels() << ' ' << keypt.depth() << endl;
	for(unsigned int i=0;i<detector.keys.size();i++){
		keypt.at<float>(i,0)=detector.keys[i].x;
		keypt.at<float>(i,1)=detector.keys[i].y;
		keypt.at<float>(i,2)=detector.keys[i].s;
		keypt.at<float>(i,3)=float(matches[i].trainIdx);

		//ks[i].x = detector.keys[i].x;
		//ks[i].y = detector.keys[i].y;
		//ks[i].s = detector.keys[i].s;
		//ks[i].nn = matches[i].trainIdx;

		//cout << matches[i].queryIdx << ' ' << matches[i].trainIdx << ' ' 
		//	<< detector.keys[i].x << ' ' << detector.keys[i].y << ' ' << detector.keys[i].s << '\t';
		//cout << ks[i].x << ' ' << ks[i].y << ' ' << ks[i].s << ' ' << ks[i].Index << endl;
		//
	}
	//cout << keypt << endl;
	//system("pause");
}

void vladExtract(VlKMeans *kmeans, Mat& feature, float* enc)
{
	int numData = feature.rows;
	float *vl_feature=(float*)(feature.data);
	vl_uint32 *indexes=(vl_uint32*)vl_malloc(sizeof(vl_uint32) * numData);
	float *distances=(float*)vl_malloc(sizeof(float) * numData);
	float *assignments;
	//float *enc;
	//寻找最近类
	vl_kmeans_quantize(kmeans,indexes,distances,vl_feature,numData);
	assignments=(float*)vl_malloc(sizeof(float)*numData*nClusters);
	memset(assignments, 0, sizeof(float)*numData*nClusters);
	for(int i=0;i<numData;i++){
		assignments[i*nClusters+indexes[i]]=1.;
	}
	//enc=(float*)vl_malloc(sizeof(float)*dimension*nClusters);
	vl_vlad_encode(enc,VL_TYPE_FLOAT,
			        vl_kmeans_get_centers(kmeans),dimension,nClusters,
					vl_feature, numData,
					assignments,
					0);
	
	vl_free(indexes);
	vl_free(distances);
	vl_free(assignments);
}