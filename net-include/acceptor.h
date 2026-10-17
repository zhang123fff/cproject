#pragma once
#include<sys/socket.h>
#include<fcntl.h>
#include<sys/epoll.h>
#include<netdb.h>
#include<errno.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<iostream>
#include<vector>
#include<unistd.h>
#include<memory>
#include<functional>
#include<map>
#include"socket.h"
#include"channel.h"


namespace ne{

    class acceptor{
            using call_back = std::function<void()>;
        private:
            //ch_和sock_在acceptor对象存在区间不为空(外部和内部函数保证)
            //注意ch_,sock_生命周期归acceptor管理
            channel* ch_ = nullptr;
            socket* sock_ = nullptr;

            //必须要有的function，不会判断是否为空
            call_back reading;
            call_back closing;
            call_back error;

            
        private:
            static bool call_back_copy(call_back& func1, const call_back& func2);

            void closing_handle();

            void error_handle();

           
        public:
            bool set_reading(const call_back& op);

            bool set_closing(const call_back& op);

            bool set_error(const call_back& op);

            bool is_valid()const;
            
        public:
            acceptor();

            acceptor(socket* sock, channel* ch);

            acceptor(socket* sock);

            acceptor(acceptor&& others)noexcept = delete;

            acceptor(const acceptor&) = delete;

            acceptor& operator=(const acceptor&) = delete;

            acceptor& operator=(acceptor&& others)noexcept = delete;

            ~acceptor();


    };








}