#include"../net-include/epoll.h"



namespace ne{
    int epoll::epoll_wait(epoll_event* ptr, int maxsize, int timeout)noexcept{
        if(ptr == nullptr)return -1;
        int siz = 0;
        siz = ::epoll_wait(epfd, ptr, maxsize, timeout);
        return siz;
    }

    int epoll::epoll_ctl(int op, int fd, epoll_event* ptr)noexcept{
        if(op != EPOLL_CTL_DEL && ptr == nullptr) return -1;
        return ::epoll_ctl(epfd, op, fd, ptr);
    }

    epoll::epoll(){
        epfd = ::epoll_create(0);
        if(epfd == -1) throw std::runtime_error("epfd 创建失败， 失败原因 : " + std::string(strerror(errno)));
    }

    epoll::epoll(epoll&& others)noexcept{
        epfd = others.epfd;
        others.epfd = -1;
    }

    epoll& epoll::operator=(epoll&& others)noexcept{
        if(this == &others) return *this;
        close(epfd);
        epfd = others.epfd;
        others.epfd = -1;
        return *this;
    }

    epoll::~epoll(){
        close(epfd);
    }

    int epoll::fd()const noexcept{
        return epfd;
    }






}