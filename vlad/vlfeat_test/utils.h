#ifndef VLAD_H
#define VLAD_H


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>

#include <string.h>
#include <fstream>

#include "opencv2/opencv.hpp"
#include "opencv2/nonfree/nonfree.hpp"

extern "C"{
#include <generic.h>
#include <stringop.h>
#include <getopt_long.h>
#include <pgm.h>
#include <sift.h>
#include <kmeans.h>
#include <vlad.h>
};

using namespace cv;
using namespace std;

struct kpt
{
	float x;
	float y;
	float s;
	int nn;
};


void readCodebook(Mat& centres);

void siftExtract(Mat& image, Mat& feature);
void siftExtractOpenCV(Mat& image, Mat& feature);
void hesaffsiftExtract(Mat& image, Mat& feature, Mat &centres, Mat &keypt); //vector<struct kpt> &ks );
void vladExtract(VlKMeans *kmeans, Mat& feature, float* enc);

void findFiles(string folder, string matchMode, vector<string>& filePaths);
string genrateLabel(string lable, vector<string> list);
string genrateTestLabel(string lable, vector<string> list);

void Match(float *feature, float *featurelib, int nClusters, int dimension, int ndbJpgs, int *matchResult, int K);
//int Match(float *feature, float *featurelib, int nClusters, int dimension, int ndbJpgs);
//void MatchK(float *enc, float *featurelib, int nClusters, int dimension, int ndbJpgs, int *matchResult,int );


#endif