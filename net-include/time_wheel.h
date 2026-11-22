#include<map>
#include<string>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<vector>
#include<queue>
#include<functional>
#include<atomic>
#include<sys/socket.h>
#include<fcntl.h>
#include<sys/epoll.h>
#include<netdb.h>
#include<errno.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<iostream>
#include<vector>
#include<unistd.h>
#include<channel.h>
#include <sys/eventfd.h>
#include<unordered_map>
#include<list>
#include<iterator>

namespace ne{

    class time_wheel{
            //默认扩容倍数
            static constexpr size_t DEFAULT_EXPAND_TIME = 2;
            using call_back = std::function<void()>;
        public:
            struct task{
                size_t id;
                call_back overtiming;

                task(size_t id, const call_back& op);

                void execute();
            };

            struct pos{
                size_t slot_idx;
                std::list<task>::iterator iter;

                pos(size_t idx, std::list<task>::iterator it);
            };

        private:
            const size_t duration;
            //数量
            size_t ms_num;
            //理论最小单位是微秒,但实际单位时间取决于外部调用tick的时间interval
            //实际duration应为 ms_num*外部interval
            size_t ms_tick;
            size_t slot_num;
            size_t time_pos;
            size_t cur_idx;

            std::vector<std::list<task>> slots;
            std::unordered_map<size_t, pos> task_pos;

        private:
            //获取唯一任务id
            size_t get_sole_id();

            //time_pos往后移动一格
            size_t next_time_pos();

            size_t get_reset_list_pos()const;

        public:
            size_t get_cur_idx()const;

            void tick();

            bool add_task(const call_back& op);

            bool delete_task(size_t id);

            //更新到最新位置
            bool update_task(size_t id);

        public:
            time_wheel(size_t ms, size_t ms_num);

            ~time_wheel();

            time_wheel(const time_wheel &) = delete;

            time_wheel &operator=(const time_wheel &) = delete;

            time_wheel(time_wheel &&) noexcept = default;

            time_wheel &operator=(time_wheel &&) noexcept = default;
    };













}