#pragma once

#include "HuffmanTree.h"
#include <string>
#include <assert.h>

struct CharInFo
{
	CharInFo(size_t count = 0)
	:_count(count)
	{}

	bool operator==(const CharInFo& info)
	{
		return _count == info._count;
	}
	bool operator!=(const CharInFo& info)
	{
		return _count != info._count;
	}
	CharInFo operator+(const CharInFo& info)
	{
		CharInFo tmp(*this);
		tmp._count += info._count;
		return tmp;
	}
	bool operator<(const CharInFo& info)
	{
		return _count < info._count;
	}
	char _ch;
	long long _count;  //字符出现的次数
	string _strCode;   //字符对应的编码
};

//文件压缩(压缩的时候写进一些压缩信息，文件后缀，压缩信息的行数)
class FileCompress
{
	typedef HuffmanTreeNode<CharInFo>* pNode;
public:
	FileCompress()
	{
		for (size_t i = 0; i < 256; i++)
		{
			_charInfo[i]._ch = i;
			_charInfo[i]._count = 0;
		}
	}
	void GetHuffmanCode(pNode pRoot)
	{
		if (pRoot)
		{
			GetHuffmanCode(pRoot->_pLeft);
			GetHuffmanCode(pRoot->_pRight);
			if (pRoot->_pLeft == NULL&&pRoot->_pRight == NULL)
			{
				pNode cur = pRoot;
				pNode parent = pRoot->_pParent;
				string& strCode = _charInfo[cur->_weight._ch]._strCode;

				while (parent)
				{
					if (cur == parent->_pLeft)
						strCode += '0';
					else
						strCode += '1';
					cur = parent;
					parent = cur->_pParent;
				}
				reverse(strCode.begin(), strCode.end());
			}
		}
	}
	void CompressFile(const string& filePath)
	{
		//1.打开文件并读取，获取每个字符出现的次数
		FILE* fIn = fopen(filePath.c_str(), "r");
		if (fIn == NULL)
		{
			cout << "打开文件失败" << endl;
			return;
		}
		char* pReadBuff = new char[1024];
		while (1)

		{
			size_t ReadSize = fread(pReadBuff, 1, 1024, fIn);

			if (ReadSize == 0)
				break;

			for (size_t i = 0; i < ReadSize; i++)
			{
				_charInfo[pReadBuff[i]]._count++;
			}
		}
		//2.以每个字符出现的次数为权值，创建Huffman树
		HuffmanTree<CharInFo> ht(_charInfo, 256, CharInFo(0));

		//3.通过Huffman树获取每个字符对应的编码
		GetHuffmanCode(ht.GetRoot());

		//4.遍历源文件，使用每个字符的新编码重写文件
		FILE* fOut = fopen("2.txt", "w");//打开一个新文件存放压缩之后的信息
		assert(fOut);

		//先给2.txt中写入解压缩时需要的信息
		string strHeadInfo;//头部信息
		string strHeadInfo = GetFileSuffix(filePath);//文件后缀
		strHeadInfo += '\n';

		string strCodeInfo;//保存字符的信息
		char strCount[32];//把字符出现次数转换为字符串的空间
		size_t strCodeLine = 0;//头部信息的行数
		for (size_t i = 0; i < 256; i++)
		{
			if (_charInfo[i]._count != 0)
			{
				strCodeInfo += _charInfo[i]._ch;
				strCodeInfo += ',';
				itoa(_charInfo[i]._count, strCount, 10);
				strCodeInfo += strCount;
				strCodeInfo += '\n';
				strCodeLine++;
			}
		}
		itoa(strCodeLine, strCount, 0);
		strHeadInfo += strCount;
		strHeadInfo += '\n';
		strHeadInfo += strCodeInfo;

		char c = 0;//一个字节存满了之后保存进c
		char* pWriteBuff = new char[1024];
		size_t pos = 0;
		size_t writeSize = 0;

		//把刚刚已经读到文件结尾的1.txt文件重新定位到文件起始位置
		fseek(fIn, 0, SEEK_SET);
		while (1)//重新读取源文件，获取编码信息
		{
			size_t ReadSize = fread(pReadBuff, 1, 1024, fIn);
			if (ReadSize == 0)
				break;
			for (size_t i = 0; i < ReadSize; i++)
			{
				string strCode = _charInfo[pReadBuff[i]]._strCode;
				for (size_t j = 0; j < strCode.size(); j++)
				{
					c <<= 1;//左移一位
					pos++;
					if (strCode[j] == '1')//字符为1的时候放进c，为0不用管
						c |= 1;
					if (pos == 8)//移位移够8位就表示c放满了
					{
						pWriteBuff[writeSize++] = c;
						if (writeSize == 1024)//写够1024个字节就把内容写进文件
						{
							fwrite(pWriteBuff, 1, 1024, fOut);
							writeSize = 0;
						}
						pos = 0;
					}
				}
			}
		}
		if (pos != 8);//如果pos没有写满，则先把pos放进writeSize中，在写
		{
			pWriteBuff[writeSize++] = (c << 8-pos);
		}
		fwrite(pWriteBuff, 1, writeSize, fOut);

		fclose(fIn);
		fclose(fOut);

		delete[] pWriteBuff;
		delete[] pReadBuff;
	}

	//获取文件名的后缀
	string GetFileSuffix(string FilePath)
	{
		size_t pos = FilePath.find_last_of('.');//从后向前找第一个.出现的位置
		return FilePath.substr(pos);//返回一个字串
	}
	//获取文件的路径
	string GetFilePath(string FilePath)
	{
		size_t pos = FilePath.find_last_of('.');//从后向前找第一个.出现的位置
		return FilePath.substr(0, pos);
	}

	//解压文件
	void UnCompressFile(string& FilePath)
	{
		FILE* fIn = fopen(FilePath.c_str(), "r");
		assert(fIn);

		//读取后缀
		string strFileSuffix;
		ReadLine(fIn, strFileSuffix);

		//读取行的次数
		string strLineCount;
		ReadLine(fIn, strLineCount);
		size_t LineCount = atoi(strLineCount.c_str());

		//读取字符信息
		string strCodeInfo;
		for (size_t i = 0; i < LineCount; i++)
		{
			strCodeInfo = "";
			ReadLine(fIn, strCodeInfo);
			_charInfo[strCodeInfo[0]]._count = atoi(strCodeInfo.c_str() + 2);
		}

		//创建Huffman树
		HuffmanTree<CharInFo> ht(_charInfo, 256, CharInFo(0));

		//打开解压文件
		pNode pCur = ht.GetRoot();
		string compressFilePath = GetFilePath(FilePath);
		FILE* fOut = fopen(compressFilePath.c_str(), "w");
		assert(fOut);

		//开始解压
		char *ReadBuff = new char[1024];
		char *WriteBuff = new char[1024];
		size_t pos = 8;
		size_t writeCount = 0;
		size_t fileSize = pCur->_weight._count;//源文件的大小
		while (1)
		{
			size_t readSize = fread(ReadBuff, 1, 1024, fOut);
			if (readSize == 0)
				break;
			for (size_t i = 0; i < readSize; ++i)
			{
				pos = 8;
				while (pos--)
				{
					if (ReadBuff[i] & 1 << pos)
						pCur = pCur->_pRight;
					else
						pCur = pCur->_pLeft;
					if (pCur->_pLeft == NULL&&pCur->_pRight == NULL)
					{
						WriteBuff[writeCount++] = pCur->_weight._ch;
						if (writeCount == 1024)
						{
							fwrite(WriteBuff, 1, 1024, fOut);
							writeCount = 0;
						}

						if (--fileSize == 0)
						{
							fwrite(WriteBuff, 1, writeCount, fOut);
							break;
						}

						pCur = ht.GetRoot();
					}
				}
			}
		}
		fwrite(WriteBuff, 1, writeCount, fOut);

		delete[] ReadBuff;
		delete[] WriteBuff;
		fclose(fIn);
		fclose(fOut);
	}

	//一次读取一行的函数
	void ReadLine(FILE* fIn, string& strInfo)
	{
		char c = fgetc(fIn);
		if (c == EOF)
			return;
		while (c != '\n')
		{
			strInfo += c;
			c = fgetc(fIn);

			if (c == EOF)
				return;
		}
	}

private:
	CharInFo _charInfo[256];

};