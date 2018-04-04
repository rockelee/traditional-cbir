#include"ImageMatch.h"

vector<string> labels;
Mat codebook;

#define FLAGS 2    //ѡ�� ��ȡ���洢���ݿ�ͼƬ���� ���� ���в��ԣ�ֵΪ 0 ��ʾ ��ȡ���洢���ݿ�ͼƬ����

int main()
{
	//�����뱾
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
	//��ȡ���洢���ݿ�ͼƬ������
	char folder_db[]="F:\\jiafang_5th\\image\\420100\\";
	FeatureDatabaseSave(folder_db, databaseFile, featuredim);
#else         
	//����ƥ�����
	int *database = NULL;
	char** labelss=NULL;  
	int imgNum = 0, featureDim = 0;
	FeatureDatabaseLoad(databaseFile, &imgNum, &featureDim, &database, &labelss);   //��ȡ���ݿ�ͼƬ������
	printf("\n\n%d\t%d\n",imgNum,featureDim);
	system("pause");
	              
	char folder_test[] = "F:\\jiafang_5th\\�����_all\\";   //����ͼƬ·��
	string fileFolderPath = folder_test;
	vector<string> filePaths;
	findFiles(fileFolderPath, "*.jpg", filePaths); 

	char fileName_c[100];
	char pathName_c[100];
	string fileName;
	string pathName;
	  
	int correctNum1=0;  //��ȷƥ��ͼƬ����
	int testNum1=0;
	for(int i=0;i<filePaths.size();i++){
		cout << i << endl;
		strcpy(fileName_c,filePaths[i].c_str());

		int matchNum=0;    
		char **matchResults=NULL;      //ͶƱƥ���ǣ�ֻ����һ��ͼƬ��ID��Ҳ����ͼƬ��·��
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
