#pragma once
#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include "Buf.h"
#include <fstream>
#include <iostream>
#include <thread>
using namespace std;



class BufferManager
{
public:
	/* static const member */
	static const int NBUF = 15;			/* ������ƿ顢������������ */
	static const int BUFFER_SIZE = 512;	/* ��������С�� ���ֽ�Ϊ��λ */

public:
	BufferManager();
	~BufferManager();

	Buf* GetBlk(int blkno);	            /* ����һ�黺�棬���ڶ�д�ַ���blkno��*/
	void Brelse(Buf* bp);				/* �ͷŻ�����ƿ�buf */

	Buf* Bread(int blkno);	            /* ��һ�����̿飬blknoΪĿ����̿��߼���š� */

	void Bwrite(Buf* bp);				/* дһ�����̿� */
	void Bdwrite(Buf* bp);				/* �ӳ�д���̿� */
	void Bawrite(Buf* bp);				/* �첽д���̿� */
	void ClrBuf(Buf* bp);				/* ��ջ��������� */
	void Bflush();				        /* ��devָ���豸�������ӳ�д�Ļ���ȫ����������� */

private:
	void NotAvail(Buf* bp);				/* �����ɶ�����ժ��ָ���Ļ�����ƿ�buf */
	
private:
	Buf bFreeList;						/* ���ɻ�����п��ƿ飬���������ɶ��ж�ͷ*/
	Buf m_Buf[NBUF];					/* ������ƿ����� */
	Buf bDevtab;                        /* �豸���ƿ飬�������豸���ж�ͷ */
	char Buffer[NBUF][BUFFER_SIZE];	    /* ���������� */
	
};

struct writeArg                         /* �첽дʱ��Ҫ��writing�������ݵĲ��� */
{
	BufferManager* b;                   
	Buf *bp;                            /* ָ���д�Ļ���� */
	writeArg(BufferManager* b_, Buf* bp_) :
		b(b_), bp(bp_) {};
};
void writing(writeArg*);           /* ����C++�ļ���д���̿飬����Bwrite�������߳������� */

#endif
