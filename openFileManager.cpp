#include "Kernel.h"
InodeTable::InodeTable()
{

}

InodeTable::~InodeTable()
{

}

Inode* InodeTable::IGet(int inumber)
{
	Inode* pInode;
	Kernel* k = Kernel::getInstance();
	BufferManager* bufMgr = Kernel::getInstance()->getBufMgr();
	while (true)
	{
		/* 检查指定设备dev中编号为inumber的外存Inode是否有内存拷贝 */
		int index = this->IsLoaded(inumber);
		if (index >= 0)	/* 找到内存拷贝 */
		{
			pInode = &(this->m_Inode[index]);
			pInode->i_count++;
			return pInode;
		}
		else	/* 没有Inode的内存拷贝，则分配一个空闲内存Inode */
		{
			pInode = this->GetFreeInode();
			if (pInode == NULL)
				return NULL;
			else	
			{
				pInode->i_number = inumber;
				pInode->i_count++;
				Buf* pBuf = bufMgr->Bread(FileSystem::INODE_ZONE_START_SECTOR + inumber / FileSystem::INODE_NUMBER_PER_SECTOR);
				pInode->ICopy(pBuf, inumber);
				bufMgr->Brelse(pBuf);
				return pInode;
			}
		}
	}
	return NULL;
}

void InodeTable::IPut(Inode *pNode)
{
	FileSystem* fileSys = Kernel::getInstance()->getFileSys();
	if (pNode->i_count == 1)
	{
		if (pNode->i_nlink <= 0)
		{
			pNode->ITrunc();
			pNode->i_mode = 0;
			fileSys->IFree(pNode->i_number);
		}

		pNode->IUpdate();
		pNode->i_flag = 0;
		pNode->i_number = -1;
	}
	pNode->i_count--;
}

void InodeTable::UpdateInodeTable()
{
	for (int i = 0; i < InodeTable::NINODE; i++)
	{
		if (this->m_Inode[i].i_count != 0)
			this->m_Inode[i].IUpdate();
	}
}

int InodeTable::IsLoaded(int inumber)
{
	for (int i = 0; i < InodeTable::NINODE; i++)
	{
		if (this->m_Inode[i].i_number == inumber && this->m_Inode[i].i_count != 0)
			return i;
	}
	return -1;
}

Inode* InodeTable::GetFreeInode()
{
	for (int i = 0; i < InodeTable::NINODE; i++)
	{
		if (this->m_Inode[i].i_count == 0)
			return &(this->m_Inode[i]);
	}
	return NULL;
}

File::File()
{
	this->f_count = 0;
	this->f_flag = 0;
	this->f_offset = 0;
	this->f_inode = NULL;
}

File::~File()
{

}

File* OpenFileTable::FAlloc()
{
	int fd;
	Kernel* k = Kernel::getInstance();

	/* 在内核打开文件描述符表中获取一个空闲项 */
	fd = k->getOpenFiles()->AllocFreeSlot();

	if (fd < 0)
	{
		return NULL;
	}

	for (int i = 0; i < OpenFileTable::NFILE; i++)
	{
		if (this->m_File[i].f_count == 0)
		{
			/* 建立描述符和File结构的勾连关系 */
			k->getOpenFiles()->SetF(fd, &this->m_File[i]);
			/* 增加对file结构的引用计数 */
			this->m_File[i].f_count++;
			/* 清空文件读、写位置 */
			this->m_File[i].f_offset = 0;
			return (&this->m_File[i]);
		}
	}
	return NULL;
}

void OpenFileTable::CloseF(File *pFile)
{
	/* 引用计数f_count将减为0，则释放File结构 */
	if (pFile->f_count <= 1)
		Kernel::getInstance()->getInodeTable()->IPut(pFile->f_inode);

	/* 引用当前File的进程数减1 */
	pFile->f_count--;
}

OpenFiles::OpenFiles()
{
	for (int i = 0; i < OpenFiles::NOFILES; i++)
		SetF(i, NULL);
}

int OpenFiles::AllocFreeSlot()
{
	int i;
	for (i = 0; i < OpenFiles::NOFILES; i++)
	{
		if (this->k_OpenFileTable[i] == NULL)
		{
			Kernel::getInstance()->callReturn = i;
			return i;
		}
	}
	Kernel::getInstance()->callReturn = -1;
	return -1;
}

File* OpenFiles::GetF(int fd)
{
	File* pFile;
	Kernel* k = Kernel::getInstance();
	if (fd < 0 || fd >= OpenFiles::NOFILES)
	{
		k->error = Kernel::BADF;
		return NULL;
	}
	if ((pFile = this->k_OpenFileTable[fd]) == NULL)
	{
		k->error = Kernel::BADF;
		return NULL;
	}
	return pFile;	
}

void OpenFiles::SetF(int fd, File* pFile)
{
	if (fd < 0 || fd >= OpenFiles::NOFILES)
		return;
	this->k_OpenFileTable[fd] = pFile;
}
