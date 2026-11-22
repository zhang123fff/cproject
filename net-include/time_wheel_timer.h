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
#include"timer.h"
#include"time_wheel.h"

namespace ne{

    class time_wheel_timer{
            using call_back = std::function<void()>;
        private:
            //单位默认秒
            const size_t s_num;
            time_wheel tw_;
            timer tim_;

        public:
            size_t get_cur_idx()const;

            bool is_valid()const;

            bool legalize_timer();

            int get_fd()const;

            channel* get_channel();

            int set_timer();

            int start();

            bool add_task(const call_back& op);

            bool delete_task(size_t id);

            //更新到最新位置
            bool update_task(size_t id);

            int enable_timerfd_read();

            int disable_timerfd_read();

            bool set_default_overtiming();

            bool set_uppercb(const call_back& op);

            bool set_default_uppercb();

            bool set_overtiming(const call_back& op);
        
        public:
            time_wheel_timer(size_t s_num);

            time_wheel_timer(size_t s_num, bool run);

            //time_wheel_timer(bool run);

            time_wheel_timer(const time_wheel_timer&) = delete;

            time_wheel_timer& operator=(const time_wheel_timer&) = delete;

            time_wheel_timer(time_wheel_timer&&) = delete;

            time_wheel_timer& operator=(time_wheel_timer&&) = delete;

            ~time_wheel_timer();

    };









}