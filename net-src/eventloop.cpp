#include"../net-include/eventloop.h"
#include"../net-include/channel.h"

namespace ne{

    //参数设置错误，抛出异常会返回false
    //while循环完成则返回true，包括0次循环
    bool eventloop::loop(){
        if(!ep.is_valid() || epoll_wait_size == 0) return false;
        std::unique_ptr<epoll_event[]> evs;
        const int maxsize = epoll_wait_size;
        int actual_size = epoll_wait_size;
        try{
            evs = std::make_unique<epoll_event[]>(maxsize);
        }
        catch(std::exception& ex){
            //补充错误输出
            return false;
        }
        int res = 0;
        channel* tmp = nullptr;
        
        //actual_size必须小于maxsize
        while(get_looping()){
            res = ep.epoll_wait(evs.get(), actual_size, epoll_wait_time);
            if(res > 0){
                for(size_t i = 0; i < res; i++){
                    tmp = static_cast<channel*>(evs[i].data.ptr);
                    tmp->set_revents(evs[i].events);
                    tmp->handling();
                }
            }
            else if(res == -1){
                //perror("epoll_wait");输出错误信息
                continue;
            }
            else if(res == 0){
                //输出错误信息
                continue;
            
            }
        }
        return true;
    }

    void eventloop::set_looping(bool flag){
        looping.store(flag, std::memory_order_release);
    }

    void eventloop::set_epoll_wait_size(size_t siz){
        epoll_wait_size = siz;
    }

    size_t eventloop::get_epoll_wait_size()const{
        return epoll_wait_size;
    }

    void eventloop::set_epoll_wait_time(int time){
        epoll_wait_time = time;
    }

    int eventloop::get_epoll_wait_time()const{
        return epoll_wait_time;
    }
    
    //失败返回 -1， 成功返回 0
    //加入epoll和map对应的channel
    //失败原因  1,参数错误（ptr == nullptr, fd不存在或已经加入epoll），
    //2,epoll::epoll_ctl失败
    //3，map::insert抛出异常
    int eventloop::add_channel(int fd, channel* ptr){
        if(!ptr) return -1;

        epoll_event ev;
        ev.events = ptr->get_events();
        ev.data.ptr = ptr;
        std::pair<std::map<int, channel*>::iterator, bool> par;
        try{
            par = map.insert({fd, ptr});
        }
        catch(std::exception& ex){           
            //输出错误信息
            return -1;
        }
        //fd已经存在
        if(!par.second) return -1;

        int res = ep.epoll_ctl(EPOLL_CTL_ADD, fd, &ev);
        if(res == -1){
            map.erase(fd);
        }
        

        return res;
    }
    
    //失败返回 -1， 成功返回 0
    //删除epoll和map对应的channel
    //失败原因  1,fd不存在或者不存在于eventloop，2,epoll::epoll_ctl失败
    int eventloop::remove_channel(int fd){
        int res = ep.epoll_ctl(EPOLL_CTL_DEL, fd, nullptr);
        if(res == -1)return -1;
        map.erase(fd);
        return 0;

    }
    
    //失败返回 -1， 成功返回 0
    //更新epoll和eventloop::map
    //失败原因  1,ptr == nullptr，2,epoll::epoll_ctl失败
    int eventloop::update_channel(int fd, channel* ptr){
        if(!ptr) return -1;
        epoll_event ev;
        ev.events = ptr->get_events();
        ev.data.ptr = ptr;
        return ep.epoll_ctl(EPOLL_CTL_MOD, fd, &ev);
    }

    void eventloop::quit(){
        looping.store(false, std::memory_order_release);
    }

    bool eventloop::get_looping()const noexcept{
        return looping.load(std::memory_order_acquire);
    }

    eventloop::eventloop(epoll&& ep): ep(std::move(ep)){
        
    }

    eventloop::eventloop(){

    }

    eventloop::~eventloop(){

    }

    bool eventloop::is_valid()const{
        return ep.is_valid();
    }

    bool eventloop::legalize_epoll(){
        if(is_valid()) return true;
        ep.epoll_create();
        return ep.is_valid();
    }




    
}