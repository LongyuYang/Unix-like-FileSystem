#pragma once
#ifndef BUF_H
#define BUF_H

class Buf
{
public:
	enum BufFlag	/* b_flags�б�־λ */
	{
		B_WRITE = 0x1,		/* д�������������е���Ϣд��Ӳ����ȥ */
		B_READ	= 0x2,		/* �����������̶�ȡ��Ϣ�������� */
		B_DONE	= 0x4,		/* I/O�������� */
		B_ERROR	= 0x8,		/* I/O��������ֹ */
		B_BUSY	= 0x10,		/* ��Ӧ��������ʹ���� */
		B_WANTED = 0x20,	/* �н������ڵȴ�ʹ�ø�buf�������Դ����B_BUSY��־ʱ��Ҫ�������ֽ��� */
		B_ASYNC	= 0x40,		/* �첽I/O������Ҫ�ȴ������ */
		B_DELWRI = 0x80		/* �ӳ�д������Ӧ����Ҫ��������ʱ���ٽ�������д����Ӧ���豸�� */
	};
	
public:
	unsigned int b_flags;	/* ������ƿ��־λ */
	
	/* ������ƿ���й���ָ�� */
	Buf*	b_forw;
	Buf*	b_back;
	Buf*	av_forw;
	Buf*	av_back;
	
	int		b_wcount;		/* �贫�͵��ֽ��� */
	char*   b_addr;	        /* ָ��û�����ƿ�������Ļ��������׵�ַ */
	int		b_blkno;		/* �����߼���� */
};


#endif
