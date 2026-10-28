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
            static constexpr sa_family_t FA_ = AF_INET;
            static constexpr in_port_t DEFAULT_PORT_NUM = 8080;
        public:
            enum class server_state{init_state, unknown_state, rescource_state, running_state};

        private:
            std::atomic<server_state> state;
            size_t thread_num;
            std::unique_ptr<acceptor> acc_;
            std::unique_ptr<eventloop> lo_;
            std::map<int, std::shared_ptr<tcpconnection>> cons_;
            std::map<std::thread::id, std::shared_ptr<eventloop>> loops;
            tool::threadpool po_;
            //必须要有的function，不会判断是否为空
            call_back newconnection;

            //上层传递协议，处理pack,必须要有的function
            pack_protocol pack_handling;

            //可有可无function
            //不能抛异常
            call_back pack_handle_error;

        private:
            bool create_all_resource();

            bool create_lacked_resource();

            static bool call_back_copy(call_back& func1, const call_back& func2);

            void set_default_newconnection();

            void default_newconnection();

            //不包括pack_handling，pack_handle_error
            //bool is_lacked_resource();

            //bool assgin_newconnection_to_eventloop();

        public:

            bool set_pack_handle(const pack_protocol& op);

            bool set_pack_handle_error(const call_back& op);

            bool run();

            bool is_init_state()const;

            bool is_rescource_state()const;

            bool is_running_state()const;

            bool is_unknown_state()const;

            void next_state();

            void set_state(server_state sta);

        public:
            tcpserver();

            tcpserver(size_t thread_num, bool running);

            tcpserver(const tcpserver&) = delete;

            tcpserver& operator=(const tcpserver&) = delete;

            tcpserver(tcpserver&&) = delete;

            tcpserver& operator=(tcpserver&&) = delete;

            










    };
}