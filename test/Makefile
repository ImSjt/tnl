OBJ = $(BASE_DIR)/build-in.o $(NET_DIR)/build-in.o

.PHONY : all
all : test_event_poll test_thread test_logger test_singleton test_log_file \
		test_async_logging test_log test_event_loop_thread test_timer test_acceptor \
		test_tcp_server

test_event_poll : TestEventPoll.o
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJ)

test_thread : TestThread.o
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJ)

test_logger : TestLogger.o
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJ)

test_singleton : TestSingleton.o
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJ)

test_log_file : TestLogFile.o
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJ)

test_async_logging : TestAsyncLogging.o
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJ)

test_log : TestLog.o
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJ)

test_event_loop_thread : TestEventLoopThread.o
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJ)

test_timer : TestTimer.o
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJ)

test_acceptor : TestAcceptor.o
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJ)

test_tcp_server : TestTcpServer.o
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJ)

clean:
	rm -f $(shell find -name "*.o")
	rm -f test_event_poll test_thread test_logger test_singleton test_log_file \
			test_async_logging test_log test_event_loop_thread test_timer test_acceptor \
			test_tcp_server

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
