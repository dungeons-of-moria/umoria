void DumpRes(
	char *fileName,
	int resType, int resID, char *resName, int resAttrs,
	char *elemPtr, unsigned elemCnt, unsigned elemSiz,
	void (*strProc)(char *elem, void (*proc)(char **str))
);

int LoadRes(
	char **memPtr,
	int resType, int resID,
	unsigned elemCnt, unsigned elemSiz,
	void (*strProc)(char *elem, void (*proc)(char **str))
);
