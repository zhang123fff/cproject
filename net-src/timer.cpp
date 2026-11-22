#include "timer.h"




namespace ne{

    timer::timer(){
        ch_ = std::make_unique<channel>();
        timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
        ch_->set_fd(timerfd);
        memset(&timer_setting, 0, sizeof(itimerspec));
        //ch_->enable_read_event();
    }

    timer::timer(int fd){
        //暂未实现
    }

    timer::timer(bool run){
        ch_ = std::make_unique<channel>();
        memset(&timer_setting, 0, sizeof(itimerspec));
        if(!run){
            timerfd = -1;
            return;
        }

        timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
        if(timerfd == -1) 
            throw std::runtime_error("timer::timer(bool run) : timerfd_create创造失败");
        
        ch_->set_fd(timerfd);
    }

    timer::~timer(){
        if(timerfd != -1)::close(timerfd);
    }

    void timer::overtiming_handle(){
        int flag = 0;
        uint64_t rec = 0;
        while(true){
            flag = ::read(timerfd, &rec, sizeof(uint64_t));
            if(flag == -1){
                if(errno == EAGAIN || errno == EWOULDBLOCK) break;
                else if(errno == EINTR) continue;
                else break;
            }
            if(flag == 0) break;
        }

        if(uppercb) uppercb();
    }

    bool timer::call_back_copy(call_back &func1, const call_back &func2){
        try{
            func1 = func2;
        }
        catch(std::exception& ex){
            //错误输出
            fprintf(stderr, "timer::call_back_copy : funtion赋值抛出异常\n");
            fprintf(stderr, ex.what());
            fprintf(stderr, "\n");
            return false;
        }
        return true;
    }

    bool timer::legalize_timerfd(){
        if(is_valid()) return true;
        timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
        if(timerfd == -1) return false;
        
        ch_->set_fd(timerfd);
        return true;
    }

    bool timer::is_valid()const{
        return timerfd != -1;
    }

    int timer::enable_timerfd_read(){
        return ch_->enable_read_event();
    }

    int timer::disable_timerfd_read(){
        return ch_->disable_read_event();
    }

    bool timer::set_default_overtiming(){
        auto op = [this](){overtiming_handle();};

        if(!ch_->set_reading(op)) return false;

        overtiming = op;

        return true;
    }

    //内部更新一定会成功，但是timerfd_settime
    int timer::set_timer(const itimerspec& its){
        timer_setting = its;
        return ::timerfd_settime(timerfd, 0, &timer_setting, nullptr);
    }

    int timer::set_timer(){
        return ::timerfd_settime(timerfd, 0, &timer_setting, nullptr);
    }

    bool timer::set_uppercb(const call_back& op){
        return call_back_copy(uppercb, op);
    }

    bool timer::set_overtiming(const call_back& op){
        call_back tmp = std::move(overtiming);
        if(!call_back_copy(overtiming, op)){
            overtiming = std::move(tmp);
            return false;
        }
        bool res = ch_->set_reading(overtiming);
        if(!res) overtiming = std::move(tmp);
        return res; 
    }

    void timer::close(){
        ::close(timerfd);
        timerfd = -1;
        ch_->channel_init();
    }

    void timer::set_fd(){
        //暂无实现
    }

    itimerspec timer::get_timer_setting()const{
        return timer_setting;
    }

    int timer::get_fd()const{
        return timerfd;
    }

    channel* timer::get_channel(){
        return ch_.get();
    }


}
