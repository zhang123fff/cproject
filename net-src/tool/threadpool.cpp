#include"net-include/tool/threadpool.h"

namespace tool{

    bool threadpool::is_running()const{
        return running.load(std::memory_order_acquire);
    }

    /*
    //先检查再加入append，所以>=
    bool threadpool::is_out_of_task_range(size_t num)const{
        return num >= MAX_TASK_NUM;
    }

    //先检查再加入assign，所以>
    bool threadpool::is_out_of_thread_range(size_t num)const{
        return num > MAX_THREAD_NUM;
    }
    */

    void threadpool::start(){
        if(is_running()) return;
        running.store(true, std::memory_order_release);

        size_t i = 0;
        try{
            ths_.reserve(thread_num);

            //不会抛出异常
            for(; i < thread_num; i++){
                ths_.emplace_back(prog_);
            }
        }
        catch(std::exception& ex){
            //输出错误信息
            fprintf(stderr, "vector::reserve(%d)或者queue<thread>::emplace(prog_)抛出异常\n", thread_num);
            fprintf(stderr, "%s\n", ex.what());
            end();
            return;
        }
        
    }

    //无异常抛出,不会失败
    //ths_的空间不复原
    void threadpool::end(){
        if(!is_running()) return;
        running.store(false, std::memory_order_release);
        va_.notify_all();
        for(auto& i : ths_){
            if(i.joinable()) i.join();
        }
        ths_.resize(0);
    }

    threadpool::threadpool()
        : thread_num(DEFAULT_THREAD_NUM), 
        running(false),
        task_num(0),
        prog_([this](){default_thread_loop();}){

    }

    void threadpool::default_thread_loop(){
        std::function<void()> tmp;
        while(true){
            {
                std::unique_lock<std::mutex> locker(mtx_);
                va_.wait(locker, [this](){ return !tas_.empty() || !is_running();});
                if(tas_.empty() && !is_running()) return;

                tmp = std::move(tas_.front());
                tas_.pop();
                task_num.fetch_sub(1, std::memory_order_acq_rel);
            }

            try{
                tmp();
            }
            catch(std::exception& ex){
                //输出错误信息
                fprintf(stderr, "任务抛出标准异常\n");
                fprintf(stderr, "%s\n", ex.what());

                continue;
            }
            catch(...){
                //输出错误信息
                fprintf(stderr, "任务抛出非标准异常\n");
                continue;
            }
        }
    }

    bool threadpool::enqueue(const std::function<void()> &ta){
        if(!ta) return false;
        
        try{
            std::lock_guard<std::mutex> locker(mtx_);
            if(!is_running()) return false;
            tas_.push(ta);
            task_num.fetch_add(1,std::memory_order_acq_rel);
        }
        catch(std::exception& ex){
             //输出错误信息
            fprintf(stderr, "std::lock_guard<std::mutex>(mtx_)或者queue::push()拷贝抛出异常\n");
            fprintf(stderr, "%s\n", ex.what());
            return false;
        }
        va_.notify_one(); 
        return true;
    }

    bool threadpool::enqueue(std::function<void()> &&ta){
        if(!ta) return false;

        try{
            std::lock_guard<std::mutex> locker(mtx_);
            if(!is_running()) return false;
            tas_.push(std::move(ta));
            task_num.fetch_add(1,std::memory_order_acq_rel);
        }
        catch(std::exception& ex){
             //输出错误信息
            fprintf(stderr, "std::lock_guard<std::mutex>(mtx_)抛出异常\n");
            fprintf(stderr, "%s\n", ex.what());
            return false;
        }
        va_.notify_one();
        return true;
    }

    size_t threadpool::get_thread_num()const{
        return thread_num;
    }

    //上锁读取精确值
    size_t threadpool::get_precise_task_num()const{
        std::lock_guard<std::mutex> locker(mtx_);
        return tas_.size();
    }

    //读取atomic值
    size_t threadpool::get_task_num()const{
        return task_num.load(std::memory_order_acquire);
    }

    threadpool::threadpool(size_t th)
        : thread_num(th), 
        task_num(0),
        running(false),
        prog_([this](){default_thread_loop();}){

        if(th == 0) 
            throw std::invalid_argument("threadpool::threadpool(size_t th) : 参数th错误");
    }

    threadpool::threadpool(size_t th, bool run)
        : thread_num(th), 
        task_num(0),
        running(run), 
        prog_([this](){default_thread_loop();}){

        if(th == 0) 
            throw std::invalid_argument("threadpool::threadpool(size_t th) : 参数th错误");

        if(!run) return;

        size_t i = 0;
        try{
            ths_.reserve(th);
            for(; i < th; i++){
                ths_.emplace_back(prog_);
            }
        }
        catch(std::exception& ex){
             //输出错误信息
            fprintf(stderr, "queue<thread>::emplace()或者vector::reserve()抛出异常\n");
            fprintf(stderr, "%s\n", ex.what());
            end();
            throw;
        }
    }

    threadpool::~threadpool(){
        end();
    }






}