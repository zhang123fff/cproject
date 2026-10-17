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
#include"tcpconnection.h"
#include"acceptor.h"
#include"eventloop.h"
#include"tool/threadpool.h"

namespace ne{

    class tcpserver{
            using call_back = std::function<void()>;
            using pack_protocol = std::function<void(std::string&&, std::string&)>;
            static constexpr size_t DEFAULT_THREAD_SIZE = 16;
        private:
            acceptor acc_;
            eventloop lo_;
            std::map<int, std::shared_ptr<tcpconnection>> cons_;
            tool::threadpool po_;
            //必须要有的function，不会判断是否为空
            call_back newconnection;

            //上层传递协议，处理pack,必须要有的function
            pack_protocol pack_handling;

            //可有可无function
            //不能抛异常
            call_back pack_handle_error;

        public:
            tcpserver();

            










    };
}