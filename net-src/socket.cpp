#include"../net-include/socket.h"


namespace ne{

    socket::socket(sa_family_t fa, in_port_t po){
        fd = ::socket(fa, po, 0);
        if(fd == -1)throw std::runtime_error(std::string("socket获取失败,errno : ") + strerror(errno));
    }
    
    socket::socket(const addr& ad){
        fd = ::socket(ad.get_fa_(), ad.get_po_(), 0);
        if(fd == -1)throw std::runtime_error(std::string("socket获取失败,errno : ") + strerror(errno));
    }

    socket::~socket(){
        ::close(fd);
    }
    //socket::socket(const socket& others) = delete;
    socket::socket(socket&& others)noexcept{
        fd = others.fd;
        others.fd = -1;
    }
    //socket& operator=(const socket& others) = delete;
    socket& socket::operator=(socket&& others)noexcept{
        if(this == &others) return *this;
        ::close(fd);
        fd = others.fd;
        others.fd = -1;
        return *this;
    }
    //注意fd只能暴露给某些类型，谨慎使用
    int socket::get_fd(){
        return fd;
    }

    int socket::bind(const addr& ad)noexcept{
        if(fd == -1) return -1;
        sockaddr tmp;
        if(!ad.get_sockaddr(tmp)) return -1;
        int res = ::bind(fd, &tmp, sizeof(sockaddr));
        return res;
    }

    int socket::connect(const addr& ad)noexcept{
        if(fd == -1) return -1;
        sockaddr tmp;
        if(!ad.get_sockaddr(tmp)) return -1;
        int res = ::connect(fd, &tmp, sizeof(sockaddr));
        return res;
    }

    int socket::send(const std::string& meg)noexcept{
        int res = ::send(fd, (void*)meg.c_str(), meg.size(), 0);
        return res;
    }

    int socket::recv(std::string& meg, int num){
        if(fd == -1) return -1;
        meg.resize(num);
        int res = ::recv(fd, (void*)meg.data(), num, 0);
        
        return res;
    }

    int socket::listen(int max)noexcept{
        return ::listen(fd, max);
    }

    int socket::accept(sockaddr* so_ad, socklen_t* len)noexcept{
        return ::accept(fd, so_ad, len);
    }

    int socket::set_non_block()noexcept{
        int flag = fcntl(fd, F_GETFL, 0);
        if(flag == -1)return -1;
        flag = fcntl(fd, F_SETFL, flag | O_NONBLOCK);
        return flag; 
    }

    int socket::set_block()noexcept{
        int flag = fcntl(fd, F_GETFL, 0);
        if(flag == -1)return -1;
        flag = fcntl(fd, F_SETFL, flag & ~O_NONBLOCK);
        return flag; 
    }







}