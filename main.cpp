#include<iostream>
#include<vector>
#include<unistd.h>
#include"net-include/addr.h"
#include"net-include/epoll.h"
#include"net-include/socket.h"


int main(int argc, char* argv[]){
	if(argc != 3){
		std::cout<<"main 参数数量错误"<<std::endl;
		return -1;
	}
	const std::string ip = argv[1];
	short port = std::stoi(argv[2]); 



	 
	return 0;


}
