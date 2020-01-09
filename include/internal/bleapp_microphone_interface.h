/*
********************************************************************
* THIS INFORMATION IS PROPRIETARY TO
* Cypress Semiconductor.
*-------------------------------------------------------------------
*
*           Copyright (c) 2013 Cypress Semiconductor.
*                      ALL RIGHTS RESERVED
*
********************************************************************

********************************************************************
*    File Name: microphone_interface.h
*
*    $History:$
*
********************************************************************
*/
#ifndef __BLEAPP_MICROPHONE_INTERFACE_H__
#define __BLEAPP_MICROPHONE_INTERFACE_H__

#include "types.h"
#include "blehidevent.h"
#include "bleapp_audiocodec_interface.h"

/**  \addtogroup interfaces
*/
/*! @{ */
/**
* Defines an audio codec interface. Applications that use this interface
* can plug in different audio codec with no impact. Audio codec that comply with this interface
* can be used by different applications. The BCM remote application uses this
* interface to control audio codec activities.
*/

#define BUFFER_SIZE 120

//Fixed report ID for audio
#define AUDIO_REPORT_ID  0xF7

// Definition of bitmap for for buffer state in VoiceReport
#define PCMBUFFSTATE_FIFO_MASK  0x0007  // 3 bits only
#define PCMBUFFSTATE_FIFO_INP_SHFT 0   // Bit 0-2
#define PCMBUFFSTATE_FIFO_OUT_SHFT 3   // Bit 3-5
#define PCMBUFFSTATE_FIFO_UNRFLOW_SHFT 6   // Bit 6-8
#define PCMBUFFSTATE_FIFO_OVRFLOW_SHFT 9   // Bit 9-11

enum
{
    AUDIO_BUTTON_NONE     = 0x00,   // For cases when remote does not have a dedicated audio button
    AUDIO_BUTTON_SEND_MSG = 0x01,   // For cases when remote requests host to send the correct command:
                                    // RC-> Host:   BRCM_RC_MIC_START_REQ,
                                    // Host->RC:    BRCM_MIC_START
    AUDIO_BUTTON_SEND_PCM = 0x02,   // For cases when remote starts/stops audio streaming upon button
                                    // PCM data is sent for as long as the audio key is held down
};

enum
{
    BRCM_AUDIO_DATA             = 0x01,
    BRCM_MIC_START              = 0x02,
    BRCM_MIC_STOP               = 0x03,
    BRCM_SPK_START              = 0x04,
    BRCM_SPK_STOP               = 0x05,

    BRCM_RC_CODECSETTINGS_RD_REQ    = 0x06,
    BRCM_RC_CODECSETTINGS_RD_ACK    = 0x07,
    BRCM_RC_CODECSETTINGS_WT_REQ    = 0x08,
    BRCM_RC_CODECSETTINGS_WT_ACK    = 0x09,
    BRCM_RC_VOICEMODE_RD_REQ        = 0x0A,
    BRCM_RC_VOICEMODE_RD_ACK        = 0x0B,

    BRCM_RC_MIC_START_REQ       = 0x0C,
    BRCM_RC_MIC_STOP_REQ        = 0x0D,
    BRCM_RC_SPEAKER_START_REQ   = 0x0E,
    BRCM_RC_SPEAKER_STOP_REQ    = 0x0F,
    BRCM_PHONECALL_START        = 0x10,
    BRCM_PHONECALL_STOP         = 0x11,
    BRCM_RC_PHONECALL_START_REQ = 0x12,
    BRCM_RC_PHONECALL_END_REQ   = 0x13,

};


enum
{
    CODEC_SR   = 0,
    CODEC_PCM  = 1,
    CODEC_PGA  = 2,
    CODEC_BPS  = 3,
    CODEC_HPF  = 4,
};


#pragma pack(1)
typedef struct
{
    BYTE   reportId;                 // Audio data report ID 0xf7
    BYTE   format;                   // BRCM_AUDIO_DATA 0x01
    UINT16 sqn;                      // sequence number
    UINT16 ts;                       // time stamp
    UINT16 buf_state;                // buffer state
    UINT16 rsvd;                     // reserved
    UINT16 dataCnt;                  // sizeof(array(databuffer[]))
    INT16  dataBuffer[BUFFER_SIZE];  // Audo data
} VoiceReport;

typedef struct
{
    BYTE reportId;                   // Audio control report ID 0xf7
    BYTE format;                     // 0x2+
    BYTE channel;                    // 0/1
    BYTE rsvd;                       // reserved
    UINT16 dataCnt;                  // sizeof(array(databuffer[]))
    BYTE   dataBuffer[6];
} VoiceControlDM1Report;

typedef PACKED struct
{
    /// Base event info
    HidEvent eventInfo;

    void * userDataPtr;
}HidEventUserDefine;

#pragma pack()


typedef struct
{
    void (*init)(void (*userfn)(void *), void* userdata);
    BYTE (*pollActivityUser)(void * data);

    void (*setActive)(BOOLEAN a);
    BOOLEAN (*isActive)(void);
    void (*stopPCM)(void);

    UINT8 (*audioFIFOCnt)(void);
    UINT8 (*audioFIFOOutIndex)(void);
    void  (*incAudioFIFOOutIndex)(void);
    UINT8 (*audioReportsToSend)(void);
    void  (*decAudioReportsToSend)(void);
    void (*overflow)(void);
    UINT16 (*isOverflow)(void);

    BOOLEAN (*readCodecSetting)(VoiceControlDM1Report * rp);
    BOOLEAN (*writeCodecSetting)(BYTE t, BYTE *d);
    tAudioCodecInterfaceVtbl * (*getCodec)(void);

}tMicrophoneInterfaceVtbl;


typedef struct
{
    tAudioCodecInterfaceVtbl *mic_codec;
    VoiceReport *audio_fifo;
    UINT16 *data_count;
    UINT8 fifo_count;
    BOOLEAN enable;
    UINT16 audio_delay;
    BYTE audio_gain;
    BYTE audio_boost;
    BYTE maxSampleInOneSlot;
}tMicrophoneInterfaceConfig;


#endif
