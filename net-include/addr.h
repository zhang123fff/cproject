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
namespace ne{
    class addr{
        private:
            sa_family_t fa_;
            in_port_t po_;
            std::string ip_;
            short port_;
        public:
            const std::string& get_ip()const noexcept;
            short get_ports()const noexcept;
            sa_family_t get_fa_()const noexcept;
            in_port_t get_po_()const noexcept;
            bool get_sockaddr(sockaddr& ad)const noexcept;



        public:
            addr(const std::string ip, short port, sa_family_t fa, in_port_t po);
            addr(const addr& others);
            addr(addr&& others)noexcept;
            addr& operator=(const addr& others);
            addr& operator=(addr&& others)noexcept;
            ~addr();






    };








}