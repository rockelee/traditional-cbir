
#include "utils.h"

void readCodebook(Mat& centres)
{
	FileStorage codebook("codebook.yml", FileStorage::READ);
	codebook["codebook"] >> centres;
	cout << centres.size() << endl;
	cout << centres.depth() << ' ' << centres.channels() << endl;
	cout << centres.isContinuous() << ' ' << centres.step << endl;
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