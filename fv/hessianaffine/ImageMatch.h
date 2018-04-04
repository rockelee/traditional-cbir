#ifndef IMAGE_MATCH_H
#define IMAGE_MATCH_H

//#define _IM_API	extern "C" _declspec(dllexport) 

#include"stdio.h"

#include "opencv.hpp"
#include "opencv2/nonfree/nonfree.hpp"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <io.h>
#include <Windows.h>
#include <string.h>
#include <fstream>

using namespace cv;
using namespace std;


int geometryVerifyMatch(Mat img_object,Mat img_scene);

string getShopId(string imgPath);
string getTestShopId(string imgPath);

#endif