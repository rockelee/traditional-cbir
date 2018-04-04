
#ifndef FISHER_VECTOR_H
#define FISHER_VECTOR_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "opencv2/opencv.hpp"
#include "opencv2/nonfree/nonfree.hpp"

extern "C"{
#include <generic.h>
#include <stringop.h>
#include <getopt_long.h>
#include <pgm.h>
#include <sift.h>
#include <gmm.h>
#include <fisher.h>
};

using namespace cv;
using namespace std;

//VlGMM* readGMM(float* means, float* covariances, float* priors);
VlGMM* readGMM();
void readCodebook(Mat& centres);

void siftExtract(Mat& image, Mat& feature);
void siftExtractOpenCV(Mat& image, Mat& feature);

void hesaffsiftExtractForTrain(Mat& image, Mat& feature);
void hesaffsiftExtract(Mat& image, Mat& feature, Mat& centres, Mat& keypt);

void fvExtract(VlGMM *gmm,Mat& feature,float* enc);

void findFiles(string folder, string matchMode, vector<string>& filePaths);
string genrateLabel(string lable, vector<string> list);
string genrateTestLabel(string lable, vector<string> list);

void Match(float *feature, float *featurelib, int nClusters, int dimension, int ndbJpgs, int *matchResult, int K);
int geoVerify1(Mat& keypt, vector<Mat>& keyptMatchs, int K);
//string geoVerify1(Mat& keypt, vector<Mat>& keyptMatchs, vector<string>& imgMatchs);

#endif