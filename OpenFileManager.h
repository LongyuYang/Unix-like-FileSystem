#pragma once
#ifndef OPEN_FILE_MANAGER_H
#define OPEN_FILE_MANAGER_H
#include "INode.h"
#include "OpenFileManager.h"
#include "FileSystem.h"

class InodeTable
{
public:
	static const int NINODE = 100;	/* 内存Inode的数量 */

public:
	InodeTable();
	~InodeTable();

	Inode* IGet(int inumber); /* 根据外存Inode编号获取对应Inode */
	void IPut(Inode* pNode);  /* 减少该内存Inode的引用计数 */
	void UpdateInodeTable();  /* 将所有被修改过的内存Inode更新到对应外存Inode中 */

	int IsLoaded(int inumber); /* 检查编号为inumber的外存inode是否有内存拷贝 */
	Inode* GetFreeInode();     /* 在内存Inode表中寻找一个空闲的内存Inode */

public:
	Inode m_Inode[NINODE];		/* 内存Inode数组，每个打开文件都会占用一个内存Inode */
};

/*打开文件控制块File类。*/
class File
{
public:
	enum FileFlags
	{
		FREAD = 0x1,			/* 读请求类型 */
		FWRITE = 0x2,			/* 写请求类型 */
		FPIPE = 0x4				/* 管道类型 */
	};
public:
	File();
	~File();

	unsigned int f_flag;		/* 对打开文件的读、写操作要求 */
	int		f_count;			/* 当前引用该文件控制块的进程数量 */
	Inode*	f_inode;			/* 指向打开文件的内存Inode指针 */
	int		f_offset;			/* 文件读写位置指针 */
};

/* 系统打开文件描述符表的定义 */
class OpenFileTable
{
public:
	static const int NFILE = 100;	/* 打开文件控制块File结构的数量 */
public:
	OpenFileTable() {};
	~OpenFileTable() {};

	File* FAlloc();            /* 在系统打开文件表中分配一个空闲的File结构 */
	void CloseF(File* pFile); /* 对打开文件控制块File结构的引用计数f_count减1. 若引用计数f_count为0，则释放File结构*/
public:
	File m_File[NFILE];			/* 系统打开文件表 */
};

/* 内核打开文件描述符表的定义 */
class OpenFiles
{
public:
	static const int NOFILES = 15;	/* 允许打开的最大文件数 */

public:
	OpenFiles() ;
	~OpenFiles() {};

	int AllocFreeSlot();            /* 在内核打开文件描述符表中分配一个空闲表项 */
	File* GetF(int fd);             /* 找到对应的打开文件控制块File结构 */
	void SetF(int fd, File* pFile); /* 为已分配到的空闲描述符fd和已分配的打开文件表中空闲File对象建立勾连关系*/

public:
	File* k_OpenFileTable[NOFILES];		/* File对象的指针数组，指向系统打开文件表中的File对象 */
};


#endif
