#include "waker.h"

namespace ne{

    void waker::wakingup_handle(){
        
        //唤醒后把数据都读出来
        int flag = 0;
        uint64_t rec = 0;
        while(true){
            flag = ::read(wakeupfd, &rec, sizeof(uint64_t));
            if(flag == -1){
                if(errno == EAGAIN || errno == EWOULDBLOCK) break;
                else if(errno == EINTR) continue;
                else break;
            }
            if(flag == 0) break;
        }

        if(uppercb) uppercb();

    }

    bool waker::call_back_copy(call_back &func1, const call_back &func2){
        try{
            func1 = func2;
        }
        catch(std::exception& ex){
            //错误输出
            fprintf(stderr, "waker::call_back_copy : funtion赋值抛出异常\n");
            fprintf(stderr, ex.what());
            fprintf(stderr, "\n");
            return false;
        }
        return true;
    
    }

    waker::waker(){
        ch_ = std::make_unique<channel>();
        wakeupfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        if(wakeupfd == -1) 
            throw std::runtime_error("waker::waker() : eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC)创造失败");
        
        ch_->set_fd(wakeupfd);
        //ch_->enable_read_event();

    }

    waker::waker(int fd){
        //暂未实现
    }

    waker::waker(bool run){
        ch_ = std::make_unique<channel>();
        if(!run){
            wakeupfd = -1;
            return;
        }

        wakeupfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        if(wakeupfd == -1) 
            throw std::runtime_error("waker::waker(bool run) : eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC)创造失败");
        
        ch_->set_fd(wakeupfd);
    }

    waker::~waker(){
        if(wakeupfd != -1)::close(wakeupfd);
    }

    //返回值＋errno判断情况
    int waker::wakeup(){
        uint64_t data = 1;
        int flag = ::write(wakeupfd, &data, sizeof(uint64_t));
        return flag;
    }

    //失败完全回滚,包含wakingup的赋值和ch_的同步
    bool waker::set_default_wakingup(){
        auto op = [this](){wakingup_handle();};

        if(!ch_->set_reading(op)) return false;

        wakingup = op;

        return true;

    }

    bool waker::set_uppercb(const call_back& op){
        return call_back_copy(uppercb, op);
    }

    //失败完全回滚,包含wakingup的赋值和ch_的同步
    bool waker::set_wakingup(const call_back& op){
        call_back tmp = std::move(wakingup);
        if(!call_back_copy(wakingup, op)){
            wakingup = std::move(tmp);
            return false;
        }
        bool res = ch_->set_reading(wakingup);
        if(!res) wakingup = std::move(tmp);
        return res; 
    }

    void waker::close(){
        ::close(wakeupfd);
        wakeupfd = -1;
        ch_->channel_init();
    }

    void waker::set_fd(){
        //暂无实现
    }

    //返回当前合法状态
    bool waker::legalize_wakeupfd(){
        if(is_valid()) return true;
        wakeupfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        if(wakeupfd == -1) return false;
        
        ch_->set_fd(wakeupfd);
        return true;
    }

    bool waker::is_valid()const{
        return wakeupfd != -1;
    }

    int waker::enable_wakeupfd_read(){
        return ch_->enable_read_event();
    }

    int waker::disable_wakeupfd_read(){
         return ch_->disable_read_event();
    }





}
