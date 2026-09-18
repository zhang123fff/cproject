#pragma once
#include<sys/socket.h>
#include<fcntl.h>
#include<sys/epoll.h>
#include<netdb.h>
#include<errno.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<iostream>
#include<vector>
#include<unistd.h>
#include<memory>
#include<cstring>
namespace ne{
    class epoll{
        private:
            int epfd;
        public:
            int epoll_wait(epoll_event* ptr, int maxsize, int timeout)noexcept;
            int epoll_ctl(int op, int fd, epoll_event* ptr)noexcept;
            int fd()const noexcept;
        public:
            epoll();
            epoll(const epoll& others) = delete;
            epoll(epoll&& others)noexcept;
            epoll& operator=(const epoll& others) = delete;
            epoll& operator=(epoll&& others)noexcept;
            ~epoll();




    };








}