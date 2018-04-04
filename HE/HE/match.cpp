#include "utils.h"



void test()
{
	Mat centers;
	const char *codebookfile = "cb_1000.cb";
	readCodebook(codebookfile, centers);

	Mat P;
	readProjectMatrix(P);

	Mat T;
	readMedianMatrix(T);

	vector<struct fea> db_fea;
	vector<string> imgID;
	databaseLoad(db_fea, imgID);
	system("pause");
	
	string folder("F:\\jiafang_5th\\世界城_all");
	vector<string> filePaths;
	findFiles(folder, "*.jpg", filePaths);
	cout << filePaths.size() << endl;
	system("pause");

	int i = 0;
	int corr = 0;
	for (vector<string>::iterator iter = filePaths.begin(); iter != filePaths.end(); iter++){
		cout << i << ' ' << *iter << endl; i++;
		Mat img = imread(*iter);
		Mat sift_feature;
		siftExtract(img, sift_feature);
		vector< bitset<64> > HE;
		vector<int> NN;
		heExtract(centers, P, T, sift_feature, HE, NN);

		int K=10;//首K位命中率
		int *matchResult=new int[K];
		clock_t start_match=clock();
		Match(HE, NN, db_fea, matchResult, K);
		clock_t end_match=clock();
		cout << "match time : " << static_cast<double>(end_match - start_match)/CLOCKS_PER_SEC*1000 << "ms" << endl;

		string label_test = (*iter).substr(0, (*iter).rfind("\\"));
		vector<string> label_list;
		string test_jpgs = genrateTestLabel(label_test, label_list);

		for(int i=0;i<K;i++){
			//cout << imgID[ matchResult[i] ] << endl;
			string label_match = imgID[ matchResult[i] ].substr(0, imgID[matchResult[i]].rfind("\\"));
			vector<string> label_list;
			string match_jpgs = genrateLabel(label_match, label_list);
			
			cout << matchResult[i] << ' ' << test_jpgs << ' ' << match_jpgs << endl;
			if(strcmp(test_jpgs.c_str(), match_jpgs.c_str())==0){
				corr++;
				break;  //只需要找到一张匹配的就行了，
			}
		}
		delete [] matchResult;
	}
	cout << "corr : " << corr << endl;
	cout << "corr%: " << corr << '/' << filePaths.size() << " = " << ((float)corr)/filePaths.size() << endl;
	system("pause");
}

struct Dist{
		int ID;
		int good_match;
};
bool cmp(const struct Dist &D1, const struct Dist &D2)
{
	return D1.good_match < D2.good_match;
}
void Match(vector< bitset<64> > &HE, vector<int> &NN, vector<struct fea> &db_fea, int *matchResult, int K)
{
	int num_jpgs = db_fea.size();
	int num_test_sift = NN.size();
	
	int h_threshold = 24;

	struct Dist *dist = new struct Dist[num_jpgs];
	for(int i = 0; i < num_jpgs; i++){

		int num_match_sift = db_fea[i].NN.size();
		dist[i].ID = i;
		dist[i].good_match = 0;

		for(int j = 0; j < num_test_sift; j++){

			for(int k = 0; k < num_match_sift; k++){

				if( NN[j] == db_fea[i].NN[k] && (HE[j] ^ db_fea[i].HE[k]).count() < h_threshold  )
					dist[i].good_match++;
			}
		}
	}

	make_heap(dist,dist+num_jpgs, cmp);
	for(int i = 0; i < K; i++){
		pop_heap(dist, dist+num_jpgs - i, cmp);
		matchResult[i] = dist[num_jpgs - i - 1].ID;
	}
	delete [] dist;
}

