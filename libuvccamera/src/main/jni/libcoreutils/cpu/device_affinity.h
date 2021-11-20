#pragma once

namespace cutils {

///TODO: remove this section, and detect in runtime
#ifdef __MTK__
    static const unsigned int BIG_CORE = 0b11000000;
    static const unsigned int SMALL_CORE = 0b00111111;
#elif defined(__QCOM__)
    static const unsigned int BIG_CORE = 0b11110000;
    static const unsigned int SMALL_CORE = 0b00001111;
#else
    static const unsigned int BIG_CORE = 0;
    static const unsigned int SMALL_CORE = 0;
//#error "not support!"
#endif
    static const int HIGH_PRIORITY = -10;  //正常prio为120，-10即为110

    static bool is_binder_core= true;
    static bool is_binder_big_core = true;

//    ANCULKStatus set_priority_for_thread(AffinityControlledDeviceId id);
//
//    ANCULKStatus set_affinity_for_device(AffinityControlledDeviceId id,
//                                        long long mask);
}  // namespace cutils
