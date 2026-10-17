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
#include<cstring>

namespace ne{

    class buffer{
            //头部字节数
            static constexpr uint16_t HEAD_SIZE = 2;
        private:
            std::string buff_;
            bool stage = false;
            size_t bytes_remaining = HEAD_SIZE;

        private:
            uint16_t get_pack_size()const;

            const void* get_head_address()const;

        public:
            void set_stage(bool st);

            void set_bytes_remaining(size_t bytes);

            void next_stage();

            void clear_buffer();

            void clear_info();

            void clear_all();

            bool get_stage()const;

            size_t get_bytes_remaining()const;

            static constexpr uint16_t get_head_size();

            static uint16_t static_get_netinfo_pack_size(const void* mem);
            
            int get_netinfo_pack_size()const;
            //待完善
            void move_bytes_out_of_buff(std::string& buf);
            //待完善
            void move_bytes_overwrite_buff(std::string&& buf);

            uint16_t get_netinfo_pack_size_no_check()const;

            int get_netinfo_pack();

            const char* c_str()const;

            char& operator[](size_t idx);

            const char& operator[](size_t idx)const;

            const char* data()const;

            char* data();

            int append(const char* msg, size_t len);

            int append(const std::string& msg, size_t len);

            int append(const std::string& msg);

            int append(const char* msg);

            int assign(const char* msg, size_t len);

            int assign(const std::string& msg, size_t len);

            int assign(const char* msg);

            int assign(const std::string& msg);

            int assign(std::string&& msg);

            int read(char* msg, size_t len);

            int read(std::string& msg, size_t len);

            bool append_substr(std::string& str, size_t front_idx, size_t num);

            bool assign_substr(std::string& str, size_t front_idx, size_t num);

            //int read(char* msg);

            int read(std::string& msg);

            void bytes_remaining_increase(size_t bytes);

            void bytes_remaining_decrease(size_t bytes);

            bool resize(size_t siz);

            bool reserve(size_t siz);

            bool extend_size(size_t siz);

            bool extend_capacity(size_t siz);

            bool empty()const;

            size_t size()const;

        public:
            buffer();

            buffer(buffer&& others)noexcept;

            buffer(const buffer& others);

            buffer& operator=(const buffer& others);

            buffer& operator=(buffer&& others)noexcept;

            ~buffer();

    };





}