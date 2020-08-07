#include "Date.h"
#include <chrono>
#include <cstring> //strstr
#include <cmath> //pow
#include "time.h"

#define BUFFER_SIZE 25

const time_t Date::getCurrentTimeMillis() {
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    std::chrono::system_clock::duration duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

const String Date::toIsoString(time_t timestamp) {
    //strftime can only handle seconds.
    time_t tSec = timestamp / 1000;
    char buf[BUFFER_SIZE];
    struct tm *pTM = gmtime(&tSec);
    int fractions = tSec ? (int) (timestamp % tSec) : 0;
    size_t size = strftime(buf, BUFFER_SIZE, "%Y-%m-%dT%H:%M:%S", pTM);
    if (fractions) {
        sprintf(buf + size, ".%03dZ", fractions);
    } 
    else {
        sprintf(buf + size, "Z");
    }
    return String(buf);
}

const bool Date::fromIsoString(const char *isoString, time_t *timestampOut) {
    int yyyy, MM, dd, hh, mm, ss, f;
    yyyy = MM = dd = -1;
    hh = mm = ss = f = 0;
    sscanf(isoString, "%04d-%02d-%02dT%02d:%02d:%02d.%03d", &yyyy, &MM, &dd, &hh, &mm, &ss, &f);
    if (yyyy < 1 || MM < 1 || dd < 1) {
        return false;
    }
    struct tm tm;
    tm.tm_isdst = 0;
    tm.tm_wday = 0;
    tm.tm_yday = 0;
    tm.tm_year = yyyy - 1900;
    tm.tm_mon = MM - 1;
    tm.tm_mday = dd;
    tm.tm_hour = hh;
    tm.tm_min = mm;
    tm.tm_sec = ss;
    time_t timesec = mktime(&tm);
    if (timesec < 0) {
        return false;
    }
    if (f) {
        const char *ptr = std::strstr(isoString, ".") + 1;
        int i;
        for (i = 0; i < 3; ++i) {
            if (ptr[i] < '0' || ptr[i] > '9') {
                f *= (int) std::pow(10, 3 - i);
                break;
            }
        }
    }
    _tzset();
    
    long timezoneDiff;
#ifdef _get_timezone
    _get_timezone(&timezoneDiff);
#else
    timezoneDiff = _timezone;

#endif

    *timestampOut = 1000 * (timesec - timezoneDiff) + f;
    return true;
}