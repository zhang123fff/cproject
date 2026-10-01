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
            channel* ch_ = nullptr;
            socket* sock_ = nullptr;

            call_back reading;
            call_back closing;
            call_back error;

        private:
            void closing_handle();

            void error_handle();
        public:
            int fd();

            bool set_reading(const call_back& op);

            bool set_closing(const call_back& op);

            bool set_error(const call_back& op);

            bool is_valid();
        public:
            acceptor();

            acceptor(socket* sock, channel* ch);

            acceptor(socket* sock);

            acceptor(acceptor&& others);

            acceptor(const acceptor&) = delete;

            acceptor& operator=(const acceptor&) = delete;

            acceptor& operator=(acceptor&& others);

            ~acceptor();


    };








}