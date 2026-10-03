#include"../net-include/buffer.h"

namespace ne{

    buffer::buffer(){

    }

    buffer::buffer(buffer&& others)noexcept{
        buff_ = std::move(others.buff_);
        stage = others.stage;
        bytes_remaining = others.bytes_remaining;
        others.stage = false;
        others.bytes_remaining = 0;
    }
    //string拷贝可能抛出异常
    buffer::buffer(const buffer& others){
        buff_ = others.buff_;
        stage = others.stage;
        bytes_remaining = others.bytes_remaining;
    }

    buffer& buffer::operator=(const buffer& others){
        if(this == &others) return *this;
        buff_ = others.buff_;
        stage = others.stage;
        bytes_remaining = others.bytes_remaining;
        return *this;
    }

    buffer& buffer::operator=(buffer&& others)noexcept{
        if(this == &others) return *this;
        buff_ = std::move(others.buff_);
        stage = others.stage;
        bytes_remaining = others.bytes_remaining;
        others.stage = false;
        others.bytes_remaining = 0;
        return *this;
    }

    buffer::~buffer(){

    }

    void buffer::set_stage(bool st){
        stage = st;
    }

    void buffer::set_bytes_remaining(size_t bytes){
        bytes_remaining = bytes;
    }

    void buffer::next_stage(){
        stage = !stage;
    }

    void buffer::clear_buffer(){
        buff_.clear();
    }

    void buffer::clear_info(){
        stage = false;
        bytes_remaining = 0;
    }

    void buffer::clear_all(){
        clear_info();
        clear_buffer();
    }

    bool buffer::get_stage()const{
        return stage;
    }

    size_t buffer::get_bytes_remaining()const{
        return bytes_remaining;
    }

    const char* buffer::c_str()const{
        return buff_.c_str();
    }

    char& buffer::operator[](size_t idx){
        return buff_[idx];
    }

    const char& buffer::operator[](size_t idx)const{
        return buff_[idx];
    }

    const char* buffer::data()const{
        return buff_.data();
    }

    char* buffer::data(){
        return buff_.data();
    }

    //返回0或者大于0的数
    int buffer::append(const char* msg, size_t len){
        size_t num = buff_.size();
        try{
            buff_.append(msg, len);
        }
        catch(std::exception& ex){
            //输出错误信息
            fprintf(stderr, "buffer::append(const char* msg, size %d) string::append抛出异常\n", len);
            fprintf(stderr, ex.what());
            num = buff_.size() - num;
            return num;
        }
        return len;
    }

    int buffer::append(const std::string& msg, size_t len){
        size_t num = buff_.size();
        try{
            buff_.append(msg, 0, len);
        }
        catch(std::exception& ex){
            //输出错误信息
            fprintf(stderr, "buffer::append(const std::string& msg, size %d) string::append抛出异常\n", len);
            fprintf(stderr, ex.what());
            num = buff_.size() - num;
            return num;
        }
        return len;
    }

    int buffer::append(const std::string& msg){
        return append(msg, msg.size());
    }

    int buffer::append(const char* msg){
        if(!msg) return -1;
        size_t num = buff_.size();
        try{
            buff_.append(msg);
        }
        catch(std::exception& ex){
            //输出错误信息
            fprintf(stderr, "buffer::append(const char* msg)  string::append抛出异常\n");
            fprintf(stderr, ex.what());
        }
        return buff_.size() - num;
    }

    int buffer::assign(const char* msg, size_t len){
        
        try{
            buff_.assign(msg, len);
        }
        catch(std::exception& ex){
            //输出错误信息
            fprintf(stderr, "buffer::write(const char* msg, size_t len)  string::assign抛出异常\n");
            fprintf(stderr, ex.what());
            return 0;
        }
        return len;
    }

    int buffer::assign(const std::string& msg, size_t len){
        try{
            buff_.assign(msg, 0, len);
        }
        catch(std::exception& ex){
            //输出错误信息
            fprintf(stderr, "buffer::write(const std::string& msg, size_t len)  string::assign抛出异常\n");
            fprintf(stderr, ex.what());
            return 0;
        }
        return len;
    }

    int buffer::assign(const char* msg){
        if(!msg) return -1;
        try{
            buff_.assign(msg);
        }
        catch(std::exception& ex){
            //输出错误信息
            fprintf(stderr, "buffer::write(const char* msg)  string::assign抛出异常\n");
            fprintf(stderr, ex.what());
            return 0;
        }
        return buff_.size();
    }

    int buffer::assign(const std::string& msg){
        try{
            buff_.assign(msg);
        }
        catch(std::exception& ex){
            //输出错误信息
            fprintf(stderr, "buffer::write(const std::string& msg)  string::assign抛出异常\n");
            fprintf(stderr, ex.what());
            return 0;
        }
        return msg.size();
    }
    
    //msg为c风格字符串
    //read函数不检查msg实际容量
    //msg容量至少为len + 1
    int buffer::read(char* msg, size_t len){
        if(!msg) return -1;
        int bytes = len;
        if(len >= size()) bytes = size();
        memcpy(msg, c_str(), bytes);
        *(msg + bytes) = '\0';
        return bytes;
    }

    int buffer::read(std::string& msg, size_t len){
        size_t res = len;
        if(len >= size()) res = size();
        size_t count = msg.size();
        try{
            msg.append(c_str(), res);
        }
        catch(std::exception& ex){
            //输出错误信息
            fprintf(stderr, "buffer::read(std::string& msg, size_t len)  string::append抛出异常\n");
            fprintf(stderr, ex.what());
            count = msg.size() - count;
            return count;
        }
        
        return res;
    }
    
    /*
    //废弃函数
    //msg为c风格字符串
    int buffer::read(char* msg){
        if(!msg) return -1;
        memcpy(msg, c_str());
        strcpy(msg, c_str());
        return size();
    }
    */
    int buffer::read(std::string& msg){
        size_t count = msg.size();
        try{
            msg.append(c_str(), size());
        }
        catch(std::exception& ex){
            //输出错误信息
            fprintf(stderr, "buffer::read(std::string& msg)  string::append抛出异常\n");
            fprintf(stderr, ex.what());
            count = msg.size() - count;
            return count;
        }
        
        return size();
    }

    bool buffer::empty()const{
        return buff_.empty();
    }

    size_t buffer::size()const{
        return buff_.size();
    }

    int buffer::get_netinfo_pack_size()const{
        if(size() >= HEAD_SIZE){
            return get_head_size();
        }
        return -1;
    }

    uint16_t buffer::get_netinfo_pack_size_no_check()const{
        return get_head_size();
    }

    int buffer::get_netinfo_pack(){
        //待实现
        return -1;
    }

    //!!无任何检查,注意使用
    //网络端到主机端字节序变化
    uint16_t buffer::get_head_size()const{
       uint16_t res = 0;
       memcpy(&res, get_head_address(), HEAD_SIZE);
       return ntohs(res);
    }

    //返回const uint16_t*首地址
    const void* buffer::get_head_address()const{
        return (const void*)(data());
    }

    //注意使用，无检查
    //不设置stage和bytes_remaining
    void buffer::move_bytes_out_of_buff(std::string& buf){
        buf = std::move(buff_);
    }

    //注意使用，无检查
    //不设置stage和bytes_remaining
    void buffer::move_bytes_overwrite_buff(std::string&& buf){
        buff_ = std::move(buf);
    }
}