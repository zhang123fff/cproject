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
    //容量充足且参数正确的情况下不会失败
    //len长度的字节流全部都可以append
    int buffer::append(const char* msg, size_t len){
        if(!msg) return -1;
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

    //len为0时候返回true
    //容量充足且参数正确的情况下不会失败
    //len长度的字节流全部都可以append
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

    //len为0时候返回true
    //容量充足且参数正确的情况下不会失败
    //字节流全部都可以append
    int buffer::append(const std::string& msg){
        return append(msg, msg.size());
    }

    //容量充足且参数正确的情况下不会失败
    //字节流全部都可以append
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

    //容量充足且参数正确的情况下不会失败
    //len长度的字节流全部都可以assign
    int buffer::assign(const char* msg, size_t len){
        if(!msg) return -1;
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

    //容量充足且参数正确的情况下不会失败
    //len长度的字节流全部都可以assign
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

    //容量充足且参数正确的情况下不会失败
    //字节流全部都可以assign
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

    //容量充足且参数正确的情况下不会失败
    //字节流全部都可以assign
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

    size_t buffer::capacity() const{
        return buff_.capacity();
    }

    int buffer::get_netinfo_pack_size()const{
        if(size() >= HEAD_SIZE){
            return get_pack_size();
        }
        return -1;
    }

    uint16_t buffer::get_netinfo_pack_size_no_check()const{
        return get_pack_size();
    }

    int buffer::get_netinfo_pack(){
        //待实现
        return -1;
    }

    //!!无任何检查,注意使用
    //网络端到主机端字节序变化
    uint16_t buffer::get_pack_size()const{
       uint16_t res = 0;
       memcpy(&res, get_head_address(), HEAD_SIZE);
       return ntohs(res);
    }

    //返回const uint16_t*首地址
    const void* buffer::get_head_address()const{
        return (const void*)(data());
    }

    void buffer::exchange_buff_memory(std::string& str){
        std::string tmp;
        tmp = std::move(buff_);
        buff_ = std::move(str);
        str = std::move(tmp);
    }

    void buffer::set_pack_bytes(uint16_t len)
    {
        pack_bytes = len;
    }

    uint16_t buffer::get_pack_bytes() const{
        return pack_bytes;
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

    constexpr uint16_t buffer::get_head_size(){
        return HEAD_SIZE;
    }

    //不检查越界
    void buffer::bytes_remaining_increase(size_t bytes){
        bytes_remaining += bytes;
    }

    //不检查越界
    void buffer::bytes_remaining_decrease(size_t bytes){
        bytes_remaining  -= bytes;
    }

    //不检查mem，注意不要越界，不要为nullptr
    uint16_t buffer::static_get_netinfo_pack_size(const void* mem){
        uint16_t res = 0;
        memcpy(&res, mem, HEAD_SIZE);
        return ntohs(res);
    }

    int buffer::assign(std::string&& msg){
        buff_ = std::move(msg);
        return buff_.size();
    }

    //谨慎使用，可能会丢失数据
    bool buffer::resize(size_t siz){
        try{
            buff_.resize(siz);
        }
        catch(std::exception& ex){
            //输出错误信息
            fprintf(stderr, "buffer::resize(size_t siz) : string::resize 抛出异常\n");
            fprintf(stderr, ex.what());
            fprintf(stderr, "\n");
            return false;
        }
        return true;
    }

    //谨慎使用，可能会丢失数据
    bool buffer::reserve(size_t siz){
        try{
            buff_.reserve(siz);
        }
        catch(std::exception& ex){
            //输出错误信息
            fprintf(stderr, "buffer::reserve(size_t siz) : string::reserve 抛出异常\n");
            fprintf(stderr, ex.what());
            fprintf(stderr, "\n");
            return false;
        }
        return true;
    }

    bool buffer::extend_size(size_t siz){
        return resize(siz + buff_.size());
    }

    bool buffer::extend_capacity(size_t siz){
        return reserve(siz + buff_.capacity());
    }

    //导出buff_的substr
    //返回false，先reserve 再 append可以完全回滚
    bool buffer::append_substr(std::string& str, size_t pos, size_t len){
        try{
            //参数检查
            if(buff_.size() <= pos) return false;
            if((pos + len) >= buff_.size()) len = buff_.size() - pos;
            
            str.reserve(len + str.size());

            //此处可优化，append内部会再检查一次参数
            str.append(buff_, pos, len);
        }
        catch(std::exception& ex){
            //输出错误信息
            fprintf(stderr, "buffer::append_substr抛出异常\n");
            fprintf(stderr, ex.what());
            fprintf(stderr, "\n");
            return false;
        }
        return true;
    }

    //导出buff_的substr
    //返回false，可以完全回滚
    bool buffer::assign_substr(std::string& str, size_t pos, size_t len){
        try{
            str.assign(buff_, pos, len);
        }
        catch(std::exception& ex){
            //输出错误信息
            fprintf(stderr, "buffer::assign_substr抛出异常\n");
            fprintf(stderr, ex.what());
            fprintf(stderr, "\n");
            return false;
        }
        return true;
    }

    

}