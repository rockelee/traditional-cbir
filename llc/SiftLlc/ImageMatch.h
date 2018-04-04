#ifndef IMAGE_MATCH_H
#define IMAGE_MATCH_H

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <io.h>
#include <Windows.h>
#include <string.h>
#include <fstream>

#include "opencv.hpp"
#include "opencv2/nonfree/nonfree.hpp"

using namespace cv;
using namespace std;

extern Mat codebook;

struct Dst
{
	int ID;
	float dist;
};

string genrateLabel(string lable, vector<string> list);
string genrateTestLabel(string lable, vector<string> list);
void findFiles(string folder, string matchMode, vector<string>& filePaths);

void __stdcall FeatureDatabaseSave(char *dir, char *databaseFile, int featuredim);
int* ImageFeatureExtract(unsigned char *Image, int width, int height, int widthstep, int featuredim);
void __stdcall FeatureDatabaseLoad(char *databaseFile, int* featureNum, int* featureDim, int** pDatabase, char*** labels);
void ReleaseFeature(int *feature);


void __stdcall MatchImg(const char *imgPath, int imgNum, int featureDim, const int *database, char** labels, int* matchNum, char*** results);
int Matching(const int *feature, const int *database, int *label, int featuredim, int imgnum, int **pMatchresult);





#endif