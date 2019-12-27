OBJ = Channel.o Poller.o PollPoller.o SocketsOps.o EventLoop.o EventLoopThread.o \
		Timer.o TimerQueue.o Socket.o InetAddress.o Acceptor.o

.PHONY : all
all : build-in.o

clean:
	rm -f $(shell find -name "*.o")

build-in.o : $(OBJ)
	$(LD) $(LDFLAGS) -r -o $@ $^

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
