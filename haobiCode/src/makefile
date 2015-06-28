obj=mapfile.o myCrc32.o myDef.o myetc.o myfdcntl.o myFileSearch.o myLock.o myLog.o myMemReg.o mySearch.o mysem.o myshm.o mySplit.o mystr.o myThread.o myTime.o
out=../libhaobilib.a

all:$(obj)
	ar rcs $(out) $(obj)
	rm -rf $(obj)

$(obj):%.o:%.cpp
	g++ -c $< -g -o $@ -I ../inc/

clean:
	rm -rf $(out) $(obj)
