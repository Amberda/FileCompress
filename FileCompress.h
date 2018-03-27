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
	long long _count;  //�ַ����ֵĴ���
	string _strCode;   //�ַ���Ӧ�ı���
};

//�ļ�ѹ��(ѹ����ʱ��д��һЩѹ����Ϣ���ļ���׺��ѹ����Ϣ������)
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
		//1.���ļ�����ȡ����ȡÿ���ַ����ֵĴ���
		FILE* fIn = fopen(filePath.c_str(), "r");
		if (fIn == NULL)
		{
			cout << "���ļ�ʧ��" << endl;
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
		//2.��ÿ���ַ����ֵĴ���ΪȨֵ������Huffman��
		HuffmanTree<CharInFo> ht(_charInfo, 256, CharInFo(0));

		//3.ͨ��Huffman����ȡÿ���ַ���Ӧ�ı���
		GetHuffmanCode(ht.GetRoot());

		//4.����Դ�ļ���ʹ��ÿ���ַ����±�����д�ļ�
		FILE* fOut = fopen("2.txt", "w");//��һ�����ļ����ѹ��֮�����Ϣ
		assert(fOut);

		//�ȸ�2.txt��д���ѹ��ʱ��Ҫ����Ϣ
		string strHeadInfo;//ͷ����Ϣ
		string strHeadInfo = GetFileSuffix(filePath);//�ļ���׺
		strHeadInfo += '\n';

		string strCodeInfo;//�����ַ�����Ϣ
		char strCount[32];//���ַ����ִ���ת��Ϊ�ַ����Ŀռ�
		size_t strCodeLine = 0;//ͷ����Ϣ������
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

		char c = 0;//һ���ֽڴ�����֮�󱣴��c
		char* pWriteBuff = new char[1024];
		size_t pos = 0;
		size_t writeSize = 0;

		//�Ѹո��Ѿ������ļ���β��1.txt�ļ����¶�λ���ļ���ʼλ��
		fseek(fIn, 0, SEEK_SET);
		while (1)//���¶�ȡԴ�ļ�����ȡ������Ϣ
		{
			size_t ReadSize = fread(pReadBuff, 1, 1024, fIn);
			if (ReadSize == 0)
				break;
			for (size_t i = 0; i < ReadSize; i++)
			{
				string strCode = _charInfo[pReadBuff[i]]._strCode;
				for (size_t j = 0; j < strCode.size(); j++)
				{
					c <<= 1;//����һλ
					pos++;
					if (strCode[j] == '1')//�ַ�Ϊ1��ʱ��Ž�c��Ϊ0���ù�
						c |= 1;
					if (pos == 8)//��λ�ƹ�8λ�ͱ�ʾc������
					{
						pWriteBuff[writeSize++] = c;
						if (writeSize == 1024)//д��1024���ֽھͰ�����д���ļ�
						{
							fwrite(pWriteBuff, 1, 1024, fOut);
							writeSize = 0;
						}
						pos = 0;
					}
				}
			}
		}
		if (pos != 8);//���posû��д�������Ȱ�pos�Ž�writeSize�У���д
		{
			pWriteBuff[writeSize++] = (c << 8-pos);
		}
		fwrite(pWriteBuff, 1, writeSize, fOut);

		fclose(fIn);
		fclose(fOut);

		delete[] pWriteBuff;
		delete[] pReadBuff;
	}

	//��ȡ�ļ����ĺ�׺
	string GetFileSuffix(string FilePath)
	{
		size_t pos = FilePath.find_last_of('.');//�Ӻ���ǰ�ҵ�һ��.���ֵ�λ��
		return FilePath.substr(pos);//����һ���ִ�
	}
	//��ȡ�ļ���·��
	string GetFilePath(string FilePath)
	{
		size_t pos = FilePath.find_last_of('.');//�Ӻ���ǰ�ҵ�һ��.���ֵ�λ��
		return FilePath.substr(0, pos);
	}

	//��ѹ�ļ�
	void UnCompressFile(string& FilePath)
	{
		FILE* fIn = fopen(FilePath.c_str(), "r");
		assert(fIn);

		//��ȡ��׺
		string strFileSuffix;
		ReadLine(fIn, strFileSuffix);

		//��ȡ�еĴ���
		string strLineCount;
		ReadLine(fIn, strLineCount);
		size_t LineCount = atoi(strLineCount.c_str());

		//��ȡ�ַ���Ϣ
		string strCodeInfo;
		for (size_t i = 0; i < LineCount; i++)
		{
			strCodeInfo = "";
			ReadLine(fIn, strCodeInfo);
			_charInfo[strCodeInfo[0]]._count = atoi(strCodeInfo.c_str() + 2);
		}

		//����Huffman��
		HuffmanTree<CharInFo> ht(_charInfo, 256, CharInFo(0));

		//�򿪽�ѹ�ļ�
		pNode pCur = ht.GetRoot();
		string compressFilePath = GetFilePath(FilePath);
		FILE* fOut = fopen(compressFilePath.c_str(), "w");
		assert(fOut);

		//��ʼ��ѹ
		char *ReadBuff = new char[1024];
		char *WriteBuff = new char[1024];
		size_t pos = 8;
		size_t writeCount = 0;
		size_t fileSize = pCur->_weight._count;//Դ�ļ��Ĵ�С
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

	//һ�ζ�ȡһ�еĺ���
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