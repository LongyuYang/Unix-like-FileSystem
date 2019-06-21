#include "Buf.h"
#include "BufferManager.h"
#include "Kernel.h"

const char* Kernel::DISK_IMG;

BufferManager::BufferManager()
{
	Buf* bp;
	this->bFreeList.av_forw = this->bFreeList.av_back = &(this->bFreeList); /* ��ʼ�����ɶ��ж�ͷ */
	this->bDevtab.b_forw = this->bDevtab.b_back = &(this->bDevtab);       /* ��ʼ���豸���ж�ͷ */

	for(int i = 0; i < NBUF; i++)
	{
		bp = &(this->m_Buf[i]);
		bp->b_addr = this->Buffer[i];

		/* ��ʼ���豸���� */
		bp->b_back = &(this->bDevtab);
		bp->b_forw = this->bDevtab.b_forw;
		this->bDevtab.b_forw->b_back = bp;
		this->bDevtab.b_forw = bp;

		/* ��ʼ�����ɶ��� */
		bp->b_flags = Buf::B_BUSY;
		Brelse(bp);
	}
	return;
}

BufferManager::~BufferManager()
{
	Bflush();
}

Buf* BufferManager::GetBlk(int blkno)
{
	Buf* bp;
	Buf* dp = &this->bDevtab;
loop:
	/* �������豸�����������Ƿ�����Ӧ�Ļ��� */
	for(bp = dp->b_forw; bp != dp; bp = bp->b_forw)
	{
		/* ����Ҫ�ҵĻ��棬����� */
		if(bp->b_blkno != blkno)
			continue;

		if(bp->b_flags & Buf::B_BUSY)
		{
			bp->b_flags |= Buf::B_WANTED;
			goto loop;
		}
		/* �����ɶ����г�ȡ���� */
		this->NotAvail(bp);
		return bp;
	}

	/* ������ɶ���Ϊ�� */
	if(this->bFreeList.av_forw == &this->bFreeList)
	{
		this->bFreeList.b_flags |= Buf::B_WANTED;
		goto loop;
	}

	/* ȡ���ɶ��е�һ�����п� */

	bp = this->bFreeList.av_forw;
	this->NotAvail(bp);

	/* ������ַ������ӳ�д�������첽д�������� */
	if(bp->b_flags & Buf::B_DELWRI)
	{
		bp->b_flags |= Buf::B_ASYNC;
		this->Bwrite(bp);
		goto loop;
	}
		
	bp->b_flags = Buf::B_BUSY;
	bp->b_blkno = blkno;
	return bp;
}

void BufferManager::Brelse(Buf* bp)
{

	bp->b_flags &= ~(Buf::B_WANTED | Buf::B_BUSY | Buf::B_ASYNC);
	/* �嵽���ɶ��ж�β */
	(this->bFreeList.av_back)->av_forw = bp;
	bp->av_back = this->bFreeList.av_back;
	bp->av_forw = &(this->bFreeList);
	this->bFreeList.av_back = bp;
	return;
}



void BufferManager::NotAvail(Buf *bp)
{

	/* �����ɶ�����ȡ�� */
	bp->av_back->av_forw = bp->av_forw;
	bp->av_forw->av_back = bp->av_back;
	/* ����B_BUSY��־ */
	bp->b_flags |= Buf::B_BUSY;
	return;
}

Buf* BufferManager::Bread(int blkno)
{
	Buf* bp;
	bp = this->GetBlk(blkno);
	if(bp->b_flags & Buf::B_DONE)
		return bp;
	/* û���ҵ���Ӧ���棬����I/O������� */
	bp->b_flags |= Buf::B_READ;
	bp->b_wcount = BufferManager::BUFFER_SIZE;
	fstream f(Kernel::DISK_IMG, ios::in|ios::binary);
	f.seekg(blkno*BufferManager::BUFFER_SIZE);
	f.read(bp->b_addr, bp->b_wcount);
	f.close();
	return bp;
}


void writing(writeArg* arg)
{
	std::fstream f(Kernel::DISK_IMG, ios::in|ios::out|ios::binary);
	f.seekp(arg->bp->b_blkno*BufferManager::BUFFER_SIZE);
	f.write((char *)arg->bp->b_addr, arg->bp->b_wcount);
	f.close();
	arg->b->Brelse(arg->bp);
}

void BufferManager::Bwrite(Buf *bp)
{
	unsigned int flags;
	flags = bp->b_flags;
	bp->b_flags &= ~(Buf::B_READ | Buf::B_DONE | Buf::B_ERROR | Buf::B_DELWRI);
	bp->b_wcount = BufferManager::BUFFER_SIZE;		/* 512�ֽ� */
	writeArg* arg = new writeArg(this, bp);
	writing(arg);
	return;
}

void BufferManager::Bdwrite(Buf *bp)
{
	/* ����B_DONE������������ʹ�øô��̿����� */
	bp->b_flags |= (Buf::B_DELWRI | Buf::B_DONE);
	this->Brelse(bp);
	return;
}

void BufferManager::Bawrite(Buf *bp)
{
	/* ���Ϊ�첽д */
	bp->b_flags |= Buf::B_ASYNC;
	this->Bwrite(bp);
	return;
}

void BufferManager::Bflush()
{
loop:
	for (Buf* bp = this->bFreeList.av_forw; bp != &this->bFreeList; bp = bp->av_forw)
		if (bp->b_flags & Buf::B_DELWRI)
		{

			this->NotAvail(bp);
			this->Bwrite(bp);
			goto loop;
		}
}


void BufferManager::ClrBuf(Buf *bp)
{
	int* pInt = (int *)bp->b_addr;
	for (int i = 0; i < BufferManager::BUFFER_SIZE / sizeof(int); i++)
		pInt[i] = 0;
}