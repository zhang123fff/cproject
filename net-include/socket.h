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
#include"addr.h"
#include<cstring>
namespace ne{
    //仍然存在一些问题要改
    class socket{
    private:
        int fd;
    public:
        //注意fd只能暴露给某些类型，谨慎使用
        int get_fd()const noexcept;

        int bind(const addr& ad)noexcept;

        int connect(const addr& ad)noexcept;

        ssize_t send_all(const std::string& meg)noexcept;

        ssize_t recv_all(std::string& meg, ssize_t num);

        int listen(int max)noexcept;

        int accept(sockaddr* so_ad, socklen_t* len)noexcept;

        int set_non_block()noexcept;

        int set_block()noexcept;

        ssize_t send(const void *__buf, size_t __n, int __flags);

        ssize_t recv(void *__buf, size_t __n, int __flags);

        int close();

        bool is_valid()const;

        int create_socket(sa_family_t fa, int po);
        
        int get_socket_error(int& res);
    public:

        socket(sa_family_t fa, int po);

        socket(const addr& ad);

        ~socket();

        socket(const socket& others) = delete;

        socket(socket&& others)noexcept;

        socket& operator=(const socket& others) = delete;

        socket& operator=(socket&& others)noexcept;


    };
}