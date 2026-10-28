#include"../net-include/epoll.h"



namespace ne{

    int epoll::epoll_wait(epoll_event* ptr, int maxsize, int timeout)noexcept{
        return ::epoll_wait(epfd, ptr, maxsize, timeout);
    }

    int epoll::epoll_ctl(int op, int fd, epoll_event* ptr)noexcept{
        return ::epoll_ctl(epfd, op, fd, ptr);
    }

    //通过is_valid判断是否成功
    epoll::epoll(){
        epfd = ::epoll_create1(0);
        //if(epfd == -1) throw std::runtime_error("epfd 创建失败， 失败原因 : " + std::string(strerror(errno)));
    }

    //begining为false时候epfd置-1
    //反之则创建epfd，但是会失败，置-1
    epoll::epoll(bool beginning){
        if(beginning){
            epfd = ::epoll_create1(0);
        }
        else{
            epfd = -1;
        }
    }

    // 推荐用fd（）直接判断是不是-1
    bool epoll::is_valid()const noexcept{
        return epfd != -1;
        
    }

    epoll::epoll(epoll&& others)noexcept{
        epfd = others.epfd;

        others.epfd = -1;
    }

    epoll& epoll::operator=(epoll&& others)noexcept{
        if(this == &others) return *this;
        close();
        epfd = others.epfd;
        others.epfd = -1;
        return *this;
    }

    epoll::~epoll(){
        if(epfd != -1) ::close(epfd);
    }

    int epoll::fd()const noexcept{
        return epfd;
    }

    int epoll::close(){
        int res = ::close(epfd);
        epfd = -1;
         
        return res;
    }

    int epoll::epoll_create(){
        if(!is_valid()){
            epfd = ::epoll_create1(0);
        }

        return epfd;
    }



}