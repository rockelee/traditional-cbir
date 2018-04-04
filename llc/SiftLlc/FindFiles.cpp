
#include <io.h>
#include <Windows.h>
#include <string>
#include <vector>
using namespace std;

void findFiles(string folder, string matchMode, vector<string>& filePaths)
{
	if (folder.empty() || matchMode.empty())
		return;

	_finddata_t fileInfo;
	INT_PTR hFile;

	if (folder[folder.size() - 1] != '\\'){
		folder = folder + "\\";
	}

	//搜索子文件夹
	hFile = _findfirst((folder + "*").c_str(), &fileInfo);
	if (hFile == -1)
		return;

	do{
		if ((string(fileInfo.name) == ".") || (string(fileInfo.name) == "..")){
			continue;
		}
		if (fileInfo.attrib == FILE_ATTRIBUTE_DIRECTORY){
			findFiles(folder + fileInfo.name, matchMode, filePaths);
		}
	} while (_findnext(hFile, &fileInfo) == 0);

	//保存当前目录的查找结果
	hFile = _findfirst((folder + matchMode).c_str(), &fileInfo);
	if (hFile == -1)
		return;

	do{
		if ((string(fileInfo.name) == ".") || (string(fileInfo.name) == "..")){
			continue;
		}
		filePaths.push_back(folder + fileInfo.name);
	} while (_findnext(hFile, &fileInfo) == 0);
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