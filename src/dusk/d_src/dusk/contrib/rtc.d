module dusk.contrib.rtc;

import tonc.tonc_types;

// #define RTC_EPOWER  ( 1 )
// #define RTC_E12HOUR ( 2 )
// #define RTC_EYEAR   ( 3 )
// #define RTC_EMON    ( 4 )
// #define RTC_EDAY    ( 5 )
// #define RTC_EWDAY   ( 6 )
// #define RTC_EHOUR   ( 7 )
// #define RTC_EMIN    ( 8 )
// #define RTC_ESEC    ( 9 )
// enum RTC_EPOWER = 1;
// enum RTC_E12HOUR = 2;
// enum RTC_EYEAR = 3;
// enum RTC_EMON = 4;
// enum RTC_EDAY = 5;
// enum RTC_EWDAY = 6;
// enum RTC_EHOUR = 7;
// enum RTC_EMIN = 8;
// enum RTC_ESEC = 9;

extern (D) {
    // #define RTC_TM_YEAR( X )    ( ( ( X ) >> 48 ) & 0xff )
    // #define RTC_TM_MON( X )     ( ( ( X ) >> 40 ) & 0xff )
    // #define RTC_TM_MDAY( X )    ( ( ( X ) >> 32 ) & 0xff )
    // #define RTC_TM_WDAY( X )    ( ( ( X ) >> 24 ) & 0xff )
    // #define RTC_TM_HOUR( X )    ( ( ( X ) >> 16 ) & 0xff )
    // #define RTC_TM_MIN( X )     ( ( ( X ) >> 8 ) & 0xff )
    // #define RTC_TM_SEC( X )     ( ( ( X ) >> 0 ) & 0xff )

    auto RTC_IN_YEAR(u64 x) {
        return (x >> 48) & 0xff;
    }

    auto RTC_IN_MON(u64 x) {
        return (x >> 40) & 0xff;
    }

    auto RTC_IN_MDAY(u64 x) {
        return (x >> 32) & 0xff;
    }

    auto RTC_IN_WDAY(u64 x) {
        return (x >> 24) & 0xff;
    }

    auto RTC_IN_HOUR(u64 x) {
        return (x >> 16) & 0xff;
    }

    auto RTC_IN_MIN(u64 x) {
        return (x >> 8) & 0xff;
    }

    auto RTC_IN_SEC(u64 x) {
        return (x >> 0) & 0xff;
    }
}

// typedef int rtc_time;
// typedef long long int rtc_tm;

alias rtc_time = int;
alias rtc_tm = s64;

extern (C) {
    int __rtc_init();
    int __rtc_get_status();
    rtc_time __rtc_get_time();
    rtc_tm __rtc_get_datetime();
    void __rtc_set_time(rtc_time time);
    void __rtc_set_datetime(rtc_tm datetime);
}
