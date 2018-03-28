#define _CRT_SECURE_NO_WARNINGS 1

#include "FileCompress.h"

void FileCompressTest()
{
	FileCompress fc;
	fc.CompressFile("1.txt");
}

int main()
{
	FileCompressTest();
	system("pause");
	return 0;
}


