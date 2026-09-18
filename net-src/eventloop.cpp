#include"../net-include/eventloop.h"


namespace ne{
    void eventloop::loop(){
        if(epfd == -1)return;
        epoll_event evs[10];
        int num = 0;
        while(get_looping()){
            num = epoll_wait(epfd, evs, 10, -1);
            if(num == -1){
                perror("epoll_wait");
                continue;
            }
            else if(num == 0){
                continue;
            }
            for(int i = 0; i < num; i++){
                evs[i].data->set_revents(evs[i].events);
                evs[i].data->handling();
                
            }
            
        }
        return;
    }

    void eventloop::remove_channel(const channel* ptr){

    }
            
    void eventloop::update_channel(const channel* ptr){

    }

    void eventloop::quit(){

    }

    bool eventloop::get_looping()const noexcept{
        
    }

    eventloop::eventloop(const epoll& ep)noexcept : epfd(ep.fd()), looping(0){
        
    }

    eventloop::eventloop(int fd)noexcept : epfd(fd), looping(0){

    }

    eventloop::~eventloop(){

    }






    
}