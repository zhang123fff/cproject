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

    //刚创建时候，需要手动设置所有回调函数，和接收事件，是否block
    //不然会抛出异常或者无效listenfd
    class acceptor{
            using call_back = std::function<void()>;
        private:
            //ch_和sock_在acceptor对象存在区间不为空(外部和内部函数保证)
            //注意ch_,sock_生命周期归acceptor管理
            channel* ch_ = nullptr;
            socket* sock_ = nullptr;

            //必须要有的function，不会判断是否为空
            call_back reading;
            call_back closing;
            call_back error;

            //上层传下来的,必须要有的
            //不能抛出异常
            std::function<bool(socket*, channel*)> newconnection;
        private:
            static bool call_back_copy(call_back& func1, const call_back& func2);

            void closing_handle();

            void error_handle();

            //非阻塞专用
            void reading_handle();

            // 阻塞专用
            void block_reading_handle();

        public:
            int enable_listen_socket_ET();

            int enable_listen_socket_unblock();

            int disable_listen_socket_unblock();

            int disable_listen_socket_ET();

            int enable_listen_socket_read();

            int disable_listen_socket_read();

            bool set_newconnection(const std::function<bool(socket*, channel*)>& op);

            bool set_reading(const call_back& op);

            bool set_closing(const call_back& op);

            bool set_error(const call_back& op);

            bool set_default_block_reading();

            bool set_default_reading();

            bool set_default_closing();

            bool set_default_error();

            bool is_valid()const;

            bool legalize_socket(int fd);

            bool legalize_socket(socket* sock);

            //!!!注意要谨慎使用，监听channel所有权属于acceptor
            channel* get_listen_channel();

            //!!!注意要谨慎使用，监听fd所有权属于acceptor
            int get_listen_fd();


            
        public:
            //待完成
            //acceptor();

            acceptor(socket* sock, channel* ch);

            acceptor(socket* sock);

            acceptor(acceptor&& others)noexcept = delete;

            acceptor(const acceptor&) = delete;

            acceptor& operator=(const acceptor&) = delete;

            acceptor& operator=(acceptor&& others)noexcept = delete;

            ~acceptor();


    };








}