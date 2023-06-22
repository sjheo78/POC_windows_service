poc_handle_service.exe : main.o
	g++ --static -o $@ main.cpp
