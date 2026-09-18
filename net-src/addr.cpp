#include"../net-include/addr.h"



namespace ne{
    const std::string& addr::get_ip()const noexcept{
        return ip_;
    }

    short addr::get_ports()const noexcept{
        return port_;
    }

    sa_family_t addr::get_fa_()const noexcept{
        return fa_;
    }

    in_port_t addr::get_po_()const noexcept{
        return po_;
    }

    bool addr::get_sockaddr(sockaddr& ad)const noexcept{
        addrinfo* res = nullptr;
    
        std::string port_str = std::to_string(port_);
        if(getaddrinfo(ip_.c_str(), port_str.c_str(), nullptr, &res) == 0){
            ad = *(res->ai_addr);
            freeaddrinfo(res);
            return true;
        }
        else return false;
    }
    
    addr::addr(const std::string ip, short port, sa_family_t fa, in_port_t po){
        ip_ = ip;
        port_ = port;
        fa_ = fa;
        po_ = po;
    }

    addr::addr(const addr& others){
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