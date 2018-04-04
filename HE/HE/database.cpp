#include "utils.h"


void databaseSave()
{
	Mat centers;
	const char *codebookfile = "cb_1000.cb";
	readCodebook(codebookfile, centers);

	Mat P;
	readProjectMatrix(P);

	Mat T;
	readMedianMatrix(T);

	string folder =  "F:\\jiafang_5th\\image5th\\420100";
	vector<string> filePaths;
	findFiles(folder, "*.jpg", filePaths);
	cout << "filePaths.size() : " << filePaths.size() << endl;
	system("pause");

	ofstream fp("databsefile_HE", ios::binary);
	ofstream ofile("imgID.txt");
	if(!fp || !ofile) { cerr << "open file failed !" << endl; return ;}

	int num_jpgs = filePaths.size();
	int num_centers = centers.rows;
	int P_rows = P.rows;
	int dimension = centers.cols;

	fp.write((char*)&num_jpgs, sizeof(int));
	fp.write((char*)&num_centers, sizeof(int));
	fp.write((char*)&P_rows, sizeof(int));
	fp.write((char*)&dimension, sizeof(int));

	int i=0;
	for (vector<string>::iterator iter = filePaths.begin(); iter != filePaths.end(); iter++){
		cout << i << ' ' << *iter << endl; i++;
		Mat img = imread(*iter);
		Mat sift_feature;
		siftExtract(img, sift_feature);
		vector< bitset<64> > HE;
		vector<int> NN;
		heExtract(centers, P, T, sift_feature, HE, NN);
		int num_sift = HE.size();
		//fwrite(&num_sift, sizeof(int), 1, fp);
		//fp << HE.size() ;
		fp.write((char*)&num_sift, sizeof(int));
		for(int i = 0; i < HE.size(); i++){
			//fp << NN[i] << HE[i];
			int t1 = NN[i];
			fp.write((char*)&t1, sizeof(int));
			//cout << sizeof(HE[i]) << endl;
			fp.write((char*)&HE[i], sizeof(HE[i]));
		}
		cout << sift_feature.rows << ' ' << HE.size() << ' ' << NN.size() << endl << endl;
		ofile << *iter << endl;
	}
	fp.close();
	ofile.close();
}

void databaseLoad(vector<struct fea> &db_fea, vector<string> &imgID)
{
	ifstream ifile("databsefile_HE", ios_base::in | ios_base::binary);
	int num_centers;
	int P_rows;
	int dimension;
	int num_jpgs;

	ifile.read((char*)&num_jpgs, sizeof(int));
	ifile.read((char*)&num_centers, sizeof(int));
	ifile.read((char*)&P_rows, sizeof(int));
	ifile.read((char*)&dimension, sizeof(int));

	cout << num_jpgs << ' ' << num_centers << ' ' << P_rows << ' ' << dimension << endl;
	//system("pause");

	db_fea.resize(num_jpgs);

	for(int i = 0; i < num_jpgs; i++){
		int nn;
		bitset<64> he;
		int num_sift;
		ifile.read((char*)&num_sift, sizeof(int));
		//cout << num_sift << endl;
		for(int j = 0; j < num_sift; j++){
			ifile.read((char*)&nn, sizeof(int));
			ifile.read((char*)&he, sizeof(he));
			db_fea[i].NN.push_back(nn);
			db_fea[i].HE.push_back(he);
		}
	}
	ifile.close();
	ifstream iifile("imgID.txt");
	string tmp;
	while(iifile){
		iifile >> tmp;
		//cout << tmp << endl;
		imgID.push_back(tmp);
	}
	iifile.close();
}