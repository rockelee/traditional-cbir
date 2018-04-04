#include"ImageMatch.h"

vector<string> labels;
Mat codebook;

#define FLAGS 2    //选择 提取并存储数据库图片特征 还是 进行测试；值为 0 表示 提取并存储数据库图片特征

int main()
{
	//读入码本
	int featuredim;
	char codebook_file[]="codebook_512_jiafang_4885_201510091410.cb";
	FILE *fp = fopen(codebook_file,"rb");
	if(fp==NULL){
		printf("can't open codebook\n");
		return NULL;
	}
	int dim;
	fread(&featuredim, sizeof(int), 1, fp);
	fread(&dim,sizeof(int),1,fp);
	//fread()
	Mat codebook1(featuredim,dim,CV_32FC1);
	codebook = codebook1;
	for(int i=0;i<featuredim;i++){
		fread(codebook.ptr<float>(i),sizeof(float),dim,fp);
	}
	fclose(fp);
	char databaseFile[100];
	sprintf(databaseFile, "databasefile_%d",featuredim);
	cout << databaseFile << ' ' << featuredim << ' ' << dim << endl;
	system("pause");

#if FLAGS == 1
	//提取并存储数据库图片的特征
	char folder_db[]="F:\\jiafang_5th\\image\\420100\\";
	FeatureDatabaseSave(folder_db, databaseFile, featuredim);
#else         
	//进行匹配测试
	int *database = NULL;
	char** labelss=NULL;  
	int imgNum = 0, featureDim = 0;
	FeatureDatabaseLoad(databaseFile, &imgNum, &featureDim, &database, &labelss);   //读取数据库图片的特征
	printf("\n\n%d\t%d\n",imgNum,featureDim);
	system("pause");
	              
	char folder_test[] = "F:\\jiafang_5th\\世界城_all\\";   //测试图片路径
	string fileFolderPath = folder_test;
	vector<string> filePaths;
	findFiles(fileFolderPath, "*.jpg", filePaths); 

	char fileName_c[100];
	char pathName_c[100];
	string fileName;
	string pathName;
	  
	int correctNum1=0;  //正确匹配图片数量
	int testNum1=0;
	for(int i=0;i<filePaths.size();i++){
		cout << i << endl;
		strcpy(fileName_c,filePaths[i].c_str());

		int matchNum=0;    
		char **matchResults=NULL;      //投票匹配是，只返回一张图片的ID，也就是图片的路径
		MatchImg(fileName_c, imgNum, featureDim, database, labelss, &matchNum, &matchResults);
		string label = filePaths[i].substr(0, filePaths[i].find_last_of("\\"));
		vector<string> label_list;
		string labell = genrateTestLabel(label,label_list);
		string match=matchResults[0];
		cout << labell << ' ' << match << endl;
		if(labell==match){
			correctNum1++;
		}
		cout << endl;
	  }
	cout << "correctNum1% : " << correctNum1 <<  '/' << filePaths.size() << '=' << (float(correctNum1))/filePaths.size() <<  endl;
#endif
	system("pause");
	return 0;
}
