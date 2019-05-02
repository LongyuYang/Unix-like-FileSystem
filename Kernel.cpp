#include "Kernel.h"
#include "Utility.h"
#include <fstream>

Kernel Kernel::instance;

Kernel::Kernel()
{
	Kernel::DISK_IMG = "myDisk.img";
	this->BufMgr = new BufferManager;
	this->fileSys = new FileSystem;
	this->fileMgr = new FileManager;
	this->k_InodeTable = new InodeTable;
	this->s_openFiles = new OpenFileTable;
	this->k_openFiles = new OpenFiles;
	this->spb = new SuperBlock;
}

Kernel::~Kernel()
{
}

void Kernel::clear()
{		
	delete this->BufMgr;
	delete this->fileSys;
	delete this->fileMgr;
	delete this->k_InodeTable;
	delete this->s_openFiles;
	delete this->k_openFiles;
	delete this->spb;
}

void Kernel::initialize()
{
	this->fileSys->LoadSuperBlock();
	this->fileMgr->rootDirInode = this->k_InodeTable->IGet(FileSystem::ROOTINO);
	this->cdir = this->fileMgr->rootDirInode;
	this->callReturn = -1;
	Utility::StringCopy("/", this->curdir);
}

void Kernel::callInit()
{
	this->fileMgr->rootDirInode = this->k_InodeTable->IGet(FileSystem::ROOTINO);
	this->callReturn = -1;
	this->error = NO_ERROR;
}

Kernel* Kernel::getInstance()
{
	return &instance;
}

BufferManager* Kernel::getBufMgr()
{
	return this->BufMgr;
}

FileSystem* Kernel::getFileSys()
{
	return this->fileSys;
}

FileManager* Kernel::getFileMgr()
{
	return this->fileMgr;
}

InodeTable* Kernel::getInodeTable()
{
	return this->k_InodeTable;
}

OpenFiles* Kernel::getOpenFiles()
{
	return this->k_openFiles;
}

OpenFileTable* Kernel::getOpenFileTable()
{
	return this->s_openFiles;
}

SuperBlock* Kernel::getSuperBlock()
{
	return this->spb;
}


void Kernel::format()
{
	/* 格式化磁盘 */
	fstream f(Kernel::DISK_IMG, ios::out | ios::binary);
	for (int i = 0; i <= this->getFileSys()->DATA_ZONE_END_SECTOR; i++)
		for (int j = 0; j < this->getBufMgr()->BUFFER_SIZE; j++)
			f.write((char*)" ", 1);
	f.close();
	/* 格式化SuperBlock */
	Buf* pBuf;
	SuperBlock spb;
	spb.s_isize = FileSystem::INODE_ZONE_SIZE;
	spb.s_fsize = FileSystem::DATA_ZONE_SIZE;
	spb.s_nfree = spb.s_ninode = 100;
	spb.s_fmod = 0;
	for (int i = 0; i < 100; i++)
	{
		spb.s_inode[99-i] = i + 1;
		spb.s_free[99-i] = FileSystem::DATA_ZONE_START_SECTOR + i;
	}
	for (int i = 0; i < 2; i++)
	{
		int* p = (int *)&spb + i * 128;
		pBuf = this->BufMgr->GetBlk(FileSystem::SUPER_BLOCK_SECTOR_NUMBER + i);
		Utility::copy<int>(p, (int *)pBuf->b_addr, 128);
		this->BufMgr->Bwrite(pBuf);
	}
	/* 格式化Inode区 */
	for (int i = 0; i < FileSystem::INODE_ZONE_SIZE; i++)
	{
		pBuf = this->BufMgr->GetBlk(FileSystem::ROOTINO + FileSystem::INODE_ZONE_START_SECTOR + i);
		DiskInode DiskInode[FileSystem::INODE_NUMBER_PER_SECTOR];
		for (int j = 0; j < FileSystem::INODE_NUMBER_PER_SECTOR; j++)
		{
			DiskInode[j].d_mode = DiskInode[j].d_nlink = DiskInode[j].d_size = 0;
			for (int k = 0; k < 10; k++) 
				DiskInode[j].d_addr[k] = 0;
		}
		/* 为根目录增加目录标志 */
		if (i == 0) 
			DiskInode[0].d_mode |= Inode::IFDIR;
		Utility::copy<int>((int*)&DiskInode, (int*)pBuf->b_addr, 128);
		this->BufMgr->Bwrite(pBuf);
	}
	this->clear();
	this->BufMgr = new BufferManager;
	this->fileSys = new FileSystem;
	this->fileMgr = new FileManager;
	this->k_InodeTable = new InodeTable;
	this->s_openFiles = new OpenFileTable;
	this->k_openFiles = new OpenFiles;
	this->spb = new SuperBlock;
}

int Kernel::open(char* pathname, int mode)
{
	this->callInit();
	this->mode = mode;
	this->pathname = this->dirp = pathname;
	this->fileMgr->Open();
	this->fileSys->Update();
	return this->callReturn;
}

int Kernel::create(char* pathname, int mode)
{
	this->callInit();
	this->isDir = false;
	this->mode = mode;
	this->pathname = this->dirp = pathname;
	this->fileMgr->Creat();
	this->fileSys->Update();
	return this->callReturn;
}

void Kernel::mkdir(char* pathname)
{
	this->callInit();
	this->isDir = true;
	this->pathname = this->dirp = pathname;
	this->fileMgr->Creat();
	this->fileSys->Update();
	if (this->callReturn != -1)
		this->close(this->callReturn);
}

int Kernel::close(int fd)
{
	this->callInit();
	this->fd = fd;
	this->fileMgr->Close();
	this->fileSys->Update();
	return this->callReturn;
}

void Kernel::cd(char* pathname)
{
	this->callInit();
	this->pathname = this->dirp = pathname;
	this->fileMgr->ChDir();
}

int Kernel::fread(int readFd, char* buf, int nbytes)
{
	this->callInit();
	this->fd = readFd;
	this->buf = buf;
	this->nbytes = nbytes;
	this->fileMgr->Read();
	return this->callReturn;
}

int Kernel::fwrite(int writeFd, char* buf, int nbytes)
{
	this->callInit();
	this->fd = writeFd;
	this->buf = buf;
	this->nbytes = nbytes;
	this->getFileMgr()->Write();
	return this->callReturn;
}

void Kernel::ls()
{
	this->k_IOParam.m_Offset = 0;
	this->getInodeTable()->IGet(this->cdir->i_number);
	this->k_IOParam.m_Count = this->cdir->i_size / (DirectoryEntry::DIRSIZ + 4);
	Buf* pBuf = NULL;
	while (true)
	{
		/* 对目录项已经搜索完毕 */
		if (this->k_IOParam.m_Count == 0)
		{
			if (pBuf != NULL)
				this->getBufMgr()->Brelse(pBuf);
			this->error = Kernel::NOENT;
			break;
		}

		/* 已读完目录文件的当前盘块，需要读入下一目录项数据盘块 */
		if (this->k_IOParam.m_Offset % Inode::BLOCK_SIZE == 0)
		{
			if (pBuf != NULL)
				this->getBufMgr()->Brelse(pBuf);
			int phyBlkno = this->cdir->Bmap(this->k_IOParam.m_Offset / Inode::BLOCK_SIZE);
			pBuf = this->getBufMgr()->Bread(phyBlkno);
		}

		/* 没有读完当前目录项盘块，则读取下一目录项至dent */
		int* src = (int *)(pBuf->b_addr + (this->k_IOParam.m_Offset % Inode::BLOCK_SIZE));
		Utility::copy<int>(src, (int *)&this->dent, sizeof(DirectoryEntry) / sizeof(int));
		this->k_IOParam.m_Offset += (DirectoryEntry::DIRSIZ + 4);
		this->k_IOParam.m_Count--;
		if (this->dent.inode == 0)
			continue;
		cout << this->dent.name << " ";
	}
	cout << endl;
	this->getInodeTable()->IPut(this->cdir);
}

void Kernel::fseek(int seekFd, int offset, int ptrname)
{
	this->callInit();
	this->fd = seekFd;
	this->offset = offset;
	this->mode = ptrname;
	this->fileMgr->Seek();
}

void Kernel::fdelete(char* pathname)
{
	this->callInit();
	this->dirp = pathname;
	this->fileMgr->Delete();
}

void Kernel::fmount(char* from, char* to)
{
	fstream f(from, ios::in | ios::binary);
	if (f)
	{
		f.seekg(0, f.end);
		int length = f.tellg();
		f.seekg(0, f.beg);
		char* tmpBuffer = new char[length];
		f.read(tmpBuffer, length);
		int tmpFd = this->open(to, 511);
		if (this->error != NO_ERROR)
			goto end;
		this->fwrite(tmpFd, tmpBuffer, length);
		if (this->error != NO_ERROR)
			goto end;
		this->close(tmpFd);
	end:
		f.close();
		delete tmpBuffer;
		return;
	}
	else
	{
		this->error = NOOUTENT;
		return;
	}
}
