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
    int rtc_init();
    int rtc_get_status();
    rtc_time rtc_get_time();
    rtc_tm rtc_get_datetime();
    void rtc_set_time(rtc_time time);
    void rtc_set_datetime(rtc_tm datetime);
}

alias dt_field_size = int;
struct RTCDateTime {
    dt_field_size year;
    dt_field_size month;
    dt_field_size day;
    dt_field_size wday;
    dt_field_size hour;
    dt_field_size min;
    dt_field_size sec;
}

extern (D) RTCDateTime rtc_get_datetime_ex() {
    RTCDateTime ret;
    auto rawdt = rtc_get_datetime();

    ret.year = cast(dt_field_size) RTC_IN_YEAR(rawdt);
    ret.month = cast(dt_field_size) RTC_IN_MON(rawdt);
    ret.day = cast(dt_field_size) RTC_IN_MDAY(rawdt);
    ret.wday = cast(dt_field_size) RTC_IN_WDAY(rawdt);
    ret.hour = cast(dt_field_size) RTC_IN_HOUR(rawdt);
    ret.min = cast(dt_field_size) RTC_IN_MIN(rawdt);
    ret.sec = cast(dt_field_size) RTC_IN_SEC(rawdt);

    return ret;
}
