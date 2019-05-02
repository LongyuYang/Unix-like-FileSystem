#pragma once
#ifndef INODE_H
#define INODE_H
#define min(a, b) a>b?b:a
#include "Buf.h"


/* 内存INode */
class Inode
{
public:
	/* i_flag中标志位 */
	enum INodeFlag
	{
		ILOCK = 0x1,		/* 索引节点上锁 */
		IUPD = 0x2,		    /* 内存inode被修改过，需要更新相应外存inode */
		IACC = 0x4,		    /* 内存inode被访问过，需要修改最近一次访问时间 */
		IMOUNT = 0x8,		/* 内存inode用于挂载子文件系统 */
		IWANT = 0x10,		/* 有进程正在等待该内存inode被解锁，清ILOCK标志时，要唤醒这种进程 */
		ITEXT = 0x20		/* 内存inode对应进程图像的正文段 */
	};

	/* static const member */
	static const unsigned int IALLOC = 0x8000;		/* 文件被使用 */
	static const unsigned int IFMT = 0x6000;		/* 文件类型掩码 */
	static const unsigned int IFDIR = 0x4000;		/* 文件类型：目录文件 */
	static const unsigned int IFCHR = 0x2000;		/* 字符设备特殊类型文件 */
	static const unsigned int IFBLK = 0x6000;		/* 块设备特殊类型文件，为0表示常规数据文件 */
	static const unsigned int ILARG = 0x1000;		/* 文件长度类型：大型或巨型文件 */
	static const unsigned int ISUID = 0x800;		/* 执行时文件时将用户的有效用户ID修改为文件所有者的User ID */
	static const unsigned int ISGID = 0x400;		/* 执行时文件时将用户的有效组ID修改为文件所有者的Group ID */
	static const unsigned int ISVTX = 0x200;		/* 使用后仍然位于交换区上的正文段 */
	static const unsigned int IREAD = 0x100;		/* 对文件的读权限 */
	static const unsigned int IWRITE = 0x80;		/* 对文件的写权限 */
	static const unsigned int IEXEC = 0x40;			/* 对文件的执行权限 */
	static const unsigned int IRWXU = (IREAD | IWRITE | IEXEC);		/* 文件主对文件的读、写、执行权限 */
	static const unsigned int IRWXG = ((IRWXU) >> 3);			/* 文件主同组用户对文件的读、写、执行权限 */
	static const unsigned int IRWXO = ((IRWXU) >> 6);			/* 其他用户对文件的读、写、执行权限 */

	static const int BLOCK_SIZE = 512;		/* 文件逻辑块大小: 512字节 */
	static const int ADDRESS_PER_INDEX_BLOCK = BLOCK_SIZE / sizeof(int);	/* 每个间接索引表(或索引块)包含的物理盘块号 */

	static const int SMALL_FILE_BLOCK = 6;	/* 小型文件：直接索引表最多可寻址的逻辑块号 */
	static const int LARGE_FILE_BLOCK = 128 * 2 + 6;	/* 大型文件：经一次间接索引表最多可寻址的逻辑块号 */
	static const int HUGE_FILE_BLOCK = 128 * 128 * 2 + 128 * 2 + 6;	/* 巨型文件：经二次间接索引最大可寻址文件逻辑块号 */

	static const int PIPSIZ = SMALL_FILE_BLOCK * BLOCK_SIZE;

public:

	Inode();
	~Inode() {};

	void ReadI();                   /* 根据Inode对象中的物理磁盘块索引表，读取相应的文件数据 */
	void WriteI();                  /* 根据Inode对象中的物理磁盘块索引表，将数据写入文件 */
	int Bmap(int lbn);              /* 将文件的逻辑块号转换成对应的物理盘块号 */
	void IUpdate();                 /* 更新外存Inode */
	void ITrunc();                  /* 释放Inode对应文件占用的磁盘块 */
	void Clean();                   /* 清空Inode对象中的数据 */
	void ICopy(Buf*, int);          /* 将包含外存Inode字符块中信息拷贝到内存Inode中 */

	/* Members */
public:
	unsigned int i_flag;	/* 状态的标志位，定义见enum INodeFlag */
	unsigned int i_mode;	/* 文件工作方式信息 */

	int		i_count;		/* 引用计数 */
	int		i_nlink;		/* 文件联结计数，即该文件在目录树中不同路径名的数量 */
	int		i_number;		/* 外存inode区中的编号 */

	int		i_size;			/* 文件大小，字节为单位 */
	int		i_addr[10];		/* 用于文件逻辑块号和物理块号转换的基本索引表 */
};


/* 外存索引节点(DiskINode)的定义 */
class DiskInode
{
public:
	DiskInode() {};
	~DiskInode() {};

public:
	unsigned int d_mode;	/* 状态的标志位，定义见enum INodeFlag */
	int		d_nlink;		/* 文件联结计数，即该文件在目录树中不同路径名的数量 */
	int		d_size;			/* 文件大小，字节为单位 */
	int		d_addr[10];		/* 用于文件逻辑块号和物理块号转换的基本索引表 */
	int		padding[3];			/* 填充使得DiskInode类占64个字节 */
};


#endif
