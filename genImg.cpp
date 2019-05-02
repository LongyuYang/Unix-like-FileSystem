#include <fstream>
using namespace std;
#include "Kernel.h"
	
int mai()
{
	
	//format();
	/*
	BufferManager* b = new BufferManager;
	Buf* buf = b->Bread(0);
	for (int j = 0; j < 512; j++)
		buf->b_addr[j] = '0';
	b->Bdwrite(buf);
	Buf* buf2 = b->Bread(0);
	b->Bdwrite(buf);
	Buf* buf3 = b->Bread(2);
	b->Brelse(buf3);
	buf3 = b->Bread(3);
	b->Brelse(buf3);
	buf3 = b->Bread(7);
	buf3 = b->Bread(4);
	for (int j = 256; j < 512; j++)
		buf3->b_addr[j] = '0';
	b->Bdwrite(buf3);
	buf3 = b->Bread(5);
	b->Bflush();
	*/

	
	int fd, fd1;
	Kernel* k = Kernel::getInstance();
	k->format();
	k->initialize();
	k->mkdir("home/usr");
	cout << k->error << endl;
	//fd = k->create("usr", 511);
	//k->close(fd);
	k->cd("home/usr");
	k->clear();
	system("pause");
	return 0;
}
