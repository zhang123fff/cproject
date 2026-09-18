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
class eventloop;
namespace ne{
    
    class channel{
            using call_back = std::function<void()>;
        private:
            int fd_ = -1;
            uint32_t events_ = 0;
            uint32_t revents_ = 0;

            eventloop* back = nullptr;
            bool adding = false;
            
            call_back closing;
            call_back writing;
            call_back reading;
            call_back error;
        public:
            void handling();
            void set_events(uint32_t ev)noexcept;
            void set_revents(uint32_t ev)noexcept;
            void set_closing(const call_back& op);
            void set_error(const call_back& op);
            void set_reading(const call_back& op);
            void set_writing(const call_back& op);
            int set_nonblock()noexcept;
            int set_block()noexcept;
            int socket_state()const noexcept;
            bool is_adding()const noexcept;
            int fd()const noexcept;
        public:
            channel(int fd);
            channel();
            channel(channel&& others)noexcept;
            channel(const channel& others) = delete;
            channel& operator=(const channel& others) = delete;
            channel& operator=(channel&& others)noexcept;

            ~channel();
        




    };
}