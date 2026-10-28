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
    //recvall,sendall的问题要完善
    class socket{
    private:
        int fd_ = -1;

    public:
        //注意fd只能暴露给某些类型，谨慎使用
        int get_fd()const noexcept;

        int bind(const addr& ad)noexcept;

        int connect(const addr& ad)noexcept;

        ssize_t send_all(const std::string& meg)noexcept;

        //废弃
        //ssize_t recv_all(std::string& meg, size_t num, bool& is_closing);

        int listen(int max)noexcept;

        int accept(sockaddr* so_ad, socklen_t* len)noexcept;

        int set_non_block()noexcept;

        int set_block()noexcept;

        ssize_t send(const void *__buf, size_t __n, int __flags);

        ssize_t recv(void *__buf, size_t __n, int __flags);

        int close();

        bool is_valid()const;

        void set_fd(int fd);
        
        int is_blocking()const;

        int create_socket(sa_family_t fa, int po);
        
        int get_socket_error(int& res);

        ssize_t recv_all(std::string& meg, size_t num);

    public:
        socket();

        socket(int fd);

        socket(sa_family_t fa, int po);

        socket(sa_family_t fa, int po, const std::string& ip, in_port_t port_num);

        socket(sa_family_t fa, int po, const std::string& ip, in_port_t port_num, bool is_acc);

        socket(const addr& ad, bool is_acc);

        socket(const addr& ad);

        ~socket();

        socket(const socket& others) = delete;

        socket(socket&& others)noexcept;

        socket& operator=(const socket& others) = delete;

        socket& operator=(socket&& others)noexcept;


    };
}