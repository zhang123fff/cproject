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

namespace ne{

    //刚创建时候，需要手动设置所有回调函数，和接收事件，是否block
    //不然会抛出异常或者无效fd
    class tcpconnection{
            using call_back = std::function<void()>;
            using pack_protocol = std::function<void(std::string&&, std::string&)>;
        private:
            //ch_和sock_在tcpconnection对象存在区间不为空(外部和内部函数保证)
            //注意ch_,sock_生命周期归tcpconnection管理
            socket* sock_;
            channel* ch_;

            //output_buff用不到成员stage和bytes_remaining
            buffer output_buff;
            buffer input_buff;

            //必须要有的function，不会判断是否为空
            call_back reading;
            call_back writing;
            call_back closing;
            call_back error;

            //上层传递协议，处理pack,必须要有的function
            pack_protocol pack_handling;

            //可有可无function
            //不能抛异常
            call_back pack_handle_error;
        private:
            //目前只能用于reading_handle
            void write_result_to_output_buff(std::string&& str);

            //目前只能用于reading_handle
            void recv_part_of_content_done(uint16_t siz);

            //目前只能用于reading_handle
            bool execute_pack_handling_with_catch(std::string&& sen, std::string& rec);

            //目前只能用于reading_handle
            void recv_pack_content_done();

            //目前只能用于reading_handle
            void recv_pack_head_done(uint16_t& siz);

            static bool call_back_copy(call_back& func1, const call_back& func2);

            void closing_handle();

            void error_handle();

            void reading_handle();

            void writing_handle();

        public:
            int enable_socket_write();

            int disable_socket_write();

            int enable_socket_ET();

            int enable_socket_unblock();

            int disable_socket_unblock();

            int disable_socket_ET();

            int enable_socket_read();

            int disable_socket_read();

            void set_default_reading();

            void set_default_writing();

            void set_default_closing();

            void set_default_error();

            bool set_reading(const call_back& op);

            bool set_closing(const call_back& op);

            bool set_error(const call_back& op);

            bool set_writing(const call_back& op);

            bool is_valid()const;
                
            bool set_pack_handling(const pack_protocol& op);

            bool set_pack_handle_error(const call_back& op);

            //自动计算packsize
            bool overwrite_responce_into_output_buff(const std::string& data);
            //自动计算packsize
            bool append_responce_into_output_buff(const std::string& data);

            bool legalize_socket(int fd);

            bool legalize_socket(socket* sock);

            //!!!注意要谨慎使用，channel所有权属于acceptor
            channel* get_channel();

            //!!!注意要谨慎使用，fd所有权属于acceptor
            int get_fd();


        public:
            tcpconnection(socket* sock, channel* ch);

            tcpconnection(socket* sock);

            tcpconnection(tcpconnection&& others)noexcept = delete;

            tcpconnection(const tcpconnection&) = delete;

            tcpconnection& operator=(const tcpconnection&) = delete;

            tcpconnection& operator=(tcpconnection&& others)noexcept = delete;

            ~tcpconnection();

    };














}