OBJ = CurThread.o Condition.o CountDownLatch.o Thread.o Logger.o Timestamp.o AsyncLogging.o \
		File.o LogFile.o

.PHONY : all
all : build-in.o

clean:
	rm -f $(shell find -name "*.o")

build-in.o : $(OBJ)
	$(LD) $(LDFLAGS) -r -o $@ $^

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
