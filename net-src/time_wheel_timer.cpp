#include "time_wheel_timer.h"







namespace ne{

    //以秒为单位
    time_wheel_timer::time_wheel_timer(size_t s_num)
        : s_num(s_num), 
        tw_(1, s_num){
        
        
    }

    time_wheel_timer::time_wheel_timer(size_t s_num, bool run)
        : s_num(s_num), 
        tw_(1, s_num),
        tim_(run){

    }

    time_wheel_timer::~time_wheel_timer(){
        
    }

    size_t time_wheel_timer::get_cur_idx() const{
        return tw_.get_cur_idx();
    }

    bool time_wheel_timer::is_valid() const
    {
        return tim_.is_valid();
    }

    bool time_wheel_timer::legalize_timer(){
        return tim_.legalize_timerfd();
    }

    int time_wheel_timer::set_timer()
    {
        itimerspec its;
        its.it_interval.tv_sec = 1;
        its.it_interval.tv_nsec = 0;
        its.it_value.tv_nsec = 1;
        its.it_value.tv_sec = 0;
        return tim_.set_timer(its);
    }

    int time_wheel_timer::start(){
        return set_timer();
    }

    bool time_wheel_timer::add_task(const call_back& op){
        return tw_.add_task(op);
    }

    bool time_wheel_timer::delete_task(size_t id){
        return tw_.delete_task(id);
    }

    //更新到最新位置
    bool time_wheel_timer::update_task(size_t id){
        return tw_.update_task(id);
    }

    int time_wheel_timer::get_fd()const{
        return tim_.get_fd();
    }

    channel* time_wheel_timer::get_channel(){
        return tim_.get_channel();
    }

    int time_wheel_timer::enable_timerfd_read(){
        return tim_.enable_timerfd_read();
    }

    int time_wheel_timer::disable_timerfd_read(){
        return tim_.disable_timerfd_read();
    }

    bool time_wheel_timer::set_default_overtiming(){
        return tim_.set_default_overtiming();
    }

    bool time_wheel_timer::set_uppercb(const call_back& op){
        return tim_.set_uppercb(op);
    }

    bool time_wheel_timer::set_default_uppercb(){
        return tim_.set_uppercb([this](){tw_.tick();});
    }

    bool time_wheel_timer::set_overtiming(const call_back& op){
        return tim_.set_overtiming(op);
    }

}
