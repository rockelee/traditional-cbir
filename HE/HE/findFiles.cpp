
#include <string>
#include <vector>
#include <Windows.h>
#include <io.h>

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