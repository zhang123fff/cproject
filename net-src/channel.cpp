#include"../net-include/channel.h"
#include"../net-include/eventloop.h"

namespace ne{

    channel::channel(int fd){
        fd_ = fd;
    }

    channel::channel(){


    }
    /*    移动操作已废弃  //注意这里要把epoll当中的channel指针也要改了，不然指向错误channel，这点暂时未改正
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
*/
    //待补全
    channel::~channel(){
        if(loop){
            detach_from_eventloop();
        }
    }

    void channel::handling(){
    
        if(revents_ & EPOLLERR){
            error();
            //归位
            revents_ = 0;
            return;
        }
        if(revents_ & (EPOLLHUP | EPOLLRDHUP)){
            closing();
            //归位
            revents_ = 0;
            return;
        }
        if(revents_ & EPOLLIN)reading();
        if(revents_ & EPOLLOUT)writing();
    

        //归位
        revents_ = 0;
    }
    //
    int channel::set_events(uint32_t ev){ 
        uint32_t tmp = events_;
        events_ = ev;
        if(belonging_to_eventloop()) {
            int res = loop->update_channel(fd_, this);
            if(res == -1){
                events_ = tmp;
            }
            return res;
        }
        
        return 0;
    }

    void channel::set_revents(uint32_t ev){
        revents_ = ev;
    }

    bool channel::set_closing(const call_back& op){
        return call_back_copy(closing, op);
    }

    bool channel::set_error(const call_back& op){
        return call_back_copy(error, op);
    }

    bool channel::set_reading(const call_back& op){
        return call_back_copy(reading, op);
    }

    bool channel::set_writing(const call_back& op){
        return call_back_copy(writing, op);
    }

    bool channel::call_back_copy(call_back& func1, const call_back& func2){
        try{
            func1 = func2;
        }
        catch(std::exception& ex){
            //补上错误输出
            return false;
        }
        return true;
    
    }

    int channel::fd()const{
        return fd_;
    }

    void channel::set_loop(eventloop* ptr){
        loop = ptr;
    }

    void channel::set_fd(int fd){
        fd_ = fd;
    }

    uint32_t channel::get_events()const{
        return events_;
    }

    uint32_t channel::get_revents()const{
        return revents_;
    }

    eventloop* channel::get_loop()const{
        return loop;
    }
    //
    int channel::socket_state()const{
        return fcntl(fd_, F_GETFL, 0);
    }

    int channel::enable_read_event(){
        return event_enable(EPOLLIN);
    }

    int channel::enable_write_event(){
        return event_enable(EPOLLOUT);
    }

    int channel::enable_ET_event(){
        return event_enable(EPOLLET);
    }

    int channel::disable_read_event(){
        return event_disable(EPOLLIN);
    }

    int channel::disable_write_event(){
        return event_disable(EPOLLOUT);
    }

    int channel::disable_ET_event(){
        return event_disable(EPOLLET);
    }

    bool channel::belonging_to_eventloop()const{
        return loop;
    }
    
    //ev参数只能包含一个event
    //返回0，代表成功更改或者events_已经被更改
    //返回-1， 代表失败，events_没被更改
    int channel::event_enable(uint32_t ev){
        if(events_ & ev) return 0;
        uint32_t tmp = events_;
        events_ |= ev;
        if(belonging_to_eventloop()){
            int res = loop->update_channel(fd_, this);
            if(res == -1){
                events_ = tmp;
            }
            return res;
        }

        return  0;
    }
    
    //ev参数只能包含一个event
    //返回0，代表成功更改或者events_已经被更改
    //返回-1， 代表失败，events_没被更改
    int channel::event_disable(uint32_t ev){
        if(!(events_ & ev)) return 0;
        uint32_t tmp = events_;
        events_ &= ~ev;
        if(belonging_to_eventloop()){
            int res = loop->update_channel(fd_, this);
            if(res == -1){
                events_ = tmp;
            }
            return res;
        }

        return  0;
    }
    
    int channel::add_to_eventloop(eventloop* lo){
        if(loop || !lo) return -1;
        int res = lo->add_channel(fd_, this);
        if(res == 0)set_loop(lo);
        return res;

    }
    
    //失败返回 -1
    //成功返回0
    int channel::detach_from_eventloop(){
        if(!loop) return 0;
        int res = loop->remove_channel(fd_);
        if(res == 0) loop = nullptr;
        return res;
    }
    
    //从eventloop移除，且fd置-1
    int channel::detach_from_eventloop_and_discard_this_channel(){
        int res = detach_from_eventloop();
        if(res == 0) set_fd(-1);
        return res;
    }
    //
    /*
    channel::channel(channel&& others)noexcept{
        fd_ = others.fd_;
        events_ = others.events_;
        revents_ = others.revents_;
        loop = others.loop;
        others.fd_ = -1;
        others.events_ = 0;
        others.revents_ = 0;
        others.loop = nullptr;

        closing = std::move(others.closing);
        writing = std::move(others.writing);
        reading = std::move(others.reading);
        error = std::move(others.closing);
    }

    channel& channel::operator=(channel&& others)noexcept{

    }
    */

}