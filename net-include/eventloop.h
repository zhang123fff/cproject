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

#include"epoll.h"
#include<atomic>


namespace ne{
    class channel;

    class eventloop{
        static constexpr size_t DEFUALT_WAIT_SIZE = 10; 
        //默认毫秒
        static constexpr int DEFUALT_WAIT_TIME = 100;
        private:
            size_t epoll_wait_size = DEFUALT_WAIT_SIZE;
            int epoll_wait_time = DEFUALT_WAIT_TIME;
            std::atomic<bool> looping = false;
            epoll ep;
            std::map<int, channel*> map;
        public:
            bool loop();

            int remove_channel(int fd);

            int add_channel(int fd, channel* ptr);
            
            int update_channel(int fd, channel* ptr);

            void quit();

            void set_looping(bool flag);

            bool get_looping()const noexcept;
            
            void set_epoll_wait_size(size_t siz);

            size_t get_epoll_wait_size()const;

            void set_epoll_wait_time(int time);

            int get_epoll_wait_time()const;
        public:
            eventloop(epoll&& ep);

            eventloop();

            ~eventloop();

            eventloop(const eventloop&) = delete;

            eventloop& operator=(const eventloop&) = delete;

            eventloop(eventloop&& others)noexcept = delete;

            eventloop& operator=(eventloop&& others)noexcept = delete;







    };




}