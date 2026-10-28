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

namespace ne{

    class waker{
            using call_back = std::function<void()>;
        private:
            //拥有所有权,保证ch_一定指向channel对象
            int wakeupfd;
            std::unique_ptr<channel> ch_;

            //必须要有的
            call_back wakingup;

            //可有可无
            //不能抛出异常
            call_back uppercb;

        private:
            void wakingup_handle();

            static bool call_back_copy(call_back& func1, const call_back& func2);

        public:
            bool legalize_wakeupfd();

            bool is_valid()const;

            int enable_wakeupfd_read();

            int disable_wakeupfd_read();

            int wakeup();

            bool set_default_wakingup();

            bool set_uppercb(const call_back& op);

            bool set_wakingup(const call_back& op);

            void close();

            void set_fd();

        public:
            waker();

            waker(int fd);

            waker(bool run);

            waker(const waker&) = delete;

            waker& operator=(const waker&) = delete;

            waker(waker&&) = delete;

            waker& operator=(waker&&) = delete;

            ~waker();





    };






















}