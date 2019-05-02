#pragma once
#ifndef OPEN_FILE_MANAGER_H
#define OPEN_FILE_MANAGER_H
#include "INode.h"
#include "OpenFileManager.h"
#include "FileSystem.h"

class InodeTable
{
public:
	static const int NINODE = 100;	/* �ڴ�Inode������ */

public:
	InodeTable();
	~InodeTable();

	Inode* IGet(int inumber); /* �������Inode��Ż�ȡ��ӦInode */
	void IPut(Inode* pNode);  /* ���ٸ��ڴ�Inode�����ü��� */
	void UpdateInodeTable();  /* �����б��޸Ĺ����ڴ�Inode���µ���Ӧ���Inode�� */

	int IsLoaded(int inumber); /* �����Ϊinumber�����inode�Ƿ����ڴ濽�� */
	Inode* GetFreeInode();     /* ���ڴ�Inode����Ѱ��һ�����е��ڴ�Inode */

public:
	Inode m_Inode[NINODE];		/* �ڴ�Inode���飬ÿ�����ļ�����ռ��һ���ڴ�Inode */
};

/*���ļ����ƿ�File�ࡣ*/
class File
{
public:
	enum FileFlags
	{
		FREAD = 0x1,			/* ���������� */
		FWRITE = 0x2,			/* д�������� */
		FPIPE = 0x4				/* �ܵ����� */
	};
public:
	File();
	~File();

	unsigned int f_flag;		/* �Դ��ļ��Ķ���д����Ҫ�� */
	int		f_count;			/* ��ǰ���ø��ļ����ƿ�Ľ������� */
	Inode*	f_inode;			/* ָ����ļ����ڴ�Inodeָ�� */
	int		f_offset;			/* �ļ���дλ��ָ�� */
};

/* ϵͳ���ļ���������Ķ��� */
class OpenFileTable
{
public:
	static const int NFILE = 100;	/* ���ļ����ƿ�File�ṹ������ */
public:
	OpenFileTable() {};
	~OpenFileTable() {};

	File* FAlloc();            /* ��ϵͳ���ļ����з���һ�����е�File�ṹ */
	void CloseF(File* pFile); /* �Դ��ļ����ƿ�File�ṹ�����ü���f_count��1. �����ü���f_countΪ0�����ͷ�File�ṹ*/
public:
	File m_File[NFILE];			/* ϵͳ���ļ��� */
};

/* �ں˴��ļ���������Ķ��� */
class OpenFiles
{
public:
	static const int NOFILES = 15;	/* ����򿪵�����ļ��� */

public:
	OpenFiles() ;
	~OpenFiles() {};

	int AllocFreeSlot();            /* ���ں˴��ļ����������з���һ�����б��� */
	File* GetF(int fd);             /* �ҵ���Ӧ�Ĵ��ļ����ƿ�File�ṹ */
	void SetF(int fd, File* pFile); /* Ϊ�ѷ��䵽�Ŀ���������fd���ѷ���Ĵ��ļ����п���File������������ϵ*/

public:
	File* k_OpenFileTable[NOFILES];		/* File�����ָ�����飬ָ��ϵͳ���ļ����е�File���� */
};


#endif
