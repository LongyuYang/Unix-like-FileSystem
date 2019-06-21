#pragma once
#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "FileSystem.h"
#include "OpenFileManager.h"


/* �ļ�������Ķ��� */
class FileManager
{
public:
	/* Ŀ¼����ģʽ������NameI()���� */
	enum DirectorySearchMode
	{
		OPEN = 0,		/* �Դ��ļ���ʽ����Ŀ¼ */
		CREATE = 1,		/* ���½��ļ���ʽ����Ŀ¼ */
		DELETE = 2		/* ��ɾ���ļ���ʽ����Ŀ¼ */
	};

public:
	FileManager() {};
	~FileManager() {};
	Inode* NameI(char(*func)(), enum DirectorySearchMode mode); /* ·������ */
	static char NextChar();  /* ��ȡ·���е���һ���ַ� */
	void Open();			 /* ��һ���ļ� */
	void Creat();             /* �½�һ���ļ� */
	void Close();             /* �ر�һ���ļ� */
	void ChDir();             /* �ı䵱ǰ����Ŀ¼ */
	Inode* MakNode(unsigned int mode); /* �½�һ���ļ�ʱ��������Դ */
	void Open1(Inode* pInode, int mode, int trf); /* Open��Create�����Ĺ������� */
	void Read();          /* ���ļ� */
	void Write();         /* д�ļ� */
	void Seek();          /* �ı䵱ǰ��дָ���λ�� */
	void Delete();        /* ɾ���ļ� */
	void Rdwr(enum File::FileFlags mode);  /* Read��Write�����Ĺ������� */
public:
	Inode* rootDirInode; /* ��Ŀ¼�ڴ�Inode */

};

class DirectoryEntry
{
public:
	static const int DIRSIZ = 28;	/* Ŀ¼����·�����ֵ�����ַ������� */
public:
	DirectoryEntry() {};
	~DirectoryEntry() {};
public:
	int inode;		        /* Ŀ¼����Inode��Ų��� */
	char name[DIRSIZ];	    /* Ŀ¼����·�������� */
};


#endif