#include "utils.h"


void func1()
{
	string file_p = "P.yml";
	FileStorage fs(file_p, FileStorage::READ);
	char *tag=new char[10];
	Mat temp,p;
	for(int i = 0; i < 64; i++){
		sprintf(tag, "P%i", i);
		fs[tag] >> temp;
		p.push_back(temp);
	}
	fs.release();
	//cout << p << endl;
	delete [] tag;
	FileStorage fs1("P1.yml", FileStorage::WRITE);
	fs1 << "P" << p;
	fs1.release();
}

void func2()
{

}