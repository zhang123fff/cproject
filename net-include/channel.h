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
    class eventloop;

    class channel{
            using call_back = std::function<void()>;
        private:
            int fd_ = -1;
            uint32_t events_ = 0;
            uint32_t revents_ = 0;

            eventloop* loop = nullptr;

            call_back closing;
            call_back writing;
            call_back reading;
            call_back error;
        private:
            static bool call_back_copy(call_back& func1, const call_back& func2);

            int event_enable(uint32_t ev);

            int event_disable(uint32_t ev);
         
        public:
            uint32_t get_events()const;

            uint32_t get_revents()const;

            eventloop* get_loop()const;

            int enable_read_event();

            int enable_write_event();

            int enable_ET_event();

            int disable_read_event();

            int disable_write_event();

            int disable_ET_event();

            void handling();

            int set_events(uint32_t ev);

            void set_revents(uint32_t ev);

            bool set_closing(const call_back& op);

            bool set_error(const call_back& op);

            bool set_reading(const call_back& op);

            bool set_writing(const call_back& op);
            
            int socket_state()const;

            void set_loop(eventloop* ptr);

            int fd()const;

            void set_fd(int fd);
            
            bool belonging_to_eventloop()const;

        public:
            //channel(int fd, eventloop* ptr);

            channel(int fd);

            channel();

            channel(channel&& others)noexcept = delete;

            channel(const channel& others) = delete;

            channel& operator=(const channel& others) = delete;

            channel& operator=(channel&& others)noexcept = delete;

            ~channel();
        




    };
}