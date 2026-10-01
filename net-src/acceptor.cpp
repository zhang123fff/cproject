#include"../net-include/acceptor.h"

namespace ne{

    void acceptor::closing_handle(){

    }

    void acceptor::error_handle(){

    }

    int acceptor::fd(){

    }

    bool acceptor::set_reading(const call_back& op){

    }

    bool acceptor::set_closing(const call_back& op){

    }

    bool acceptor::set_error(const call_back& op){

    }

    acceptor::acceptor(){

    }

    acceptor::acceptor(socket* sock, channel* ch){
        sock_ = sock;
        ch_ = ch;
    }
    //       可能会抛出异常(bad_alloc)
    //构造失败
    acceptor::acceptor(socket* sock){
        //if(!sock) throw std::invalid_argument("acceptor::acceptor(socket* sock) : sock不能为nullptr")
        sock_ = sock;
        
        if(sock_) ch_ = new channel(sock_->get_fd());
        else ch_ = nullptr;
        
 
    }

    acceptor::acceptor(acceptor&& others){
        
    }

    acceptor& acceptor::operator=(acceptor&& others){

    }
            
    acceptor::~acceptor(){

    }
    //返回false ch_ 和sock_都为nullptr，或者有一个为nullptr
    //返回true ch_和sock_都不为nullptr
    bool acceptor::is_valid(){
        return ch_ && sock_;
    }




}