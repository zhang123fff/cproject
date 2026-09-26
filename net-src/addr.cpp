#include"../net-include/addr.h"



namespace ne{
    const std::string& addr::get_ip()const noexcept{
        return ip_;
    }

    in_port_t addr::get_ports()const noexcept{
        return port_;
    }

    sa_family_t addr::get_fa_()const noexcept{
        return fa_;
    }
/*
    bool addr::get_sockaddrs_ipv4_tcp(std::vector<std::pair<socklen_t, sockaddr>>& addrs)const{
        addrinfo* hint;
        hint->ai_family = AF_INET;
        hint->ai_socktype = SOCK_STREAM;
        
        addrinfo* res = nullptr;
        
    }
   
    bool addr::get_sockaddr_ipv4_tcp(sockaddr& ad, socklen_t& len)const{

    }
*/

    int addr::get_po_()const noexcept{
        return po_;
    }

    //只获取string ip_的sockaddr
   
    //*ad需要用户自己释放
    bool addr::get_sockaddr(sockaddr** ad, socklen_t* len)const noexcept{
        if(!ad || !len) return false;
        
        addrinfo* res = nullptr;
        
        std::string port_str = std::to_string(port_);

        //过滤器
        addrinfo hint;
        memset(&hint, 0, sizeof(addrinfo));
        hint.ai_family = fa_;
        hint.ai_socktype = po_;

        if(getaddrinfo(ip_.c_str(), port_str.c_str(), &hint, &res) == 0){
            sockaddr* tmp = (sockaddr*)malloc(res->ai_addrlen);
            if(tmp == nullptr) {
                freeaddrinfo(res);
                return false;
            }
            memcpy(tmp, res->ai_addr, res->ai_addrlen);
            *ad = tmp;
            *len = res->ai_addrlen;
            freeaddrinfo(res);
            return true;
        }
        else return false;
    }
    
    addr::addr(const std::string& ip, in_port_t port, sa_family_t fa, int po){
        ip_ = ip;
        port_ = port;
        fa_ = fa;
        po_ = po;
    }

    addr::addr(const addr& others){
        //可能抛出异常
        ip_ = others.ip_;
        port_ = others.port_;
        fa_ = others.fa_;
        po_ = others.po_;
    }

    addr::addr(addr&& others)noexcept{
        ip_ = std::move(others.ip_);
        port_ = others.port_;
        fa_ = others.fa_;
        po_ = others.po_;
    }

    addr& addr::operator=(const addr& others){
        if (this == &others) return *this;
        //可能抛出异常
        ip_ = others.ip_;
        port_ = others.port_;
        fa_ = others.fa_;
        po_ = others.po_;
        return *this;
    }

    addr& addr::operator=(addr&& others)noexcept{
        if (this == &others) return *this;
        ip_ = std::move(others.ip_);
        port_ = others.port_;
        fa_ = others.fa_;
        po_ = others.po_;
        return *this;
    }

    addr::~addr(){

    }










}