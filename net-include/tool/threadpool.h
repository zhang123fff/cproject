#include<map>
#include<string>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<vector>
#include<queue>
#include<functional>
#include<atomic>

namespace tool{

    //start，end，构造函数，析构函数都要在一个线程执行
    //其他可多线程执行
    class threadpool{
            static constexpr size_t DEFAULT_THREAD_NUM = 5;
            //static constexpr size_t MAX_TASK_NUM = 18048;
            //static constexpr size_t MAX_THREAD_NUM = 1024;
        private:
            //  初始化一定要初始化running，prog_, thread_num,task_num
            //thread_num赋值后不再变化
            const size_t thread_num;

            std::atomic<bool> running;
            std::atomic<size_t> task_num;

            std::vector<std::thread> ths_;
            std::condition_variable va_;
            mutable std::mutex mtx_;
            std::queue<std::function<void()>> tas_;

            //必须要有的
            std::function<void()> prog_;

        private:
            void default_thread_loop();

        public:
            bool is_running()const;

            //bool is_out_of_task_range(size_t num)const;

            //bool is_out_of_thread_range(size_t num)const;

            bool enqueue(const std::function<void()>& ta);

            bool enqueue(std::function<void()>&& ta);

            size_t get_thread_num()const;

            size_t get_precise_task_num()const;

            size_t get_task_num()const;

            void start();

            void end();

        public:
            threadpool();

            threadpool(size_t th);

            threadpool(size_t th, bool run);

            threadpool(const threadpool&) = delete;

            threadpool& operator=(const threadpool&) = delete;

            threadpool(threadpool&&) = delete;

            threadpool& operator=(threadpool&&) = delete;

            ~threadpool();




    };







}