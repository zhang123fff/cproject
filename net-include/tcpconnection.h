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
#include"buffer.h"

namespace ne{

    class tcpconnection{
            using call_back = std::function<void()>;
        private:
            //ch_和sock_在tcpconnection对象存在区间不为空(外部和内部函数保证)
            //注意ch_,sock_生命周期归tcpconnection管理
            socket* sock_;
            channel* ch_;

            buffer output_buff;
            buffer input_buff;

            //必须要有的function，不会判断是否为空
            call_back reading;
            call_back writing;
            call_back closing;
            call_back error;
        private:
            static bool call_back_copy(call_back& func1, const call_back& func2);

            void closing_handle();

            void error_handle();

            void reading_handle();

            void writing_handle();

        public:
            bool set_reading(const call_back& op);

            bool set_closing(const call_back& op);

            bool set_error(const call_back& op);

            bool set_writing(const call_back& op);

            bool is_valid()const;
                
        public:
            tcpconnection(socket* sock, channel* ch);

            tcpconnection(socket* sock);

            tcpconnection(tcpconnection&& others)noexcept = delete;

            tcpconnection(const tcpconnection&) = delete;

            tcpconnection& operator=(const tcpconnection&) = delete;

            tcpconnection& operator=(tcpconnection&& others)noexcept = delete;

            ~tcpconnection();

    };














}