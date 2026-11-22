#include<map>
#include<string>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<vector>
#include<queue>
#include<functional>
#include<atomic>
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
#include<memory.h>
#include"channel.h"
#include <sys/eventfd.h>
#include<sys/timerfd.h>

namespace ne{

    class timer{
            using call_back = std::function<void()>;
        private:
            int timerfd;
            std::unique_ptr<channel> ch_;
            itimerspec timer_setting;

            //必须要有的
            call_back overtiming;

            //可有可无
            //不能抛出异常
            call_back uppercb;

        private:
            void overtiming_handle();

            static bool call_back_copy(call_back& func1, const call_back& func2);

        public:
            bool legalize_timerfd();

            bool is_valid()const;

            int enable_timerfd_read();

            int disable_timerfd_read();

            bool set_default_overtiming();

            int set_timer(const itimerspec& its);

            int set_timer();

            bool set_uppercb(const call_back& op);

            bool set_overtiming(const call_back& op);

            void close();

            void set_fd();

            itimerspec get_timer_setting()const;

            int get_fd()const;

            channel* get_channel();
            
        public:
            timer();

            timer(int fd);

            timer(bool run);

            timer(const timer&) = delete;

            timer& operator=(const timer&) = delete;

            timer(timer&&) = delete;

            timer& operator=(timer&&) = delete;

            ~timer();





    };









}