#include "utils.h"
#include <algorithm>
/*
 *frame：图片里的每个关键点的坐标(x,y)及尺度(s),为3*N矩阵(N为关键点个数)
 *word:	 图片的每个关键点量化到码本中码字的序号
 *fvDescriptor:图片的fisher编码特征
 */
struct Feature {
	Mat frame;
    Mat word;
	Mat fvDescriptor;
};

/*
 *input：两张图片的特征(fea1,fea2),
 *		 程序里用到了fea1.frame, fea1.word, fea2.frame, fea2.word,
 *output:两张图片的匹配的得分。
 */
float geometricVerification(const Feature &fea1, const Feature &fea2){
	
	const Feature *p = 0;
	const Feature *q = 0;
	
	//sample
	Feature fea;
	fea.frame = Mat( 3, 800, CV_32F, Scalar::all(0.0) );
	fea.word = Mat( 1, 800, CV_32F, Scalar::all(0.0) );
	if (fea1.frame.cols > 800){
		vector<int> ra(fea1.frame.cols);
		for (size_t i = 0; i < fea1.frame.cols; i++)
			ra[i] = i;
		random_shuffle ( ra.begin(), ra.end() );
		for (size_t i = 0; i < 800; i++){
			fea.frame.ptr<float>(0)[i] = fea1.frame.ptr<float>(0)[ra[i]];
			fea.frame.ptr<float>(1)[i] = fea1.frame.ptr<float>(1)[ra[i]];
			fea.frame.ptr<float>(2)[i] = fea1.frame.ptr<float>(2)[ra[i]];
			fea.word.ptr<float>(0)[i] = fea1.word.ptr<float>(0)[ra[i]];
		}
		p = &fea; q = &fea2;
	}
	else{
		p = &fea1; q = &fea2;
	}
	
	//matchwords
	vector<vector<int>> matches(2);
	for (size_t i = 0; i < (p->word).cols; i++){
		for (size_t j = 0; j < (q->word).cols; j++){
			if ((p->word).ptr<float>(0)[i] == (q->word).ptr<float>(0)[j]){
				matches[0].push_back(i);
				matches[1].push_back(j);
				break;
			}
		}
	}
	
	//geometricVerification
	int numMatches = matches[0].size();
	if (numMatches == 0)
		return 0;
	vector<vector<int>> inliers(numMatches);
	Mat frame1(3, numMatches, CV_32F);
	Mat frame2(3, numMatches, CV_32F);
	for (size_t i = 0; i < numMatches; i++){
		frame1.ptr<float>(0)[i] = (p->frame).ptr<float>(0)[matches[0][i]];
		frame1.ptr<float>(1)[i] = (p->frame).ptr<float>(1)[matches[0][i]];
		frame1.ptr<float>(2)[i] = (p->frame).ptr<float>(2)[matches[0][i]];
		frame2.ptr<float>(0)[i] = (q->frame).ptr<float>(0)[matches[1][i]];
		frame2.ptr<float>(1)[i] = (q->frame).ptr<float>(1)[matches[1][i]];
		frame2.ptr<float>(2)[i] = (q->frame).ptr<float>(2)[matches[1][i]];
	}
	Mat x1hom = frame1.clone();
	Mat x2hom = frame2.clone();
	for (size_t i = 0; i < numMatches; i++){
		x1hom.ptr<float>(2)[i] = 1;
		x2hom.ptr<float>(2)[i] = 1;
	}
	
	Mat x1p;
	float tol;
	for (size_t m = 0; m < numMatches; ++m){
		for (size_t t = 0; t < 3; t++){
			if (t == 0){
				float m1[3][3] = { { frame1.ptr<float>(2)[m], 0, frame1.ptr<float>(0)[m] },
				{ 0, frame1.ptr<float>(2)[m], frame1.ptr<float>(1)[m] }, { 0, 0, 1 } };
				float m2[3][3] = { { frame2.ptr<float>(2)[m], 0, frame2.ptr<float>(0)[m] },
				{ 0, frame2.ptr<float>(2)[m], frame2.ptr<float>(1)[m] }, { 0, 0, 1 } };
				Mat A1 = Mat(3, 3, CV_32F, m1);
				Mat A2 = Mat(3, 3, CV_32F, m2);
				Mat A21 = A2 * A1.inv(DECOMP_SVD);
				x1p = A21.rowRange(0, 2)*x1hom;
				tol = 20 * sqrt(determinant(A21.rowRange(0, 2).colRange(0, 2)));
			}
			else{
				Mat A1(3, inliers[m].size(), CV_32F), A2(2, inliers[m].size(), CV_32F);
				for (size_t i = 0; i < inliers[m].size(); i++){
					A1.ptr<float>(0)[i] = (x1hom.ptr<float>(0)[inliers[m][i]]);
					A1.ptr<float>(1)[i] = (x1hom.ptr<float>(1)[inliers[m][i]]);
					A1.ptr<float>(2)[i] = (x1hom.ptr<float>(2)[inliers[m][i]]);
					A2.ptr<float>(0)[i] = (x2hom.ptr<float>(0)[inliers[m][i]]);
					A2.ptr<float>(1)[i] = (x2hom.ptr<float>(1)[inliers[m][i]]);
				}
				Mat A21 = A2 * A1.inv(DECOMP_SVD);
				x1p = A21 * x1hom;
				tol = 20 * sqrt(determinant(A21.rowRange(0, 2).colRange(0, 2)));
				inliers[m].clear();
			}
			Mat delta = x2hom.rowRange(0, 2) - x1p;
			for (size_t i = 0; i < numMatches; i++){
				if ((delta.ptr<float>(0)[i]*delta.ptr<float>(0)[i] + delta.ptr<float>(1)[i]*delta.ptr<float>(1)[i]) < tol*tol)
					inliers[m].push_back(i);
			}
			if (inliers[m].size() < 6)
				break;
			if (inliers[m].size() > 0.7 * numMatches)
				break;
		}
	}

	vector<int> score(numMatches);
	for (size_t i = 0; i < numMatches; i++){
		score[i] = inliers[i].size();
	}
	int best = (int)(max_element(score.begin(), score.end()) - score.begin());
	
	return (float)inliers[best].size()/(p->frame).cols;
}