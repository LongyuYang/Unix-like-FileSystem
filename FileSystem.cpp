#include "FileSystem.h"
#include "Utility.h"
#include "Kernel.h"

FileSystem::FileSystem()
{

}

FileSystem::~FileSystem()
{

}

void FileSystem::LoadSuperBlock()
{
	Kernel* k = Kernel::getInstance();
	BufferManager* bufMgr = k->getBufMgr();
	SuperBlock* spb = k->getSuperBlock();
	Buf* pBuf;

	for (int i = 0; i < 2; i++)
	{
		int* p = (int *)spb + i * 128;
		pBuf = bufMgr->Bread(FileSystem::SUPER_BLOCK_SECTOR_NUMBER + i);
		Utility::copy<int>((int *)pBuf->b_addr, p, 128);
		bufMgr->Brelse(pBuf);
	}	
}

void FileSystem::Update()
{
	SuperBlock* spb = Kernel::getInstance()->getSuperBlock();
	BufferManager* bufMgr = Kernel::getInstance()->getBufMgr();
	Buf* pBuf;

	/* 同步SuperBlock到磁盘 */
	if (spb->s_fmod == 0)
		return;

	/* 清SuperBlock修改标志 */
	spb->s_fmod = 0;

	for (int i = 0; i < 2; i++)
	{
		int* p = (int *)spb + i * 128;
		pBuf = bufMgr->GetBlk(FileSystem::SUPER_BLOCK_SECTOR_NUMBER + i);
		Utility::copy<int>(p, (int *)pBuf->b_addr, 128);
		bufMgr->Bwrite(pBuf);
	}

	/* 同步修改过的内存Inode到对应外存Inode */
	InodeTable* k_InodeTable = Kernel::getInstance()->getInodeTable();
	k_InodeTable->UpdateInodeTable();

	/* 将延迟写的缓存块写到磁盘上 */
	bufMgr->Bflush();
}

Buf* FileSystem::Alloc()
{
	int blkno;	/* 分配到的空闲磁盘块编号 */
	SuperBlock* spb = Kernel::getInstance()->getSuperBlock();
	BufferManager* bufMgr = Kernel::getInstance()->getBufMgr();
	Buf* pBuf;
	Kernel* k = Kernel::getInstance();

	blkno = spb->s_free[--spb->s_nfree];

	if (blkno == 0)
	{
		spb->s_nfree = 0;
		return NULL;
	}

	if (spb->s_nfree <= 0)
	{

		/* 读入该空闲磁盘块 */
		pBuf = bufMgr->Bread(blkno);

		/* 从该磁盘块的0字节开始记录，共占据4(s_nfree)+400(s_free[100])个字节 */
		int* p = (int *)pBuf->b_addr;

		/* 首先读出空闲盘块数s_nfree */
		spb->s_nfree = *p++;

		/* 读取缓存中后续位置的数据，写入到SuperBlock空闲盘块索引表s_free[100]中 */
		Utility::copy<int>(p, spb->s_free, 100);

		bufMgr->Brelse(pBuf);
	}

	pBuf = bufMgr->GetBlk(blkno);	        /* 为该磁盘块申请缓存 */
	bufMgr->ClrBuf(pBuf);	                /* 清空缓存中的数据 */
	spb->s_fmod = 1;	                    /* 设置SuperBlock被修改标志 */

	return pBuf;
}

Inode* FileSystem::IAlloc()
{
	SuperBlock* spb = Kernel::getInstance()->getSuperBlock();
	BufferManager* bufMgr = Kernel::getInstance()->getBufMgr();
	Buf* pBuf;
	Inode* pNode;
	Kernel* k = Kernel::getInstance();
	InodeTable* k_InodeTable = Kernel::getInstance()->getInodeTable();

	int ino;	/* 分配到的空闲外存Inode编号 */
	
	/* SuperBlock直接管理的空闲Inode索引表已空，到磁盘上搜索空闲Inode。*/
	if (spb->s_ninode <= 0)
	{
		ino = -1;

		/* 依次读入磁盘Inode区中的磁盘块，搜索其中空闲外存Inode，记入空闲Inode索引表 */
		for (int i = 0; i < spb->s_isize; i++)
		{
			pBuf = bufMgr->Bread(FileSystem::INODE_ZONE_START_SECTOR + i);

			/* 获取缓冲区首址 */
			int* p = (int *)pBuf->b_addr;

			/* 检查该缓冲区中每个外存Inode，若i_mode != 0，表示已经被占用 */
			for (int j = 0; j < FileSystem::INODE_NUMBER_PER_SECTOR; j++)
			{
				ino++;
				int mode = *(p + j * sizeof(DiskInode) / sizeof(int));

				/* 该外存Inode已被占用，不能记入空闲Inode索引表 */
				if (mode != 0)
				{
					continue;
				}

				/*
				 * 如果外存inode的i_mode==0，此时并不能确定
				 * 该inode是空闲的，因为有可能是内存inode没有写到
				 * 磁盘上,所以要继续搜索内存inode中是否有相应的项
				 */
				if (k_InodeTable->IsLoaded(ino) == -1)
				{
					spb->s_inode[spb->s_ninode++] = ino;
					if (spb->s_ninode >= 100)
						break;
				}
			}

			/* 至此已读完当前磁盘块，释放相应的缓存 */
			bufMgr->Brelse(pBuf);
			if (spb->s_ninode >= 100)
				break;
		}

		if (spb->s_ninode <= 0)
			return NULL;
	}

	while (true)
	{
		ino = spb->s_inode[--spb->s_ninode];
		pNode = k_InodeTable->IGet(ino);
		
		if (pNode == NULL)
			return NULL;

		/* 如果该Inode空闲,清空Inode中的数据 */
		if (pNode->i_mode == 0)
		{
			pNode->Clean();
			/* 设置SuperBlock被修改标志 */
			spb->s_fmod = 1;
			return pNode;
		}
		else	/* 如果该Inode已被占用 */
		{
			k_InodeTable->IPut(pNode);
			continue;	/* while循环 */
		}
	}
	return NULL;	
}

void FileSystem::IFree(int number)
{
	SuperBlock* spb = Kernel::getInstance()->getSuperBlock();
	if (spb->s_ninode >= 100)
		return;
	spb->s_inode[spb->s_ninode++] = number;
	spb->s_fmod = 1;
}

void FileSystem::Free(int blkno)
{
	SuperBlock* spb = Kernel::getInstance()->getSuperBlock();
	Buf* pBuf;
	BufferManager* bufMgr = Kernel::getInstance()->getBufMgr();

	spb->s_fmod = 1;

	if (spb->s_nfree <= 0)
	{
		spb->s_nfree = 1;
		spb->s_free[0] = 0;	
	}

	if (spb->s_nfree >= 100)
	{
		pBuf =  bufMgr->GetBlk(blkno);	
		/* 从该磁盘块的0字节开始记录，共占据4(s_nfree)+400(s_free[100])个字节 */
		int* p = (int *)pBuf->b_addr;
		/* 首先写入空闲盘块数，除了第一组为99块，后续每组都是100块 */
		*p++ = spb->s_nfree;
		/* 将SuperBlock的空闲盘块索引表s_free[100]写入缓存中后续位置 */
		Utility::copy<int>(spb->s_free, p, 100);
		spb->s_nfree = 0;
		/* 将存放空闲盘块索引表的“当前释放盘块”写入磁盘，即实现了空闲盘块记录空闲盘块号的目标 */
		bufMgr->Bwrite(pBuf);
	}
	spb->s_free[spb->s_nfree++] = blkno;	/* SuperBlock中记录下当前释放盘块号 */
	spb->s_fmod = 1;
}