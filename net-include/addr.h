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
#include <cstdio>      
#include <cstdlib>     
#include <cstring>     
#include <cstdint>   
#include <cassert>

namespace ne{
    //仍需要扩展
    class addr{
            static constexpr size_t IPV4_STRING_MAXLEN = 16;
        private:
            sa_family_t fa_;
            int po_;
            std::string ip_;
            in_port_t port_;

        public:
            //待实现
            static addr get_localhost_addr(sa_family_t fa, int po, const std::string& ip, in_port_t port);

            const std::string& get_ip()const;

            in_port_t get_ports()const;

            sa_family_t get_fa_()const;

            int get_po_()const;

            bool get_sockaddr(sockaddr** ad, socklen_t* len)const;

            void set_socktype(int socktype);

            void set_sa_family(sa_family_t fa);

            bool set_ip(const std::string& ip);

            void set_port_num(in_port_t port);

            static bool domain_to_ipv4_string(const std::string& dom, std::string& rec);

            bool to_ipv4_string(std::string& rec)const;
            
            //待扩展，转化域名->ipv4,ipv6,udp,tcp,返回sockaddr
            //bool get_sockaddrs_ipv4_tcp(std::vector<std::pair<socklen_t, sockaddr>>& addrs)const;
            //bool get_sockaddr_ipv4_tcp(sockaddr& ad, socklen_t& len)const;


        public:
            addr(const std::string& ip, in_port_t port, sa_family_t fa, int po);

            addr(const addr& others);

            addr(addr&& others)noexcept;

            addr& operator=(const addr& others);

            addr& operator=(addr&& others)noexcept;
            
            ~addr();






    };








}