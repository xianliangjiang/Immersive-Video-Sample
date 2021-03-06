/*
 * Copyright (c) 2019, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.

 *
 */


/*!
 *	\brief NTP seconds from 1900 to 1970
 *	\hideinitializer
 *
 *	Macro giving the number of seconds from from 1900 to 1970
*/

#include "general.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>

VCD_OMAF_BEGIN

#define NTP_SEC_1900_TO_1970 2208988800ul

static uint32_t ntp_shift         = NTP_SEC_1900_TO_1970;
static uint32_t sys_start_time    = 0;
static uint64_t sys_start_time_hr = 0;

uint32_t sys_clock()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	return (uint32_t) ( ( (now.tv_sec)*1000 + (now.tv_usec) / 1000) - sys_start_time );
}

uint64_t sys_clock_high_res()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    return (now.tv_sec)*1000000 + (now.tv_usec) - sys_start_time_hr;
}

void net_set_ntp_shift(int32_t shift)
{
    ntp_shift = NTP_SEC_1900_TO_1970 + shift;
}

uint64_t net_parse_date(const char *val)
{
    uint64_t current_time;
    char szDay[50], szMonth[50];
    int32_t year, month, day, h, m, s, ms;
    int32_t oh, om;
    float secs;
    bool neg_time_zone = false;

    struct tm t;
    memset(&t, 0, sizeof(struct tm));

    szDay[0] = szMonth[0] = 0;
    year = month = day = h = m = s = 0;
    oh = om = 0;
    secs = 0;

    if (sscanf(val, "%d-%d-%dT%d:%d:%gZ", &year, &month, &day, &h, &m, &secs) == 6) {
    }
    else if (sscanf(val, "%d-%d-%dT%d:%d:%g-%d:%d", &year, &month, &day, &h, &m, &secs, &oh, &om) == 8) {
        neg_time_zone = true;
    }
    else if (sscanf(val, "%d-%d-%dT%d:%d:%g+%d:%d", &year, &month, &day, &h, &m, &secs, &oh, &om) == 8) {
    }
    else if (sscanf(val, "%3s, %d %3s %d %d:%d:%d", szDay, &day, szMonth, &year, &h, &m, &s)==7) {
        secs  = (float) s;
    }
    else if (sscanf(val, "%9s, %d-%3s-%d %02d:%02d:%02d GMT", szDay, &day, szMonth, &year, &h, &m, &s)==7) {
        secs  = (float) s;
    }
    else if (sscanf(val, "%3s %3s %d %02d:%02d:%02d %d", szDay, szMonth, &day, &year, &h, &m, &s)==7) {
        secs  = (float) s;
    }
    else {
        LOG(ERROR) << "[Core] Cannot parse date string" <<  val;
        return 0;
    }

    if (month) {
        month -= 1;
    } else {
        if (!strcmp(szMonth, "Jan")) month = 0;
        else if (!strcmp(szMonth, "Feb")) month = 1;
        else if (!strcmp(szMonth, "Mar")) month = 2;
        else if (!strcmp(szMonth, "Apr")) month = 3;
        else if (!strcmp(szMonth, "May")) month = 4;
        else if (!strcmp(szMonth, "Jun")) month = 5;
        else if (!strcmp(szMonth, "Jul")) month = 6;
        else if (!strcmp(szMonth, "Aug")) month = 7;
        else if (!strcmp(szMonth, "Sep")) month = 8;
        else if (!strcmp(szMonth, "Oct")) month = 9;
        else if (!strcmp(szMonth, "Nov")) month = 10;
        else if (!strcmp(szMonth, "Dec")) month = 11;
    }

    t.tm_year = year>1000 ? year-1900 : year;
    t.tm_mday = day;
    t.tm_hour = h;
    t.tm_min = m;
    t.tm_sec = (uint32_t) secs;
    t.tm_mon = month;

    if (strlen(szDay) ) {
        if (!strcmp(szDay, "Mon") || !strcmp(szDay, "Monday")) t.tm_wday = 0;
        else if (!strcmp(szDay, "Tue") || !strcmp(szDay, "Tuesday")) t.tm_wday = 1;
        else if (!strcmp(szDay, "Wed") || !strcmp(szDay, "Wednesday")) t.tm_wday = 2;
        else if (!strcmp(szDay, "Thu") || !strcmp(szDay, "Thursday")) t.tm_wday = 3;
        else if (!strcmp(szDay, "Fri") || !strcmp(szDay, "Friday")) t.tm_wday = 4;
        else if (!strcmp(szDay, "Sat") || !strcmp(szDay, "Saturday")) t.tm_wday = 5;
        else if (!strcmp(szDay, "Sun") || !strcmp(szDay, "Sunday")) t.tm_wday = 6;
    }

    current_time = mktime_utc(&t);

    if ((int64_t) current_time == -1) {
        //use 1 ms
        return 1;
    }
    if (current_time == 0) {
        //use 1 ms
        return 1;
    }

    if (om > 0 && om <= 60 && oh > 0 && oh <= 12) {
        int32_t diff = (60*oh + om)*60;
        if (neg_time_zone) diff = -diff;
        current_time = current_time + diff;
    }
    current_time *= 1000;
    ms = (uint32_t) ( (secs - (uint32_t) secs) * 1000);
    return current_time + ms;
}

uint64_t net_get_utc()
{
    uint64_t current_time;
    double msec;
    uint32_t sec, frac;

    net_get_ntp(&sec, &frac);
    current_time = sec - NTP_SEC_1900_TO_1970;
    current_time *= 1000;
    msec = frac*1000.0;
    msec /= 0xFFFFFFFF;
    current_time += (uint64_t) msec;
    return current_time;
}

void net_get_ntp(uint32_t *sec, uint32_t *frac)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    *sec = (uint32_t) (now.tv_sec) + NTP_SEC_1900_TO_1970;
    *frac = (uint32_t) ( (now.tv_usec << 12) + (now.tv_usec << 8) - ((now.tv_usec * 3650) >> 6) );
}

uint64_t net_get_ntp_ts()
{
    uint64_t res;
    uint32_t sec, frac;
    net_get_ntp(&sec, &frac);
    res = sec;
    res<<= 32;
    res |= frac;
    return res;
}

int32_t net_get_ntp_diff_ms(uint64_t ntp)
{
    uint32_t remote_s, remote_f, local_s, local_f;
    int64_t local, remote;

    remote_s = (ntp >> 32);
    remote_f = (uint32_t) (ntp & 0xFFFFFFFFULL);
    net_get_ntp(&local_s, &local_f);

    local = local_s;
    local *= 1000;
    local += ((uint64_t) local_f)*1000 / 0xFFFFFFFFULL;

    remote = remote_s;
    remote *= 1000;
    remote += ((uint64_t) remote_f)*1000 / 0xFFFFFFFFULL;

    return (int32_t) (local - remote);
}

int32_t net_get_timezone()
{
    //this has been commented due to some reports of broken implementation on some systems ...
    //		s32 val = timezone;
    //		return val;
    /*FIXME - avoid errors at midnight when estimating timezone this does not work !!*/
    int32_t t_timezone;
    tm *t_gmt = nullptr, *t_local = nullptr;
    time_t t_time;
    t_time = time(NULL);
    t_gmt = gmtime(&t_time);
    if(!t_gmt)
        return 0;
    t_local = localtime(&t_time);
    if(!t_local)
        return 0;

    t_timezone = (t_gmt->tm_hour - t_local->tm_hour) * 3600 + (t_gmt->tm_min - t_local->tm_min) * 60;
    return t_timezone;

}

time_t mktime_utc(struct tm *tm)
{
    return timegm(tm);
}

bool parse_bool(const char * const attr)
{
    if (!strcmp(attr, "true")) return 1;
    if (!strcmp(attr, "1")) return 1;
    return ERROR_NONE;
}

uint32_t parse_int(const char * const attr)
{
    return atoi(attr);
}

uint64_t parse_long_int(const char * const attr)
{
    uint64_t longint;
    sscanf(attr, "%lu", &longint);
    return longint;
}

double parse_double(const char * const attr)
{
    return atof(attr);
}

uint64_t parse_date(const char * const attr)
{
    return net_parse_date(attr);
}

uint64_t parse_duration(const char * const duration)
{
    uint32_t i;
    char *sep1, *sep2;
    uint32_t h, m;
    double s;
    const char *startT;
    if (!duration) {
        LOG(ERROR) << "[MPD] Error parsing duration: no value indicated\n";
        return ERROR_PARSE;
    }
    i = 0;
    while (1) {
        if (duration[i] == ' ') i++;
        else if (duration[i] == 0) return 0;
        else {
            break;
        }
    }
    if (duration[i] != 'P') {
        LOG(ERROR) << "[MPD] Error parsing duration: no value indicated\n";
        return ERROR_PARSE;
    }
    startT = strchr(duration+1, 'T');

    if (duration[i+1] == 0) {
        LOG(ERROR) << "[MPD] Error parsing duration: no value indicated\n";
        return ERROR_PARSE;
    }

    if (! startT) {
        LOG(ERROR) << "[MPD] Error parsing duration: no Time section found\n";
        return ERROR_PARSE;
    }

    h = m = 0;
    s = 0;
    if (NULL != (sep1 = strchr(const_cast<char*>(startT)+1, 'H'))) {
        *sep1 = 0;
        h = atoi(duration+i+2);
        *sep1 = 'H';
        sep1++;
    } else {
        sep1 = (char *) startT+1;
    }
    if (NULL != (sep2 = strchr(sep1, 'M'))) {
        *sep2 = 0;
        m = atoi(sep1);
        *sep2 = 'M';
        sep2++;
    } else {
        sep2 = sep1;
    }

    if (NULL != (sep1 = strchr(sep2, 'S'))) {
        *sep1 = 0;
        s = atof(sep2);
        *sep1 = 'S';
    }

    return (uint64_t)((h*3600+m*60+s)*(uint64_t)1000);
}

uint32_t mpd_parse_duration_u32(const char* const duration)
{
    uint64_t dur = parse_duration(duration);
    if (dur <= UINT_MAX) {
        return (uint32_t)dur;
    } else {
        LOG(ERROR) << "[MPD] Parsed duration" << dur << "doesn't fit on 32 bits! Setting to the 32 bits max.\n";
        return UINT_MAX;
    }
}


void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
    std::string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while(std::string::npos != pos2){
        v.push_back(s.substr(pos1, pos2 - pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }

    if(pos1 != s.length())
        v.push_back(s.substr(pos1));
}

std::string GetSubstr(std::string str, char sep, bool bBefore)
{
    std::string ret = "";
    std::size_t found = str.rfind(sep);
    if(found != std::string::npos){
        if(bBefore){
            ret = str.substr(0, found);
        }else{
            ret = str.substr( found+1, str.length()-1-found );
        }
    }
    return ret;
}

char *strlwr(char *s)
{
    char *str;
    str = s;
    while(*str != '\0')
    {
        if(*str >= 'A' && *str <= 'Z') {
            *str += 'a'-'A';
        }
        str++;
    }
    return s;
 }

// function to splice basePath + appendedPath
std::string PathSplice( std::string basePath, std::string appendedPath)
{
    uint32_t baseLen = basePath.length();
    uint32_t appendedLen = appendedPath.length();
    if(!baseLen) return appendedPath;
    if(!appendedLen) return basePath;

    string splicedPath;
    if(basePath.back() == '/' && appendedPath[0] == '/')
    {
        splicedPath = basePath.append(appendedPath.substr(1, appendedLen - 1));
    }
    else if(basePath.back() == '/' || appendedPath[0] == '/')
    {
        splicedPath = basePath + appendedPath;
    }
    else
    {
        splicedPath = basePath + "/" + appendedPath;
    }

    return splicedPath;
}

int32_t StringToInt(string str)
{
    if(!str.length())
        return -11;

    return stoi(str);
}

VCD_OMAF_END
