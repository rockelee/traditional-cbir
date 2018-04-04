#include "utils.h"

#define FLAGS  2

int nClusters = 16;
int dimension = 128;
int kmeansClusters = 1200;

Mat gmmData;
Mat kmeansData;

int main()
{
#if FLAGS == 0 //训练高斯混合模型
	string codebookFolder = "F:\\jiafang\\jiafang_5th\\image\\420100\\";
	vector<string> filePaths;
	findFiles(codebookFolder,"*.jpg",filePaths);
	cout << filePaths.size() << endl;
	system("pause");

	/*Mat gmmData;*/
	for(vector<string>::size_type i = 0; i < filePaths.size(); i++){
		cout << i << ' ' << filePaths[i] << endl;
		Mat img=imread(filePaths[i],0);
		img.convertTo(img,CV_32FC1);
		Mat feature;
		//siftExtractOpenCV(img,feature);
		hesaffsiftExtractForTrain(img,feature);
		cout << feature.size() << endl;
		gmmData.push_back(feature);
		img.release();
		feature.release();
	}
	kmeansData=gmmData;
	cout << gmmData.size() << endl;
	//system("pause");
	float* data=(float*)gmmData.data;
	int numData=gmmData.rows;
	float* means;
	float* covariances;
	float* priors;
	float* posteriors;
	double loglikelihood;

	VlGMM* gmm=vl_gmm_new(VL_TYPE_FLOAT, dimension, nClusters);
	vl_gmm_set_max_num_iterations (gmm, 100) ;
	vl_gmm_set_initialization (gmm,VlGMMRand);
	vl_gmm_cluster (gmm, data, numData);
	means=(float*)vl_gmm_get_means(gmm);
	covariances=(float*)vl_gmm_get_covariances(gmm);
	priors=(float*)vl_gmm_get_priors(gmm);
	loglikelihood=vl_gmm_get_loglikelihood(gmm);
	posteriors=(float*)vl_gmm_get_posteriors(gmm);

	FILE *fp1=fopen("gmm_means_covariances_priors_kmeans.cb","wb");
	fwrite(&nClusters,sizeof(int),1,fp1);
	fwrite(&dimension,sizeof(int),1,fp1);
	fwrite(means,sizeof(float),nClusters*dimension,fp1);
	fwrite(covariances,sizeof(float),nClusters*dimension,fp1);
	fwrite(priors,sizeof(float),nClusters,fp1);
	vl_gmm_delete(gmm);
	printf("gmm is completed!\n");

	Mat centers(kmeansClusters,dimension,CV_32FC1),labels;
	kmeans(kmeansData, kmeansClusters, labels,
               TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0),
               3, KMEANS_PP_CENTERS, centers);  //聚类3次，取结果最好的那次，聚类的初始化采用PP特定的随机算法。
	FileStorage codebook("codebook.yml",FileStorage::WRITE);
	codebook << "codebook" << centers;
	codebook.release();
	fwrite(&kmeansClusters,sizeof(int),1,fp1);
	fwrite(&dimension,sizeof(int),1,fp1);
	fwrite(centers.data,sizeof(float),kmeansClusters*dimension,fp1);
	fclose(fp1);
	printf("kmeans is completed!\n");
	printf("the end\n");

#elif FLAGS == 1  //提取特征库
	//读取gmm参数
	VlGMM* gmm = readGMM();
	cout << nClusters << ' ' << dimension << endl;
	//system("pause");
	//读取kmeans码本
	Mat centers;
	FileStorage codebook("codebook.yml", FileStorage::READ);
	codebook["codebook"] >> centers;
	cout << centers.size() << endl;
	cout << centers.depth() << ' ' << centers.channels() << endl;
	cout << centers.isContinuous() << ' ' << centers.step << endl;
	cout << "centers.size() : " << centers.size() << endl;
	//system("pause");

	//查找数据库中的所有图片,注意不满足要求的图片会在后面的提取特征部分剔除掉，
	string databaseFolder = "F:\\jiafang\\jiafang_5th\\image\\420100\\";
	vector<string> filePaths;
	findFiles(databaseFolder,"*.jpg",filePaths);
	cout << filePaths.size() << endl;
	//system("pause");

	FILE *fp2=fopen("databasefile_fv","wb");
	fseek(fp2,sizeof(int)*3,SEEK_SET);
	FileStorage fs("keypoint_coordinate_scale_nn.yml",FileStorage::WRITE);

	clock_t start=clock();
	vector<string>::iterator it;
	int k=0;
	for(it=filePaths.begin();it!=filePaths.end();it++){
		Mat img = imread(*it,0);
		equalizeHist(img,img);
		img.convertTo(img,CV_32FC1);
		Mat feature;
		Mat keypt;
		float *enc = (float*)vl_malloc(sizeof(float)*2*dimension*nClusters);
		clock_t start_ext=clock();
		hesaffsiftExtract(img,feature,centers,keypt);
		if(feature.rows < 10){
			static ofstream hesaffDbLess10("hesaffDbLess10.txt");
			hesaffDbLess10 << *it << endl;
			cout << "the problematical feature's size : " << feature.size() << endl;
			filePaths.erase(it);
			it--;
			continue;			
		}
		fvExtract(gmm,feature,enc); 
		clock_t end_ext=clock();
		cout << "fv time : " << static_cast<double>(end_ext - start_ext)/CLOCKS_PER_SEC*1000 << "ms" << endl;//
		fwrite(enc,sizeof(float),2*dimension*nClusters,fp2);  
		vl_free(enc);

		//提取特征点的坐标，尺度，以及该特征点在kmeans码本中的所属类
		//int backslash = (*it).rfind("\\");
		//int dot = (*it).rfind(".");
		//string label =  (*it).substr(0,backslash);
		//vector<string> label_list;
		//string labell = genrateLabel(label,label_list);
		//labell="a"+labell;
		//string jpgname = (*it).substr( backslash + 1, dot - backslash - 1 );
		//labell+=jpgname;
		//cout << labell << endl;
		string labell;
		char s[10];
		sprintf(s,"%d",k);
		labell=s;
		labell="a"+labell;
		//cout << labell <<endl;
		fs << labell << keypt;
		cout << k << ' ' << *it << endl;k++;
		cout << endl;
	}
	clock_t end=clock();
	cout << "all jpgs' fv time : " << static_cast<double>(end - start)/CLOCKS_PER_SEC*1000 << "ms" << endl;
	cout << "filePaths.size() : " << filePaths.size() << endl;
	//system("pause");
	vl_gmm_delete(gmm);
	for(unsigned int i=0;i<filePaths.size();i++){
		fprintf(fp2,"%s\n",filePaths[i].c_str());
		cout << filePaths[i] << endl;
	}
	fseek(fp2,0,SEEK_SET);
	int nJpgs=filePaths.size();
	fwrite(&nClusters,sizeof(int),1,fp2);
	fwrite(&dimension,sizeof(int),1,fp2);
	fwrite(&nJpgs,sizeof(int),1,fp2);
	fclose(fp2);

#else //测试
	//---------------------------------------
	//读取gmm参数
	VlGMM* gmm = readGMM();
	cout << nClusters << ' ' << dimension << endl;
	system("pause");
	//读取kmeans码本
	Mat centers;
	FileStorage codebook("codebook.yml", FileStorage::READ);
	codebook["codebook"] >> centers;
	cout << centers.size() << endl;
	cout << centers.depth() << ' ' << centers.channels() << endl;
	cout << centers.isContinuous() << ' ' << centers.step << endl;
	cout << "centers.size() : " << centers.size() << endl;
	system("pause");

	//---------------------------------------
	FILE *fp1=fopen("databasefile_fv","rb");//读取特征库，以及特征对应的图片ID
	FileStorage fs("keypoint_coordinate_scale_nn.yml",FileStorage::READ);//读取所有图片的所有特征点的坐标，尺度，以及在码本中的最近邻
	int ndbJpgs;
	fread(&nClusters,sizeof(int),1,fp1);
	fread(&dimension,sizeof(int),1,fp1);
	fread(&ndbJpgs,sizeof(int),1,fp1);

	float *featurelib=(float*)malloc(sizeof(float)*2*dimension*nClusters*ndbJpgs);
	fread(featurelib,sizeof(float),2*dimension*nClusters*ndbJpgs,fp1);
	char **imgID=(char**)malloc(sizeof(char*)*ndbJpgs);
	vector<Mat> coor_sca_nn;
	string labell;
	char s[10];
	
	for(int i=0;i<ndbJpgs;i++){
		imgID[i]=(char*)malloc(sizeof(char)*100);
		fscanf(fp1,"%s\n",imgID[i]);
		printf("%s\n",imgID[i]);
		
		Mat tmp;
		sprintf(s,"%d",i);
		labell=s;
		labell="a"+labell;
		//cout << labell << endl;
		//if(i==65)
			//system("pause");
		fs[labell] >> tmp;
		//cout << tmp.size() << endl;
		coor_sca_nn.push_back(tmp);
		tmp.release();
	}
	fclose(fp1);
	fs.release();
	cout << nClusters << ' ' << dimension << ' ' << ndbJpgs << ' ' << coor_sca_nn.size() << endl;
	system("pause");

	//-------------------------------------------
	//读取测试集并匹配
	string testFolder = "F:\\jiafang\\jiafang_5th\\世界城_all\\";
	vector<string> filePaths;
	findFiles(testFolder,"*.jpg",filePaths);
	cout << filePaths.size() << endl;
	system("pause");

	int corr=0;
	int corrGeoVer=0;
	vector<string>::iterator it;
	int k=0;
	for(it=filePaths.begin();it!=filePaths.end();it++){
		Mat img=imread(*it,0);
		equalizeHist(img,img);
		img.convertTo(img,CV_32FC1);
		Mat feature;
		Mat keypt;
		float *enc = (float*)vl_malloc(sizeof(float)*2*dimension*nClusters);
		clock_t start_ext=clock();
		hesaffsiftExtract(img,feature,centers,keypt);
		if(/*feature.rows < 10*/0){
			static ofstream hesaffTestLess10("hesaffTestLess10.txt");
			hesaffTestLess10 << *it << endl;
			filePaths.erase(it);
			it--;
			continue;
		}
		fvExtract(gmm,feature,enc);
		clock_t end_ext=clock();
		cout << "fv time : " << static_cast<double>(end_ext - start_ext)/CLOCKS_PER_SEC*1000 << "ms" << endl;

		string label=(*it).substr(0,(*it).find_last_of("\\"));
		vector<string> label_list;
		//string labell=genrateLabel(label,label_list);
		string labell=genrateTestLabel(label,label_list);

		int K=100;
		int *matchResult=(int*)malloc(sizeof(int)*K);
		clock_t start_match=clock();
		Match(enc,featurelib,nClusters,dimension,ndbJpgs,matchResult,K);
		clock_t end_match=clock();
		cout << "match time : " << static_cast<double>(end_match - start_match)/CLOCKS_PER_SEC*1000 << "ms" << endl;

		//首 K 位命中率
		for(int i=0;i<K;i++){
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
		//在首 K 位中做GV的命中率
		vector<Mat> keyptMatchs;
		vector<string> imgMatchs;
		for(int i=0;i<K;i++){
			keyptMatchs.push_back(coor_sca_nn[matchResult[i]]);
			string tmp=imgID[matchResult[i]];
			imgMatchs.push_back(tmp);
		}
		clock_t start_geo=clock();
		//string labell_match=geoVerify1(keypt,keyptMatchs,imgMatchs);
		int keyptMatchResult=geoVerify1(keypt, keyptMatchs, K);
		clock_t end_geo=clock();
		cout << "geo time : " << static_cast<double>(end_geo - start_geo)/CLOCKS_PER_SEC*1000 << "ms" << endl;
		string tmp = imgID[ matchResult[ keyptMatchResult ] ];
		tmp=tmp.substr(0,tmp.rfind("\\"));
		vector<string> tmp_list;
		string labell_match = genrateLabel(tmp,tmp_list);
		cout << "GEO : " << labell << ' ' << labell_match << endl;
		if(labell == labell_match){
			corrGeoVer++;
		}
		free(matchResult);
		vl_free(enc);
		cout << k << ' ' << *it << endl << endl;k++;
	}
	cout << "corr : " << corr << endl;
	cout << "corr%: " << corr << '/' << filePaths.size() << " = " << ((float)corr)/filePaths.size() << endl;
	cout << "corrGeoVer : " << corrGeoVer << endl;
	cout << "corrGeoVer%: " << corrGeoVer << '/' << filePaths.size() << " = " << ((float)corrGeoVer)/filePaths.size() << endl;
	vl_gmm_delete(gmm);
#endif
	system("pause");
	return 0;
}

