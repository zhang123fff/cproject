#include"../net-include/acceptor.h"

namespace ne{

    void acceptor::closing_handle(){
        sock_->close();
        ch_->detach_from_eventloop_and_discard_this_channel();
        //输出关闭信息
        
        fprintf(stdout, "监听fd已关闭");
        
    }

    void acceptor::error_handle(){
        int res = 0;
        if(sock_->get_socket_error(res) == 0) fprintf(stderr, "socket-error : %d", res);
        else fprintf(stderr, "socket-error获取失败");

        closing_handle();
    }

    bool acceptor::set_reading(const call_back& op){
        call_back tmp = std::move(reading);
        if(!call_back_copy(reading, op)){
            reading = std::move(tmp);
            return false;
        }
        bool res = ch_->set_reading(reading);
        if(!res) reading = std::move(tmp);
        return res; 

        
    }

    bool acceptor::set_closing(const call_back& op){
        call_back tmp = std::move(closing);
        if(!call_back_copy(closing, op)){
            closing = std::move(tmp);
            return false;
        }
        bool res = ch_->set_closing(closing);
        if(!res) closing = std::move(tmp);
        return res; 
    }

    bool acceptor::set_error(const call_back& op){
        call_back tmp = std::move(error);
        if(!call_back_copy(error, op)){
            error = std::move(tmp);
            return false;
        }
        bool res = ch_->set_error(error);
        if(!res) error = std::move(tmp);
        return res; 
    }
    
    //       可能会抛出异常(invalid_argument)
    //sock或ch为空，sock或ch状态错误
    acceptor::acceptor(socket* sock, channel* ch){
        if(!sock || !ch) 
            throw std::invalid_argument("acceptor::acceptor(socket* sock, channel* ch) : ch或sock为空");
        if(!sock->is_valid() || sock->get_fd() != ch->fd()) 
            throw std::invalid_argument("acceptor::acceptor(socket* sock, channel* ch) : ch或sock状态错误");
        sock_ = sock;
        ch_ = ch;
    }
    
    //       可能会抛出异常(bad_alloc，invalid_argument)
    //new失败，sock为空，sock状态错误
    acceptor::acceptor(socket* sock){
        
        if(!sock) 
            throw std::invalid_argument("acceptor::acceptor(socket* sock) : sock为空");
        if(!sock->is_valid()) 
            throw std::invalid_argument("acceptor::acceptor(socket* sock) : sock状态错误");
        sock_ = sock;
        
        ch_ = new channel(sock_->get_fd());
    }
       
    acceptor::~acceptor(){
        delete ch_;
        delete sock_;
    }
    
    //返回false ch_ 和sock_都为nullptr，或者有一个为nullptr
    //返回true ch_和sock_都不为nullptr
    bool acceptor::is_valid()const{
        return ch_ && sock_;
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



}