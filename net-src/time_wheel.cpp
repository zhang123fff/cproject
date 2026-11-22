#include "time_wheel.h"

namespace ne{

    time_wheel::time_wheel(size_t ms, size_t ms_num)
        : duration(ms * ms_num), 
        ms_num(ms_num),
        slot_num(ms_num * DEFAULT_EXPAND_TIME),
        ms_tick(ms),
        cur_idx(0),
        time_pos(0){
        
        if(ms == 0 || ms_num == 0)
            throw std::invalid_argument("time_wheel::time_wheel(size_t ms, size_t ms_num) : 参数错误");
        slots.resize(slot_num);

    }

    //会++cur_idx，获取全局唯一的id
    size_t time_wheel::get_sole_id(){
        return ++cur_idx;
    }

    // 更改time_pos值,并返回
    size_t time_wheel::next_time_pos(){
        time_pos = (time_pos + 1) % slot_num;
        return time_pos;
    }

    //
    size_t time_wheel::get_reset_list_pos() const{
        return (time_pos + ms_num) % slot_num;
    }

    //获取当前cur_idx
    size_t time_wheel::get_cur_idx()const{
        return cur_idx;
    }

    void time_wheel::tick()
    {
        next_time_pos();
        if(slots[time_pos].empty()) return;
        call_back tmp;
        for (auto &i : slots[time_pos]){
            tmp = std::move(i.overtiming);
            try{
                if(tmp) tmp();
            }
            catch (...){
                // 输出错误信息

            }
            task_pos.erase(i.id);
            continue;
        }

        slots[time_pos].clear();
    }

    bool time_wheel::add_task(const call_back& op){
        size_t tmp = get_sole_id();
        size_t idx = get_reset_list_pos();
        try{
            slots[idx].emplace_back(tmp, op);
            task_pos.emplace(tmp, pos(idx, std::prev(slots[idx].end())));
            return true;
        }
        catch(std::exception& ex){
            if(task_pos.find(tmp) == task_pos.end()) slots[idx].pop_back();
            //输出错误信息
            return false;
        }
    }

    //只要id存在，那么就不会失败
    bool time_wheel::delete_task(size_t id){
        auto it = task_pos.find(id);
        if(it == task_pos.end()) return false;
        slots[it->second.slot_idx].erase(it->second.iter);
        task_pos.erase(id);
        return true;
    }

    //只要id存在，那么就不会失败
    bool time_wheel::update_task(size_t id){
        auto it = task_pos.find(id);
        if(it == task_pos.end()) return false;
        size_t idx = get_reset_list_pos();
        slots[idx].splice(slots[idx].end(), slots[it->second.slot_idx], it->second.iter);
        
        it->second.slot_idx = idx;
        it->second.iter = std::prev(slots[idx].end());
        return true;
    }

    time_wheel::task::task(size_t id, const call_back &op)
        : id(id),
        overtiming(op){

    }

    //可能会抛出异常,取决于overtiming
    void time_wheel::task::execute(){
        if(overtiming) overtiming();
    }

    time_wheel::pos::pos(size_t idx, std::list<task>::iterator it)
        : slot_idx(idx),
        iter(it){

    }

}
