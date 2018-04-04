
#include "utils.h"

#define FLAGS  0  //0：训练码本；1：提取数据库vlad特征；2：匹配

int nClusters = 16; //需要聚多少类
int dimension = 128;
Mat kmeansData;

int main()
{
#if FLAGS == 0 //训练码本
	string codebookFolder = "F:\\jiafang\\jiafang_5th\\image\\420100";
	vector<string> filePaths;
	findFiles(codebookFolder, "*.jpg", filePaths);
	cout << filePaths.size() << endl;
	system("pause");
	
	/*Mat kmeansData;*/
	for(unsigned int i=0;i<filePaths.size();i++){
		cout << i << ' ' << filePaths[i] << endl;
		Mat img=imread(filePaths[i],0);
		img.convertTo(img,CV_32FC1);
		Mat feature;
		//siftExtractOpenCV(img,feature);
		hesaffsiftExtract(img,feature);
		cout << feature.size() << endl;
		kmeansData.push_back(feature);
		img.release();
		feature.release();
	}
	cout << kmeansData.size() << endl;
	Mat centers(nClusters,dimension,CV_32FC1),labels; 
	cout << centers.size() << endl;
	system("pause");
	kmeans(kmeansData, nClusters, labels,
               TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0),
               3, KMEANS_PP_CENTERS, centers);  //聚类3次，取结果最好的那次，聚类的初始化采用PP特定的随机算法。
	FileStorage codebook("codebook.yml",FileStorage::WRITE);
	codebook << "codebook" << centers;
	printf("the end\n");

#elif FLAGS == 1
	//读取码本
	Mat centers;
	readCodebook(centers);
	float *vl_centers=(float*)centers.data;
	VlKMeans *kmeans=vl_kmeans_new(VL_TYPE_FLOAT,VlDistanceL2);
	vl_kmeans_set_centers(kmeans, vl_centers, dimension, nClusters);   
	system("pause");
	//查找数据库中的所有图片
	string databaseFolder = "F:\\jiafang_5th\\image\\420100";
	vector<string> filePaths;
	findFiles(databaseFolder,"*.jpg",filePaths);
	cout << filePaths.size() << endl;
	system("pause");

	FILE *fp1 = fopen("databasefile_vlad", "wb");
	FileStorage fs("keypoint_coordinate_scale_nn.yml",FileStorage::WRITE);
	int nJpgs = filePaths.size();
	fwrite(&nClusters,sizeof(int),1,fp1);
	fwrite(&dimension,sizeof(int),1,fp1);
	fwrite(&nJpgs,sizeof(int),1,fp1);

	vector<string>::iterator it;
	//for(unsigned int i=0;i<filePaths.size();i++){
	int i=0;
	for(it=filePaths.begin();it!=filePaths.end();it++){
		cout << i << ' ' << *it << endl;i++;
		Mat img = imread(*it,0);
		img.convertTo(img,CV_32FC1);
		Mat feature;
		//vector<struct kpt> ks;
		Mat keypt;
		float *enc = (float*)vl_malloc(sizeof(float)*dimension*nClusters);

		clock_t start_ext=clock();	
		hesaffsiftExtract(img,feature,centers,keypt);
		vladExtract(kmeans,feature,enc); 
		clock_t end_ext=clock();

		cout << "vlad time : " << static_cast<double>(end_ext - start_ext)/CLOCKS_PER_SEC*1000 << "ms" << endl;

		int backslash = (*it).rfind("\\");
		int dot = (*it).rfind(".");
		string label =  (*it).substr(0,backslash);
		vector<string> label_list;
		string labell = genrateLabel(label,label_list);
		labell="a"+labell;
		string jpgname = (*it).substr( backslash + 1, dot - backslash - 1 );
		labell+=jpgname;
		cout << labell << endl;
		fs << labell << keypt;
		fwrite(enc,sizeof(float),dimension*nClusters,fp1);  //没有对vlad特征进行归一化
		vl_free(enc);
		img.release();
		feature.release();
		keypt.release();
	}
	fs.release();
	vl_kmeans_delete(kmeans);
	for(it=filePaths.begin();it!=filePaths.end();it++){
	//for(unsigned int i=0;i<filePaths.size();i++){
		//string label =  filePaths[i].substr(0,filePaths[i].find_last_of("\\"));
		//vector<string> label_list;
		//string labell = genrateLabel(label,label_list);
		//fprintf(fp1,"%s\n",labell.c_str());
		//printf("%s\n",labell.c_str());
		fprintf(fp1,"%s\n",(*it).c_str());
		printf("%s\n",(*it).c_str());
	}
	fclose(fp1);
#else 
	//读取码本，并设置好kmeans结构体
	Mat centers;
	readCodebook(centers);
	float *vl_centers=(float*)centers.data;//malloc(sizeof(float)*centers.rows*centers.cols);
	VlKMeans *kmeans=vl_kmeans_new(VL_TYPE_FLOAT,VlDistanceL2);
	vl_kmeans_set_centers(kmeans, vl_centers, dimension, nClusters);   //
	system("pause");

	//---------------------------------------
	//读取特征库，以及特征对应的图片ID
	FILE *fp1 = fopen("databasefile_vlad", "rb");
	int ndbJpgs;
	fread(&nClusters,sizeof(int),1,fp1);
	fread(&dimension,sizeof(int),1,fp1);
	fread(&ndbJpgs,sizeof(int),1,fp1);
	
	float *featurelib=(float*)malloc(sizeof(float)*dimension*nClusters*ndbJpgs);
	fread(featurelib,sizeof(float),dimension*nClusters*ndbJpgs,fp1);
	//ofstream out("vlad.txt"); 
	char **imgID=(char**)malloc(sizeof(char*)*ndbJpgs);
	for(int i=0;i<ndbJpgs;i++){
		imgID[i] = (char*)malloc(sizeof(char)*100);
		fscanf(fp1,"%s\n",imgID[i]);
		//if(i<20){
		//	out << imgID[i] << endl;
		//	for(int j=0;j<dimension*nClusters;j++){
		//		out << featurelib[i*dimension*nClusters+j] << ' '; 
		//	}
		//	out << endl << endl;
		//}
		printf("%s\n",imgID[i]);
	}
	cout << nClusters << ' ' << dimension << ' ' << ndbJpgs << endl;
	fclose(fp1);
	system("pause");
	//读取所有图片的所有特征点的坐标，尺度，以及在码本中的最近邻
	FileStorage fs("keypoint_coordinate_scale_nn.yml",FileStorage::READ);
	vector<Mat> coor_sca_nn;
	string dbFolder = "F:\\jiafang_5th\\image\\420100";
	vector<string> dbJpgsNames;
	findFiles(dbFolder, "*.jpg", dbJpgsNames);
	cout << "db jpgs' size() : " << dbJpgsNames.size() << endl;
	system("pause");
	vector<string>::iterator iter;
	for(iter=dbJpgsNames.begin();iter!=dbJpgsNames.end();iter++){
		int backslash = (*iter).rfind("\\");
		int dot = (*iter).rfind(".");
		string label =  (*iter).substr(0,backslash);
		vector<string> label_list;
		string labell = genrateLabel(label,label_list);
		labell="a"+labell;
		string jpgname = (*iter).substr( backslash + 1, dot - backslash - 1 );
		labell+=jpgname;
		cout << labell << endl;
		Mat tmp;
		fs[labell] >> tmp;
		//cout << tmp << endl;
		//system("pause");
		coor_sca_nn.push_back(tmp);
	}
	cout << coor_sca_nn.size() << endl;
	system("pause");

	//----------------------------------------
	//读取测试集并匹配
	string testFolder = "F:\\jiafang_5th\\世界城_all1";
	vector<string> filePaths;
	findFiles(testFolder,"*.jpg",filePaths);
	cout << "test jpgs'size() : " << filePaths.size() << endl;
	system("pause");

	int corr=0;
	int corrGeoVer=0;
	vector<string>::iterator it;
	int k=0;
	//ofstream out1("vlad1.txt");
	//for(unsigned int k=0;k<filePaths.size();k++){
	for(it=filePaths.begin();it!=filePaths.end();it++){
		cout << k << ' ' << *it << endl;k++;
		Mat img=imread(*it,0);
		img.convertTo(img,CV_32FC1);
		Mat feature;
		Mat keypt;
		float *enc = (float*)vl_malloc(sizeof(float)*dimension*nClusters);

		clock_t start_ext=clock();	
		hesaffsiftExtract(img,feature,centers,keypt);
		vladExtract(kmeans,feature,enc);
		clock_t end_ext=clock();
		cout << "vlad time : " << static_cast<double>(end_ext - start_ext)/CLOCKS_PER_SEC*1000 << "ms" << endl;

		//if(k<20){
		//	out1 << filePaths[k] << endl;
		//	for(int j=0;j<dimension*nClusters;j++){
		//		out1 << enc[j] << ' '; 
		//	}
		//	out1 << endl << endl;
		//}
		string label =  (*it).substr(0,(*it).rfind("\\"));
		vector<string> label_list;
		//string labell = genrateLabel(label,label_list);
		string labell = genrateTestLabel(label,label_list);

		int K=100;//首K位命中率
		int *matchResult=(int*)malloc(sizeof(int)*K);
		clock_t start_match=clock();
		Match(enc,featurelib,nClusters,dimension,ndbJpgs,matchResult,K);
		clock_t end_match=clock();
		cout << "match time : " << static_cast<double>(end_match - start_match)/CLOCKS_PER_SEC*1000 << "ms" << endl;

		for(int i=0;i<K;i++){
			//首 K 位命中率
			string tmp = imgID[matchResult[i]];
			tmp=tmp.substr(0,tmp.rfind("\\"));
			vector<string> tmp_list;
			string labell_match = genrateLabel(tmp,tmp_list);
			cout << matchResult[i] << ' ' << labell << ' ' << labell_match << endl;
			if(labell==labell_match){
				corr++;
				break;  //只需要找到一张匹配的就行了，
			}
		}
		//在首 K 位中做空间校验的命中率
		//vector<string> imgMatchs;
		//for(int i=0;i<K;i++){
		//	string tmp=imgID[matchResult[i]];
		//	imgMatchs.push_back(tmp);
		//}
		vector<Mat> matchKeypts;
		for(int i=0;i<K;i++){
			matchKeypts.push_back(coor_sca_nn[i]);
		}
		void geoVerify1(Mat &, vector<Mat> & , int *);
		geoVerify1(keypt, matchKeypts, &corrGeoVer);
		free(matchResult);
		vl_free(enc);
	}
	cout << "corr : " << corr << endl;
	cout << "corr%: " << corr << '/' << filePaths.size() << " = " << ((float)corr)/filePaths.size() << endl;
	cout << "corrGeoVer : " << corrGeoVer << endl;
	cout << "corrGeoVer%: " << corrGeoVer << '/' << filePaths.size() << " = " << ((float)corrGeoVer)/filePaths.size() << endl;
	vl_kmeans_delete(kmeans);
#endif
	system("pause");
    return 0;
}

