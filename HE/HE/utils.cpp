#include "utils.h"


void readCodebook(const char *codebookfile, Mat &centers)
{
	FILE *fp = fopen(codebookfile, "rb");
	int num_centers;
	int dim;
	fread(&num_centers, sizeof(int), 1, fp);
	fread(&dim, sizeof(int), 1, fp);

	Mat centres(num_centers, dim, CV_32FC1);
	for(int i = 0; i < num_centers; i++){
		fread(centres.ptr<float>(i), sizeof(float), dim, fp);
	}
	fclose(fp);
	centers = centres;
	cout << "centers : " << centers.depth() << ' ' << centers.channels() << endl;
}


void readProjectMatrix(Mat &p)
{
	string file_p = "P.yml";
	FileStorage fs(file_p, FileStorage::READ);
	fs["P"] >> p;
	p.convertTo(p, CV_32FC1);
	cout << "p : " << p.depth() << ' ' << p.channels() << endl;
	fs.release();
}

void readMedianMatrix(Mat &T)
{
	FileStorage fs1("T.yml", FileStorage::READ);
	fs1["T"] >> T;
	cout << "T : " << T.depth() << ' ' << T.channels() << endl;
	fs1.release();
}

void findMedian(Mat &median, Mat &multi)
{
	for(int i = 0; i < multi.rows; i++){
		Mat tmp;
		multi.row(i).copyTo(tmp);
		float *p = (float*)tmp.data;
		sort(p, p + tmp.cols);
		float md = (tmp.cols % 2 == 0) ? (p[tmp.cols / 2 - 1] + p[tmp.cols / 2]) / 2 : p[tmp.cols / 2];
		median.push_back(md);
	}
}


void generateTMatrix()
{
	const char *codebookfile = "cb_1000.cb";
	Mat centers;
	readCodebook(codebookfile, centers);
	int num_centers = centers.rows;
	Mat matrix_p;
	readProjectMatrix(matrix_p);

	FILE *fpk = fopen("kmeansdata","rb");
	int dim, pointnum;
	fread(&dim,sizeof(int),1,fpk);
	fread(&pointnum,sizeof(int),1,fpk);

	Mat KmeansMat(pointnum, dim, CV_32FC1);
	for(int i = 0; i < pointnum; i++){
		fread(KmeansMat.ptr<float>(i), sizeof(float), dim, fpk);
	}
	fclose(fpk);

	vector<Mat> belong_centers(num_centers);
	cout << belong_centers.size() << endl
		 << belong_centers[0] << endl;


	BFMatcher matcher;
	vector<DMatch> matches;
	matcher.match(KmeansMat, centers, matches);
	cout << KmeansMat.size() << ' ' << centers.size() << ' ' << matches.size() << endl;
	for(int i = 0; i < matches.size(); i++){
		int queryIdx = matches[i].queryIdx;
		int trainIdx = matches[i].trainIdx; 
		CV_Assert( queryIdx == i );
		belong_centers[trainIdx].push_back( KmeansMat.row(queryIdx) );
	}

	vector<Mat> median(num_centers);
	for(int i = 0; i < num_centers; i++){
		cout << belong_centers[i].size() << endl;
		Mat multi;
		cout << matrix_p.depth() << ' ' << belong_centers[i].depth() << endl;
		multi = matrix_p * ( belong_centers[i].t() );
		cout << "multi.size(): " << ' ' << multi.size() << endl;
		findMedian(median[i], multi);
	}
	Mat med;
	for(int i = 0; i < num_centers; i++){
		Mat tmp = median[i].t();
		//median[i].t();
		med.push_back( tmp );
	}
	FileStorage fs1("T.yml", FileStorage::WRITE);
	fs1 << "T" << med;
	fs1.release();

}


string genrateLabel(string lable, vector<string> list)
{
	int comma_n;
	do{
		string tmp_s = "";
		comma_n = lable.find("\\");
		if(-1==comma_n){
			tmp_s = lable.substr(0,lable.length());
			list.push_back(tmp_s);
			break;
		}
		tmp_s = lable.substr(0,comma_n);
		lable.erase(0,comma_n+1);
		list.push_back(tmp_s);

	}while(true);
	string labell;
	labell+=list[3];
	labell+=list[4];
	labell+=list[5];
	labell+=list[6];
	labell+=list[7];
	return labell;
}

string genrateTestLabel(string lable, vector<string> list)
{
	int comma_n;
	do{
		string tmp_s = "";
		comma_n = lable.find("\\");
		if(-1==comma_n){
			tmp_s = lable.substr(0,lable.length());
			list.push_back(tmp_s);
			break;
		}
		tmp_s = lable.substr(0,comma_n);
		lable.erase(0,comma_n+1);
		list.push_back(tmp_s);

	}while(true);
	string labell;
	labell+=list[3];
	return labell;
}


