#pragma once
#ifndef UTILITY_H
#define UTILITY_H
#include <vector>

class Utility {
public:
	template<class T>
	static void copy(T* from, T* to, int count)
	{
		while (count--)
			*to++ = *from++;
	}
	static void StringCopy(char* src, char* dst)
	{
		while ((*dst++ = *src++) != 0);
	}
	static int strlen(char* pString)
	{
		int length = 0;
		char* pChar = pString;
		while (*pChar++)
			length++;
		return length;
	}
	static vector<char*> parseCmd(char* s)
	{
		char* p = s, *q = s;
		vector<char*> result;
		while (*q != '\0')
		{
			if (*p == ' ') p++, q++;
			else
			{
				while (*q != '\0' && *q != ' ') q++;
				char* newString = new char[q - p + 1];
				for (int i = 0; i < q - p; i++) newString[i] = *(p + i);
				newString[q - p] = '\0';
				result.push_back(newString);
				p = q;
			}
		}
		return result;
	}

};

#endif 




