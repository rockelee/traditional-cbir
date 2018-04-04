#include "utils.h"


void TrainCodebook()
{
	string folder = "F:\\jiafang_5th\\image5th\\420100";
	const char *codebookfile = "cb_1000.cb";
	const int num_centers = 1000;

	vector<string> filePaths;
	findFiles(folder, "*.jpg", filePaths);
	cout << "filePaths.size() : " << filePaths.size() << endl;
	system("pause");

	initModule_nonfree();//��ʼ��ģ�飬ʹ��SIFT��SURFʱ�õ�
    Ptr<FeatureDetector> detector = FeatureDetector::create( "SIFT" );//����SIFT���������
    Ptr<DescriptorExtractor> extractor = DescriptorExtractor::create( "SIFT" );//������������������
    if( detector.empty() || extractor.empty() ){
        cout<<"fail to create detector!";
		return;
	}
	int dim = 0;
	int pointnum = 0;

	FILE *fpk = fopen("kmeansdata","wb+");
	fseek(fpk,sizeof(int)*2,SEEK_SET);

	int num =0;	

	int i=0;
	for (vector<string>::const_iterator iter = filePaths.begin(); iter != filePaths.end(); iter++,i++){
		cout << *iter << endl;
	    Mat originalImg = imread((*iter));
		int maxImSize = 300;
		int maxEdge = originalImg.cols > originalImg.rows?originalImg.cols: originalImg.rows;
		Mat img;
		if (maxEdge > maxImSize){
			resize(originalImg,img,Size(originalImg.cols/(maxEdge/(float)maxImSize),originalImg.rows/(maxEdge/(float)maxImSize)));
		}
		else{
			img = originalImg;
		}		
		vector<KeyPoint> keypoints1;
		detector->detect( img, keypoints1);
		printf("%d / %d : sift�� %d\n\n", i, filePaths.size(), keypoints1.size());

		Mat descriptors1;
		extractor->compute(img, keypoints1, descriptors1 );

		for(int i=0;i<descriptors1.rows;i++){
			normalize(descriptors1.row(i),descriptors1.row(i));
			fwrite(descriptors1.ptr<float>(i),sizeof(float),descriptors1.cols,fpk);
		}
		
		dim = descriptors1.cols;
		pointnum += descriptors1.rows;
	}
	
	fseek(fpk,0,SEEK_SET);
	fwrite(&dim,sizeof(int),1,fpk);
	fwrite(&pointnum,sizeof(int),1,fpk);
	fclose(fpk);

	fpk = fopen("kmeansdata","rb");
	fread(&dim,sizeof(int),1,fpk);
	fread(&pointnum,sizeof(int),1,fpk);

	Mat KmeansMat(pointnum,dim,CV_32FC1),centers(num_centers, dim, CV_32FC1), labels;
	for(int i=0;i<pointnum;i++){
		fread(KmeansMat.ptr<float>(i),sizeof(float),dim,fpk);
	}
	kmeans(KmeansMat, num_centers, labels,
               TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1),
               3, KMEANS_PP_CENTERS, centers);  //����3�Σ�ȡ�����õ��ǴΣ�����ĳ�ʼ������PP�ض�������㷨��

	FILE *fp = fopen(codebookfile,"wb+");
	fwrite(&num_centers, sizeof(int), 1, fp);
	fwrite(&dim, sizeof(int), 1, fp);

	for(int i = 0; i < num_centers; i++){
		fwrite(centers.ptr<float>(i),sizeof(float),dim,fp);
	}
	fclose(fpk);
	fclose(fp);
}