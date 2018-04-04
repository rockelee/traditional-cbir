#include "ImageMatch.h"
#include <algorithm>

/*---------------------------------------------------
function:将提取的训练集的特征存成文本
input   ：dir 为训练集文件夹名
databaseFile 存储训练集特征的文件名
----------------------------------------------------*/
void __stdcall FeatureDatabaseSave(char *dir, char *databaseFile, int featuredim)
{
	string fileFolderPath = dir;
	string fileExtension = "jpg";
	string fileFolder = fileFolderPath + "*." + fileExtension;
	struct _finddata_t fileInfo;    // 文件信息结构体  
	vector<string> filePaths;
	findFiles(fileFolderPath, "*.jpg", filePaths);
	extern vector<string> labels;//

	int num = filePaths.size();
	FILE *fp1 = fopen(databaseFile, "wb");

	//写入样本集大小
	fwrite(&num, sizeof(int), 1, fp1);
	//写入特征维度大小
	fwrite(&featuredim, sizeof(int), 1, fp1);
	int i=0;
	for (vector<string>::const_iterator iter = filePaths.begin(); iter != filePaths.end(); iter++){
		//读入图像
		cout << *iter << endl;
		Mat Img = imread((*iter));
		int *feature = ImageFeatureExtract(Img.data, Img.cols, Img.rows, 0, featuredim);//,fp);
		string label =  (*iter).substr(0,(*iter).find_last_of("\\"));
		vector<string> label_list;
		string labell = genrateLabel(label, label_list);
		labels.push_back(labell);
		cout << "labell:" << labell << endl;
		fwrite(feature, sizeof(int), featuredim, fp1);
		i++;
	}

	//写入特征对应类别
	for (size_t i = 0; i < labels.size(); i++){
		fprintf(fp1, "%s\n", labels[i].c_str());
		printf("%s\n", labels[i].c_str());
	}
	fclose(fp1);
	return;
}



/*---------------------------------------------------
function:从文件中读取特征数目和维度，并将特征和对应类别存入数组
input   ：
databasefile 存储训练集特征的文件名
featureNum    特征数目
featuredim   特征维度
pDatabase	特征数组的指针
labels		特征对应标签
----------------------------------------------------*/
void __stdcall FeatureDatabaseLoad(char *databaseFile, int* featureNum, int* featureDim, int** pDatabase, char*** labels)
{
	FILE *fp1 = fopen(databaseFile, "rb");
	if (fp1 == NULL){
		fclose(fp1);
		return ;
	}
	fread(featureNum, sizeof(int), 1, fp1);
	fread(featureDim, sizeof(int), 1, fp1);
	*pDatabase = (int*)malloc(sizeof(int)*(*featureNum)*(*featureDim));
	fread(*pDatabase, sizeof(int), (*featureNum)*(*featureDim), fp1);
	(*labels) = (char**)malloc(sizeof(char*)*(*featureNum));

	//读入特征对应类别
	for (size_t i = 0; i < (*featureNum); i++){
		(*labels)[i] = (char*)malloc(sizeof(char)*256);
		fscanf(fp1, "%s\n", (*labels)[i]);
		printf("%s\n",(*labels)[i]);
	}
	fclose(fp1);
	return ;
}