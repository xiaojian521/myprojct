#define LOG_TAG "audio_hw_primary"
#define LOG_NDEBUG 0
#define VERY_VERY_VERBOSE_LOGGING
#ifdef VERY_VERY_VERBOSE_LOGGING
#define ALOGVV ALOGV
#else
#define ALOGVV(a...) do { } while(0)
#endif

#include "CommonDefine.h"
#ifdef ALSA_DEBUG
    #include "DumpAdapter.h"
#endif

#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>
#include <stdlib.h>
#include <math.h>
#include <dlfcn.h>
#include <sys/resource.h>
#include <sys/prctl.h>

#include <cutils/log.h>
#include <cutils/str_parms.h>
#include <cutils/properties.h>
#include <cutils/atomic.h>
#include <cutils/sched_policy.h>

#include <hardware/audio_effect.h>
#include <system/thread_defs.h>
#include <audio_effects/effect_aec.h>
#include <audio_effects/effect_ns.h>
#include "audio_hw.h"
#include "platform_api.h"
#include <platform.h>
#include "audio_extn.h"
#include "voice_extn.h"

#include "sound/compress_params.h"
#include "sound/asound.h"

#ifdef VHAL_HELPER_ENABLED
#include <vehicle-hal-audio-helper-for-c.h>
#endif

#define COMPRESS_OFFLOAD_NUM_FRAGMENTS 4
/*DIRECT PCM has same buffer sizes as DEEP Buffer*/
#define DIRECT_PCM_NUM_FRAGMENTS 2
/* ToDo: Check and update a proper value in msec */
#define COMPRESS_OFFLOAD_PLAYBACK_LATENCY 50
#define COMPRESS_PLAYBACK_VOLUME_MAX 0x2000

#define PROXY_OPEN_RETRY_COUNT           100
#define PROXY_OPEN_WAIT_TIME             20

#ifdef USE_LL_AS_PRIMARY_OUTPUT
#define USECASE_AUDIO_PLAYBACK_PRIMARY USECASE_AUDIO_PLAYBACK_LOW_LATENCY
#define PCM_CONFIG_AUDIO_PLAYBACK_PRIMARY pcm_config_low_latency
#else
#define USECASE_AUDIO_PLAYBACK_PRIMARY USECASE_AUDIO_PLAYBACK_DEEP_BUFFER
#define PCM_CONFIG_AUDIO_PLAYBACK_PRIMARY pcm_config_deep_buffer
#endif

#ifdef BUS_ADDRESS_ENABLED
#define MAX_CAR_AUDIO_STREAMS    8
/* This defines the physical streams supported in audio HAL,
   limited by the available front-end PCM driver in H/W.
   Max number of physical streams supported currently is 8 and is represented
   by stream bit flag as indicated in vehicle HAL interface. */
enum {
    /** Media playback (deep buffer playback). */
    CAR_AUDIO_STREAM_MEDIA               = 0x1,
    /** System/notification sound (low latency playback). */
    CAR_AUDIO_STREAM_SYS_NOTIFICATION    = 0x2,
    /** Navigation guidance (driver side playback). */
    CAR_AUDIO_STREAM_NAV_GUIDANCE        = 0x4,
   /** Voice call (call). */
    CAR_AUDIO_STREAM_PHONE               = 0x8,
};
#endif

static unsigned int configured_low_latency_capture_period_size =
        LOW_LATENCY_CAPTURE_PERIOD_SIZE;

struct pcm_config pcm_config_deep_buffer = {
    .channels = 2,
    .rate = DEFAULT_OUTPUT_SAMPLING_RATE,
    .period_size = DEEP_BUFFER_OUTPUT_PERIOD_SIZE,
    .period_count = DEEP_BUFFER_OUTPUT_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
    .start_threshold = DEEP_BUFFER_OUTPUT_PERIOD_SIZE / 4,
    .stop_threshold = INT_MAX,
    .avail_min = DEEP_BUFFER_OUTPUT_PERIOD_SIZE / 4,
};

struct pcm_config pcm_config_low_latency = {
    .channels = 2,
    .rate = DEFAULT_OUTPUT_SAMPLING_RATE,
    .period_size = LOW_LATENCY_OUTPUT_PERIOD_SIZE,
    .period_count = LOW_LATENCY_OUTPUT_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
    .start_threshold = LOW_LATENCY_OUTPUT_PERIOD_SIZE / 4,
    .stop_threshold = INT_MAX,
    .avail_min = LOW_LATENCY_OUTPUT_PERIOD_SIZE / 4,
};

struct pcm_config pcm_config_hdmi_multi = {
    .channels = HDMI_MULTI_DEFAULT_CHANNEL_COUNT, /* changed when the stream is opened */
    .rate = DEFAULT_OUTPUT_SAMPLING_RATE, /* changed when the stream is opened */
    .period_size = HDMI_MULTI_PERIOD_SIZE,
    .period_count = HDMI_MULTI_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
    .start_threshold = 0,
    .stop_threshold = INT_MAX,
    .avail_min = 0,
};

struct pcm_config pcm_config_driver_side = {
    .channels = 2,
    .rate = DEFAULT_OUTPUT_SAMPLING_RATE,
    .period_size = DEEP_BUFFER_OUTPUT_PERIOD_SIZE,
    .period_count = DEEP_BUFFER_OUTPUT_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
    .start_threshold = DEEP_BUFFER_OUTPUT_PERIOD_SIZE / 4,
    .stop_threshold = INT_MAX,
    .avail_min = DEEP_BUFFER_OUTPUT_PERIOD_SIZE / 4,
};

struct pcm_config pcm_config_audio_capture = {
    .channels = 2,
    .period_count = AUDIO_CAPTURE_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
};

struct pcm_config pcm_config_audio_capture_eight_channel = {
    .channels = 8,
    .period_count = AUDIO_CAPTURE_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
};

#define AFE_PROXY_CHANNEL_COUNT 2
#define AFE_PROXY_SAMPLING_RATE 48000

#define AFE_PROXY_PLAYBACK_PERIOD_SIZE  768
#define AFE_PROXY_PLAYBACK_PERIOD_COUNT 4

struct pcm_config pcm_config_afe_proxy_playback = {
    .channels = AFE_PROXY_CHANNEL_COUNT,
    .rate = AFE_PROXY_SAMPLING_RATE,
    .period_size = AFE_PROXY_PLAYBACK_PERIOD_SIZE,
    .period_count = AFE_PROXY_PLAYBACK_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
    .start_threshold = AFE_PROXY_PLAYBACK_PERIOD_SIZE,
    .stop_threshold = INT_MAX,
    .avail_min = AFE_PROXY_PLAYBACK_PERIOD_SIZE,
};

#define AFE_PROXY_RECORD_PERIOD_SIZE  768
#define AFE_PROXY_RECORD_PERIOD_COUNT 4

struct pcm_config pcm_config_afe_proxy_record = {
    .channels = AFE_PROXY_CHANNEL_COUNT,
    .rate = AFE_PROXY_SAMPLING_RATE,
    .period_size = AFE_PROXY_RECORD_PERIOD_SIZE,
    .period_count = AFE_PROXY_RECORD_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
    .start_threshold = AFE_PROXY_RECORD_PERIOD_SIZE,
    .stop_threshold = INT_MAX,
    .avail_min = AFE_PROXY_RECORD_PERIOD_SIZE,
};

const char * const use_case_table[AUDIO_USECASE_MAX] = {
    [USECASE_AUDIO_PLAYBACK_DEEP_BUFFER] = "deep-buffer-playback",
    [USECASE_AUDIO_PLAYBACK_LOW_LATENCY] = "low-latency-playback",
    [USECASE_AUDIO_PLAYBACK_ULL]         = "audio-ull-playback",
    [USECASE_AUDIO_PLAYBACK_MULTI_CH]    = "multi-channel-playback",
    [USECASE_AUDIO_PLAYBACK_OFFLOAD] = "compress-offload-playback",
#ifdef MULTIPLE_OFFLOAD_ENABLED
    [USECASE_AUDIO_PLAYBACK_OFFLOAD2] = "compress-offload-playback2",
    [USECASE_AUDIO_PLAYBACK_OFFLOAD3] = "compress-offload-playback3",
    [USECASE_AUDIO_PLAYBACK_OFFLOAD4] = "compress-offload-playback4",
    [USECASE_AUDIO_PLAYBACK_OFFLOAD5] = "compress-offload-playback5",
    [USECASE_AUDIO_PLAYBACK_OFFLOAD6] = "compress-offload-playback6",
    [USECASE_AUDIO_PLAYBACK_OFFLOAD7] = "compress-offload-playback7",
    [USECASE_AUDIO_PLAYBACK_OFFLOAD8] = "compress-offload-playback8",
    [USECASE_AUDIO_PLAYBACK_OFFLOAD9] = "compress-offload-playback9",
#endif
    [USECASE_AUDIO_DIRECT_PCM_OFFLOAD] = "compress-offload-playback2",

    [USECASE_AUDIO_RECORD] = "audio-record",
    [USECASE_AUDIO_RECORD_COMPRESS] = "audio-record-compress",
    [USECASE_AUDIO_RECORD_LOW_LATENCY] = "low-latency-record",
    [USECASE_AUDIO_RECORD_FM_VIRTUAL] = "fm-virtual-record",
    [USECASE_AUDIO_PLAYBACK_FM] = "play-fm",
    [USECASE_AUDIO_HFP_SCO_UPLINK] = "hfp-sco",
    [USECASE_AUDIO_HFP_SCO_WB_UPLINK] = "hfp-sco-wb",
    [USECASE_VOICE_CALL] = "voice-call",

    [USECASE_VOICE2_CALL] = "voice2-call",
    [USECASE_VOLTE_CALL] = "volte-call",
    [USECASE_QCHAT_CALL] = "qchat-call",
    [USECASE_VOWLAN_CALL] = "vowlan-call",
    [USECASE_VOICEMMODE1_CALL] = "voicemmode1-call",
    [USECASE_VOICEMMODE2_CALL] = "voicemmode2-call",
    [USECASE_COMPRESS_VOIP_CALL] = "compress-voip-call",
    [USECASE_INCALL_REC_UPLINK] = "incall-rec-uplink",
    [USECASE_INCALL_REC_DOWNLINK] = "incall-rec-downlink",
    [USECASE_INCALL_REC_UPLINK_AND_DOWNLINK] = "incall-rec-uplink-and-downlink",
    [USECASE_INCALL_REC_UPLINK_COMPRESS] = "incall-rec-uplink-compress",
    [USECASE_INCALL_REC_DOWNLINK_COMPRESS] = "incall-rec-downlink-compress",
    [USECASE_INCALL_REC_UPLINK_AND_DOWNLINK_COMPRESS] = "incall-rec-uplink-and-downlink-compress",

    [USECASE_INCALL_MUSIC_UPLINK] = "incall_music_uplink",
    [USECASE_INCALL_MUSIC_UPLINK2] = "incall_music_uplink2",
    [USECASE_AUDIO_SPKR_CALIB_RX] = "spkr-rx-calib",
    [USECASE_AUDIO_SPKR_CALIB_TX] = "spkr-vi-record",

    [USECASE_AUDIO_PLAYBACK_AFE_PROXY] = "afe-proxy-playback",
    [USECASE_AUDIO_RECORD_AFE_PROXY] = "afe-proxy-record",
    [USECASE_AUDIO_PLAYBACK_DRIVER_SIDE] = "driver-side-playback",
    [USECASE_AUDIO_FM_TUNER_EXT] = "fm-tuner-ext",
    [USECASE_ICC_CALL] = "icc-call",
    [USECASE_ANC_LOOPBACK] = "anc-loopback",
#ifdef BUS_ADDRESS_ENABLED
    [USECASE_AUDIO_PLAYBACK_MEDIA] = "media-playback",
    [USECASE_AUDIO_PLAYBACK_SYS_NOTIFICATION] = "sys-notification-playback",
#endif
    [USECASE_AUDIO_LINE_IN_PASSTHROUGH] = "line-in-passthrough",
    [USECASE_AUDIO_HDMI_IN_PASSTHROUGH] = "hdmi-in-passthrough",
    [USECASE_AUDIO_RECORD_EIGHT_CHANNEL] = "audio-record-eight-channel",
    [USECASE_AUDIO_PLAYBACK_PHONE] = "phone-playback",
};

static const audio_usecase_t offload_usecases[] = {
    USECASE_AUDIO_PLAYBACK_OFFLOAD,
#ifdef MULTIPLE_OFFLOAD_ENABLED
    USECASE_AUDIO_PLAYBACK_OFFLOAD2,
    USECASE_AUDIO_PLAYBACK_OFFLOAD3,
    USECASE_AUDIO_PLAYBACK_OFFLOAD4,
    USECASE_AUDIO_PLAYBACK_OFFLOAD5,
    USECASE_AUDIO_PLAYBACK_OFFLOAD6,
    USECASE_AUDIO_PLAYBACK_OFFLOAD7,
    USECASE_AUDIO_PLAYBACK_OFFLOAD8,
    USECASE_AUDIO_PLAYBACK_OFFLOAD9,
#endif
    USECASE_AUDIO_DIRECT_PCM_OFFLOAD,
};

#define STRING_TO_ENUM(string) { #string, string }

struct string_to_enum {
    const char *name;
    uint32_t value;
};

static const struct string_to_enum out_channels_name_to_enum_table[] = {
    STRING_TO_ENUM(AUDIO_CHANNEL_OUT_STEREO),
    STRING_TO_ENUM(AUDIO_CHANNEL_OUT_QUAD),/* QUAD_BACK is same as QUAD */
    STRING_TO_ENUM(AUDIO_CHANNEL_OUT_QUAD_SIDE),
#if 0
    STRING_TO_ENUM(AUDIO_CHANNEL_OUT_PENTA),
#endif
    STRING_TO_ENUM(AUDIO_CHANNEL_OUT_5POINT1), /* 5POINT1_BACK is same as 5POINT1 */
    STRING_TO_ENUM(AUDIO_CHANNEL_OUT_5POINT1_SIDE),
    STRING_TO_ENUM(AUDIO_CHANNEL_OUT_7POINT1),
};

static const struct string_to_enum out_formats_name_to_enum_table[] = {
    STRING_TO_ENUM(AUDIO_FORMAT_AC3),
    STRING_TO_ENUM(AUDIO_FORMAT_E_AC3),
    STRING_TO_ENUM(AUDIO_FORMAT_E_AC3_JOC),
};

static struct audio_device *adev = NULL;
static pthread_mutex_t adev_init_lock;
static unsigned int audio_device_ref_count;

static int set_voice_volume_l(struct audio_device *adev, float volume);

static int parse_snd_card_status(struct str_parms *parms, int *card,
                                 card_status_t *status)
{
    char value[32]={0};
    char state[32]={0};

    int  ret = str_parms_get_str(parms, "SND_CARD_STATUS", value, sizeof(value));
    if (ret < 0)
        return -1;

    // sscanf should be okay as value is of max length 32.
    // same as sizeof state.
    if (sscanf(value, "%d,%s", card, state) < 2)
        return -1;

    *status = !strcmp(state, "ONLINE") ? CARD_STATUS_ONLINE :
                                         CARD_STATUS_OFFLINE;
    return 0;
}

__attribute__ ((visibility ("default")))
bool audio_hw_send_gain_dep_calibration(int level) {
    bool ret_val = false;
    ALOGV("%s: called ... ", __func__);

    pthread_mutex_lock(&adev_init_lock);

    if (adev != NULL && adev->platform != NULL) {
        pthread_mutex_lock(&adev->lock);
        ret_val = platform_send_gain_dep_cal(adev->platform, level);
        pthread_mutex_unlock(&adev->lock);
    } else {
        ALOGE("%s: %s is NULL", __func__, adev == NULL ? "adev" : "adev->platform");
    }

    pthread_mutex_unlock(&adev_init_lock);

    return ret_val;
}

static int check_and_set_gapless_mode(struct audio_device *adev, bool enable_gapless)
{
    bool gapless_enabled = false;
    const char *mixer_ctl_name = "Compress Gapless Playback";
    struct mixer_ctl *ctl;

    ALOGV("%s:", __func__);
    gapless_enabled = property_get_bool("audio.offload.gapless.enabled", false);

    /*Disable gapless if its AV playback*/
    gapless_enabled = gapless_enabled && enable_gapless;

    ctl = mixer_get_ctl_by_name(adev->mixer, mixer_ctl_name);
    if (!ctl) {
        ALOGE("%s: Could not get ctl for mixer cmd - %s",
                               __func__, mixer_ctl_name);
        return -EINVAL;
    }

    if (mixer_ctl_set_value(ctl, 0, gapless_enabled) < 0) {
        ALOGE("%s: Could not set gapless mode %d",
                       __func__, gapless_enabled);
         return -EINVAL;
    }
    return 0;
}

static bool is_supported_format(audio_format_t format)
{
    if (format == AUDIO_FORMAT_MP3 ||
        format == AUDIO_FORMAT_AAC_LC ||
        format == AUDIO_FORMAT_AAC_HE_V1 ||
        format == AUDIO_FORMAT_AAC_HE_V2 ||
#ifdef AAC_ADTS_OFFLOAD_ENABLED
        format == AUDIO_FORMAT_AAC_ADTS_LC ||
        format == AUDIO_FORMAT_AAC_ADTS_HE_V1 ||
        format == AUDIO_FORMAT_AAC_ADTS_HE_V2 ||
#endif
        format == AUDIO_FORMAT_PCM_24_BIT_PACKED ||
        format == AUDIO_FORMAT_PCM_8_24_BIT ||
        format == AUDIO_FORMAT_PCM_16_BIT ||
#ifdef FLAC_OFFLOAD_ENABLED
        format == AUDIO_FORMAT_FLAC ||
#endif
#ifdef ALAC_OFFLOAD_ENABLED
        format == AUDIO_FORMAT_ALAC ||
#endif
#ifdef APE_OFFLOAD_ENABLED
        format == AUDIO_FORMAT_APE ||
#endif
        format == AUDIO_FORMAT_VORBIS
#ifdef WMA_OFFLOAD_ENABLED
        || format == AUDIO_FORMAT_WMA ||
        format == AUDIO_FORMAT_WMA_PRO
#endif
#ifdef APTX_OFFLOAD_ENABLED
        || format == AUDIO_FORMAT_APTX
#endif
       )
           return true;

    return false;
}

static int get_snd_codec_id(audio_format_t format)
{
    int id = 0;

    switch (format & AUDIO_FORMAT_MAIN_MASK) {
    case AUDIO_FORMAT_MP3:
        id = SND_AUDIOCODEC_MP3;
        break;
    case AUDIO_FORMAT_AAC:
        id = SND_AUDIOCODEC_AAC;
        break;
#ifdef AAC_ADTS_OFFLOAD_ENABLED
    case AUDIO_FORMAT_AAC_ADTS:
        id = SND_AUDIOCODEC_AAC;
        break;
#endif
    case AUDIO_FORMAT_PCM:
        id = SND_AUDIOCODEC_PCM;
        break;
#ifdef FLAC_OFFLOAD_ENABLED
    case AUDIO_FORMAT_FLAC:
        id = SND_AUDIOCODEC_FLAC;
        break;
#endif
#ifdef ALAC_OFFLOAD_ENABLED
    case AUDIO_FORMAT_ALAC:
        id = SND_AUDIOCODEC_ALAC;
        break;
#endif
#ifdef APE_OFFLOAD_ENABLED
    case AUDIO_FORMAT_APE:
        id = SND_AUDIOCODEC_APE;
        break;
#endif
    case AUDIO_FORMAT_VORBIS:
        id = SND_AUDIOCODEC_VORBIS;
        break;
#ifdef WMA_OFFLOAD_ENABLED
    case AUDIO_FORMAT_WMA:
        id = SND_AUDIOCODEC_WMA;
        break;
    case AUDIO_FORMAT_WMA_PRO:
        id = SND_AUDIOCODEC_WMA_PRO;
        break;
#endif
#ifdef APTX_OFFLOAD_ENABLED
    case AUDIO_FORMAT_APTX:
        id = SND_AUDIOCODEC_APTX;
        break;
#endif
    default:
        ALOGE("%s: Unsupported audio format :%x", __func__, format);
    }

    return id;
}

static int enable_audio_route_for_voice_usecases(struct audio_device *adev,
                                                 struct audio_usecase *uc_info)
{
    struct listnode *node;
    struct audio_usecase *usecase;

    if (uc_info == NULL)
        return -EINVAL;

    /* Re-route all voice usecases on the shared backend other than the
       specified usecase to new snd devices */
    list_for_each(node, &adev->usecase_list) {
        usecase = node_to_item(node, struct audio_usecase, list);
        if ((usecase->type == VOICE_CALL) && (usecase != uc_info))
            enable_audio_route(adev, usecase);
    }
    return 0;
}

int pcm_ioctl(struct pcm *pcm, int request, ...)
{
    va_list ap;
    void * arg;
    int pcm_fd = *(int*)pcm;

    va_start(ap, request);
    arg = va_arg(ap, void *);
    va_end(ap);

    return ioctl(pcm_fd, request, arg);
}

int enable_audio_route(struct audio_device *adev,
                       struct audio_usecase *usecase)
{
    snd_device_t snd_device;
    char mixer_path[MIXER_PATH_MAX_LENGTH];

    if (usecase == NULL)
        return -EINVAL;

    ALOGV("%s: enter: usecase(%d)", __func__, usecase->id);

    if (usecase->type == PCM_CAPTURE)
        snd_device = usecase->in_snd_device;
    else
        snd_device = usecase->out_snd_device;

#ifdef DS1_DOLBY_DAP_ENABLED
    audio_extn_dolby_set_dmid(adev);
    audio_extn_dolby_set_endpoint(adev);
#endif
    audio_extn_dolby_ds2_set_endpoint(adev);
    audio_extn_sound_trigger_update_stream_status(usecase, ST_EVENT_STREAM_BUSY);
    audio_extn_listen_update_stream_status(usecase, LISTEN_EVENT_STREAM_BUSY);
    audio_extn_utils_send_audio_calibration(adev, usecase);
    audio_extn_utils_send_app_type_cfg(adev, usecase);
    strlcpy(mixer_path, use_case_table[usecase->id], MIXER_PATH_MAX_LENGTH);
    platform_add_backend_name(mixer_path, snd_device, usecase);
    ALOGD("%s: apply mixer and update path: %s", __func__, mixer_path);
    audio_route_apply_and_update_path(adev->audio_route, mixer_path);
    ALOGV("%s: exit", __func__);
    return 0;
}

int disable_audio_route(struct audio_device *adev,
                        struct audio_usecase *usecase)
{
    snd_device_t snd_device;
    char mixer_path[MIXER_PATH_MAX_LENGTH];

    if (usecase == NULL || usecase->id == USECASE_INVALID)
        return -EINVAL;

    ALOGV("%s: enter: usecase(%d)", __func__, usecase->id);
    if (usecase->type == PCM_CAPTURE)
        snd_device = usecase->in_snd_device;
    else
        snd_device = usecase->out_snd_device;
    strlcpy(mixer_path, use_case_table[usecase->id], MIXER_PATH_MAX_LENGTH);
    platform_add_backend_name(mixer_path, snd_device, usecase);
    ALOGD("%s: reset and update mixer path: %s", __func__, mixer_path);
    audio_route_reset_and_update_path(adev->audio_route, mixer_path);
    audio_extn_sound_trigger_update_stream_status(usecase, ST_EVENT_STREAM_FREE);
    audio_extn_listen_update_stream_status(usecase, LISTEN_EVENT_STREAM_FREE);
    ALOGV("%s: exit", __func__);
    return 0;
}

int enable_snd_device(struct audio_device *adev,
                      snd_device_t snd_device)
{
    char device_name[DEVICE_NAME_MAX_SIZE] = {0};

    if (snd_device < SND_DEVICE_MIN ||
        snd_device >= SND_DEVICE_MAX) {
        ALOGE("%s: Invalid sound device %d", __func__, snd_device);
        return -EINVAL;
    }

    adev->snd_dev_ref_cnt[snd_device]++;

    if(platform_get_snd_device_name_extn(adev->platform, snd_device, device_name) < 0 ) {
        ALOGE("%s: Invalid sound device returned", __func__);
        return -EINVAL;
    }
    if (adev->snd_dev_ref_cnt[snd_device] > 1) {
        ALOGV("%s: snd_device(%d: %s) is already active",
              __func__, snd_device, device_name);
        return 0;
    }

    if (audio_extn_spkr_prot_is_enabled())
         audio_extn_spkr_prot_calib_cancel(adev);
    /* start usb playback thread */
    if(SND_DEVICE_OUT_USB_HEADSET == snd_device ||
       SND_DEVICE_OUT_SPEAKER_AND_USB_HEADSET == snd_device)
        audio_extn_usb_start_playback(adev);

    /* start usb capture thread */
    if(SND_DEVICE_IN_USB_HEADSET_MIC == snd_device)
       audio_extn_usb_start_capture(adev);

    if ((snd_device == SND_DEVICE_OUT_SPEAKER ||
         snd_device == SND_DEVICE_OUT_SPEAKER_VBAT ||
         snd_device == SND_DEVICE_OUT_VOICE_SPEAKER_VBAT ||
         snd_device == SND_DEVICE_OUT_VOICE_SPEAKER) &&
         audio_extn_spkr_prot_is_enabled()) {
       if (audio_extn_spkr_prot_get_acdb_id(snd_device) < 0) {
           adev->snd_dev_ref_cnt[snd_device]--;
           return -EINVAL;
       }
       audio_extn_dev_arbi_acquire(snd_device);
        if (audio_extn_spkr_prot_start_processing(snd_device)) {
            ALOGE("%s: spkr_start_processing failed", __func__);
            audio_extn_dev_arbi_release(snd_device);
            return -EINVAL;
        }
    } else {
        ALOGD("%s: snd_device(%d: %s)", __func__, snd_device, device_name);
        /* due to the possibility of calibration overwrite between listen
            and audio, notify listen hal before audio calibration is sent */
        audio_extn_sound_trigger_update_device_status(snd_device,
                                        ST_EVENT_SND_DEVICE_BUSY);
        audio_extn_listen_update_device_status(snd_device,
                                        LISTEN_EVENT_SND_DEVICE_BUSY);
        if (platform_get_snd_device_acdb_id(snd_device) < 0) {
            adev->snd_dev_ref_cnt[snd_device]--;
            audio_extn_sound_trigger_update_device_status(snd_device,
                                            ST_EVENT_SND_DEVICE_FREE);
            audio_extn_listen_update_device_status(snd_device,
                                        LISTEN_EVENT_SND_DEVICE_FREE);
            return -EINVAL;
        }
        audio_extn_dev_arbi_acquire(snd_device);
        audio_route_apply_and_update_path(adev->audio_route, device_name);
    }
    return 0;
}

int disable_snd_device(struct audio_device *adev,
                       snd_device_t snd_device)
{
    char device_name[DEVICE_NAME_MAX_SIZE] = {0};

    if (snd_device < SND_DEVICE_MIN ||
        snd_device >= SND_DEVICE_MAX) {
        ALOGE("%s: Invalid sound device %d", __func__, snd_device);
        return -EINVAL;
    }
    if (adev->snd_dev_ref_cnt[snd_device] <= 0) {
        ALOGE("%s: device ref cnt is already 0", __func__);
        return -EINVAL;
    }

    adev->snd_dev_ref_cnt[snd_device]--;

    if(platform_get_snd_device_name_extn(adev->platform, snd_device, device_name) < 0) {
        ALOGE("%s: Invalid sound device returned", __func__);
        return -EINVAL;
    }

    if (adev->snd_dev_ref_cnt[snd_device] == 0) {
        ALOGD("%s: snd_device(%d: %s)", __func__, snd_device, device_name);
        /* exit usb play back thread */
        if(SND_DEVICE_OUT_USB_HEADSET == snd_device ||
           SND_DEVICE_OUT_SPEAKER_AND_USB_HEADSET == snd_device)
            audio_extn_usb_stop_playback();

        /* exit usb capture thread */
        if(SND_DEVICE_IN_USB_HEADSET_MIC == snd_device)
            audio_extn_usb_stop_capture();

        if ((snd_device == SND_DEVICE_OUT_SPEAKER ||
             snd_device == SND_DEVICE_OUT_SPEAKER_VBAT ||
             snd_device == SND_DEVICE_OUT_VOICE_SPEAKER_VBAT ||
             snd_device == SND_DEVICE_OUT_VOICE_SPEAKER) &&
             audio_extn_spkr_prot_is_enabled()) {
            audio_extn_spkr_prot_stop_processing(snd_device);
        } else {
            audio_route_reset_and_update_path(adev->audio_route, device_name);
        }

        if (snd_device == SND_DEVICE_OUT_HDMI)
            adev->is_channel_status_set = false;

        audio_extn_dev_arbi_release(snd_device);
        audio_extn_sound_trigger_update_device_status(snd_device,
                                        ST_EVENT_SND_DEVICE_FREE);
        audio_extn_listen_update_device_status(snd_device,
                                        LISTEN_EVENT_SND_DEVICE_FREE);
    }

    return 0;
}

static void check_usecases_codec_backend(struct audio_device *adev,
                                          struct audio_usecase *uc_info,
                                          snd_device_t snd_device)
{
    struct listnode *node;
    struct audio_usecase *usecase;
    bool switch_device[AUDIO_USECASE_MAX];
    int i, num_uc_to_switch = 0;
    int backend_idx = DEFAULT_CODEC_BACKEND;
    int usecase_backend_idx = DEFAULT_CODEC_BACKEND;

    /*
     * This function is to make sure that all the usecases that are active on
     * the hardware codec backend are always routed to any one device that is
     * handled by the hardware codec.
     * For example, if low-latency and deep-buffer usecases are currently active
     * on speaker and out_set_parameters(headset) is received on low-latency
     * output, then we have to make sure deep-buffer is also switched to headset,
     * because of the limitation that both the devices cannot be enabled
     * at the same time as they share the same backend.
     */
    /*
     * This call is to check if we need to force routing for a particular stream
     * If there is a backend configuration change for the device when a
     * new stream starts, then ADM needs to be closed and re-opened with the new
     * configuraion. This call check if we need to re-route all the streams
     * associated with the backend. Touch tone + 24 bit + native playback.
     */
    bool force_routing = platform_check_and_set_codec_backend_cfg(adev, uc_info,
                         snd_device);
    backend_idx = platform_get_backend_index(snd_device);
    /* Disable all the usecases on the shared backend other than the
     * specified usecase.
     */
    for (i = 0; i < AUDIO_USECASE_MAX; i++)
        switch_device[i] = false;

    list_for_each(node, &adev->usecase_list) {
        usecase = node_to_item(node, struct audio_usecase, list);

        if (usecase == uc_info)
            continue;
        usecase_backend_idx = platform_get_backend_index(usecase->out_snd_device);
        ALOGV("%s: backend_idx: %d,"
              "usecase_backend_idx: %d, curr device: %s, usecase device:"
              "%s", __func__, backend_idx, usecase_backend_idx, platform_get_snd_device_name(snd_device),
        platform_get_snd_device_name(usecase->out_snd_device));

        if (usecase->type != PCM_CAPTURE &&
                usecase->type != PCM_PASSTHROUGH &&
                (usecase->out_snd_device != snd_device || force_routing)  &&
                usecase->devices & AUDIO_DEVICE_OUT_ALL_CODEC_BACKEND &&
                usecase_backend_idx == backend_idx) {
            ALOGD("%s: Usecase (%s) is active on (%s) - disabling ..", __func__,
                  use_case_table[usecase->id],
                  platform_get_snd_device_name(usecase->out_snd_device));
            disable_audio_route(adev, usecase);
            switch_device[usecase->id] = true;
            num_uc_to_switch++;
        }
    }

    if (num_uc_to_switch) {
        /* All streams have been de-routed. Disable the device */

        /* Make sure the previous devices to be disabled first and then enable the
           selected devices */
        list_for_each(node, &adev->usecase_list) {
            usecase = node_to_item(node, struct audio_usecase, list);
            if (switch_device[usecase->id]) {
                disable_snd_device(adev, usecase->out_snd_device);
            }
        }

        list_for_each(node, &adev->usecase_list) {
            usecase = node_to_item(node, struct audio_usecase, list);
            if (switch_device[usecase->id]) {
                enable_snd_device(adev, snd_device);
            }
        }

        /* Re-route all the usecases on the shared backend other than the
           specified usecase to new snd devices */
        list_for_each(node, &adev->usecase_list) {
            usecase = node_to_item(node, struct audio_usecase, list);
            /* Update the out_snd_device only for the usecases that are enabled here */
            if (switch_device[usecase->id] && (usecase->type != VOICE_CALL)) {
                    usecase->out_snd_device = snd_device;
                    enable_audio_route(adev, usecase);
            }
        }
    }
}

static void check_and_route_capture_usecases(struct audio_device *adev,
                                             struct audio_usecase *uc_info,
                                             snd_device_t snd_device)
{
    struct listnode *node;
    struct audio_usecase *usecase;
    bool switch_device[AUDIO_USECASE_MAX];
    int i, num_uc_to_switch = 0;

    /*
     * This function is to make sure that all the active capture usecases
     * are always routed to the same input sound device.
     * For example, if audio-record and voice-call usecases are currently
     * active on speaker(rx) and speaker-mic (tx) and out_set_parameters(earpiece)
     * is received for voice call then we have to make sure that audio-record
     * usecase is also switched to earpiece i.e. voice-dmic-ef,
     * because of the limitation that two devices cannot be enabled
     * at the same time if they share the same backend.
     */
    for (i = 0; i < AUDIO_USECASE_MAX; i++)
        switch_device[i] = false;

    list_for_each(node, &adev->usecase_list) {
        usecase = node_to_item(node, struct audio_usecase, list);
        if (usecase->type != PCM_PLAYBACK &&
                usecase->type != PCM_PASSTHROUGH &&
                usecase != uc_info &&
                usecase->in_snd_device != snd_device &&
                ((uc_info->devices & AUDIO_DEVICE_OUT_ALL_CODEC_BACKEND) &&
                ((usecase->devices & ~AUDIO_DEVICE_BIT_IN) & AUDIO_DEVICE_IN_ALL_CODEC_BACKEND)) &&
                (usecase->id != USECASE_AUDIO_SPKR_CALIB_TX)) {
            ALOGV("%s: Usecase (%s) is active on (%s) - disabling ..",
                  __func__, use_case_table[usecase->id],
                  platform_get_snd_device_name(usecase->in_snd_device));
            disable_audio_route(adev, usecase);
            switch_device[usecase->id] = true;
            num_uc_to_switch++;
        }
    }

    if (num_uc_to_switch) {
        /* All streams have been de-routed. Disable the device */

        /* Make sure the previous devices to be disabled first and then enable the
           selected devices */
        list_for_each(node, &adev->usecase_list) {
            usecase = node_to_item(node, struct audio_usecase, list);
            if (switch_device[usecase->id]) {
                disable_snd_device(adev, usecase->in_snd_device);
            }
        }

        list_for_each(node, &adev->usecase_list) {
            usecase = node_to_item(node, struct audio_usecase, list);
            if (switch_device[usecase->id]) {
                enable_snd_device(adev, snd_device);
            }
        }

        /* Re-route all the usecases on the shared backend other than the
           specified usecase to new snd devices */
        list_for_each(node, &adev->usecase_list) {
            usecase = node_to_item(node, struct audio_usecase, list);
            /* Update the in_snd_device only before enabling the audio route */
            if (switch_device[usecase->id] ) {
                usecase->in_snd_device = snd_device;
                if (usecase->type != VOICE_CALL)
                    enable_audio_route(adev, usecase);
            }
        }
    }
}

/* must be called with hw device mutex locked */
static int read_hdmi_channel_masks(struct stream_out *out)
{
    int ret = 0, i = 0;
    int channels = platform_edid_get_max_channels(out->dev->platform);

    switch (channels) {
        /*
         * Do not handle stereo output in Multi-channel cases
         * Stereo case is handled in normal playback path
         */
    case 6:
        ALOGV("%s: HDMI supports Quad and 5.1", __func__);
        out->supported_channel_masks[i++] = AUDIO_CHANNEL_OUT_QUAD;
        out->supported_channel_masks[i++] = AUDIO_CHANNEL_OUT_QUAD_SIDE;
#if 0
        out->supported_channel_masks[i++] = AUDIO_CHANNEL_OUT_PENTA;
#endif
        out->supported_channel_masks[i++] = AUDIO_CHANNEL_OUT_5POINT1;
        out->supported_channel_masks[i++] = AUDIO_CHANNEL_OUT_5POINT1_SIDE;
        break;
    case 8:
        ALOGV("%s: HDMI supports Quad, 5.1 and 7.1 channels", __func__);
        out->supported_channel_masks[i++] = AUDIO_CHANNEL_OUT_QUAD;
        out->supported_channel_masks[i++] = AUDIO_CHANNEL_OUT_QUAD_SIDE;
#if 0
        out->supported_channel_masks[i++] = AUDIO_CHANNEL_OUT_PENTA;
#endif
        out->supported_channel_masks[i++] = AUDIO_CHANNEL_OUT_5POINT1;
        out->supported_channel_masks[i++] = AUDIO_CHANNEL_OUT_5POINT1_SIDE;
        out->supported_channel_masks[i++] = AUDIO_CHANNEL_OUT_7POINT1;
        break;
    default:
        ALOGE("HDMI does not support multi channel playback");
        ret = -ENOSYS;
        break;
    }
    return ret;
}

audio_usecase_t get_usecase_id_from_usecase_type(struct audio_device *adev,
                                                 usecase_type_t type)
{
    struct audio_usecase *usecase;
    struct listnode *node;

    list_for_each(node, &adev->usecase_list) {
        usecase = node_to_item(node, struct audio_usecase, list);
        if (usecase->type == type) {
            ALOGV("%s: usecase id %d", __func__, usecase->id);
            return usecase->id;
        }
    }
    return USECASE_INVALID;
}

struct audio_usecase *get_usecase_from_list(struct audio_device *adev,
                                            audio_usecase_t uc_id)
{
    struct audio_usecase *usecase;
    struct listnode *node;

    list_for_each(node, &adev->usecase_list) {
        usecase = node_to_item(node, struct audio_usecase, list);
        if (usecase->id == uc_id)
            return usecase;
    }
    return NULL;
}

int select_devices(struct audio_device *adev, audio_usecase_t uc_id)
{
    snd_device_t out_snd_device = SND_DEVICE_NONE;
    snd_device_t in_snd_device = SND_DEVICE_NONE;
    struct audio_usecase *usecase = NULL;
    struct audio_usecase *vc_usecase = NULL;
    struct audio_usecase *voip_usecase = NULL;
    struct audio_usecase *hfp_usecase = NULL;
    audio_usecase_t hfp_ucid;
    struct listnode *node;
    int status = 0;

    usecase = get_usecase_from_list(adev, uc_id);
    if (usecase == NULL) {
        ALOGE("%s: Could not find the usecase(%d)", __func__, uc_id);
        return -EINVAL;
    }

    if ((usecase->type == VOICE_CALL) ||
        (usecase->type == VOIP_CALL)  ||
        (usecase->type == PCM_HFP_CALL) ||
        (usecase->type == ICC_CALL) ||
        (usecase->type == ANC_LOOPBACK)) {
        out_snd_device = platform_get_output_snd_device(adev->platform,
                                                        usecase->stream.out);
        in_snd_device = platform_get_input_snd_device(adev->platform, usecase->stream.out->devices);
        usecase->devices = usecase->stream.out->devices;
    } else {
        /*
         * If the voice call is active, use the sound devices of voice call usecase
         * so that it would not result any device switch. All the usecases will
         * be switched to new device when select_devices() is called for voice call
         * usecase. This is to avoid switching devices for voice call when
         * check_usecases_codec_backend() is called below.
         */
        if (voice_is_in_call(adev) && adev->mode == AUDIO_MODE_IN_CALL) {
            vc_usecase = get_usecase_from_list(adev,
                                               get_usecase_id_from_usecase_type(adev, VOICE_CALL));
            if ((vc_usecase) && ((vc_usecase->devices & AUDIO_DEVICE_OUT_ALL_CODEC_BACKEND) ||
                (usecase->devices == AUDIO_DEVICE_IN_VOICE_CALL))) {
                in_snd_device = vc_usecase->in_snd_device;
                out_snd_device = vc_usecase->out_snd_device;
            }
        } else if (voice_extn_compress_voip_is_active(adev)) {
            voip_usecase = get_usecase_from_list(adev, USECASE_COMPRESS_VOIP_CALL);
            if ((voip_usecase) && ((voip_usecase->devices & AUDIO_DEVICE_OUT_ALL_CODEC_BACKEND) &&
                (usecase->devices & AUDIO_DEVICE_OUT_ALL_CODEC_BACKEND) &&
                 (voip_usecase->stream.out != adev->primary_output))) {
                    in_snd_device = voip_usecase->in_snd_device;
                    out_snd_device = voip_usecase->out_snd_device;
            }
        } else if (audio_extn_hfp_is_active(adev)) {
            hfp_ucid = audio_extn_hfp_get_usecase();
            hfp_usecase = get_usecase_from_list(adev, hfp_ucid);
            if ((hfp_usecase) && (hfp_usecase->devices & AUDIO_DEVICE_OUT_ALL_CODEC_BACKEND)) {
                   in_snd_device = hfp_usecase->in_snd_device;
                   out_snd_device = hfp_usecase->out_snd_device;
            }
        }
        if (usecase->type == PCM_PLAYBACK) {
            usecase->devices = usecase->stream.out->devices;
            in_snd_device = SND_DEVICE_NONE;
            if (out_snd_device == SND_DEVICE_NONE) {
                if (is_offload_usecase(usecase->id))
                    /* Using SND_DEVICE_OUT_BUS for compress offload usecase*/
                    out_snd_device = SND_DEVICE_OUT_BUS;
                else
                    out_snd_device = platform_get_output_snd_device(adev->platform,
                                            usecase->stream.out);
                if (usecase->stream.out == adev->primary_output &&
                        adev->active_input &&
                        out_snd_device != usecase->out_snd_device) {
                    select_devices(adev, adev->active_input->usecase);
                }
            }
        } else if (usecase->type == PCM_CAPTURE) {
            usecase->devices = usecase->stream.in->device;
            out_snd_device = SND_DEVICE_NONE;
            if (in_snd_device == SND_DEVICE_NONE) {
                audio_devices_t out_device = AUDIO_DEVICE_NONE;
                if ((adev->active_input->source == AUDIO_SOURCE_VOICE_COMMUNICATION ||
                    (adev->mode == AUDIO_MODE_IN_COMMUNICATION &&
                     adev->active_input->source == AUDIO_SOURCE_MIC)) &&
                     adev->primary_output && !adev->primary_output->standby) {
                    out_device = adev->primary_output->devices;
                    platform_set_echo_reference(adev, false, AUDIO_DEVICE_NONE);
                } else if (usecase->id == USECASE_AUDIO_RECORD_AFE_PROXY) {
                    out_device = AUDIO_DEVICE_OUT_TELEPHONY_TX;
                }
                in_snd_device = platform_get_input_snd_device(adev->platform, out_device);
            }
        }
    }

    if (out_snd_device == usecase->out_snd_device &&
        in_snd_device == usecase->in_snd_device) {
        return 0;
    }

    ALOGD("%s: out_snd_device(%d: %s) in_snd_device(%d: %s)", __func__,
          out_snd_device, platform_get_snd_device_name(out_snd_device),
          in_snd_device,  platform_get_snd_device_name(in_snd_device));

    /*
     * Limitation: While in call, to do a device switch we need to disable
     * and enable both RX and TX devices though one of them is same as current
     * device.
     */
    if ((usecase->type == VOICE_CALL) &&
        (usecase->in_snd_device != SND_DEVICE_NONE) &&
        (usecase->out_snd_device != SND_DEVICE_NONE)) {
        status = platform_switch_voice_call_device_pre(adev->platform);
    }

    if (((usecase->type == VOICE_CALL) ||
         (usecase->type == VOIP_CALL)) &&
        (usecase->out_snd_device != SND_DEVICE_NONE)) {
        /* Disable sidetone only if voice/voip call already exists */
        if (voice_is_call_state_active(adev) ||
            voice_extn_compress_voip_is_started(adev))
            voice_set_sidetone(adev, usecase->out_snd_device, false);
    }

    /* Disable current sound devices */
    if (usecase->out_snd_device != SND_DEVICE_NONE) {
        disable_audio_route(adev, usecase);
        disable_snd_device(adev, usecase->out_snd_device);
    }

    if (usecase->in_snd_device != SND_DEVICE_NONE) {
        disable_audio_route(adev, usecase);
        disable_snd_device(adev, usecase->in_snd_device);
    }

    /* Applicable only on the targets that has external modem.
     * New device information should be sent to modem before enabling
     * the devices to reduce in-call device switch time.
     */
    if ((usecase->type == VOICE_CALL) &&
        (usecase->in_snd_device != SND_DEVICE_NONE) &&
        (usecase->out_snd_device != SND_DEVICE_NONE)) {
        status = platform_switch_voice_call_enable_device_config(adev->platform,
                                                                 out_snd_device,
                                                                 in_snd_device);
    }

    /* Enable new sound devices */
    if (out_snd_device != SND_DEVICE_NONE) {
        if (usecase->devices & AUDIO_DEVICE_OUT_ALL_CODEC_BACKEND)
            check_usecases_codec_backend(adev, usecase, out_snd_device);
        enable_snd_device(adev, out_snd_device);
    }

    if (in_snd_device != SND_DEVICE_NONE) {
        check_and_route_capture_usecases(adev, usecase, in_snd_device);
        enable_snd_device(adev, in_snd_device);
    }

    if (usecase->type == VOICE_CALL || usecase->type == VOIP_CALL) {
        status = platform_switch_voice_call_device_post(adev->platform,
                                                        out_snd_device,
                                                        in_snd_device);
        enable_audio_route_for_voice_usecases(adev, usecase);
        /* Enable sidetone only if voice/voip call already exists */
        if (voice_is_call_state_active(adev) ||
            voice_extn_compress_voip_is_started(adev))
            voice_set_sidetone(adev, out_snd_device, true);
    }

    usecase->in_snd_device = in_snd_device;
    usecase->out_snd_device = out_snd_device;

    audio_extn_utils_update_stream_app_type_cfg_for_usecase(adev,
                                                            usecase);

    enable_audio_route(adev, usecase);

    /* Applicable only on the targets that has external modem.
     * Enable device command should be sent to modem only after
     * enabling voice call mixer controls
     */
    if (usecase->type == VOICE_CALL)
        status = platform_switch_voice_call_usecase_route_post(adev->platform,
                                                               out_snd_device,
                                                               in_snd_device);
    ALOGD("%s: done",__func__);

    return status;
}

static int stop_input_stream(struct stream_in *in)
{
    int i, ret = 0;
    struct audio_usecase *uc_info;
    struct audio_device *adev = in->dev;

    adev->active_input = NULL;

    ALOGV("%s: enter: usecase(%d: %s)", __func__,
          in->usecase, use_case_table[in->usecase]);
    uc_info = get_usecase_from_list(adev, in->usecase);
    if (uc_info == NULL) {
        ALOGE("%s: Could not find the usecase (%d) in the list",
              __func__, in->usecase);
        return -EINVAL;
    }

    if (uc_info->in_snd_device != SND_DEVICE_NONE) {
        if (audio_extn_ext_hw_plugin_usecase_stop(adev->ext_hw_plugin, uc_info))
            ALOGE("%s: failed to stop ext hw plugin", __func__);
    }

    /* Close in-call recording streams */
    voice_check_and_stop_incall_rec_usecase(adev, in);

    /* 1. Disable stream specific mixer controls */
    disable_audio_route(adev, uc_info);

    /* 2. Disable the tx device */
    disable_snd_device(adev, uc_info->in_snd_device);

    list_remove(&uc_info->list);
    free(uc_info);

    ALOGV("%s: exit: status(%d)", __func__, ret);
    return ret;
}

int start_input_stream(struct stream_in *in)
{
    /* 1. Enable output device and stream routing controls */
    int ret = 0;
    struct audio_usecase *uc_info;
    struct audio_device *adev = in->dev;

    int usecase = platform_update_usecase_from_source(in->source,in->usecase);
    if (get_usecase_from_list(adev, usecase) == NULL)
        in->usecase = usecase;
    ALOGD("%s: enter: stream(%p)usecase(%d: %s)",
          __func__, &in->stream, in->usecase, use_case_table[in->usecase]);


    if (CARD_STATUS_OFFLINE == in->card_status||
        CARD_STATUS_OFFLINE == adev->card_status) {
        ALOGW("in->card_status or adev->card_status offline, try again");
        ret = -EIO;
        goto error_config;
    }

    /* Check if source matches incall recording usecase criteria */
    ret = voice_check_and_set_incall_rec_usecase(adev, in);
    if (ret)
        goto error_config;
    else
        ALOGV("%s: usecase(%d)", __func__, in->usecase);

    if (get_usecase_from_list(adev, in->usecase) != NULL) {
        ALOGE("%s: use case assigned already in use, stream(%p)usecase(%d: %s)",
            __func__, &in->stream, in->usecase, use_case_table[in->usecase]);
        goto error_config;
    }

    in->pcm_device_id = platform_get_pcm_device_id(in->usecase, PCM_CAPTURE);
    if (in->pcm_device_id < 0) {
        ALOGE("%s: Could not find PCM device id for the usecase(%d)",
              __func__, in->usecase);
        ret = -EINVAL;
        goto error_config;
    }

    adev->active_input = in;
    uc_info = (struct audio_usecase *)calloc(1, sizeof(struct audio_usecase));

    if (!uc_info) {
        ret = -ENOMEM;
        goto error_config;
    }

    uc_info->id = in->usecase;
    uc_info->type = PCM_CAPTURE;
    uc_info->stream.in = in;
    uc_info->devices = in->device;
    uc_info->in_snd_device = SND_DEVICE_NONE;
    uc_info->out_snd_device = SND_DEVICE_NONE;

    list_add_tail(&adev->usecase_list, &uc_info->list);
    audio_extn_perf_lock_acquire();
    select_devices(adev, in->usecase);

    if (uc_info->in_snd_device != SND_DEVICE_NONE) {
        if (audio_extn_ext_hw_plugin_usecase_start(adev->ext_hw_plugin, uc_info))
            ALOGE("%s: failed to start ext hw plugin", __func__);
    }

    ALOGV("%s: Opening PCM device card_id(%d) device_id(%d), channels %d",
          __func__, adev->snd_card, in->pcm_device_id, in->config.channels);

    unsigned int flags = PCM_IN;
    unsigned int pcm_open_retry_count = 0;

    if (in->usecase == USECASE_AUDIO_RECORD_AFE_PROXY) {
        flags |= PCM_MMAP | PCM_NOIRQ;
        pcm_open_retry_count = PROXY_OPEN_RETRY_COUNT;
    }

    while (1) {
        in->pcm = pcm_open(adev->snd_card, in->pcm_device_id,
                           flags, &in->config);
        if (in->pcm == NULL || !pcm_is_ready(in->pcm)) {
            ALOGE("%s: %s", __func__, pcm_get_error(in->pcm));
            if (in->pcm != NULL) {
                pcm_close(in->pcm);
                in->pcm = NULL;
            }
            if (pcm_open_retry_count-- == 0) {
                ret = -EIO;
                goto error_open;
            }
            usleep(PROXY_OPEN_WAIT_TIME * 1000);
            continue;
        }
        break;
    }

    ALOGV("%s: pcm_prepare", __func__);
    ret = pcm_prepare(in->pcm);
    if (ret < 0) {
        ALOGE("%s: pcm_prepare returned %d", __func__, ret);
        pcm_close(in->pcm);
        in->pcm = NULL;
        goto error_open;
    }

    if (platform_get_eccarstate(adev->platform)) {
        (void)platform_set_multi_channel_ec_pri_mic_ch(adev->platform);
    }

    audio_extn_perf_lock_release();

    ALOGD("%s: exit", __func__);

    return ret;

error_open:
    stop_input_stream(in);
    audio_extn_perf_lock_release();

error_config:
    adev->active_input = NULL;
    /*
     * sleep 50ms to allow sufficient time for kernel
     * drivers to recover incases like SSR.
     */
    usleep(50000);
    ALOGD("%s: exit: status(%d)", __func__, ret);

    return ret;
}

void lock_input_stream(struct stream_in *in)
{
    pthread_mutex_lock(&in->pre_lock);
    pthread_mutex_lock(&in->lock);
    pthread_mutex_unlock(&in->pre_lock);
}

void lock_output_stream(struct stream_out *out)
{
    pthread_mutex_lock(&out->pre_lock);
    pthread_mutex_lock(&out->lock);
    pthread_mutex_unlock(&out->pre_lock);
}

/* must be called with out->lock locked */
static int send_offload_cmd_l(struct stream_out* out, int command)
{
    struct offload_cmd *cmd = (struct offload_cmd *)calloc(1, sizeof(struct offload_cmd));

    if (!cmd) {
        ALOGE("failed to allocate mem for command 0x%x", command);
        return -ENOMEM;
    }

    ALOGVV("%s %d", __func__, command);

    cmd->cmd = command;
    list_add_tail(&out->offload_cmd_list, &cmd->node);
    pthread_cond_signal(&out->offload_cond);
    return 0;
}

/* must be called iwth out->lock locked */
static void stop_compressed_output_l(struct stream_out *out)
{
    out->offload_state = OFFLOAD_STATE_IDLE;
    out->playback_started = 0;
    out->send_new_metadata = 1;
    if (out->compr != NULL) {
        compress_stop(out->compr);
        while (out->offload_thread_blocked) {
            pthread_cond_wait(&out->cond, &out->lock);
        }
    }
}

bool is_offload_usecase(audio_usecase_t uc_id)
{
    unsigned int i;
    for (i = 0; i < sizeof(offload_usecases)/sizeof(offload_usecases[0]); i++) {
        if (uc_id == offload_usecases[i])
            return true;
    }
    return false;
}

static audio_usecase_t get_offload_usecase(struct audio_device *adev)
{
    audio_usecase_t ret = USECASE_AUDIO_PLAYBACK_OFFLOAD;
    unsigned int i, num_usecase = sizeof(offload_usecases)/sizeof(offload_usecases[0]);
    char value[PROPERTY_VALUE_MAX] = {0};

    property_get("audio.offload.multiple.enabled", value, NULL);
    if (!(atoi(value) || !strncmp("true", value, 4)))
        num_usecase = 1; /* If prop is not set, limit the num of offload usecases to 1 */

    ALOGV("%s: num_usecase: %d", __func__, num_usecase);
    for (i = 0; i < num_usecase; i++) {
        if (!(adev->offload_usecases_state & (0x1<<i))) {
            adev->offload_usecases_state |= 0x1 << i;
            ret = offload_usecases[i];
            break;
        }
    }
    ALOGV("%s: offload usecase is %d", __func__, ret);
    return ret;
}

static void free_offload_usecase(struct audio_device *adev,
                                 audio_usecase_t uc_id)
{
    unsigned int i;
    for (i = 0; i < sizeof(offload_usecases)/sizeof(offload_usecases[0]); i++) {
        if (offload_usecases[i] == uc_id) {
            adev->offload_usecases_state &= ~(0x1<<i);
            break;
        }
    }
    ALOGV("%s: free offload usecase %d", __func__, uc_id);
}

static void *offload_thread_loop(void *context)
{
    struct stream_out *out = (struct stream_out *) context;
    struct listnode *item;
    int ret = 0;

    setpriority(PRIO_PROCESS, 0, ANDROID_PRIORITY_AUDIO);
    set_sched_policy(0, SP_FOREGROUND);
    prctl(PR_SET_NAME, (unsigned long)"Offload Callback", 0, 0, 0);

    ALOGV("%s", __func__);
    lock_output_stream(out);
    for (;;) {
        struct offload_cmd *cmd = NULL;
        stream_callback_event_t event;
        bool send_callback = false;

        ALOGVV("%s offload_cmd_list %d out->offload_state %d",
              __func__, list_empty(&out->offload_cmd_list),
              out->offload_state);
        if (list_empty(&out->offload_cmd_list)) {
            ALOGV("%s SLEEPING", __func__);
            pthread_cond_wait(&out->offload_cond, &out->lock);
            ALOGV("%s RUNNING", __func__);
            continue;
        }

        item = list_head(&out->offload_cmd_list);
        cmd = node_to_item(item, struct offload_cmd, node);
        list_remove(item);

        ALOGVV("%s STATE %d CMD %d out->compr %p",
               __func__, out->offload_state, cmd->cmd, out->compr);

        if (cmd->cmd == OFFLOAD_CMD_EXIT) {
            free(cmd);
            break;
        }

        // allow OFFLOAD_CMD_ERROR reporting during standby
        // this is needed to handle failures during compress_open
        // Note however that on a pause timeout, the stream is closed
        // and no offload usecase will be active. Therefore this
        // special case is needed for compress_open failures alone
        if (cmd->cmd != OFFLOAD_CMD_ERROR &&
            out->compr == NULL) {
            ALOGE("%s: Compress handle is NULL", __func__);
            pthread_cond_signal(&out->cond);
            continue;
        }
        out->offload_thread_blocked = true;
        pthread_mutex_unlock(&out->lock);
        send_callback = false;
        switch(cmd->cmd) {
        case OFFLOAD_CMD_WAIT_FOR_BUFFER:
            ALOGD("copl(%p):calling compress_wait", out);
            compress_wait(out->compr, -1);
            ALOGD("copl(%p):out of compress_wait", out);
            send_callback = true;
            event = STREAM_CBK_EVENT_WRITE_READY;
            break;
        case OFFLOAD_CMD_PARTIAL_DRAIN:
            ret = compress_next_track(out->compr);
            if(ret == 0) {
                ALOGD("copl(%p):calling compress_partial_drain", out);
                ret = compress_partial_drain(out->compr);
                ALOGD("copl(%p):out of compress_partial_drain", out);
                if (ret < 0)
                    ret = -errno;
            }
            else if (ret == -ETIMEDOUT)
                compress_drain(out->compr);
            else
                ALOGE("%s: Next track returned error %d",__func__, ret);
            if (ret != -ENETRESET) {
                send_callback = true;
                pthread_mutex_lock(&out->lock);
                out->send_new_metadata = 1;
                out->send_next_track_params = true;
                pthread_mutex_unlock(&out->lock);
                event = STREAM_CBK_EVENT_DRAIN_READY;
                ALOGV("copl(%p):send drain callback, ret %d", out, ret);
            } else
                ALOGI("%s: Block drain ready event during SSR", __func__);
            break;
        case OFFLOAD_CMD_DRAIN:
            ALOGD("copl(%p):calling compress_drain", out);
            compress_drain(out->compr);
            ALOGD("copl(%p):calling compress_drain", out);
            send_callback = true;
            event = STREAM_CBK_EVENT_DRAIN_READY;
            break;
        case OFFLOAD_CMD_ERROR:
            ALOGD("copl(%p): sending error callback to AF", out);
            send_callback = true;
            event = STREAM_CBK_EVENT_ERROR;
            break;
        default:
            ALOGE("%s unknown command received: %d", __func__, cmd->cmd);
            break;
        }
        lock_output_stream(out);
        out->offload_thread_blocked = false;
        pthread_cond_signal(&out->cond);
        if (send_callback && out->offload_callback) {
            ALOGVV("%s: sending offload_callback event %d", __func__, event);
            out->offload_callback(event, NULL, out->offload_cookie);
        }
        free(cmd);
    }

    pthread_cond_signal(&out->cond);
    while (!list_empty(&out->offload_cmd_list)) {
        item = list_head(&out->offload_cmd_list);
        list_remove(item);
        free(node_to_item(item, struct offload_cmd, node));
    }
    pthread_mutex_unlock(&out->lock);

    return NULL;
}

static int create_offload_callback_thread(struct stream_out *out)
{
    pthread_cond_init(&out->offload_cond, (const pthread_condattr_t *) NULL);
    list_init(&out->offload_cmd_list);
    pthread_create(&out->offload_thread, (const pthread_attr_t *) NULL,
                    offload_thread_loop, out);
    return 0;
}

static int destroy_offload_callback_thread(struct stream_out *out)
{
    lock_output_stream(out);
    stop_compressed_output_l(out);
    send_offload_cmd_l(out, OFFLOAD_CMD_EXIT);

    pthread_mutex_unlock(&out->lock);
    pthread_join(out->offload_thread, (void **) NULL);
    pthread_cond_destroy(&out->offload_cond);

    return 0;
}

static bool allow_hdmi_channel_config(struct audio_device *adev)
{
    struct listnode *node;
    struct audio_usecase *usecase;
    bool ret = true;

    list_for_each(node, &adev->usecase_list) {
        usecase = node_to_item(node, struct audio_usecase, list);
        if (usecase->devices & AUDIO_DEVICE_OUT_AUX_DIGITAL) {
            /*
             * If voice call is already existing, do not proceed further to avoid
             * disabling/enabling both RX and TX devices, CSD calls, etc.
             * Once the voice call done, the HDMI channels can be configured to
             * max channels of remaining use cases.
             */
            if (usecase->id == USECASE_VOICE_CALL) {
                ALOGD("%s: voice call is active, no change in HDMI channels",
                      __func__);
                ret = false;
                break;
            } else if (usecase->id == USECASE_AUDIO_PLAYBACK_MULTI_CH) {
                ALOGD("%s: multi channel playback is active, "
                      "no change in HDMI channels", __func__);
                ret = false;
                break;
            } else if (is_offload_usecase(usecase->id) &&
                       audio_channel_count_from_out_mask(usecase->stream.out->channel_mask) > 2) {
                ALOGD("%s:multi-channel(%x) compress offload playback is active"
                      ", no change in HDMI channels", __func__,
                      usecase->stream.out->channel_mask);
                ret = false;
                break;
            }
        }
    }
    return ret;
}

static int check_and_set_hdmi_channels(struct audio_device *adev,
                                       unsigned int channels)
{
    struct listnode *node;
    struct audio_usecase *usecase;

    unsigned int supported_channels = platform_edid_get_max_channels(
                                          adev->platform);
    ALOGV("supported_channels %d, channels %d", supported_channels, channels);
    /* Check if change in HDMI channel config is allowed */
    if (!allow_hdmi_channel_config(adev))
        return 0;

    if (channels > supported_channels)
        channels = supported_channels;

    if (channels == adev->cur_hdmi_channels) {
        ALOGD("%s: Requested channels are same as current channels(%d)",
               __func__, channels);
        return 0;
    }

    /*TODO: CHECK for passthrough don't set channel map for passthrough*/
    platform_set_hdmi_channels(adev->platform, channels);
    platform_set_edid_channels_configuration(adev->platform, channels);
    adev->cur_hdmi_channels = channels;

    /*
     * Deroute all the playback streams routed to HDMI so that
     * the back end is deactivated. Note that backend will not
     * be deactivated if any one stream is connected to it.
     */
    list_for_each(node, &adev->usecase_list) {
        usecase = node_to_item(node, struct audio_usecase, list);
        if (usecase->type == PCM_PLAYBACK &&
                usecase->devices & AUDIO_DEVICE_OUT_AUX_DIGITAL) {
            disable_audio_route(adev, usecase);
        }
    }

    /*
     * Enable all the streams disabled above. Now the HDMI backend
     * will be activated with new channel configuration
     */
    list_for_each(node, &adev->usecase_list) {
        usecase = node_to_item(node, struct audio_usecase, list);
        if (usecase->type == PCM_PLAYBACK &&
                usecase->devices & AUDIO_DEVICE_OUT_AUX_DIGITAL) {
            enable_audio_route(adev, usecase);
        }
    }

    return 0;
}

static int stop_output_stream(struct stream_out *out)
{
    int i, ret = 0;
    struct audio_usecase *uc_info;
    struct audio_device *adev = out->dev;

    ALOGV("%s: enter: usecase(%d: %s)", __func__,
          out->usecase, use_case_table[out->usecase]);
    uc_info = get_usecase_from_list(adev, out->usecase);
    if (uc_info == NULL) {
        ALOGE("%s: Could not find the usecase (%d) in the list",
              __func__, out->usecase);
        return -EINVAL;
    }

#ifdef BUS_ADDRESS_ENABLED
#ifdef VHAL_HELPER_ENABLED
    if (out->car_audio_stream) {
        if (out->vhal_audio_helper == NULL) {
            out->vhal_audio_helper =
                vehicle_hal_audio_helper_create(FOCUS_WAIT_DEFAULT_TIMEOUT_NS);
            if (out->vhal_audio_helper == NULL)
                ALOGE("%s: vhal audio helper not allocated, continue", __func__);
        }
        // notify stream stop
        if (out->vhal_audio_helper != NULL)
            vehicle_hal_audio_helper_notify_stream_stopped(out->vhal_audio_helper,
                                                           out->car_audio_stream);
    } else { // legacy stream will disable codec via vhal helper
        if (uc_info->out_snd_device != SND_DEVICE_NONE) {
            if (audio_extn_ext_hw_plugin_usecase_stop(adev->ext_hw_plugin, uc_info))
                ALOGE("%s: failed to stop ext hw plugin", __func__);
        }
    }
#else
    if (uc_info->out_snd_device != SND_DEVICE_NONE) {
        if (audio_extn_ext_hw_plugin_usecase_stop(adev->ext_hw_plugin, uc_info))
            ALOGE("%s: failed to stop ext hw plugin", __func__);
    }
#endif // VHAL_HELPER_ENABLED
#else
    if (uc_info->out_snd_device != SND_DEVICE_NONE) {
        if (audio_extn_ext_hw_plugin_usecase_stop(adev->ext_hw_plugin, uc_info))
            ALOGE("%s: failed to stop ext hw plugin", __func__);
    }
#endif // BUS_ADDRESS_ENABLED

    if (is_offload_usecase(out->usecase) &&
        !(audio_extn_dolby_is_passthrough_stream(out->flags))) {
        if (adev->visualizer_stop_output != NULL)
            adev->visualizer_stop_output(out->handle, out->pcm_device_id);

        audio_extn_dts_remove_state_notifier_node(out->usecase);

        if (adev->offload_effects_stop_output != NULL)
            adev->offload_effects_stop_output(out->handle, out->pcm_device_id);
    }
#if defined(AUTOEFFECTS_ENABLE)
    // stop AUTO effects
    if (adev->auto_effects_stop_output != NULL){
        adev->auto_effects_stop_output(out->handle, out->usecase);
        ALOGD("%s: Stop AUTO effects", __func__);
    }
#endif
    /* 1. Get and set stream specific mixer controls */
    disable_audio_route(adev, uc_info);

    /* 2. Disable the rx device */
    disable_snd_device(adev, uc_info->out_snd_device);

    list_remove(&uc_info->list);
    free(uc_info);

    if (is_offload_usecase(out->usecase) &&
        (out->devices & AUDIO_DEVICE_OUT_AUX_DIGITAL) &&
        (audio_extn_dolby_is_passthrough_stream(out->flags))) {
        ALOGV("Disable passthrough , reset mixer to pcm");
        /* NO_PASSTHROUGH */
        out->compr_config.codec->compr_passthr = 0;
        audio_extn_dolby_set_hdmi_config(adev, out);
        audio_extn_dolby_set_dap_bypass(adev, DAP_STATE_ON);
    }
    /* Must be called after removing the usecase from list */
    if (out->devices & AUDIO_DEVICE_OUT_AUX_DIGITAL)
        check_and_set_hdmi_channels(adev, DEFAULT_HDMI_OUT_CHANNELS);

    ALOGV("%s: exit: status(%d)", __func__, ret);
    return ret;
}

int start_output_stream(struct stream_out *out)
{
    int ret = 0;
    int sink_channels = 0;
    char prop_value[PROPERTY_VALUE_MAX] = {0};
    struct audio_usecase *uc_info;
    struct audio_device *adev = out->dev;

    if ((out->usecase < 0) || (out->usecase >= AUDIO_USECASE_MAX)) {
        ret = -EINVAL;
        goto error_config;
    }

    ALOGD("%s: enter: stream(%p)usecase(%d: %s) devices(%#x)",
          __func__, &out->stream, out->usecase, use_case_table[out->usecase],
          out->devices);

    if (CARD_STATUS_OFFLINE == out->card_status ||
        CARD_STATUS_OFFLINE == adev->card_status) {
        ALOGW("out->card_status or adev->card_status offline, try again");
        ret = -EIO;
        goto error_config;
    }

    out->pcm_device_id = platform_get_pcm_device_id(out->usecase, PCM_PLAYBACK);
    if (out->pcm_device_id < 0) {
        ALOGE("%s: Invalid PCM device id(%d) for the usecase(%d)",
              __func__, out->pcm_device_id, out->usecase);
        ret = -EINVAL;
        goto error_open;
    }

    uc_info = (struct audio_usecase *)calloc(1, sizeof(struct audio_usecase));

    if (!uc_info) {
        ret = -ENOMEM;
        goto error_config;
    }

    uc_info->id = out->usecase;
    uc_info->type = PCM_PLAYBACK;
    uc_info->stream.out = out;
    uc_info->devices = out->devices;
    uc_info->in_snd_device = SND_DEVICE_NONE;
    uc_info->out_snd_device = SND_DEVICE_NONE;
    /* This must be called before adding this usecase to the list */
    if (out->devices & AUDIO_DEVICE_OUT_AUX_DIGITAL) {
        if (is_offload_usecase(out->usecase)) {
            if (audio_extn_dolby_is_passthrough_stream(out->flags)) {
                audio_extn_dolby_update_passt_stream_configuration(adev, out);
            }
        }
        property_get("audio.use.hdmi.sink.cap", prop_value, NULL);
        if (!strncmp("true", prop_value, 4)) {
            sink_channels = platform_edid_get_max_channels(out->dev->platform);
            ALOGD("%s: set HDMI channel count[%d] based on sink capability",
                   __func__, sink_channels);
            check_and_set_hdmi_channels(adev, sink_channels);
        } else {
            if (is_offload_usecase(out->usecase)) {
                unsigned int ch_count =  out->compr_config.codec->ch_in;
                if (audio_extn_dolby_is_passthrough_stream(out->flags))
                    /* backend channel config for passthrough stream is stereo */
                    ch_count = 2;
                check_and_set_hdmi_channels(adev, ch_count);
            } else
                check_and_set_hdmi_channels(adev, out->config.channels);
        }
        audio_extn_dolby_set_hdmi_config(adev, out);
    }
    list_add_tail(&adev->usecase_list, &uc_info->list);

    select_devices(adev, out->usecase);

#ifdef BUS_ADDRESS_ENABLED
#ifdef VHAL_HELPER_ENABLED
    if (out->car_audio_stream) {
        if (out->vhal_audio_helper == NULL) {
            out->vhal_audio_helper =
                vehicle_hal_audio_helper_create(FOCUS_WAIT_DEFAULT_TIMEOUT_NS);
            if (out->vhal_audio_helper == NULL) {
                ALOGE("%s: vhal audio helper not allocated, continue", __func__);
            }
        }
        // notify stream start
        if (out->vhal_audio_helper != NULL)
            vehicle_hal_audio_helper_notify_stream_started(out->vhal_audio_helper,
                                                           out->car_audio_stream);
    } else { // legacy stream will enable codec via vhal helper
        if (uc_info->out_snd_device != SND_DEVICE_NONE) {
            if (audio_extn_ext_hw_plugin_usecase_start(adev->ext_hw_plugin, uc_info))
                ALOGE("%s: failed to start ext hw plugin", __func__);
        }
    }
#else
    if (uc_info->out_snd_device != SND_DEVICE_NONE) {
        if (audio_extn_ext_hw_plugin_usecase_start(adev->ext_hw_plugin, uc_info))
            ALOGE("%s: failed to start ext hw plugin", __func__);
    }
#endif // VHAL_HELPER_ENABLED
#else
    if (uc_info->out_snd_device != SND_DEVICE_NONE) {
        if (audio_extn_ext_hw_plugin_usecase_start(adev->ext_hw_plugin, uc_info))
            ALOGE("%s: failed to start ext hw plugin", __func__);
    }
#endif // BUS_ADDRESS_ENABLED

    ALOGV("%s: Opening PCM device card_id(%d) device_id(%d) format(%#x)",
          __func__, adev->snd_card, out->pcm_device_id, out->config.format);
    if (!is_offload_usecase(out->usecase)) {
        unsigned int flags = PCM_OUT;
        unsigned int pcm_open_retry_count = 0;
        if (out->usecase == USECASE_AUDIO_PLAYBACK_AFE_PROXY) {
            flags |= PCM_MMAP | PCM_NOIRQ;
            pcm_open_retry_count = PROXY_OPEN_RETRY_COUNT;
        } else
            flags |= PCM_MONOTONIC;

        while (1) {
            out->pcm = pcm_open(adev->snd_card, out->pcm_device_id,
                               flags, &out->config);
            if (out->pcm == NULL || !pcm_is_ready(out->pcm)) {
                ALOGE("%s: %s", __func__, pcm_get_error(out->pcm));
                if (out->pcm != NULL) {
                    pcm_close(out->pcm);
                    out->pcm = NULL;
                }
                if (pcm_open_retry_count-- == 0) {
                    ret = -EIO;
                    goto error_open;
                }
                usleep(PROXY_OPEN_WAIT_TIME * 1000);
                continue;
            }
            break;
        }

        platform_set_stream_channel_map(adev->platform, out->channel_mask,
                                    out->pcm_device_id);

        ALOGV("%s: pcm_prepare", __func__);
        if (pcm_is_ready(out->pcm)) {
            ret = pcm_prepare(out->pcm);
            if (ret < 0) {
                ALOGE("%s: pcm_prepare returned %d", __func__, ret);
                pcm_close(out->pcm);
                out->pcm = NULL;
                goto error_open;
            }
        }
    } else {
        platform_set_stream_channel_map(adev->platform, out->channel_mask,
                                    out->pcm_device_id);
        out->pcm = NULL;
        out->compr = compress_open(adev->snd_card,
                                   out->pcm_device_id,
                                   COMPRESS_IN, &out->compr_config);
        if (out->compr && !is_compress_ready(out->compr)) {
            ALOGE("%s: failed /w error %s", __func__, compress_get_error(out->compr));
            compress_close(out->compr);
            out->compr = NULL;
            ret = -EIO;
            goto error_open;
        }
        /* compress_open sends params of the track, so reset the flag here */
        out->is_compr_metadata_avail = false;

        if (out->offload_callback)
            compress_nonblock(out->compr, out->non_blocking);

        /* Since small bufs uses blocking writes, a write will be blocked
           for the default max poll time (20s) in the event of an SSR.
           Reduce the poll time to observe and deal with SSR faster.
        */
        if (!out->non_blocking) {
            compress_set_max_poll_wait(out->compr, 1000);
        }

        audio_extn_dts_create_state_notifier_node(out->usecase);
        audio_extn_dts_notify_playback_state(out->usecase, 0, out->sample_rate,
                                             popcount(out->channel_mask),
                                             out->playback_started);

#ifdef DS1_DOLBY_DDP_ENABLED
        if (audio_extn_is_dolby_format(out->format))
            audio_extn_dolby_send_ddp_endp_params(adev);
#endif
        if (!(audio_extn_dolby_is_passthrough_stream(out->flags))) {
            if (adev->visualizer_start_output != NULL)
                adev->visualizer_start_output(out->handle, out->pcm_device_id);
            if (adev->offload_effects_start_output != NULL)
                adev->offload_effects_start_output(out->handle, out->pcm_device_id, adev->mixer);
            audio_extn_check_and_set_dts_hpx_state(adev);
        }
    }


    ALOGD("%s: exit", __func__);

    return 0;
error_open:
    stop_output_stream(out);
error_config:
    /*
     * sleep 50ms to allow sufficient time for kernel
     * drivers to recover incases like SSR.
     */
    usleep(50000);
    return ret;
}

static int check_input_parameters(uint32_t sample_rate,
                                  audio_format_t format,
                                  int channel_count)
{
    int ret = 0;

    if ((format != AUDIO_FORMAT_PCM_16_BIT) &&
        !voice_extn_compress_voip_is_format_supported(format) &&
            !audio_extn_compr_cap_format_supported(format))
    {
        ALOGE("%s: Unsupported format : %d",__func__,format);
        ret = -EINVAL;
    }
    switch (channel_count) {
    case 1:
    case 2:
    case 6:
    case 8:
        break;
    default:
        ALOGE("%s: Unsupported channel count: %d",__func__,channel_count);
        ret = -EINVAL;
    }

    switch (sample_rate) {
    case 8000:
    case 11025:
    case 12000:
    case 16000:
    case 22050:
    case 24000:
    case 32000:
    case 44100:
    case 48000:
        break;
    default:
        ALOGE("%s: Unsupported SampleRate : %d",__func__,sample_rate);
        ret = -EINVAL;
    }

    return ret;
}

static size_t get_input_buffer_size(uint32_t sample_rate,
                                    audio_format_t format,
                                    int channel_count,
                                    bool is_low_latency)
{
    size_t size = 0;

    if (check_input_parameters(sample_rate, format, channel_count) != 0)
        return 0;

    size = (sample_rate * AUDIO_CAPTURE_PERIOD_DURATION_MSEC) / 1000;
    if (is_low_latency)
        size = configured_low_latency_capture_period_size;
    /* ToDo: should use frame_size computed based on the format and
       channel_count here. */
    size *= sizeof(short) * channel_count;

    /* make sure the size is multiple of 32 bytes
     * At 48 kHz mono 16-bit PCM:
     *  5.000 ms = 240 frames = 15*16*1*2 = 480, a whole multiple of 32 (15)
     *  3.333 ms = 160 frames = 10*16*1*2 = 320, a whole multiple of 32 (10)
     */
    size += 0x1f;
    size &= ~0x1f;

    return size;
}

static uint64_t get_actual_pcm_frames_rendered(struct stream_out *out)
{
    uint64_t actual_frames_rendered = 0;
    size_t kernel_buffer_size = out->compr_config.fragment_size * out->compr_config.fragments;

    /* This adjustment accounts for buffering after app processor.
     * It is based on estimated DSP latency per use case, rather than exact.
     */
    int64_t platform_latency =  platform_render_latency(out->usecase) *
                                out->sample_rate / 1000000LL;

    /* not querying actual state of buffering in kernel as it would involve an ioctl call
     * which then needs protection, this causes delay in TS query for pcm_offload usecase
     * hence only estimate.
     */
    int64_t signed_frames = out->written - kernel_buffer_size;

    signed_frames = signed_frames / (audio_bytes_per_sample(out->format) * popcount(out->channel_mask)) - platform_latency;

    if (signed_frames > 0)
        actual_frames_rendered = signed_frames;

    ALOGVV("%s signed frames %lld out_written %lld kernel_buffer_size %d"
            "bytes/sample %zu channel count %d", __func__,(long long int)signed_frames,
             (long long int)out->written, (int)kernel_buffer_size,
             audio_bytes_per_sample(out->compr_config.codec->format),
             popcount(out->channel_mask));

    return actual_frames_rendered;
}

static uint32_t out_get_sample_rate(const struct audio_stream *stream)
{
    struct stream_out *out = (struct stream_out *)stream;

    return out->sample_rate;
}

static int out_set_sample_rate(struct audio_stream *stream __unused,
                               uint32_t rate __unused)
{
    return -ENOSYS;
}

static size_t out_get_buffer_size(const struct audio_stream *stream)
{
    struct stream_out *out = (struct stream_out *)stream;

    if (is_offload_usecase(out->usecase))
        return out->compr_config.fragment_size;
    else if(out->usecase == USECASE_COMPRESS_VOIP_CALL)
        return voice_extn_compress_voip_out_get_buffer_size(out);

    return out->config.period_size *
                audio_stream_out_frame_size((const struct audio_stream_out *)stream);
}

static uint32_t out_get_channels(const struct audio_stream *stream)
{
    struct stream_out *out = (struct stream_out *)stream;

    return out->channel_mask;
}

static audio_format_t out_get_format(const struct audio_stream *stream)
{
    struct stream_out *out = (struct stream_out *)stream;

    return out->format;
}

static int out_set_format(struct audio_stream *stream __unused,
                          audio_format_t format __unused)
{
    return -ENOSYS;
}

static int out_standby(struct audio_stream *stream)
{
    struct stream_out *out = (struct stream_out *)stream;
    struct audio_device *adev = out->dev;

    ALOGD("%s: enter: stream (%p) usecase(%d: %s)", __func__,
          stream, out->usecase, use_case_table[out->usecase]);
    if (out->usecase == USECASE_COMPRESS_VOIP_CALL) {
        /* Ignore standby in case of voip call because the voip output
         * stream is closed in adev_close_output_stream()
         */
        ALOGD("%s: Ignore Standby in VOIP call", __func__);
        return 0;
    }

    lock_output_stream(out);
    if (!out->standby) {
        if (adev->adm_deregister_stream)
            adev->adm_deregister_stream(adev->adm_data, out->handle);

        pthread_mutex_lock(&adev->lock);
        out->standby = true;
        if (!is_offload_usecase(out->usecase)) {
            if (out->pcm) {
                pcm_close(out->pcm);
                out->pcm = NULL;
            }
        } else {
            ALOGD("copl(%p):standby", out);
            stop_compressed_output_l(out);
            out->send_next_track_params = false;
            out->is_compr_metadata_avail = false;
            out->gapless_mdata.encoder_delay = 0;
            out->gapless_mdata.encoder_padding = 0;
            if (out->compr != NULL) {
                compress_close(out->compr);
                out->compr = NULL;
            }
        }
        stop_output_stream(out);
        pthread_mutex_unlock(&adev->lock);
    }
    pthread_mutex_unlock(&out->lock);
    ALOGV("%s: exit", __func__);
    return 0;
}

static int out_on_error(struct audio_stream *stream)
{
    struct stream_out *out = (struct stream_out *)stream;
    int status = 0;

    lock_output_stream(out);
    // always send CMD_ERROR for offload streams, this
    // is needed e.g. when SSR happens within compress_open
    // since the stream is active, offload_callback_thread is also active.
    if (out->flags & AUDIO_OUTPUT_FLAG_COMPRESS_OFFLOAD) {
        stop_compressed_output_l(out);
    }
    pthread_mutex_unlock(&out->lock);

    status = out_standby(&out->stream.common);

    lock_output_stream(out);
    if (out->flags & AUDIO_OUTPUT_FLAG_COMPRESS_OFFLOAD) {
        send_offload_cmd_l(out, OFFLOAD_CMD_ERROR);
    }
    pthread_mutex_unlock(&out->lock);

    return status;
}

static int out_dump(const struct audio_stream *stream __unused,
                    int fd __unused)
{
    return 0;
}

static int parse_compress_metadata(struct stream_out *out, struct str_parms *parms)
{
    int ret = 0;
    char value[32];

    if (!out || !parms) {
        ALOGE("%s: return invalid ",__func__);
        return -EINVAL;
    }

    ret = audio_extn_parse_compress_metadata(out, parms);

    ret = str_parms_get_str(parms, AUDIO_OFFLOAD_CODEC_DELAY_SAMPLES, value, sizeof(value));
    if (ret >= 0) {
        out->gapless_mdata.encoder_delay = atoi(value); //whats a good limit check?
    }
    ret = str_parms_get_str(parms, AUDIO_OFFLOAD_CODEC_PADDING_SAMPLES, value, sizeof(value));
    if (ret >= 0) {
        out->gapless_mdata.encoder_padding = atoi(value);
    }

    ALOGV("%s new encoder delay %u and padding %u", __func__,
          out->gapless_mdata.encoder_delay, out->gapless_mdata.encoder_padding);

    return 0;
}

static bool output_drives_call(struct audio_device *adev, struct stream_out *out)
{
    return out == adev->primary_output || out == adev->voice_tx_output;
}

// note: this call is safe only if the stream_cb is
// removed first in close_output_stream (as is done now).
static void out_snd_mon_cb(void * stream, struct str_parms * parms)
{
    if (!stream || !parms)
        return;

    struct stream_out *out = (struct stream_out *)stream;
    struct audio_device *adev = out->dev;

    card_status_t status;
    int card;
    if (parse_snd_card_status(parms, &card, &status) < 0)
        return;

    pthread_mutex_lock(&adev->lock);
    bool valid_cb = (card == adev->snd_card);
    pthread_mutex_unlock(&adev->lock);

    if (!valid_cb)
        return;

    lock_output_stream(out);
    if (out->card_status != status)
        out->card_status = status;
    pthread_mutex_unlock(&out->lock);

    ALOGI("out_snd_mon_cb for card %d usecase %s, status %s", card,
          use_case_table[out->usecase],
          status == CARD_STATUS_OFFLINE ? "offline" : "online");

    if (status == CARD_STATUS_OFFLINE)
        out_on_error(stream);

    return;
}

static int out_set_parameters(struct audio_stream *stream, const char *kvpairs)
{
    struct stream_out *out = (struct stream_out *)stream;
    struct audio_device *adev = out->dev;
    struct audio_usecase *usecase;
    struct listnode *node;
    struct str_parms *parms;
    char value[32];
    int ret = 0, val = 0, err;
    bool select_new_device = false;

    ALOGD("%s: enter: usecase(%d: %s) kvpairs: %s",
          __func__, out->usecase, use_case_table[out->usecase], kvpairs);
    parms = str_parms_create_str(kvpairs);
    if (!parms)
        goto error;
    err = str_parms_get_str(parms, AUDIO_PARAMETER_STREAM_ROUTING, value, sizeof(value));
    if (err >= 0) {
        val = atoi(value);
        lock_output_stream(out);
        pthread_mutex_lock(&adev->lock);

        /*
         * When HDMI cable is unplugged/usb hs is disconnected the
         * music playback is paused and the policy manager sends routing=0
         * But the audioflingercontinues to write data until standby time
         * (3sec). As the HDMI core is turned off, the write gets blocked.
         * Avoid this by routing audio to speaker until standby.
         */
        if ((out->devices == AUDIO_DEVICE_OUT_AUX_DIGITAL ||
                out->devices == AUDIO_DEVICE_OUT_ANLG_DOCK_HEADSET) &&
                val == AUDIO_DEVICE_NONE) {
            if (!audio_extn_dolby_is_passthrough_stream(out->flags))
                val = AUDIO_DEVICE_OUT_SPEAKER;
        }

        /*
         * select_devices() call below switches all the usecases on the same
         * backend to the new device. Refer to check_usecases_codec_backend() in
         * the select_devices(). But how do we undo this?
         *
         * For example, music playback is active on headset (deep-buffer usecase)
         * and if we go to ringtones and select a ringtone, low-latency usecase
         * will be started on headset+speaker. As we can't enable headset+speaker
         * and headset devices at the same time, select_devices() switches the music
         * playback to headset+speaker while starting low-lateny usecase for ringtone.
         * So when the ringtone playback is completed, how do we undo the same?
         *
         * We are relying on the out_set_parameters() call on deep-buffer output,
         * once the ringtone playback is ended.
         * NOTE: We should not check if the current devices are same as new devices.
         *       Because select_devices() must be called to switch back the music
         *       playback to headset.
         */
        if (val != 0) {
            out->devices = val;

            if (!out->standby)
                select_devices(adev, out->usecase);

            if (output_drives_call(adev, out)) {
                if(!voice_is_in_call(adev)) {
                    if (adev->mode == AUDIO_MODE_IN_CALL) {
                        adev->current_call_output = out;
                        ret = voice_start_call(adev);
                    }
                } else {
                    adev->current_call_output = out;
                    voice_update_devices_for_all_voice_usecases(adev);
                }
            }
        }

        pthread_mutex_unlock(&adev->lock);
        pthread_mutex_unlock(&out->lock);
    }

    if (out == adev->primary_output) {
        pthread_mutex_lock(&adev->lock);
        audio_extn_set_parameters(adev, parms);
        pthread_mutex_unlock(&adev->lock);
    }
    if (is_offload_usecase(out->usecase)) {
        lock_output_stream(out);
        parse_compress_metadata(out, parms);

        audio_extn_dts_create_state_notifier_node(out->usecase);
        audio_extn_dts_notify_playback_state(out->usecase, 0, out->sample_rate,
                                                 popcount(out->channel_mask),
                                                 out->playback_started);

        pthread_mutex_unlock(&out->lock);
    }

    str_parms_destroy(parms);
error:
    ALOGV("%s: exit: code(%d)", __func__, ret);
    return ret;
}

static char* out_get_parameters(const struct audio_stream *stream, const char *keys)
{
    struct stream_out *out = (struct stream_out *)stream;
    struct str_parms *query = str_parms_create_str(keys);
    char *str;
    char value[256];
    struct str_parms *reply = str_parms_create();
    size_t i, j;
    int ret;
    bool first = true;

    if (!query || !reply) {
        ALOGE("out_get_parameters: failed to allocate mem for query or reply");
        return NULL;
    }

    ALOGV("%s: enter: keys - %s", __func__, keys);
    ret = str_parms_get_str(query, AUDIO_PARAMETER_STREAM_SUP_CHANNELS, value, sizeof(value));
    if (ret >= 0) {
        value[0] = '\0';
        i = 0;
        while (out->supported_channel_masks[i] != 0) {
            for (j = 0; j < ARRAY_SIZE(out_channels_name_to_enum_table); j++) {
                if (out_channels_name_to_enum_table[j].value == out->supported_channel_masks[i]) {
                    if (!first) {
                        strlcat(value, "|", sizeof(value));
                    }
                    strlcat(value, out_channels_name_to_enum_table[j].name, sizeof(value));
                    first = false;
                    break;
                }
            }
            i++;
        }
        str_parms_add_str(reply, AUDIO_PARAMETER_STREAM_SUP_CHANNELS, value);
        str = str_parms_to_str(reply);
    } else {
        voice_extn_out_get_parameters(out, query, reply);
        str = str_parms_to_str(reply);
        if (str && !strncmp(str, "", sizeof(""))) {
            free(str);
            str = strdup(keys);
        }
    }

    ret = str_parms_get_str(query, AUDIO_PARAMETER_STREAM_SUP_FORMATS, value, sizeof(value));
    if (ret >= 0) {
        value[0] = '\0';
        i = 0;
        first = true;
        while (out->supported_formats[i] != 0) {
            for (j = 0; j < ARRAY_SIZE(out_formats_name_to_enum_table); j++) {
                if (out_formats_name_to_enum_table[j].value == out->supported_formats[i]) {
                    if (!first) {
                        strlcat(value, "|", sizeof(value));
                    }
                    strlcat(value, out_formats_name_to_enum_table[j].name, sizeof(value));
                    first = false;
                    break;
                }
            }
            i++;
        }
        str_parms_add_str(reply, AUDIO_PARAMETER_STREAM_SUP_FORMATS, value);
        str = str_parms_to_str(reply);
    }
    str_parms_destroy(query);
    str_parms_destroy(reply);
    ALOGV("%s: exit: returns - %s", __func__, str);
    return str;
}

static uint32_t out_get_latency(const struct audio_stream_out *stream)
{
    struct stream_out *out = (struct stream_out *)stream;
    uint32_t latency = 0;

    if (is_offload_usecase(out->usecase)) {
        latency = COMPRESS_OFFLOAD_PLAYBACK_LATENCY;
    } else {
        latency = (out->config.period_count * out->config.period_size * 1000) /
           (out->config.rate);
    }

    ALOGV("%s: Latency %d", __func__, latency);
    return latency;
}

static int out_set_volume(struct audio_stream_out *stream, float left,
                          float right)
{
    struct stream_out *out = (struct stream_out *)stream;
    int volume[2];

    if (out->usecase == USECASE_AUDIO_PLAYBACK_MULTI_CH) {
        /* only take left channel into account: the API is for stereo anyway */
        out->muted = (left == 0.0f);
        return 0;
    } else if (is_offload_usecase(out->usecase)) {
        if (audio_extn_dolby_is_passthrough_stream(out->flags)) {
            /*
             * Set mute or umute on HDMI passthrough stream.
             * Only take left channel into account.
             * Mute is 0 and unmute 1
             */
            audio_extn_dolby_set_passt_volume(out, (left == 0.0f));
        } else {
            char mixer_ctl_name[128];
            struct audio_device *adev = out->dev;
            struct mixer_ctl *ctl;
            int pcm_device_id = platform_get_pcm_device_id(out->usecase,
                                                       PCM_PLAYBACK);

            snprintf(mixer_ctl_name, sizeof(mixer_ctl_name),
                     "Compress Playback %d Volume", pcm_device_id);
            ctl = mixer_get_ctl_by_name(adev->mixer, mixer_ctl_name);
            if (!ctl) {
                ALOGE("%s: Could not get ctl for mixer cmd - %s",
                      __func__, mixer_ctl_name);
                return -EINVAL;
            }
            volume[0] = (int)(left * COMPRESS_PLAYBACK_VOLUME_MAX);
            volume[1] = (int)(right * COMPRESS_PLAYBACK_VOLUME_MAX);
            mixer_ctl_set_array(ctl, volume, sizeof(volume)/sizeof(volume[0]));
            return 0;
        }
    }

    return -ENOSYS;
}

#ifdef ALSA_DEBUG
static void dump_data(char* type, const void* data, unsigned int count) {
    char value[92];
    int filesize = 0;
    if(strcmp(type, DUMP_TYPE_OUT) == 0)
        property_get(DUMP_OUT_PROPERTY, value, "0");
    else if(strcmp(type, DUMP_TYPE_IN) == 0)
        property_get(DUMP_IN_PROPERTY, value, "0");
    filesize = (atoi(value) > DUMP_FILE_MAX_SIZE) ? DUMP_FILE_MAX_SIZE : atoi(value);
    if(filesize > 0) {
        Start(type);
        MessageInfo msg;
        msg.count = count;
        msg.filesize = filesize;
        memcpy(msg.type, type, strlen(type));
        memcpy(msg.buffer, (char*)data, count);
        PostMessage(type,msg);
    } else {
        Stop(type);
    }
}
#endif

static ssize_t out_write(struct audio_stream_out *stream, const void *buffer,
                         size_t bytes)
{
    struct stream_out *out = (struct stream_out *)stream;
    struct audio_device *adev = out->dev;
    ssize_t ret = 0;
    int apply_autoeff;
    lock_output_stream(out);

    if (CARD_STATUS_OFFLINE == out->card_status) {
        // increase written size during SSR to avoid mismatch
        // with the written frames count in AF
        if (!is_offload_usecase(out->usecase))
            out->written += bytes / (out->config.channels * sizeof(short));

        if (out->pcm) {
            ALOGD(" %s: sound card is not active/SSR state", __func__);
            ret= -EIO;
            goto exit;
        } else if (is_offload_usecase(out->usecase)) {
            //during SSR for compress usecase we should return error to flinger
            ALOGD(" copl %s: sound card is not active/SSR state", __func__);
            pthread_mutex_unlock(&out->lock);
            return -ENETRESET;
        }
    }
    apply_autoeff=0;
    if (out->standby) {
        out->standby = false;
        pthread_mutex_lock(&adev->lock);
        if (out->usecase == USECASE_COMPRESS_VOIP_CALL)
            ret = voice_extn_compress_voip_start_output_stream(out);
        else {
            ret = start_output_stream(out);
            apply_autoeff=1;
        }

        pthread_mutex_unlock(&adev->lock);
        /* ToDo: If use case is compress offload should return 0 */
        if (ret != 0) {
            out->standby = true;
            goto exit;
        }
        if (out->usecase != USECASE_AUDIO_PLAYBACK_OFFLOAD && adev->adm_register_output_stream)
            adev->adm_register_output_stream(adev->adm_data, out->handle, out->flags);
    }

    if (adev->is_channel_status_set == false && (out->devices & AUDIO_DEVICE_OUT_AUX_DIGITAL)){
        audio_utils_set_hdmi_channel_status(out, buffer, bytes);
        adev->is_channel_status_set = true;
    }

#ifdef BUS_ADDRESS_ENABLED
    if (out->car_audio_stream) {
#ifdef VHAL_HELPER_ENABLED
        if (!property_get_bool("persist.vendor.audio.vehicle.focus.enabled", true)) {
            ALOGVV("%s: vhal audio focus disabled, continue", __func__);
        } else if (out->vhal_audio_helper == NULL) {
            ALOGE("%s: vhal audio helper not allocated, continue", __func__);
        } else {
            int focus_state =
                vehicle_hal_audio_helper_get_stream_focus_state(out->vhal_audio_helper,
                                                                out->car_audio_stream);
            if (focus_state == VEHICLE_HAL_AUDIO_HELPER_FOCUS_STATE_NO_FOCUS) {
                // Audioflinger keeps busy loop for 0 return. So wait for focus up to payload length.
                nsecs_t wait_time_ns = (nsecs_t) bytes * 1000000000 /
                                       audio_stream_out_frame_size(stream) /
                                       out_get_sample_rate(&out->stream.common);
                if (vehicle_hal_audio_helper_wait_for_stream_focus(out->vhal_audio_helper,
                                                                   out->car_audio_stream,
                                                                   wait_time_ns) == 0) {
                    ALOGVV("%s: NO FOCUS, wait time %d", __func__, wait_time_ns);
                    pthread_mutex_unlock(&out->lock);
                    return 0;
                }
            } else if (focus_state == VEHICLE_HAL_AUDIO_HELPER_FOCUS_STATE_TIMEOUT) {
                // Throw data as it timed out.
                nsecs_t wait_time_ns = (nsecs_t) bytes * 1000000000 /
                                       audio_stream_out_frame_size(stream) /
                                       out_get_sample_rate(&out->stream.common);
                if (vehicle_hal_audio_helper_wait_for_stream_focus(out->vhal_audio_helper,
                                                                   out->car_audio_stream,
                                                                   wait_time_ns) == 0) {
                    ALOGVV("%s: TIMEOUT, wait time %d", __func__, wait_time_ns);
                    pthread_mutex_unlock(&out->lock);
                    return bytes; // Throw away
                }
            } else
                ALOGVV("%s: FOCUS", __func__);
        }
#endif
    }
#endif

    if (is_offload_usecase(out->usecase)) {
        ALOGVV("copl(%p): writing buffer (%zu bytes) to compress device", out, bytes);
        if (out->send_new_metadata) {
            ALOGD("copl(%p):send new gapless metadata", out);
            compress_set_gapless_metadata(out->compr, &out->gapless_mdata);
            out->send_new_metadata = 0;
            if (out->send_next_track_params && out->is_compr_metadata_avail) {
                ALOGD("copl(%p):send next track params in gapless", out);
#ifdef COMPRESS_METADATA_NEEDED
                compress_set_next_track_param(out->compr, &(out->compr_config.codec->options));
#endif
                out->send_next_track_params = false;
                out->is_compr_metadata_avail = false;
            }
        }

        ret = compress_write(out->compr, buffer, bytes);
        if (ret < 0)
            ret = -errno;
        ALOGVV("%s: writing buffer (%d bytes) to compress device returned %d", __func__, bytes, ret);
        if (ret >= 0 && ret < (ssize_t)bytes) {
            ALOGD("No space available in compress driver, post msg to cb thread");
            send_offload_cmd_l(out, OFFLOAD_CMD_WAIT_FOR_BUFFER);
        } else if (-ENETRESET == ret) {
            ALOGE("copl %s: received sound card offline state on compress write", __func__);
            out->card_status = CARD_STATUS_OFFLINE;
            pthread_mutex_unlock(&out->lock);
            out_on_error(&out->stream.common);
            return ret;
        }
        if ( ret == (ssize_t)bytes && !out->non_blocking)
            out->written += bytes;

        if (!out->playback_started && ret >= 0) {
            compress_start(out->compr);
            audio_extn_dts_eagle_fade(adev, true, out);
            out->playback_started = 1;
            out->offload_state = OFFLOAD_STATE_PLAYING;

            audio_extn_dts_notify_playback_state(out->usecase, 0, out->sample_rate,
                                                     popcount(out->channel_mask),
                                                     out->playback_started);
        }
        pthread_mutex_unlock(&out->lock);
        return ret;
    } else {
        if (out->pcm) {
            if (out->muted)
                memset((void *)buffer, 0, bytes);

            ALOGVV("%s: writing buffer (%d bytes) to pcm device", __func__, bytes);

            if (adev->adm_request_focus)
                adev->adm_request_focus(adev->adm_data, out->handle);

            if (out->usecase == USECASE_AUDIO_PLAYBACK_AFE_PROXY)
                ret = pcm_mmap_write(out->pcm, (void *)buffer, bytes);
            else {
#ifdef ALSA_DEBUG
                dump_data(DUMP_TYPE_OUT,buffer,bytes);
#endif
                ret = pcm_write(out->pcm, (void *)buffer, bytes);
            }

            if (ret < 0)
                ret = -errno;
            else if (ret == 0)
                out->written += bytes / (out->config.channels * sizeof(short));

            if (adev->adm_abandon_focus)
                adev->adm_abandon_focus(adev->adm_data, out->handle);
        }
    }

exit:
    if (-ENETRESET == ret) {
        out->card_status = CARD_STATUS_OFFLINE;
    }
    pthread_mutex_unlock(&out->lock);

    if (ret != 0) {
        if (out->pcm)
            ALOGE("%s: error %zu - %s", __func__, ret, pcm_get_error(out->pcm));
        if (out->usecase == USECASE_COMPRESS_VOIP_CALL) {
            pthread_mutex_lock(&adev->lock);
            voice_extn_compress_voip_close_output_stream(&out->stream.common);
            pthread_mutex_unlock(&adev->lock);
            out->standby = true;
        }
        out_on_error(&out->stream.common);
        usleep(bytes * 1000000 / audio_stream_out_frame_size(stream) /
                        out_get_sample_rate(&out->stream.common));

    }
#if defined(AUTOEFFECTS_ENABLE)
    else {
        if (apply_autoeff){
            ALOGD("%s: Start AUTO effects", __func__);
            if (adev->auto_effects_start_output != NULL){
                pthread_mutex_lock(&adev->lock);
                adev->auto_effects_start_output(out->handle, out->usecase);
                pthread_mutex_unlock(&adev->lock);
            }
        }
    }
#endif
    return bytes;
}

static int out_get_render_position(const struct audio_stream_out *stream,
                                   uint32_t *dsp_frames)
{
    struct stream_out *out = (struct stream_out *)stream;

    if (dsp_frames == NULL)
        return -EINVAL;

    *dsp_frames = 0;
    if (is_offload_usecase(out->usecase)) {
        ssize_t ret = 0;
        lock_output_stream(out);
        if (out->compr != NULL) {
            ret = compress_get_tstamp(out->compr, (unsigned long *)dsp_frames,
                    &out->sample_rate);
            if (ret < 0)
                ret = -errno;
            ALOGVV("%s rendered frames %d sample_rate %d",
                   __func__, *dsp_frames, out->sample_rate);
        }
        if (-ENETRESET == ret) {
            ALOGE(" ERROR: sound card not active Unable to get time stamp from compress driver");
            out->card_status = CARD_STATUS_OFFLINE;
            ret = -EINVAL;
        } else if(ret < 0) {
            ALOGE(" ERROR: Unable to get time stamp from compress driver");
            ret = -EINVAL;
        } else if (out->card_status == CARD_STATUS_OFFLINE) {
            /*
             * Handle corner case where compress session is closed during SSR
             * and timestamp is queried
             */
            ALOGE(" ERROR: sound card not active, return error");
            ret = -EINVAL;
        } else {
            ret = 0;
        }
        pthread_mutex_unlock(&out->lock);
        return ret;
    } else if (audio_is_linear_pcm(out->format)) {
        *dsp_frames = out->written;
        return 0;
    } else
        return -EINVAL;
}

static int out_add_audio_effect(const struct audio_stream *stream __unused,
                                effect_handle_t effect __unused)
{
    return 0;
}

static int out_remove_audio_effect(const struct audio_stream *stream __unused,
                                   effect_handle_t effect __unused)
{
    return 0;
}

static int out_get_next_write_timestamp(const struct audio_stream_out *stream __unused,
                                        int64_t *timestamp __unused)
{
    return -EINVAL;
}

static int out_get_presentation_position(const struct audio_stream_out *stream,
                                   uint64_t *frames, struct timespec *timestamp)
{
    struct stream_out *out = (struct stream_out *)stream;
    int ret = -EINVAL;
    unsigned long dsp_frames;

    lock_output_stream(out);

    if (is_offload_usecase(out->usecase)) {
        if (out->compr != NULL) {
            ret = compress_get_tstamp(out->compr, &dsp_frames,
                    &out->sample_rate);
            ALOGVV("%s rendered frames %ld sample_rate %d",
                   __func__, dsp_frames, out->sample_rate);
            *frames = dsp_frames;
            if (ret < 0)
                ret = -errno;
            if (-ENETRESET == ret) {
                ALOGE(" ERROR: sound card not active Unable to get time stamp from compress driver");
                out->card_status = CARD_STATUS_OFFLINE;
                ret = -EINVAL;
            } else
                ret = 0;

            /* this is the best we can do */
            clock_gettime(CLOCK_MONOTONIC, timestamp);
        }
    } else {
        if (out->pcm) {
            unsigned int avail;
            if (pcm_get_htimestamp(out->pcm, &avail, timestamp) == 0) {
                size_t kernel_buffer_size = out->config.period_size * out->config.period_count;
                int64_t signed_frames = out->written - kernel_buffer_size + avail;
                // This adjustment accounts for buffering after app processor.
                // It is based on estimated DSP latency per use case, rather than exact.
                signed_frames -=
                    (platform_render_latency(out->usecase) * out->sample_rate / 1000000LL);

                // It would be unusual for this value to be negative, but check just in case ...
                if (signed_frames >= 0) {
                    *frames = signed_frames;
                    ret = 0;
                }
            }
        }
    }

    pthread_mutex_unlock(&out->lock);

    return ret;
}

static int out_set_callback(struct audio_stream_out *stream,
            stream_callback_t callback, void *cookie)
{
    struct stream_out *out = (struct stream_out *)stream;

    ALOGV("%s", __func__);
    lock_output_stream(out);
    out->offload_callback = callback;
    out->offload_cookie = cookie;
    pthread_mutex_unlock(&out->lock);
    return 0;
}

static int out_pause(struct audio_stream_out* stream)
{
    struct stream_out *out = (struct stream_out *)stream;
    int status = -ENOSYS;
    ALOGV("%s", __func__);
    if (is_offload_usecase(out->usecase)) {
        ALOGD("copl(%p):pause compress driver", out);
        lock_output_stream(out);
        if (out->compr != NULL && out->offload_state == OFFLOAD_STATE_PLAYING) {
            if (out->card_status != CARD_STATUS_OFFLINE)
                status = compress_pause(out->compr);

            out->offload_state = OFFLOAD_STATE_PAUSED;

            audio_extn_dts_eagle_fade(adev, false, out);
            audio_extn_dts_notify_playback_state(out->usecase, 0,
                                                 out->sample_rate, popcount(out->channel_mask),
                                                 0);
        }
        pthread_mutex_unlock(&out->lock);
    }
    return status;
}

static int out_resume(struct audio_stream_out* stream)
{
    struct stream_out *out = (struct stream_out *)stream;
    int status = -ENOSYS;
    ALOGV("%s", __func__);
    if (is_offload_usecase(out->usecase)) {
        ALOGD("copl(%p):resume compress driver", out);
        status = 0;
        lock_output_stream(out);
        if (out->compr != NULL && out->offload_state == OFFLOAD_STATE_PAUSED) {
            if (out->card_status != CARD_STATUS_OFFLINE)
                status = compress_resume(out->compr);

            out->offload_state = OFFLOAD_STATE_PLAYING;

            audio_extn_dts_eagle_fade(adev, true, out);
            audio_extn_dts_notify_playback_state(out->usecase, 0, out->sample_rate,
                                                     popcount(out->channel_mask), 1);
        }
        pthread_mutex_unlock(&out->lock);
    }
    return status;
}

static int out_drain(struct audio_stream_out* stream, audio_drain_type_t type )
{
    struct stream_out *out = (struct stream_out *)stream;
    int status = -ENOSYS;
    ALOGV("%s", __func__);
    if (is_offload_usecase(out->usecase)) {
        lock_output_stream(out);
        if (type == AUDIO_DRAIN_EARLY_NOTIFY)
            status = send_offload_cmd_l(out, OFFLOAD_CMD_PARTIAL_DRAIN);
        else
            status = send_offload_cmd_l(out, OFFLOAD_CMD_DRAIN);
        pthread_mutex_unlock(&out->lock);
    }
    return status;
}

static int out_flush(struct audio_stream_out* stream)
{
    struct stream_out *out = (struct stream_out *)stream;
    ALOGV("%s", __func__);
    if (is_offload_usecase(out->usecase)) {
        ALOGD("copl(%p):calling compress flush", out);
        lock_output_stream(out);
        stop_compressed_output_l(out);
        out->written = 0;
        pthread_mutex_unlock(&out->lock);
        ALOGD("copl(%p):out of compress flush", out);
        return 0;
    }
    return -ENOSYS;
}

/** audio_stream_in implementation **/
static uint32_t in_get_sample_rate(const struct audio_stream *stream)
{
    struct stream_in *in = (struct stream_in *)stream;

    return in->config.rate;
}

static int in_set_sample_rate(struct audio_stream *stream __unused,
                              uint32_t rate __unused)
{
    return -ENOSYS;
}

static size_t in_get_buffer_size(const struct audio_stream *stream)
{
    struct stream_in *in = (struct stream_in *)stream;

    if(in->usecase == USECASE_COMPRESS_VOIP_CALL)
        return voice_extn_compress_voip_in_get_buffer_size(in);
    else if(audio_extn_compr_cap_usecase_supported(in->usecase))
        return audio_extn_compr_cap_get_buffer_size(in->config.format);

    return in->config.period_size *
                audio_stream_in_frame_size((const struct audio_stream_in *)stream);
}

static uint32_t in_get_channels(const struct audio_stream *stream)
{
    struct stream_in *in = (struct stream_in *)stream;

    return in->channel_mask;
}

static audio_format_t in_get_format(const struct audio_stream *stream)
{
    struct stream_in *in = (struct stream_in *)stream;

    return in->format;
}

static int in_set_format(struct audio_stream *stream __unused,
                         audio_format_t format __unused)
{
    return -ENOSYS;
}

static int in_standby(struct audio_stream *stream)
{
    struct stream_in *in = (struct stream_in *)stream;
    struct audio_device *adev = in->dev;
    int status = 0;
    ALOGD("%s: enter: stream (%p) usecase(%d: %s)", __func__,
          stream, in->usecase, use_case_table[in->usecase]);

    if (in->usecase == USECASE_COMPRESS_VOIP_CALL) {
        /* Ignore standby in case of voip call because the voip input
         * stream is closed in adev_close_input_stream()
         */
        ALOGV("%s: Ignore Standby in VOIP call", __func__);
        return status;
    }
    else if (adev->voice_barge_in_enabled) {
        ALOGD("%s: Ignore Standby because Voice Barge-In enabled.", __func__);
        return status;
    }

    lock_input_stream(in);
    if (!in->standby && in->is_st_session) {
        ALOGD("%s: sound trigger pcm stop lab", __func__);
        audio_extn_sound_trigger_stop_lab(in);
        in->standby = 1;
    }

    if (!in->standby) {
        if (adev->adm_deregister_stream)
            adev->adm_deregister_stream(adev->adm_data, in->capture_handle);

        pthread_mutex_lock(&adev->lock);
        in->standby = true;
        if (in->pcm) {
            pcm_close(in->pcm);
            in->pcm = NULL;
        }
        status = stop_input_stream(in);
        pthread_mutex_unlock(&adev->lock);
    }
    pthread_mutex_unlock(&in->lock);
    ALOGV("%s: exit:  status(%d)", __func__, status);
    return status;
}

static int in_dump(const struct audio_stream *stream __unused,
                   int fd __unused)
{
    return 0;
}

static void in_snd_mon_cb(void * stream, struct str_parms * parms)
{
    if (!stream || !parms)
        return;

    struct stream_in *in = (struct stream_in *)stream;
    struct audio_device *adev = in->dev;

    card_status_t status;
    int card;
    if (parse_snd_card_status(parms, &card, &status) < 0)
        return;

    pthread_mutex_lock(&adev->lock);
    bool valid_cb = (card == adev->snd_card);
    pthread_mutex_unlock(&adev->lock);

    if (!valid_cb)
        return;

    lock_input_stream(in);
    if (in->card_status != status)
        in->card_status = status;
    pthread_mutex_unlock(&in->lock);

    ALOGW("in_snd_mon_cb for card %d usecase %s, status %s", card,
          use_case_table[in->usecase],
          status == CARD_STATUS_OFFLINE ? "offline" : "online");

    // a better solution would be to report error back to AF and let
    // it put the stream to standby
    if (status == CARD_STATUS_OFFLINE)
        in_standby(&in->stream.common);

    return;
}

static int in_set_parameters(struct audio_stream *stream, const char *kvpairs)
{
    struct stream_in *in = (struct stream_in *)stream;
    struct audio_device *adev = in->dev;
    struct str_parms *parms;
    char *str;
    char value[32];
    int ret = 0, val = 0, err;

    ALOGD("%s: enter: kvpairs=%s", __func__, kvpairs);
    parms = str_parms_create_str(kvpairs);

    if (!parms)
        goto error;
    lock_input_stream(in);
    pthread_mutex_lock(&adev->lock);

    err = str_parms_get_str(parms, AUDIO_PARAMETER_STREAM_INPUT_SOURCE, value, sizeof(value));
    if (err >= 0) {
        val = atoi(value);
        /* no audio source uses val == 0 */
        if ((in->source != val) && (val != 0)) {
            in->source = val;
            if ((in->source == AUDIO_SOURCE_VOICE_COMMUNICATION) &&
                (in->dev->mode == AUDIO_MODE_IN_COMMUNICATION) &&
                (voice_extn_compress_voip_is_format_supported(in->format)) &&
                (in->config.rate == 8000 || in->config.rate == 16000) &&
                (audio_channel_count_from_in_mask(in->channel_mask) == 1)) {
                err = voice_extn_compress_voip_open_input_stream(in);
                if (err != 0) {
                    ALOGE("%s: Compress voip input cannot be opened, error:%d",
                          __func__, err);
                }
            }
        }
    }

    err = str_parms_get_str(parms, AUDIO_PARAMETER_STREAM_ROUTING, value, sizeof(value));
    if (err >= 0) {
        val = atoi(value);
        if (((int)in->device != val) && (val != 0)) {
            in->device = val;
            /* If recording is in progress, change the tx device to new device */
            if (!in->standby && !in->is_st_session)
                ret = select_devices(adev, in->usecase);
        }
    }

done:
    pthread_mutex_unlock(&adev->lock);
    pthread_mutex_unlock(&in->lock);

    str_parms_destroy(parms);
error:
    ALOGV("%s: exit: status(%d)", __func__, ret);
    return ret;
}

static char* in_get_parameters(const struct audio_stream *stream,
                               const char *keys)
{
    struct stream_in *in = (struct stream_in *)stream;
    struct str_parms *query = str_parms_create_str(keys);
    char *str;
    char value[256];
    struct str_parms *reply = str_parms_create();

    if (!query || !reply) {
        ALOGE("in_get_parameters: failed to create query or reply");
        return NULL;
    }

    ALOGV("%s: enter: keys - %s", __func__, keys);

    voice_extn_in_get_parameters(in, query, reply);

    str = str_parms_to_str(reply);
    str_parms_destroy(query);
    str_parms_destroy(reply);

    ALOGV("%s: exit: returns - %s", __func__, str);
    return str;
}

static int in_set_gain(struct audio_stream_in *stream __unused,
                       float gain __unused)
{
    return 0;
}

static ssize_t in_read(struct audio_stream_in *stream, void *buffer,
                       size_t bytes)
{
    struct stream_in *in = (struct stream_in *)stream;
    struct audio_device *adev = in->dev;
    int ret = -1;

    lock_input_stream(in);

    if (in->is_st_session) {
        ALOGVV(" %s: reading on st session bytes=%zu", __func__, bytes);
        /* Read from sound trigger HAL */
        audio_extn_sound_trigger_read(in, buffer, bytes);
        pthread_mutex_unlock(&in->lock);
        return bytes;
    }

    if (in->standby && !adev->vad_stream_running) {
        pthread_mutex_lock(&adev->lock);
        if (in->usecase == USECASE_COMPRESS_VOIP_CALL)
            ret = voice_extn_compress_voip_start_input_stream(in);
        else
            ret = start_input_stream(in);
        pthread_mutex_unlock(&adev->lock);
        if (ret != 0) {
            goto exit;
        }
        in->standby = 0;
        if (adev->adm_register_input_stream)
            adev->adm_register_input_stream(adev->adm_data, in->capture_handle, in->flags);
    }

    if (adev->adm_request_focus)
        adev->adm_request_focus(adev->adm_data, in->capture_handle);

    if (adev->voice_barge_in_enabled && !adev->vad_stream_running){
        audio_extn_vad_circ_buf_create_read_loop(in);
        adev->vad_stream = stream;
        adev->vad_stream_running = true;
    }

    if (adev->vad_stream_running){
      in = adev->vad_stream;
    }

    if (in->pcm) {
        if (audio_extn_ssr_get_enabled() &&
                audio_channel_count_from_in_mask(in->channel_mask) == 6)
            ret = audio_extn_ssr_read(stream, buffer, bytes);
        else if (audio_extn_compr_cap_usecase_supported(in->usecase))
            ret = audio_extn_compr_cap_read(in, buffer, bytes);
        else if (in->usecase == USECASE_AUDIO_RECORD_AFE_PROXY)
            ret = pcm_mmap_read(in->pcm, buffer, bytes);
        else if (adev->vad_stream_running)
            ret = audio_extn_vad_circ_buf_read(in, buffer, bytes);
        else
            ret = pcm_read(in->pcm, buffer, bytes);
        if (ret < 0) {
            ret = -errno;
        }
        else {
#ifdef ALSA_DEBUG
            dump_data(DUMP_TYPE_IN,buffer,bytes);
#endif
        }
    }

    if (adev->adm_abandon_focus)
        adev->adm_abandon_focus(adev->adm_data, in->capture_handle);

    /*
     * Instead of writing zeroes here, we could trust the hardware
     * to always provide zeroes when muted.
     */
    if (ret == 0 && voice_get_mic_mute(adev) && !voice_is_in_call_rec_stream(in) &&