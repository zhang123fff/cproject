#include"../net-include/acceptor.h"

namespace ne{

    /*//待完善,未完成
    acceptor::acceptor(){
        sock_ = new socket(AF_INET, SOCK_STREAM);
        try{
            ch_ = new channel(sock_->get_fd());
        }
        catch(std::exception& ex){
            delete sock_;
            //输出错误信息
            fprintf(stderr, "acceptor::acceptor() new抛出异常\n");
            fprintf(stderr, "%s\n", ex.what());
            throw;
        }

        std::string tmp;
        
        if(!addr::domain_to_ipv4_string("localhost", tmp)) 
            throw std::runtime_error("acceptor::acceptor() : addr::domain_to_ipv4_string(localhost, tmp)抛出异常");
        addr ad(tmp, 8080, AF_INET, SOCK_STREAM);
        if(sock_->bind(ad) == -1) 
            throw std::runtime_error("acceptor::acceptor() : sock_->bind(ad)抛出异常");
        //enable不会失败
        ch_->enable_read_event();
    }
    */

    void acceptor::closing_handle(){
        ch_->detach_from_eventloop_and_discard_this_channel();
        sock_->close();
        //输出关闭信息
        
        fprintf(stdout, "监听fd已关闭");
        
    }

    void acceptor::error_handle(){
        int res = 0;
        if(sock_->get_socket_error(res) == 0) fprintf(stderr, "socket-error : %d", res);
        else fprintf(stderr, "socket-error获取失败");

        closing_handle();
    }

    //默认前提是socket返回读事件才执行,
    //非阻塞的边缘触发，水平触发都可以用
    //注意阻塞不能用这个回调函数
    void acceptor::reading_handle(){
        int newfd = -1;
        socket* sock = nullptr;
        channel* ch = nullptr;

        while(true){
            newfd = sock_->accept(nullptr, nullptr);
            if(newfd == -1){
                if(errno == EAGAIN || errno == EWOULDBLOCK){
                    return;
                }
                else if(errno == ENFILE || errno == EMFILE){
                    //输出错判信息
                    return;
                }
                else if(errno == EINTR || errno == ECONNABORTED){
                    continue;
                }
                else{
                    //输出错误信息
                    return;
                }
            }
            try{
                sock = new socket(newfd);
                ch = new channel(newfd);
                //不会失败
                ch->enable_read_event(); 
            }
            catch(std::exception& ex){
                //输出错误信息
                if(!sock && !ch) ::close(newfd);
                delete sock;
                delete ch;
                ch = nullptr;
                sock = nullptr;
                newfd = -1;
                continue;
            }
        
            try{
                if(!newconnection(sock, ch)){
                    delete sock;
                    delete ch;
                }
            }
            catch(...){
                //输出错误信息
                if(sock) delete sock;
                if(ch) delete ch;
            }
            //初始化
            ch = nullptr;
            sock = nullptr;
            newfd = -1;

        
        
        
        
        }
        

    }

    //默认前提是socket返回读事件才执行
    //阻塞专用
    void acceptor::block_reading_handle(){
        int newfd = sock_->accept(nullptr, nullptr);
        socket* sock = nullptr;
        channel* ch = nullptr;

        if(newfd == -1){
            return;
        }

        try{
            sock = new socket(newfd);
            ch = new channel(newfd);

            //不会失败
            //默认开write和read事件
            ch->enable_read_event();
        }
        catch(std::exception& ex){
            //输出错误信息
            if(!sock && !ch) ::close(newfd);
            delete sock;
            delete ch;
            return;
        }
        
        try{
            if(!newconnection(sock, ch)){
                delete sock;
                delete ch;
            }
        }
        catch(...){
            //输出错误信息
            if(sock) delete sock;
            if(ch) delete ch;
            return;
        }

    }

    bool acceptor::set_newconnection(const std::function<bool(socket *, channel *)> &op){
        try{
            newconnection = op;
        }
        catch(std::exception& ex){
            //输出错误信息
            return false;
        }
        return true;
    }

    bool acceptor::set_reading(const call_back &op)
    {
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

        /*  废弃部分
        int flag = sock->is_blocking();
        if(flag == -1) 
            throw std::runtime_error("acceptor::acceptor(socket* sock, channel* ch) : 获取socket is_blocking()失败");

        //只要有一个
        if(flag == 0){
            if(!set_default_reading())
                throw std::runtime_error("acceptor::acceptor(socket* sock, channel* ch) : set_default_reading()失败");
        }
        else{
            if(!set_default_block_reading())
                throw std::runtime_error("acceptor::acceptor(socket* sock, channel* ch) : set_default_block_reading()失败");
        }
        if(!set_default_closing())
            throw std::runtime_error("acceptor::acceptor(socket* sock, channel* ch) : set_default_closing()失败");
        if(!set_default_error())
            throw std::runtime_error("acceptor::acceptor(socket* sock, channel* ch) : set_default_error()失败");
        */
        
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

        ch_->enable_read_event();
        
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

    bool acceptor::legalize_socket(int fd){
        if(fd == sock_->get_fd()) return false;
        if(fd == -1 || sock_->is_valid()) return false;
        sock_->set_fd(fd);

        ch_->channel_init();
        //ch_的event等用户负责改
        ch_->set_fd(fd);
        return true;
    }

    //复用channel，用户要保证channel初始化
    //成功后，channel换了，需要重新配置events，和channel的回调函数
    bool acceptor::legalize_socket(socket* sock){
        if(sock_ == sock || sock_->is_valid()) return false;
        if(!sock || !sock->is_valid()) return false;  

        delete sock_;
        sock_ = sock;
        ch_->channel_init();
        ch_->set_fd(sock_->get_fd());
        return true;
    }

    bool acceptor::call_back_copy(call_back& func1, const call_back& func2){
        try{
            func1 = func2;
        }
        catch(std::exception& ex){
            //补上错误输出
            return false;
        }
        return true;
    
    }

    //同步ch_
    //默认ch_和sock_不是nullptr
    bool acceptor::set_default_block_reading(){
        call_back tmp = std::move(reading);
        reading = [this](){block_reading_handle();};
        bool res = ch_->set_reading(reading);
        if(!res) reading = std::move(tmp);
        return res; 
    }

    //同步ch_
    //默认ch_和sock_不是nullptr
    bool acceptor::set_default_reading(){
        call_back tmp = std::move(reading);
        reading = [this](){reading_handle();};
        bool res = ch_->set_reading(reading);
        if(!res) reading = std::move(tmp);
        return res; 
    }

    //同步ch_
    //默认ch_和sock_不是nullptr
    bool acceptor::set_default_closing(){
        call_back tmp = std::move(closing);
        closing = [this](){closing_handle();};
        bool res = ch_->set_closing(closing);
        if(!res) closing = std::move(tmp);
        return res; 
    }

    //同步ch_
    //默认ch_和sock_不是nullptr
    bool acceptor::set_default_error(){
        call_back tmp = std::move(error);
        error = [this](){error_handle();};
        bool res = ch_->set_error(error);
        if(!res) error = std::move(tmp);
        return res; 
    }

    int acceptor::enable_listen_socket_ET(){
        return ch_->enable_ET_event();
    }

    int acceptor::enable_listen_socket_unblock(){
        return sock_->set_non_block();
    }

    int acceptor::disable_listen_socket_unblock(){
        return sock_->set_block();
    }

    int acceptor::disable_listen_socket_ET(){
        return ch_->disable_ET_event();
    }

    int acceptor::enable_listen_socket_read(){
        return ch_->enable_read_event();
    }

    int acceptor::disable_listen_socket_read(){
        return ch_->disable_read_event();
    }

    //!!!注意要谨慎使用，监听channel所有权属于acceptor
    channel* acceptor::get_listen_channel(){
        return ch_;
    }

    //!!!注意要谨慎使用，监听fd所有权属于acceptor
    int acceptor::get_listen_fd(){
        return sock_->get_fd();
    }










}