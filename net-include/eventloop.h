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
#include"channel.h"
#include"epoll.h"
#include<atomic>
namespace ne{

    class eventloop{
        private:
            std::atomic<bool> looping;
            int epfd;
            std::unordered_map<int, channel*> ca;
        public:
            void loop();
            void remove_channel(const channel* ptr);
            
            void update_channel(const channel* ptr);
            void quit();
            bool get_looping()const noexcept;
        public:
            eventloop(const epoll& ep)noexcept;
            eventloop(int fd);
            ~eventloop();
            eventloop(const eventloop&) = delete;
            eventloop& operator=(const eventloop&) = delete;
            eventloop(eventloop&& others)noexcept = delete;
            eventloop& operator=(eventloop&& others)noexcept = delete;







    };




}