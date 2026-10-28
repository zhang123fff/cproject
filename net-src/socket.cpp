#include"../net-include/socket.h"


namespace ne{

    socket::socket(){
        
    }

    //注意这是fd的所有权转移
    socket::socket(int fd){
        fd_ = fd;
    }

    socket::socket(sa_family_t fa, int po){
        fd_ = ::socket(fa, po, 0);
        //if(fd == -1)throw std::runtime_error(std::string("socket获取失败,errno : ") + strerror(errno));
    }
   
    //默认bind
    socket::socket(const addr& ad){
        fd_ = ::socket(ad.get_fa_(), ad.get_po_(), 0);
        if(fd_ == -1) throw std::runtime_error(
            "socket::socket(const addr& ad) : fd为-1");

        
      
        if(bind(ad) == -1) {
            ::close(fd_);
            throw std::runtime_error(
                "socket::socket(const addr& ad) : socket bind失败");
        }
        
    }

    //默认bind
    socket::socket(sa_family_t fa, int po, const std::string& ip, in_port_t port_num){
        fd_ = ::socket(fa, po, 0);
        if(fd_ == -1) throw std::runtime_error(
            "socket::socket(sa_family_t fa, int po, const std::string& ip, in_port_t port_num) : fd为-1");

        addr ad;
        
        if(!ad.write_all_info_into_addr(ip, port_num, fa, po)) {
            ::close(fd_);
            throw std::runtime_error(
                "socket::socket(sa_family_t fa, int po, const std::string& ip, in_port_t port_num) : ip信息写入失败");
        }
        
        if(bind(ad) == -1) {
            ::close(fd_);
            throw std::runtime_error(
                "socket::socket(sa_family_t fa, int po, const std::string& ip, in_port_t port_num) : socket bind失败");
        }

        
    }

    socket::socket(sa_family_t fa, int po, const std::string& ip, in_port_t port_num, bool is_acc){
        fd_ = ::socket(fa, po, 0);
        if(fd_ == -1) throw std::runtime_error(
            "socket::socket(sa_family_t fa, int po, const std::string& ip, in_port_t port_num, bool is_acc) : fd为-1");

        addr ad;
        if(!ad.write_all_info_into_addr(ip, port_num, fa, po)){
            ::close(fd_);
            throw std::runtime_error(
                "socket::socket(sa_family_t fa, int po, const std::string& ip, in_port_t port_num, bool is_acc) : ip信息写入失败");
        }

        if(is_acc){
            if(bind(ad) == -1) {
                ::close(fd_);
                throw std::runtime_error(
                    "socket::socket(sa_family_t fa, int po, const std::string& ip, in_port_t port_num) : socket bind失败");
            }
        }
        else{
            if(connect(ad) == -1) {
                ::close(fd_);
                throw std::runtime_error(
                    "socket::socket(sa_family_t fa, int po, const std::string& ip, in_port_t port_num, bool is_acc) : socket connect失败");
            }
            
        }
    }

    socket::socket(const addr& ad, bool is_acc){
        fd_ = ::socket(ad.get_fa_(), ad.get_po_(), 0);
        if(fd_ == -1) throw std::runtime_error(
            "socket::socket(const addr& ad, bool is_acc) : fd为-1");

        if(is_acc){
            if(bind(ad) == -1) {
                ::close(fd_);
                throw std::runtime_error(
                    "socket::socket(const addr& ad, bool is_acc) : socket bind失败");
            }
        }
        else{
            if(connect(ad) == -1) {
                ::close(fd_);
                throw std::runtime_error(
                    "socket::socket(const addr& ad, bool is_acc) : socket connect失败");
            }
            
        }

    }

    socket::~socket(){
        if(fd_ != -1) ::close(fd_);
    }
    
    //socket::socket(const socket& others) = delete;
    socket::socket(socket&& others)noexcept{
        fd_ = others.fd_;
        others.fd_ = -1;
    }
    
    //socket& operator=(const socket& others) = delete;
    socket& socket::operator=(socket&& others)noexcept{
        if(this == &others) return *this;
        if(fd_ != -1) ::close(fd_);
        fd_ = others.fd_;
        others.fd_ = -1;
        return *this;
    }
    
    //注意fd只能暴露给某些类型，谨慎使用
    int socket::get_fd()const noexcept{
        return fd_;
    }

    //
    int socket::bind(const addr& ad)noexcept{
        if(!is_valid()) return -1;
        sockaddr* tmp = nullptr;
        socklen_t len = 0;

        if(!ad.get_sockaddr(&tmp, &len)) return -1;
        int res = ::bind(fd_, tmp, len);
        free(tmp);
        return res;
    }

    int socket::connect(const addr& ad)noexcept{
        if(!is_valid()) return -1;
        sockaddr* tmp = nullptr;
        socklen_t len = 0;
        if(!ad.get_sockaddr(&tmp, &len)) return -1;
        int res = ::connect(fd_, tmp, len);
        free(tmp);
        return res;
    }
    
    //待完善
    //返回0代表对端关闭
    //返回-1代表参数错误，fd非法，resize抛出异常，第一次调用::recv()
    //大于0代表读取了res个字节，但无法知道最后一次调用::recv()返回什么值
    ssize_t socket::recv_all(std::string& meg, size_t num){
        if(!is_valid() || num == 0) return -1;
        try{
            meg.resize(num);
        }
        catch(std::exception& ex){
            //补上错误输出
            return -1;
        }
        ssize_t res = 0;
        ssize_t flag = 1;
        bool fir = false;
        while(res != num){
            flag = ::recv(fd_, (void*)meg.data() + res, num - res, 0);
            if(flag <= 0) {
                if(flag == -1 && errno == EINTR) continue;
                if(!fir) return flag;

                return res;
            }            
            res += flag;
            fir = true;
        }
        return res;
    }

    //待完善
    //返回值代表实际发送，为meg的长度
    //可能会失败
    //非阻塞，send失败则返回，不失败则全部发完
    ssize_t socket::send_all(const std::string& meg)noexcept{
        if(!is_valid()) return -1;
        ssize_t res = 0;
        ssize_t flag = 1;
        while(res != meg.size()){
            
            flag = ::send(fd_, (const void*)meg.c_str() + res, meg.size() - res, MSG_NOSIGNAL);
            if(flag <= 0){
                //补上错误输出
                if(flag == -1 && errno == EINTR) continue;
                return res;
            }
            res += flag;

            
        }
        return res;
    }

    /*废弃函数
    //！！！is_closing代表对端是否关闭，传进来前一定要初始化
    //返回-1，表示参数传错（num == 0），fd不合法，meg扩容失败
    //返回大于等于0的值代表读取了多少字节
    //返回值大于0的时候具体错误可以看errno，errno代表最后一次执行recv的错误
    //待完成
    ssize_t socket::recv_all(std::string& meg, size_t num, bool& is_closing){
        if(!is_valid() || num == 0) return -1;
        try{
            meg.resize(num);
        }
        catch(std::exception& ex){
            //补上错误输出
            return -1;
        }
        ssize_t res = 0;
        ssize_t flag = 1;
        while(res != num){
            flag = ::recv(fd, (void*)meg.data() + res, num - res, 0);
            if(flag <= 0){
                if(flag == -1 && errno == EINTR) continue;
                //要区分-1，0还有errno
                //补上错误输出
                if(flag == 0) is_closing = true;
                meg.resize(res);
                return res;
            }
            res += flag;
        }
        meg.resize(res);
        return res;
    }
    */

    int socket::listen(int max)noexcept{
        return ::listen(fd_, max);
    }

    int socket::accept(sockaddr* so_ad, socklen_t* len)noexcept{
        return ::accept(fd_, so_ad, len);
    }

    int socket::set_non_block()noexcept{
        int flag = fcntl(fd_, F_GETFL, 0);
        if(flag == -1)return -1;
        flag = fcntl(fd_, F_SETFL, flag | O_NONBLOCK);
        return flag; 
    }

    int socket::set_block()noexcept{
        int flag = fcntl(fd_, F_GETFL, 0);
        if(flag == -1)return -1;
        flag = fcntl(fd_, F_SETFL, flag & ~O_NONBLOCK);
        return flag; 
    }

    int socket::create_socket(sa_family_t fa, int po){
        if(!is_valid()){
            fd_ = ::socket(fa, po, 0);
        }
        return fd_;
    }

    ssize_t socket::send(const void *__buf, size_t __n, int __flags){
        return ::send(fd_, __buf,  __n,  __flags);
    }

    ssize_t socket::recv(void *__buf, size_t __n, int __flags){
        return ::recv(fd_, __buf,  __n,  __flags);
    }

    int socket::close(){
        int res = ::close(fd_);
        fd_ = -1;
        return res;
    }

    bool socket::is_valid()const{
        return fd_ != -1;
    }

    //不管fd是什么都直接赋值
    //外部保证fd的为-1或者有效的fd
    void socket::set_fd(int fd){
        ::close(fd_);
        fd_ = fd;
    }

    int socket::is_blocking() const{
        int flag = fcntl(fd_, F_GETFL, 0);
        if(flag == -1) return -1;
        
        return (flag & O_NONBLOCK) == 0;
    }

    int socket::get_socket_error(int& res){
        socklen_t len = sizeof(res);
        return getsockopt(fd_, SOL_SOCKET, SO_ERROR, &res, &len);
    }

}