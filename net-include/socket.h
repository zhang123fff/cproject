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
    class socket{
    private:
        int fd;
    public:
        //注意fd只能暴露给某些类型，谨慎使用
        int get_fd()noexcept;

        int bind(const addr& ad)noexcept;

        int connect(const addr& ad)noexcept;

        int send(const std::string& meg)noexcept;

        int recv(std::string& meg, int num);

        int listen(int max)noexcept;

        int accept(sockaddr* so_ad, socklen_t* len)noexcept;

        int set_non_block()noexcept;

        int set_block()noexcept;

    public:

        socket(sa_family_t fa, in_port_t po);

        socket(const addr& ad);

        ~socket();

        socket(const socket& others) = delete;

        socket(socket&& others)noexcept;

        socket& operator=(const socket& others) = delete;

        socket& operator=(socket&& others)noexcept;











    };
}