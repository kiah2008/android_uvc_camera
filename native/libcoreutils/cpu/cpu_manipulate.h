//
// Created by zhaokai on 2020/6/28.
//

#pragma once

namespace cutils {

    void bind_core_and_set_priority();

    void set_current_thread_affinity_mask(int mask);

    void set_current_thread_priority(int prio);

//! get ID of the cpu on which caller thread runs; return -1 on error
    int get_cpu_id();

//! get cpu frequency in MHz; return -1 on error
    float get_cpu_freq();

} //cutils
