#include <DumpAdapter.h>
#include <DumpThread.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cutils/log.h>
#include <cutils/properties.h>

#define LOG_TAG "DumpThread"

static DumpThread mDumpOut;
static DumpThread mDumpIn;

void Start(char* type) {
    if(strcmp(type,DUMP_TYPE_OUT) == 0)
        mDumpOut.Start();
    if(strcmp(type,DUMP_TYPE_IN) == 0)
        mDumpIn.Start();
}

void Stop(char* type) {
    if(strcmp(type,DUMP_TYPE_OUT) == 0)
        mDumpOut.Stop();
    if(strcmp(type,DUMP_TYPE_IN) == 0)
        mDumpIn.Stop();
}

void PostMessage(char* type, MessageInfo msg) {
    if(strcmp(type,DUMP_TYPE_OUT) == 0) {
        mDumpOut.PostMessage(msg);
    }
    if(strcmp(type,DUMP_TYPE_IN) == 0) {
        mDumpIn.PostMessage(msg);
    }
}

int Dump(int argv, char** argc) {

    if(argv < 3) {
        fprintf(stderr, "\nUsage: %s [out/in] [on/off]\n", argc[0]);
        return 0;
    }
    if(strcmp(argc[1],"out") == 0 && strcmp(argc[2],"on") == 0) {
        if(access(DUMP_OUT_FILE_PATH,0) != 0) {
            mode_t mask = umask(0);
            int ret = creat(DUMP_OUT_FILE_PATH, 0777);
            if (ret == -1) {
                ALOGD("create file failed");
            }
            umask(mask);
        }
        property_set(DUMP_OUT_PROPERTY, DUMP_PROPERTY_MAX_SIZE);
        return 1;
    }
    else if(strcmp(argc[1],"out") == 0 && strcmp(argc[2],"off") == 0) {
        property_set(DUMP_OUT_PROPERTY, DUMP_PROPERTY_MIN_SIZE);
        return 1;
    }
    else if(strcmp(argc[1],"in") == 0 && strcmp(argc[2],"on") == 0) {
        if(access(DUMP_IN_FILE_PATH,0) != 0) {
            mode_t mask = umask(0);
            int ret = creat(DUMP_IN_FILE_PATH, 0777);
            if (ret == -1) {
                fprintf(stderr, "create file failed : %s \n", strerror(errno));
            }
            umask(mask);
        }
        property_set(DUMP_IN_PROPERTY, DUMP_PROPERTY_MAX_SIZE);
        return 1;
    }
    else if(strcmp(argc[1],"in") == 0 && strcmp(argc[2],"off") == 0) {
        property_set(DUMP_IN_PROPERTY, DUMP_PROPERTY_MIN_SIZE);
        return 1;
    } else {
        fprintf(stderr, "\nUsage: %s [out/in] [on/off]\n", argc[0]);
    }
    return 0;
}