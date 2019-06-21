#include "Buf.h"
#include "BufferManager.h"
#include "Kernel.h"

const char* Kernel::DISK_IMG;

BufferManager::BufferManager()
{
	Buf* bp;
	this->bFreeList.av_forw = this->bFreeList.av_back = &(this->bFreeList); /* 初始化自由队列队头 */
	this->bDevtab.b_forw = this->bDevtab.b_back = &(this->bDevtab);       /* 初始化设备队列队头 */

	for(int i = 0; i < NBUF; i++)
	{
		bp = &(this->m_Buf[i]);
		bp->b_addr = this->Buffer[i];

		/* 初始化设备队列 */
		bp->b_back = &(this->bDevtab);
		bp->b_forw = this->bDevtab.b_forw;
		this->bDevtab.b_forw->b_back = bp;
		this->bDevtab.b_forw = bp;

		/* 初始化自由队列 */
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
	/* 首先在设备队列中搜索是否有相应的缓存 */
	for(bp = dp->b_forw; bp != dp; bp = bp->b_forw)
	{
		/* 不是要找的缓存，则继续 */
		if(bp->b_blkno != blkno)
			continue;

		if(bp->b_flags & Buf::B_BUSY)
		{
			bp->b_flags |= Buf::B_WANTED;
			goto loop;
		}
		/* 从自由队列中抽取出来 */
		this->NotAvail(bp);
		return bp;
	}

	/* 如果自由队列为空 */
	if(this->bFreeList.av_forw == &this->bFreeList)
	{
		this->bFreeList.b_flags |= Buf::B_WANTED;
		goto loop;
	}

	/* 取自由队列第一个空闲块 */

	bp = this->bFreeList.av_forw;
	this->NotAvail(bp);

	/* 如果该字符块是延迟写，将其异步写到磁盘上 */
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
	/* 插到自由队列队尾 */
	(this->bFreeList.av_back)->av_forw = bp;
	bp->av_back = this->bFreeList.av_back;
	bp->av_forw = &(this->bFreeList);
	this->bFreeList.av_back = bp;
	return;
}



void BufferManager::NotAvail(Buf *bp)
{

	/* 从自由队列中取出 */
	bp->av_back->av_forw = bp->av_forw;
	bp->av_forw->av_back = bp->av_back;
	/* 设置B_BUSY标志 */
	bp->b_flags |= Buf::B_BUSY;
	return;
}

Buf* BufferManager::Bread(int blkno)
{
	Buf* bp;
	bp = this->GetBlk(blkno);
	if(bp->b_flags & Buf::B_DONE)
		return bp;
	/* 没有找到相应缓存，构成I/O读请求块 */
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
	bp->b_wcount = BufferManager::BUFFER_SIZE;		/* 512字节 */
	writeArg* arg = new writeArg(this, bp);
	writing(arg);
	return;
}

void BufferManager::Bdwrite(Buf *bp)
{
	/* 置上B_DONE允许其它进程使用该磁盘块内容 */
	bp->b_flags |= (Buf::B_DELWRI | Buf::B_DONE);
	this->Brelse(bp);
	return;
}

void BufferManager::Bawrite(Buf *bp)
{
	/* 标记为异步写 */
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