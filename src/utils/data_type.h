/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   data_type.h
 * Author: media
 *
 * Created on January 15, 2019, 1:48 PM
 */

#ifndef DATA_TYPE_H
#define DATA_TYPE_H

#include "ns_def.h"
#include <stdint.h>
#include "360SCVPAPI.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
   MediaType_NONE = 0,
   MediaType_Video,
   MediaType_Audio,
} MediaType;

typedef enum{
    MODE_DEFAULT = 0,
    MODE_TILE_MultiRes,
    MODE_TILE_MultiRate,
    MODE_NONE,
}MPD_MODE;

typedef struct HEADPOSE {
    float yaw;
    float pitch;
}HeadPose;

typedef struct HEADSETINFO {
    int32_t    input_geoType;
    int32_t    output_geoType;
    HeadPose   *pose;
    float      viewPort_hFOV;
    float      viewPort_vFOV;
    int32_t    viewPort_Width;
    int32_t    viewPort_Height;
}HeadSetInfo;

typedef struct Viewport {
    int32_t x;
    int32_t y;
    int32_t height;
    int32_t width;
    int32_t faceId;
}Viewport;

typedef struct SOURCERESOLUTION {
    int32_t  qualityRanking;
    uint32_t width;
    uint32_t height;
}SourceResolution;

/*
 * avg_bandwidth : average bandwidth since the begin of downloading
 * immediate_bandwidth: immediate bandwidth at the moment
 */
typedef struct DASHSTATISTICINFO{
    int32_t avg_bandwidth;
    int32_t immediate_bandwidth;
}DashStatisticInfo;

/*
 * stream_type : Video or Audio stream
 * height : the height of original video
 * width : the width of original video
 * framerate_num : frame rate
 * framerate_den : frame rate
 * channels : for Audio
 * sample_rate : for Audio
 * 
 */
typedef struct DASHSTREAMINFO{
    MediaType             stream_type;
    int32_t               height;
    int32_t               width;
    uint32_t              tileRowNum;
    uint32_t              tileColNum;
    int32_t               framerate_num;
    int32_t               framerate_den;
    uint64_t              segmentDuration;
    int32_t               bit_rate;
    int32_t               channels;
    int32_t               sample_rate;
    int32_t               channel_bytes;
    int32_t               mProjFormat;
    int32_t               mFpt;
    const char*           mime_type;
    const char*           codec;
    int32_t               source_number;
    SourceResolution*     source_resolution;
}DashStreamInfo;

/*
 * duration : the duration of the media. it is meaningless if streaming_type is dynamic
 * streaming_type: the DASH stream type: 1. static or 2. dynamic
 * stream_count: stream count of this media
 * stream_info: the array to store the information of each streams
 */
typedef struct DASHMEDIAINFO{
    uint64_t           duration;
    int32_t            streaming_type;
    int32_t            stream_count;    
    DashStreamInfo     stream_info[16];
}DashMediaInfo;

typedef struct DASHPACKET{
    uint64_t  size;
    char*     buf;
    RegionWisePacking *rwpk;
}DashPacket;

#ifdef __cplusplus
}
#endif

#endif /* DATA_TYPE_H */

