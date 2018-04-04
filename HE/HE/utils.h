#ifndef UTILS_H_
#define UTILS_H_


#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <bitset>
#include <cstdio>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp> 

using namespace std;
using namespace cv;

struct fea
{
	vector< bitset<64> > HE;
	vector< int > NN;
};

void findFiles(string folder, string matchMode, vector<string>& filePaths);

void TrainCodebook();

void readCodebook(const char *codebookfile, Mat &centers);

void readProjectMatrix(Mat &p);

void readMedianMatrix(Mat &T);

void findMedian(Mat &median, Mat &multi);

void generateTMatrix();

void databaseSave();

void siftExtract(Mat &img, Mat &feature);

void heExtract(const Mat &centers, const Mat &P, const Mat &T, const Mat &feature, vector< bitset<64> > &HE, vector<int> &NN);

void databaseLoad(vector<struct fea> &, vector<string> &);

void test();

void Match(vector< bitset<64> > &HE, vector<int> &NN, vector<struct fea> &db_fea, int *matchResult, int K);

string genrateLabel(string lable, vector<string> list);

string genrateTestLabel(string lable, vector<string> list);


#endif