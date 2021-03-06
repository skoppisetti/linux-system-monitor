#include <string>
#include <math.h>
#include <algorithm>

#include "format.h"

using std::string;

/**
 * Helper function to format seconds into HH:MM:SS format
 * 
 * @param seconds: The time in seconds which needs to be formatted
 * @return a string in HH:MM:SS format
 */
string Format::ElapsedTime(long seconds) { 
    double hr_frac = seconds/3600.0;
    double min_frac = seconds/60.0;
    long hours = floor(hr_frac);
    long mins = floor(min_frac - (hours * 60));
    long secs = (min_frac - (hours * 60) - mins) * 60;
    string hh = std::to_string(hours);
    int hh_l = hh.length();
    hh.insert(hh.begin(), std::max(2, hh_l) - hh_l, '0');
    string mm = std::to_string(mins);
    mm.insert(mm.begin(), 2 - mm.length(), '0');
    string ss = std::to_string(secs);
    ss.insert(ss.begin(), 2 - ss.length(), '0');
    return hh + ":" + mm + ":" + ss; 
}