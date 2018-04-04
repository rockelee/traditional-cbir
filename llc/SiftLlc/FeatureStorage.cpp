#include "ImageMatch.h"
#include <algorithm>

/*---------------------------------------------------
function:����ȡ��ѵ��������������ı�
input   ��dir Ϊѵ�����ļ�����
databaseFile �洢ѵ�����������ļ���
----------------------------------------------------*/
void __stdcall FeatureDatabaseSave(char *dir, char *databaseFile, int featuredim)
{
	string fileFolderPath = dir;
	string fileExtension = "jpg";
	string fileFolder = fileFolderPath + "*." + fileExtension;
	struct _finddata_t fileInfo;    // �ļ���Ϣ�ṹ��  
	vector<string> filePaths;
	findFiles(fileFolderPath, "*.jpg", filePaths);
	extern vector<string> labels;//

	int num = filePaths.size();
	FILE *fp1 = fopen(databaseFile, "wb");

	//д����������С
	fwrite(&num, sizeof(int), 1, fp1);
	//д������ά�ȴ�С
	fwrite(&featuredim, sizeof(int), 1, fp1);
	int i=0;
	for (vector<string>::const_iterator iter = filePaths.begin(); iter != filePaths.end(); iter++){
		//����ͼ��
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

	//д��������Ӧ���
	for (size_t i = 0; i < labels.size(); i++){
		fprintf(fp1, "%s\n", labels[i].c_str());
		printf("%s\n", labels[i].c_str());
	}
	fclose(fp1);
	return;
}



/*---------------------------------------------------
function:���ļ��ж�ȡ������Ŀ��ά�ȣ����������Ͷ�Ӧ����������
input   ��
databasefile �洢ѵ�����������ļ���
featureNum    ������Ŀ
featuredim   ����ά��
pDatabase	���������ָ��
labels		������Ӧ��ǩ
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

	//����������Ӧ���
	for (size_t i = 0; i < (*featureNum); i++){
		(*labels)[i] = (char*)malloc(sizeof(char)*256);
		fscanf(fp1, "%s\n", (*labels)[i]);
		printf("%s\n",(*labels)[i]);
	}
	fclose(fp1);
	return ;
}