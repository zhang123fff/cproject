#include"../net-include/tcpserver.h"

namespace ne{

    /*
        tcpserver成员private

            size_t thread_num;
            std::unique_ptr<acceptor> acc_;
            std::unique_ptr<acceptor> lo_;
            std::map<int, std::shared_ptr<tcpconnection>> cons_;
            tool::threadpool po_;
            //必须要有的function，不会判断是否为空
            call_back newconnection;

            //上层传递协议，处理pack,必须要有的function
            pack_protocol pack_handling;

            //可有可无function
            //不能抛异常
            call_back pack_handle_error;


            tcpserver的状态------初始态-->资源态-->运行态,必须按照顺序变化
            处于初始态和资源态之间的是未知态
    */


    //处于初始态
    tcpserver::tcpserver()
        : thread_num(DEFAULT_THREAD_SIZE),
        po_(DEFAULT_THREAD_SIZE),
        state(server_state::init_state){

    }

    //成功则处于运行态
    tcpserver::tcpserver(size_t thread_num, bool running)
        : thread_num(thread_num),
        po_(thread_num),
        state(server_state::init_state){
        
        if(!running) return;
        if(!run()) throw std::runtime_error("tcpserver::tcpserver(size_t thread_num, bool running) : run()失败");

    }

    //获取acceptor对象，eventloop对象，线程资源等等
    //完全失败或者处于unknown_state返回false
    //当前已经处于资源态或者运行态返回true
    //完全成功返回true
    bool tcpserver::create_all_resource(){
        if(is_unknown_state()) return false;
        if(!is_init_state()) return true;

        if(!po_.start()) return false;

        socket* tmp = nullptr;
        try{
            lo_ = std::make_unique<eventloop>();
            tmp = new socket(FA_, SOCK_STREAM, "localhost", DEFAULT_PORT_NUM);
            acc_ = std::make_unique<acceptor>(tmp);
           
        }
        catch(std::exception& ex){
            delete tmp;
            lo_.reset();
            acc_.reset();
            po_.end();
            //之后补上错误信息
            //........
            //........
            //........
            return false;
        }
        
        set_state(server_state::rescource_state);
        return true;
    }

    //仅在处于unknown_state或者init_state时有用
    //处于unknown_state补充对应缺少的资源
    //处于init_state时 调用create_all_resource()
    //返回true代表变成resource_state
    //返回false，表示有操作失败，不会回滚，state不会变
    bool tcpserver::create_lacked_resource(){
        if(is_init_state()) return create_all_resource();
        else if(!is_unknown_state()) return true;

        //unknow_state处理逻辑
        socket* tmp = nullptr;
        try{
            if(!lo_)lo_ = std::make_unique<eventloop>();
            if(!lo_->is_valid()) if(!lo_->legalize_epoll()) return false;
            if(!acc_){
                tmp = new socket(FA_, SOCK_STREAM, "localhost", DEFAULT_PORT_NUM);
                acc_ = std::make_unique<acceptor>(tmp);
                tmp = nullptr;
            }
            if(!acc_->is_valid()) {
                tmp = new socket(FA_, SOCK_STREAM, "localhost", DEFAULT_PORT_NUM);
                if(!acc_->legalize_socket(tmp)){
                    delete tmp;
                    return false;
                }
            }
            set_state(server_state::rescource_state);
            return true;
        }
        catch(std::exception& ex){
            //补上错误信息
            delete tmp;
            return false;
        }
    }

    bool tcpserver::call_back_copy(call_back &func1, const call_back &func2){
         try{
            func1 = func2;
            return true;
        }
        catch(std::exception& ex){
            //错误输出
            fprintf(stderr, "tcpconnection::call_back_copy : funtion赋值抛出异常\n");
            fprintf(stderr, ex.what());
            fprintf(stderr, "\n");
            return false;
        }
    }

    // 先去获取所有资源，再run
    // 成功loop()或者本来处于运行态返回true
    // 获取资源失败或者loop()异常就返回false
    bool tcpserver::run(){      
        if(is_running_state()) return true;
        create_all_resource();
        bool flag = false;
        if(!is_rescource_state()) return false;
        set_state(server_state::running_state);
        flag = lo_->loop();
        
        //可能lo_->loop()会破坏资源态
        //待完善
        set_state(server_state::rescource_state);
        return flag;
    }

    bool tcpserver::is_init_state()const{
        return state.load(std::memory_order_acquire) == server_state::init_state;
    }

    bool tcpserver::is_rescource_state()const{
        return state.load(std::memory_order_acquire) == server_state::rescource_state;
    }

    bool tcpserver::is_running_state()const{
        return state.load(std::memory_order_acquire) == server_state::running_state;
    }

    bool tcpserver::is_unknown_state() const{
        return state.load(std::memory_order_acquire) == server_state::unknown_state;
    }

    //相对较慢，要判断当前是哪个阶段
    //建议用set_state(server_state sta)
    //unknown_state的下一个状态还是unknown_state，因为unknowstate是异常状态
    void tcpserver::next_state(){
        if(is_init_state())  set_state(server_state::rescource_state);
        else if(is_rescource_state()) set_state(server_state::running_state);
        else set_state(server_state::init_state);
    }

    void tcpserver::set_state(server_state sta){
        state.store(sta, std::memory_order_release);
    }

    void tcpserver::set_default_newconnection(){
        newconnection = [this](){default_newconnection();};
    }

    void tcpserver::default_newconnection(){

    }

    bool tcpserver::set_pack_handle(const pack_protocol &op){
        pack_protocol tmp = std::move(pack_handling);
        try{
            pack_handling = op;
            return true;
        }
        catch(std::exception& ex){
            //补上错误输出
            pack_handling = std::move(tmp);
            return false;
        }
    }

    bool tcpserver::set_pack_handle_error(const call_back &op){
        call_back tmp = std::move(pack_handle_error);
        if(!call_back_copy(pack_handle_error, op)){
            pack_handle_error = std::move(tmp);
            return false;
        }
        
        return true; 
    }





}