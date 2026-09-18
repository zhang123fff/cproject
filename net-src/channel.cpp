#include"../net-include/channel.h"


namespace ne{

channel::channel(int fd) : fd_(fd){

}

channel::channel(){

}
//注意这里要把epoll当中的channel指针也要改了，不然指向错误channel，这点暂时未改正
channel::channel(channel&& others)noexcept : fd_(others.fd_), events_(others.events_),
                                revents_(others.revents_), back(others.back),
                                adding(others.adding), 
                                writing(std::move(others.writing)),
                                reading(std::move(others.reading)),
                                error(std::move(others.error)), closing(std::move(others.closing)){
    others.fd_ = -1;
    others.adding = false;
    others.back = nullptr;
    others.events_ = 0;
    others.revents_ = 0;
}

channel& channel::operator=(channel&& others)noexcept{
    if(this == &others)return *this;

    fd_ = others.fd_;
    events_ = others.events_;
    revents_ = others.revents_;
    back = others.back;
    adding = others.adding;
    writing = std::move(others.writing);
    reading = std::move(others.reading);
    error = std::move(others.error);
    closing = std::move(others.closing);

    others.fd_ = -1;
    others.adding = false;
    others.back = nullptr;
    others.events_ = 0;
    others.revents_ = 0;

    return *this;
}
//这里可能也要通知epoll
channel::~channel(){

}

void channel::handling(){
    if(revents_ == 0)return;
    if(!adding)return;
    
    if(revents_ & EPOLLIN)reading();
    if(revents_ & EPOLLOUT)writing();
    if(revents_ & (EPOLLHUP | EPOLLRDHUP))closing();
    if(revents_ & EPOLLERR)error();
}

void channel::set_events(uint32_t ev)noexcept{
    events_ = ev;
}

void channel::set_revents(uint32_t ev)noexcept{
    revents_ = ev;
}

void channel::set_closing(const call_back& op){
    closing = op;
}

void channel::set_error(const call_back& op){
    error = op;
}

void channel::set_reading(const call_back& op){
    reading = op;
}

void channel::set_writing(const call_back& op){
    writing = op;
}

int channel::set_nonblock()noexcept{
    int flag = fcntl(fd_, F_GETFL, 0);
    if(flag == -1)return -1;
    flag |= O_NONBLOCK;
    return fcntl(fd_, F_SETFL, flag);
}

int channel::set_block()noexcept{
    int flag = fcntl(fd_, F_GETFL, 0);
    if(flag == -1)return -1;
    flag &= ~O_NONBLOCK;
    return fcntl(fd_, F_SETFL, flag);
}

int channel::socket_state()const noexcept{
    return fcntl(fd_, F_GETFL, 0);
}

bool channel::is_adding()const noexcept{
    return adding;
}

int channel::fd()const noexcept{
    return fd_;
}







}