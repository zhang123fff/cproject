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
    //!!!注意epoll不能由外部close，只能由内部成员函数close()释放
    //以保证epfd只有两种状态
    class epoll{
        private:
            //只有两种状态，无效的-1，其他情况都默认是有效的
            int epfd;
            
        public:
            int epoll_wait(epoll_event* ptr, int maxsize, int timeout)noexcept;

            int epoll_ctl(int op, int fd, epoll_event* ptr)noexcept;

            int fd()const noexcept;

            bool is_valid()const noexcept;

            int close();

            int epoll_create();

        public:
            epoll();

            epoll(bool beginning);

            epoll(const epoll& others) = delete;

            epoll(epoll&& others)noexcept;

            epoll& operator=(const epoll& others) = delete;

            epoll& operator=(epoll&& others)noexcept;
            
            ~epoll();




    };








}