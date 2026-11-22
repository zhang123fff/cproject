#include"../net-include/eventloop.h"
#include"../net-include/channel.h"

namespace ne{

    //参数设置错误，抛出异常会返回false
    //while循环完成则返回true，包括0次循环
    bool eventloop::loop(){
        if(!ep.is_valid() || epoll_wait_size == 0) return false;
        std::unique_ptr<epoll_event[]> evs;
        const int maxsize = epoll_wait_size;
        int actual_size = epoll_wait_size;
        try{
            evs = std::make_unique<epoll_event[]>(maxsize);
        }
        catch(std::exception& ex){
            //补充错误输出
            return false;
        }
        int res = 0;
        channel* tmp = nullptr;
        
        //actual_size必须小于maxsize
        while(get_looping()){
            res = ep.epoll_wait(evs.get(), actual_size, epoll_wait_time);
            if(res > 0){
                for(size_t i = 0; i < res; i++){
                    tmp = static_cast<channel*>(evs[i].data.ptr);
                    tmp->set_revents(evs[i].events);
                    tmp->handling();
                }
            }
            else if(res == -1){
                //perror("epoll_wait");输出错误信息
                continue;
            }
            else if(res == 0){
                //输出错误信息
                continue;
            
            }
        }
        return true;
    }

    void eventloop::set_looping(bool flag){
        looping.store(flag, std::memory_order_release);
    }

    void eventloop::set_epoll_wait_size(size_t siz){
        epoll_wait_size = siz;
    }

    size_t eventloop::get_epoll_wait_size()const{
        return epoll_wait_size;
    }

    void eventloop::set_epoll_wait_time(int time){
        epoll_wait_time = time;
    }

    int eventloop::get_epoll_wait_time()const{
        return epoll_wait_time;
    }
    
    //失败返回 -1， 成功返回 0
    //加入epoll和map对应的channel
    //失败原因  1,参数错误（ptr == nullptr, fd不存在或已经加入epoll），
    //2,epoll::epoll_ctl失败
    //3，map::insert抛出异常
    //成功的话channel的loop指针要同步
    int eventloop::add_channel(int fd, channel* ptr){
        if(!ptr) return -1;

        epoll_event ev;
        ev.events = ptr->get_events();
        ev.data.ptr = ptr;
        std::pair<std::map<int, channel*>::iterator, bool> par;
        try{
            par = map.insert({fd, ptr});
        }
        catch(std::exception& ex){           
            //输出错误信息
            return -1;
        }
        //fd已经存在
        if(!par.second) return -1;

        int res = ep.epoll_ctl(EPOLL_CTL_ADD, fd, &ev);
        if(res == -1){
            map.erase(fd);
            return -1;
        }
        ptr->set_loop(this);
        //上次修改到这，要改update和addchannel和legalize timer的加入时间轮的cb回调addtask，还有容器的处理
        return 0;
    }
    
    //失败返回 -1， 成功返回 0
    //删除epoll和map对应的channel
    //失败原因  1,fd不存在或者不存在于eventloop，2,epoll::epoll_ctl失败等等
    //如果成功，状态会同步到对应channel
    //待完善
    int eventloop::remove_channel(int fd){
        int res = ep.epoll_ctl(EPOLL_CTL_DEL, fd, nullptr);
        if(res == -1)return -1;
        auto it = map.find(fd);
        if(it != map.end()) it->second->set_loop(nullptr);
        map.erase(fd);
        return 0;

    }
    
    //失败返回 -1， 成功返回 0
    //更新epoll和eventloop::map
    //失败原因  1,ptr == nullptr，2,epoll::epoll_ctl失败
    int eventloop::update_channel(int fd, channel* ptr){
        if(!ptr) return -1;
        epoll_event ev;
        ev.events = ptr->get_events();
        ev.data.ptr = ptr;
        return ep.epoll_ctl(EPOLL_CTL_MOD, fd, &ev);
    }

    void eventloop::quit(){
        looping.store(false, std::memory_order_release);
    }

    bool eventloop::get_looping()const noexcept{
        return looping.load(std::memory_order_acquire);
    }

    eventloop::eventloop(epoll&& ep)
        : ep(std::move(ep)),
        wa_(std::make_unique<waker>()),
        tim_(std::make_unique<time_wheel_timer>(s_num)){
        
    }

    eventloop::eventloop()
        : wa_(std::make_unique<waker>()),
        tim_(std::make_unique<time_wheel_timer>(s_num)){
        
    }

    eventloop::eventloop(bool run)
        : ep(run),
        wa_(std::make_unique<waker>(run)),
        tim_(std::make_unique<time_wheel_timer>(s_num, run)){

    }

    //tim_有没有效果不影响加入op并且获取唯一的id
    bool eventloop::add_task_to_time_wheel_timer(const call_back& op, size_t& id){
        if(!tim_->add_task(op)) return false;
        id = tim_->get_cur_idx();
        return true;
    }

    // 返回false代表enable—read失败，导致完全回滚
    bool eventloop::init_waker(){
        if(wa_->enable_wakeupfd_read() == -1) return false;
        wa_->set_default_wakingup();
        wa_->set_uppercb([this](){waker_uppercb_handle();});
        return true;
    }

    //返回false代表enable—read失败，导致完全回滚
    bool eventloop::init_time_wheel_timer(){
        if(tim_->enable_timerfd_read() == -1) return false;
        tim_->set_default_overtiming();
        tim_->set_default_uppercb();
        return true;
    }

    eventloop::eventloop(init_state pre)
        : ep(false),
        wa_(std::make_unique<waker>(false)),
        tim_(std::make_unique<time_wheel_timer>(s_num, false)){

        switch (pre){
            case init_state::prepared_epoll:
                //只合法化epollfd
                if(!ep.legalize_epoll()) 
                    throw std::runtime_error("eventloop::eventloop(init_state pre) : ep.legalize_epoll()出错");
                break;
            case init_state::prepared_all:
            case init_state::prepared_all_and_init:
                //合法化fd
                if(!ep.legalize_epoll()) 
                    throw std::runtime_error("eventloop::eventloop(init_state pre) : ep.legalize_epoll()出错");
                if(!wa_->legalize_waker())
                    throw std::runtime_error("eventloop::eventloop(init_state pre) : wa_->legalize_waker()出错");
                if(!tim_->legalize_timer())
                    throw std::runtime_error("eventloop::eventloop(init_state pre) : tim_->legalize_timer()出错");

                if(pre == init_state::prepared_all) break;
                //初始化
                wa_->set_default_wakingup();
                wa_->set_uppercb([this](){waker_uppercb_handle();});
                wa_->enable_wakeupfd_read();
                tim_->set_default_overtiming();
                tim_->set_default_uppercb();
                tim_->enable_timerfd_read();
                
                //加入eventloop
                if(add_channel(tim_->get_fd(), tim_->get_channel()) == -1) 
                    throw std::runtime_error("eventloop::eventloop(init_state pre) : add_channel出错");
                if(add_channel(wa_->get_fd(), wa_->get_channel()) == -1)
                    throw std::runtime_error("eventloop::eventloop(init_state pre) : add_channel出错");
                break;
            default:
                throw std::invalid_argument("eventloop::eventloop(init_state pre) : pre参数错误");
        }
    }

    eventloop::~eventloop(){

    }

    bool eventloop::is_valid()const{
        return ep.is_valid();
    }

    bool eventloop::legalize_epoll(){
        if(is_valid()) return true;
        ep.epoll_create();
        return ep.is_valid();
    }

    void eventloop::waker_uppercb_handle(){

       
    }

    bool eventloop::is_waker_valid()const{
        return wa_->is_valid();
    }

    bool eventloop::is_time_wheel_timer_valid()const{
        return tim_->is_valid();
    }

    bool eventloop::legalize_waker(){
        return wa_->legalize_waker();
    }

    bool eventloop::legalize_time_wheel_timer(){
        return tim_->legalize_timer();
    }

    int eventloop::add_waker_channel(){
        return add_channel(wa_->get_fd(), wa_->get_channel());
    }

    int eventloop::add_time_wheel_timer_channel(){
        return add_channel(tim_->get_fd(), tim_->get_channel());
    }

/* 废弃函数
    bool eventloop::time_wheel_timer_initialize(){
        //
    }

    bool eventloop::waker_initialize(){
        wa_->set_default_wakingup();
        wa_->enable_wakeupfd_read();
        wa_->set_uppercb([this](){waker_uppercb_handle();});
    }


    bool eventloop::add_waker(){
        if(wa_) return true;
        wa_ = std::make_unique<waker>(true);
        wa_->set_default_wakingup();
        wa_->enable_wakeupfd_read();
        wa_->set_uppercb([this](){waker_uppercb_handle();});
        if(!ep.is_valid()) return true;
        int flag = add_channel(wa_->get_fd(), wa_->get_channel());
        if(flag == 0) return true;
        wa_.reset();
        return false;
    }

*/



    
}