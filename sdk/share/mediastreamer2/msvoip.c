﻿/*
mediastreamer2 library - modular sound and video processing and streaming
Copyright (C) 2006  Simon MORLAT (simon.morlat@linphone.org)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "mediastreamer2/mscommon.h"
#include "mediastreamer2/mscodecutils.h"
#include "mediastreamer2/msfilter.h"

#define MS2_FILTERS

extern void __register_ffmpeg_encoders_if_possible(void);
extern void ms_ffmpeg_check_init();

#include "voipdescs.h"
#include "mediastreamer2/mssndcard.h"
#include "mediastreamer2/mswebcam.h"

#ifdef __APPLE__
   #include "TargetConditionals.h"
#endif

#ifdef ANDROID
#include <android/log.h>
#endif


#ifdef MS2_FILTERS

#ifdef __ALSA_ENABLED__
extern MSSndCardDesc alsa_card_desc;
#endif

#ifdef __QSA_ENABLED__
extern MSSndCardDesc ms_qsa_card_desc;
#endif

#ifdef HAVE_SYS_SOUNDCARD_H
extern MSSndCardDesc oss_card_desc;
#endif

#ifdef __ARTS_ENABLED__
extern MSSndCardDesc arts_card_desc;
#endif

#ifdef WIN32
extern MSSndCardDesc winsnd_card_desc;
#endif

#ifdef __DIRECTSOUND_ENABLED__
extern MSSndCardDesc winsndds_card_desc;
#endif

#ifdef __MACSND_ENABLED__
extern MSSndCardDesc ca_card_desc;
#endif

#ifdef __PORTAUDIO_ENABLED__
extern MSSndCardDesc pasnd_card_desc;
#endif

#ifdef __MAC_AQ_ENABLED__
extern MSSndCardDesc aq_card_desc;
#endif

#ifdef __PULSEAUDIO_ENABLED__
extern MSSndCardDesc pulse_card_desc;
#endif

#if TARGET_OS_IPHONE
extern MSSndCardDesc au_card_desc;
#endif

#ifdef ANDROID
extern MSSndCardDesc msandroid_sound_card_desc;
extern MSSndCardDesc android_native_snd_card_desc;
extern MSSndCardDesc android_native_snd_opensles_card_desc;
#endif

#endif /* MS2_FILTERS */

extern MSSndCardDesc castor3snd_card_desc;
static MSSndCardDesc * ms_snd_card_descs[]={
#ifdef MS2_FILTERS
#ifdef __ALSA_ENABLED__
    &alsa_card_desc,
#endif
#ifdef __QSA_ENABLED__
    &ms_qsa_card_desc,
#endif
#ifdef HAVE_SYS_SOUNDCARD_H
    &oss_card_desc,
#endif
#ifdef __ARTS_ENABLED__
    &arts_card_desc,
#endif
#if 0 //def WIN32
    &winsnd_card_desc,
#endif
#ifdef __DIRECTSOUND_ENABLED__
    &winsndds_card_desc,
#endif
#ifdef __PORTAUDIO_ENABLED__
    &pasnd_card_desc,
#endif
#ifdef __MACSND_ENABLED__
    &ca_card_desc,
#endif

#ifdef __PULSEAUDIO_ENABLED__
    &pulse_card_desc,
#endif

#if TARGET_OS_IPHONE
    &au_card_desc,
#endif
#ifdef __MAC_AQ_ENABLED__
    &aq_card_desc,
#endif
#ifdef ANDROID
    &android_native_snd_opensles_card_desc,
    &android_native_snd_card_desc,
    &msandroid_sound_card_desc,
#endif
#endif /* MS2_FILTERS */
#if 1 //ndef CFG_WIN32_SIMULATOR
    &castor3snd_card_desc,
#endif
NULL
};

#ifdef VIDEO_ENABLED

#ifdef HAVE_LINUX_VIDEODEV_H
extern MSWebCamDesc v4l_desc;
#endif

#ifdef HAVE_LINUX_VIDEODEV2_H
extern MSWebCamDesc v4l2_card_desc;
#endif

#ifdef WIN32
extern MSWebCamDesc ms_vfw_cam_desc;
#endif

#if defined(CFG_TS_MODULE_ENABLE) && defined(CFG_FFMPEG_H264_SW)
extern MSWebCamDesc castor3cam_desc;
#endif

#if defined(CFG_SENSOR_ENABLE) && !defined(CFG_FFMPEG_H264_SW)
    extern MSWebCamDesc es_cam_desc;
#endif

#if defined(WIN32) && defined(HAVE_DIRECTSHOW)
extern MSWebCamDesc ms_directx_cam_desc;
#endif

#if defined(__MINGW32__) || defined (HAVE_DIRECTSHOW)
extern MSWebCamDesc ms_dshow_cam_desc;
#endif

#if TARGET_OS_MAC && !TARGET_OS_IPHONE
extern MSWebCamDesc ms_v4m_cam_desc;
#endif

extern MSWebCamDesc static_image_desc;

#if !defined(NO_FFMPEG)
extern MSWebCamDesc mire_desc;
#endif
#ifdef ANDROID
extern MSWebCamDesc ms_android_video_capture_desc;
#endif

#if TARGET_OS_IPHONE && !TARGET_IPHONE_SIMULATOR
extern MSWebCamDesc ms_v4ios_cam_desc;
#endif
static MSWebCamDesc * ms_web_cam_descs[]={
#ifdef HAVE_LINUX_VIDEODEV2_H
    &v4l2_card_desc,
#endif
#ifdef HAVE_LINUX_VIDEODEV_H
    &v4l_desc,
#endif
#if defined(WIN32) && defined(HAVE_VFW)
    &ms_vfw_cam_desc,
#endif
#if defined(__MINGW32__) || defined (HAVE_DIRECTSHOW)
    &ms_dshow_cam_desc,
#endif
#if TARGET_OS_MAC && !TARGET_OS_IPHONE
    &ms_v4m_cam_desc,
#endif
#if defined (ANDROID)
    &ms_android_video_capture_desc,
#endif
#if TARGET_OS_IPHONE &&  !TARGET_IPHONE_SIMULATOR
    &ms_v4ios_cam_desc,
#endif
#if defined(CFG_TS_MODULE_ENABLE) && defined(CFG_FFMPEG_H264_SW)
    &castor3cam_desc,
#endif
    //&mire_desc,
#if defined(CFG_SENSOR_ENABLE) && !defined(CFG_FFMPEG_H264_SW)
    &es_cam_desc,
#endif
    &static_image_desc,
    NULL
};
#endif

static int ms_voip_ref=0;
void ms_voip_init(){
    MSSndCardManager *cm;
    int i;

    if (ms_voip_ref++ >0 ) {
        ms_message ("Skiping ms_voip_init, because [%i] ref",ms_voip_ref);
        return;
    }
    /* register builtin VoIP MSFilter's */
    for (i=0;ms_voip_filter_descs[i]!=NULL;i++){
        ms_filter_register(ms_voip_filter_descs[i]);
    }
    ms_message("Registering all soundcard handlers");
    cm=ms_snd_card_manager_get();
    for (i=0;ms_snd_card_descs[i]!=NULL;i++){
        ms_snd_card_manager_register_desc(cm,ms_snd_card_descs[i]);
    }

#ifdef VIDEO_ENABLED
    ms_message("Registering all webcam handlers");
    {
        MSWebCamManager *wm;
        wm=ms_web_cam_manager_get();
        for (i=0;ms_web_cam_descs[i]!=NULL;i++){
            ms_web_cam_manager_register_desc(wm,ms_web_cam_descs[i]);
        }
    }
#if !defined(NO_FFMPEG)
    ms_ffmpeg_check_init();
    __register_ffmpeg_encoders_if_possible();
#endif
#endif

#if defined(ANDROID) && defined (VIDEO_ENABLED)
    if (1) {
        libmsandroidopengldisplay_init();
    } else {
        if (!libmsandroiddisplay_init()) {
            libmsandroiddisplaybad_init();
        }
    }
#endif

    ms_message("ms_voip_init() done");
}

void ms_voip_exit(){
    if (--ms_voip_ref >0 ) {
        ms_message ("Skiping ms_voip_exit, still [%i] ref",ms_voip_ref);
        return;
    }

    ms_snd_card_manager_destroy();
#ifdef VIDEO_ENABLED
    ms_web_cam_manager_destroy();
#endif
}