#include"../net-include/tcpconnection.h"

namespace ne{

    //       可能会抛出异常(invalid_argument)
    //sock或ch为空，sock或ch状态错误
    tcpconnection::tcpconnection(socket* sock, channel* ch){
        if(!sock || !ch) 
            throw std::invalid_argument("tcpconnection::tcpconnection(socket* sock, channel* ch) : ch或sock为空");
        if(!sock->is_valid() || sock->get_fd() != ch->fd()) 
            throw std::invalid_argument("tcpconnection::tcpconnection(socket* sock, channel* ch) : ch或sock状态错误");
        
        set_default_closing();
        set_default_error();
        set_default_reading();
        set_default_writing();

        sock_ = sock;
        ch_ = ch;
    }

    //       可能会抛出异常(bad_alloc，invalid_argument)
    //new失败，sock为空，sock状态错误
    tcpconnection::tcpconnection(socket* sock){
        if(!sock) 
            throw std::invalid_argument("acceptor::acceptor(socket* sock) : sock为空");
        if(!sock->is_valid()) 
            throw std::invalid_argument("acceptor::acceptor(socket* sock) : sock状态错误");
            
        sock_ = sock;
        ch_ = new channel(sock_->get_fd());

        set_default_closing();
        set_default_error();
        set_default_reading();
        set_default_writing();

    }

    tcpconnection::~tcpconnection(){
        delete ch_;
        delete sock_;
    }

    bool tcpconnection::set_reading(const call_back& op){
        call_back tmp = std::move(reading);
        if(!call_back_copy(reading, op)){
            reading = std::move(tmp);
            return false;
        }
        bool res = ch_->set_reading(reading);
        if(!res) reading = std::move(tmp);
        return res; 

    }

    bool tcpconnection::set_closing(const call_back& op){
        call_back tmp = std::move(closing);
        if(!call_back_copy(closing, op)){
            closing = std::move(tmp);
            return false;
        }
        bool res = ch_->set_closing(closing);
        if(!res) closing = std::move(tmp);
        return res; 
    }

    bool tcpconnection::set_error(const call_back& op){
        call_back tmp = std::move(error);
        if(!call_back_copy(error, op)){
            error = std::move(tmp);
            return false;
        }
        bool res = ch_->set_error(error);
        if(!res) error = std::move(tmp);
        return res; 
    }

    bool tcpconnection::set_writing(const call_back& op){
        call_back tmp = std::move(writing);
        if(!call_back_copy(writing, op)){
            writing = std::move(tmp);
            return false;
        }
        bool res = ch_->set_writing(writing);
        if(!res) writing = std::move(tmp);
        return res; 
    }

    bool tcpconnection::is_valid()const{
        return ch_ && sock_;
    }

    bool tcpconnection::call_back_copy(call_back& func1, const call_back& func2){
        try{
            func1 = func2;
        }
        catch(std::exception& ex){
            //错误输出
            fprintf(stderr, "tcpconnection::call_back_copy : funtion赋值抛出异常\n");
            fprintf(stderr, ex.what());
            fprintf(stderr, "\n");
            return false;
        }
        return true;
    }

    bool tcpconnection::set_pack_handle_error(const call_back& op){
        call_back tmp = std::move(pack_handle_error);
        if(!call_back_copy(pack_handle_error, op)){
            pack_handle_error = std::move(tmp);
            return false;
        }
        
        return true; 
    }

    bool tcpconnection::set_pack_handling(const pack_protocol& op){
        pack_protocol tmp = std::move(pack_handling);
        try{
            pack_handling = op;
        }
        catch(std::exception& ex){
            //补上错误输出
            pack_handling = std::move(tmp);
            return false;
        }
        return true;
    }

    void tcpconnection::closing_handle(){
        int fd = sock_->get_fd();
        ch_->detach_from_eventloop_and_discard_this_channel();
        sock_->close();
        //输出关闭信息
        //补上更详细的输出信息
        fprintf(stdout, "客户socket(fd: %d)已关闭", fd);
    }

    void tcpconnection::error_handle(){
        int res = 0;
        if(sock_->get_socket_error(res) == 0) fprintf(stderr, "socket-error : %d\n", res);
        else fprintf(stderr, "socket-error获取失败\n");

        closing_handle();
    }

    void tcpconnection::reading_handle(){
        std::string tmp;
        ssize_t res = 0;
        uint16_t pack_size = input_buff.get_bytes_remaining();
        while(true){
            tmp.clear();
            if(!input_buff.get_stage()){
                res = sock_->recv_all(tmp, input_buff.get_bytes_remaining());
                if(res <= -1) return;

                //如果能一次读完就直接获取pack_size
                if(res == input_buff.get_head_size()){
                    pack_size = buffer::static_get_netinfo_pack_size(tmp.c_str());
                    input_buff.set_stage(true);
                    input_buff.set_bytes_remaining(pack_size);
                
                    continue;
                
                }
                //如果读不完就先存进input_buff,等凑够headsize字节再解析
                else{
                    input_buff.append(tmp);
                    input_buff.bytes_remaining_decrease(res);

                    //r如果够headsize，直接进入下一阶段
                    if(input_buff.get_bytes_remaining() == 0){
                        pack_size = buffer::static_get_netinfo_pack_size(input_buff.c_str());
                        input_buff.clear_buffer();
                        input_buff.set_stage(true);
                        input_buff.set_bytes_remaining(pack_size);
                        continue;
                    }
                    return;
                }
            }
            else{
                res = sock_->recv_all(tmp, input_buff.get_bytes_remaining());
                if(res == -1) return;

                //提前扩容
                if(!input_buff.extend_capacity(res)){
                    //待完善
                    //失败会丢失数据,后果严重
                    return;
                }

                if(res <= input_buff.get_bytes_remaining()){
                    //不会出错，因为上面已经提前扩容了
                    input_buff.append(tmp);
                    input_buff.bytes_remaining_decrease(res);

                    //如果没接收完则return
                    if(input_buff.get_bytes_remaining() != 0) return;
                    
                    //接收完整pack则执行下面的代码
                    //pack接收完毕,先对buffer初始化，再开始处理
                    input_buff.move_bytes_out_of_buff(tmp);
                    input_buff.set_stage(false);
                    input_buff.set_bytes_remaining(input_buff.get_head_size());

                    //接收上层协议处理pack后的responce
                    std::string result;
                    try{
                        //抛出异常会导致pack丢失
                        pack_handling(std::move(tmp), result);
                    }
                    catch(std::exception& ex){
                        //输出错误信息，处理错误
                        fprintf(stderr, "pack_handling抛出异常 ");
                        fprintf(stderr, ex.what());
                        fprintf(stderr, "\n");
                        if(pack_handle_error) pack_handle_error();
                        continue;
                    }
                    catch(...){
                        //输出错误信息，处理错误
                        fprintf(stderr, "pack_handling抛出非标准异常 \n");
                        if(pack_handle_error) pack_handle_error();
                        continue;

                    }

                    //将result写回output_buff中

                    //如果失败则responce丢失
                    if(!append_responce_into_output_buff(result)){
                        //补上输出错误信息
                        return;
                    } 
                    else {
                        int flag = ch_->enable_write_event();
                        if(flag == -1) {
                            //补上输出错误信息
                            return;
                        }
                    }
                    
                    

                }

            }
        }       
    }

    //自动计算packsize
    //返回false能完全回滚
    //注意这是覆盖原有数据
    bool tcpconnection::overwrite_responce_into_output_buff(const std::string& data){
        uint16_t pack_size = data.size();
        //提前扩容,扩容成功，则整体成功
        if(!output_buff.reserve(pack_size + output_buff.get_head_size()))return false;
        //清空原有内容
        //buffer的append在容量充足且参数正确的情况下不会失败
        output_buff.resize(0);

        pack_size = htons(pack_size);
        output_buff.append((const char*)(&pack_size), output_buff.get_head_size());
        output_buff.append(data.c_str(), data.size());
        return true;
    }

    //自动计算packsize
    //追加pack
    //返回false能完全回滚
    bool tcpconnection::append_responce_into_output_buff(const std::string& data){
        uint16_t pack_size = data.size();
        //提前扩容,扩容成功，则整体成功
        if(!output_buff.reserve(output_buff.size()+ pack_size + output_buff.get_head_size()))return false;
        //buffer的append在容量充足且参数正确的情况下不会失败
        pack_size = htons(pack_size);
        output_buff.append((const char*)(&pack_size), output_buff.get_head_size());
        output_buff.append(data.c_str(), data.size());
        return true;
    }

    //不会抛出异常
    void tcpconnection::set_default_reading(){
        reading = [this](){reading_handle();};
    }

    //不会抛出异常
    void tcpconnection::set_default_writing(){
        writing = [this](){writing_handle();};
    }

    //不会抛出异常
    void tcpconnection::set_default_closing(){
        closing = [this](){closing_handle();};
    }

    //不会抛出异常
    void tcpconnection::set_default_error(){
        error = [this](){error_handle();};
    }

    //buff为空时候调用不出错
    void tcpconnection::writing_handle(){
        std::string meg;
        ssize_t res = 0;
        output_buff.move_bytes_out_of_buff(meg);

        try{
            //提前扩容，防止失败丢失数据
            //如果抛出异常则停止写入socket,防止数据丢失
            output_buff.reserve(meg.size());
        }
        catch(std::exception& ex){
            //输出错误信息，处理错误
            fprintf(stderr, "tcpconnection::writing_handle : string::reserve抛出异常\n ");
            fprintf(stderr, ex.what());
            fprintf(stderr, "\n");
            //回滚
            output_buff.move_bytes_overwrite_buff(std::move(meg));
            return;
        }

        //非阻塞
        res = sock_->send_all(meg);
        if(res == -1){
            //补上输出信息
            //回滚
            output_buff.move_bytes_overwrite_buff(std::move(meg));
            return;
        }
        ssize_t remaining = meg.size() - res;

        //可能会失败，后果严重，待完善
        //
        ch_->disable_write_event();

        //全部发完直接返回
        if(remaining == 0) return;

        output_buff.append(meg.c_str() + res, remaining);

        //可能会失败，后果严重可能该连接卡死，待完善
        //如果是边缘触发，可以重置状态
        ch_->enable_write_event();
    }


}