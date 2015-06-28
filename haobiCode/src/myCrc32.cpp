#include "myfdcntl.h"
#include "mapfile.h"
#include "myCrc32.h"

int DumpFileWithCrc32(const char *file, const void *buf, long size, int mode) {

	long nRs;
	unsigned int nCrc32Code;

	_CHECK_RS(file, NULL, -1);
	_CHECK_RS(buf, NULL, -1);
	if (0 >= size) {
		_CHECK_RS(-1, -1, -1);
	}

	nCrc32Code = GetCrc32((const char *)buf, size);

	nRs = WriteFile(file, buf, size, mode);
	_CHECK_RS(nRs, -1, -1);

	nRs = AppendFile(file, &nCrc32Code, CRC32_LEN, mode);
	_CHECK_RS(nRs, -1, -1);

	return 0;
}

int CheckFileWithCrc32(const char *file) {

	int nRs;
	unsigned int nBufCrc32Code, nFileCrc32Code;
	long nSize;
	void *addr;
	TMapFile mf;

	nRs = mf.Open(file);
	_CHECK_RS(nRs, TMapFile::OP_FAIL, -1);

	nSize = mf.Size();
	addr = mf.Addr();

	nFileCrc32Code = GetCrc32((const char *)addr, nSize - CRC32_LEN);
	memcpy(&nBufCrc32Code, (char *)addr + nSize - CRC32_LEN, CRC32_LEN);
	
	mf.Close();

	if (nFileCrc32Code != nBufCrc32Code)
		return 1;
	return 0;
}