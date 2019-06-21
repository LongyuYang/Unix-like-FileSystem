#pragma once
#ifndef KERNEL_H
#define KERNEL_H
#include "BufferManager.h"
#include "FileManager.h"
#include "FileSystem.h"
#include "OpenFileManager.h"
#include <iostream>
using namespace std;

struct IOParameter
{
	char* m_Base;	/* 当前读、写用户目标区域的首地址 */
	int m_Offset;	/* 当前读、写文件的字节偏移量 */
	int m_Count;	/* 当前还剩余的读、写字节数量 */
};

/* 本系统的核心类(内核)，只初始化一个实例*/
class Kernel
{
public:
	enum ERROR {
		NO_ERROR = 0,            /* 没有出错 */
		ISDIR = 1,               /* 操纵非数据文件 */
		NOTDIR = 2,               /* cd命令操纵数据文件 */
		NOENT = 3,                 /* 文件不存在 */
		BADF = 4,                  /* 文件标识fd错误 */
		NOOUTENT = 5,               /* windows外部文件不存在 */
		NOSPACE = 6                 /* 磁盘空间不足 */
	};
	Kernel();
	~Kernel();
	
	BufferManager* getBufMgr();    /* 获取内核的高速缓存管理实例 */
	FileSystem* getFileSys();      /* 获取内核的文件系统实例 */
	FileManager* getFileMgr();     /* 获取内核的文件管理实例 */
	InodeTable* getInodeTable();   /* 获取内核的内存Inode表 */
	OpenFiles* getOpenFiles();     /* 获取内核的打开文件描述符表 */
	OpenFileTable* getOpenFileTable(); /* 获取系统全局的打开文件描述符表 */
	SuperBlock* getSuperBlock();    /* 获取全局的SuperBlock内存副本*/
	static Kernel* getInstance();  /* 获取唯一的内核类实例 */
public:
	static const char* DISK_IMG; 
	IOParameter k_IOParam;
	int callReturn;               /* 记录调用函数的返回值 */
	char* dirp;			   	      /* 指向路径名的指针,用于nameI函数 */
	char* pathname;               /* 目标路径名 */
	DirectoryEntry dent;		  /* 当前目录的目录项 */
	Inode* cdir;		          /* 指向当前目录的Inode指针 */
	Inode* pdir;                  /* 指向当前目录父目录的Inode指针 */
	bool isDir;                   /* 当前操作是否针对目录文件 */
	char dbuf[DirectoryEntry::DIRSIZ];	/* 当前路径分量 */
	char curdir[128];            /* 当前完整工作目录 */
	int mode;                     /* 记录操纵文件的方式或seek的模式 */
	int fd;                       /* 记录文件标识 */
	char* buf;                    /* 指向读写的缓冲区 */
	int nbytes;                   /* 记录读写的字节数 */
	int offset;                   /* 记录Seek的读写指针位移 */
	ERROR error;                  /* 出错标识 */
private:
	static Kernel instance;      /* 唯一的内核类实例 */
	BufferManager* BufMgr;       /* 内核的高速缓存管理实例 */
	FileSystem* fileSys;         /* 内核的文件系统实例 */
	FileManager* fileMgr;        /* 内核的文件管理实例 */
	InodeTable* k_InodeTable;    /* 内核的内存Inode表 */
	OpenFileTable* s_openFiles;  /* 系统全局打开文件描述符表 */
	OpenFiles* k_openFiles;      /* 内核的打开文件描述符表 */
	SuperBlock* spb;              /* 全局的SuperBlock内存副本 */
public:
	void initialize();             /* 内核初始化 */
	void callInit();              /* 每个函数调用的初始化工作 */
	void format();               /* 格式化磁盘 */
	int open(char* pathname, int mode); /* 打开文件 */
	int create(char* pathname, int mode); /* 新建文件 */
	void mkdir(char* pathname);   /* 新建目录 */
	void cd(char* pathname);      /* 改变当前工作目录 */
	void ls();                    /* 显示当前目录下的所有文件 */
	int fread(int readFd, char* buf, int nbytes); /* 读一个文件到目标区 */
	int fwrite(int writeFd, char* buf, int nbytes); /* 根据目标区的字符写一个文件 */
	void fseek(int seekFd, int offset, int ptrname);/* 改变读写指针的位置 */
	void fdelete(char* pathname);  /* 删除文件 */
	void fmount(char* from, char* to); /* 将windows文件拷贝到磁盘某目录下 */
	int close(int fd);           /* 关闭文件 */
	void clear();                /* 系统关闭时收尾工作 */
};



#endif
