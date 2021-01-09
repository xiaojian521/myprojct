#ifndef COMMONDEFINE_H
#define COMMONDEFINE_H

#define ALSA_DEBUG

#ifdef ALSA_DEBUG
#define DUMP_TYPE_IN "in"
#define DUMP_IN_PROPERTY "audio.tinyalsa.in"
#define DUMP_IN_FILE_PATH "/data/dump_in.pcm"

#define DUMP_TYPE_OUT "out"
#define DUMP_OUT_PROPERTY "audio.tinyalsa.out"
#define DUMP_OUT_FILE_PATH "/data/dump_out.pcm"

#define DUMP_FILE_MAX_SIZE 50
#define DUMP_PROPERTY_MAX_SIZE "50"
#define DUMP_PROPERTY_MIN_SIZE "0"

typedef struct MsgInfo
{
    char type[10];
    char buffer[20*1024];
    unsigned int count;
    unsigned int filesize;
} MessageInfo;
#endif //ALSA_DEBUG

#endif //COMMONDEFINE_H