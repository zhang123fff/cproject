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

namespace ne{

    class time_wheel{
            using call_back = std::function<void()>;
        public:
            struct task{
                size_t id;
                call_back overtiming;

                task(size_t id, const call_back& op);
            };
        private:
            const size_t duration;
            size_t time_pos;
            size_t ms_tick;
            size_t slot_num;

            std::vector<std::list<task>> slots;
            std::unordered_map<size_t, std::list<task>::iterator> task_pos;

        public:
            void tick();

            bool add_task();

            bool delete_task();

        public:
            time_wheel();

            ~time_wheel();








    };













}