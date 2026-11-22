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
#include"waker.h"
#include"time_wheel_timer.h"
#include"tool/container/container.h"
#include"epoll.h"
#include<atomic>


namespace ne{
    class channel;

    /*
        eventloop的状态
            std::unique_ptr<waker> wa_;
            std::unique_ptr<time_wheel_timer> tim_;
            epoll ep;这三个该分配的空间一定会先分配好（初始）
            但是里面的fd不一定有效
            ep的fd之后一定要设成有效，wa和tim的可选择设置有效
            如果wa和tim有效，那么别忘了加入该epoll里面

    */
    class eventloop{
            static constexpr size_t DEFUALT_WAIT_SIZE = 10; 
            //默认毫秒
            static constexpr int DEFUALT_WAIT_TIME = 100;
            static constexpr size_t DEFAULT_SEC_NUM = 120;
        public:
            using call_back = std::function<void()>;
            enum class init_state{prepared_all, prepared_epoll, prepared_all_and_init};

        private:
            const size_t s_num = DEFAULT_SEC_NUM;
            size_t epoll_wait_size = DEFUALT_WAIT_SIZE;
            int epoll_wait_time = DEFUALT_WAIT_TIME;
            std::atomic<bool> looping = false;
            epoll ep;
            std::map<int, channel*> map;

            //一定要指向有效空间，可选择fd是否有效
            std::unique_ptr<waker> wa_;
            std::unique_ptr<time_wheel_timer> tim_;

        private:
            //tim_有没有效果不影响加入op并且获取唯一的id
            bool add_task_to_time_wheel_timer(const call_back& op, size_t& id);

        public:
            //初始化
            bool init_waker();

            bool init_time_wheel_timer();

            bool is_waker_valid()const;

            bool is_time_wheel_timer_valid()const;

            //合法化
            bool legalize_waker();

            bool legalize_time_wheel_timer();

            //加入epoll
            int add_waker_channel();

            int add_time_wheel_timer_channel();

            //bool time_wheel_timer_initialize();

            //bool waker_initialize();

            void waker_uppercb_handle();

            bool add_waker();///

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

            bool is_valid()const;

            bool legalize_epoll();

        public:
            eventloop(epoll&& ep);

            eventloop();

            eventloop(bool run);

            eventloop(init_state pre);

            ~eventloop();

            eventloop(const eventloop&) = delete;

            eventloop& operator=(const eventloop&) = delete;

            eventloop(eventloop&& others)noexcept = delete;

            eventloop& operator=(eventloop&& others)noexcept = delete;







    };




}