
#pragma once
#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "Buf.h"
#include "BufferManager.h"
#include "OpenFileManager.h"
#include "INode.h"

/* �ļ�ϵͳ�洢��Դ�����(Super Block)�Ķ��塣*/
class SuperBlock
{
public:
	SuperBlock() {};
	~SuperBlock() {};

public:
	int		s_isize;		/* ���Inode��ռ�õ��̿��� */
	int		s_fsize;		/* �̿����� */
	int		s_nfree;		/* ֱ�ӹ���Ŀ����̿����� */
	int		s_free[100];	/* ֱ�ӹ���Ŀ����̿������� */
	int		s_ninode;		/* ֱ�ӹ���Ŀ������Inode���� */
	int		s_inode[100];	/* ֱ�ӹ���Ŀ������Inode������ */
	int		s_fmod;			/* �ڴ���super block�������޸ı�־����ζ����Ҫ��������Ӧ��Super Block */
	int		padding[51];	/* ���ʹSuperBlock���С����1024�ֽڣ�ռ��2������ */
};


/*
 * �ļ�ϵͳ��(FileSystem)�����ļ��洢�豸��
 * �ĸ���洢��Դ�����̿顢���INode�ķ��䡢
 * �ͷš�
 */
class FileSystem
{
public:
	/* static consts */
	static const int NMOUNT = 5;			/* ϵͳ�����ڹ������ļ�ϵͳ��װ������� */

	static const int SUPER_BLOCK_SECTOR_NUMBER = 200;	/* ����SuperBlockλ�ڴ����ϵ������ţ�ռ��200��201���������� */

	static const int ROOTINO = 0;			/* �ļ�ϵͳ��Ŀ¼���Inode��� */

	static const int INODE_NUMBER_PER_SECTOR = 8;		/* ���INode���󳤶�Ϊ64�ֽڣ�ÿ�����̿���Դ��512/64 = 8�����Inode */
	static const int INODE_ZONE_START_SECTOR = 202;		/* ���Inode��λ�ڴ����ϵ���ʼ������ */
	static const int INODE_ZONE_SIZE = 1024 - 202;		/* ���������Inode��ռ�ݵ������� */

	static const int DATA_ZONE_START_SECTOR = 1024;		/* ����������ʼ������ */
	static const int DATA_ZONE_END_SECTOR = 18000 - 1;	/* �������Ľ��������� */
	static const int DATA_ZONE_SIZE = 18000 - DATA_ZONE_START_SECTOR;	/* ������ռ�ݵ��������� */

public:
	FileSystem();
	~FileSystem();

	void LoadSuperBlock(); 	/* ϵͳ��ʼ��ʱ����SuperBlock */
	void Update();          /* ��SuperBlock������ڴ渱�����µ��洢�豸��SuperBlock��ȥ */
	Inode* IAlloc();        /* ����һ���������INode�����ڴ����µ��ļ���*/
	void IFree(int number); /* �ͷű��Ϊnumber�����INode������ɾ���ļ�*/
	Buf* Alloc();             /* ������д��̿� */
	void Free(int blkno);     /* �ͷű��Ϊblkno�Ĵ��̿� */
};

#endif
