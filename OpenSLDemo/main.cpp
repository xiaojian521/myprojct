#include <stdio.h>
#include <stdlib.h>

#include "OpenSLES.h"
#include "OpenSLES_Android.h"
#include <cutils/log.h>
#include <utils/CallStack.h>
#include <cutils/properties.h>
#include <pthread.h>
#include <unistd.h>
#define LOG_TAG "xjtest"

#define SLEEP(x)

//最大接口数量
#define MAX_NUMBER_INTERFACES 3
//最大输出设备数量
#define MAX_NUMBER_OUTPUT_DEVICES 6
//本地缓存空间大小
#define AUDIO_DATA_STORAGE_SIZE 4096
//本地缓存字节数
#define AUDIO_DATA_BUFFER_SIZE 4096/8

static pthread_mutex_t  audioEngineLock = PTHREAD_MUTEX_INITIALIZER;

// aux effect on the output mix, used by the buffer queue player
static const SLEnvironmentalReverbSettings reverbSettings =
    SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

void CheckErr(SLresult res, int id) {
    if(res != SL_RESULT_SUCCESS) {
        android::CallStack stack("xjtest"); 
        stack.update();
        stack.log("[debug]",ANDROID_LOG_ERROR,"prefix");
        ALOGD("error is :%d, id:%d",res,id);
    }
}

//回调操作缓存信息的结构体
typedef struct CallbackCntxt_ {
    SLPlayItf   playItf;
    SLint16*    pDataBase;
    SLint16*    pData;
    SLuint32    size;
} CallbackCntxt;

//本地缓存数组
SLint16 pcmData[AUDIO_DATA_STORAGE_SIZE];

static FILE* file = NULL;

void getPcmData() {
    if(file == NULL) {
        file = fopen("/data/music.pcm","rb");
    }
    if(file == NULL ) {
        ALOGD("getPcmData() file is not open() ");
        return;
    }
    fread(pcmData,AUDIO_DATA_STORAGE_SIZE*2, 1,file);
}


void BufferQueueCallback(SLBufferQueueItf queueItf, void* pContext) {
    // pthread_mutex_trylock(&audioEngineLock);
    SLresult res;
    CallbackCntxt* pCntxt = (CallbackCntxt*)pContext;

    if(pCntxt->pData < (pCntxt->pDataBase + AUDIO_DATA_STORAGE_SIZE)) {
        
        res = (*queueItf)->Enqueue(queueItf, (void*)pCntxt->pData, 4 * AUDIO_DATA_BUFFER_SIZE);
        CheckErr(res,1);
        pCntxt->pData += AUDIO_DATA_BUFFER_SIZE * 2;
    } else {
        pCntxt->pData = pCntxt->pDataBase;
        getPcmData();
        BufferQueueCallback(queueItf, pContext);
    }
    // pthread_mutex_unlock(&audioEngineLock);
}


void TestPlayMusicBufferQueue(SLObjectItf sl) {
    SLEngineItf                     EngineItf;
    SLint32                         numOutputs = 0;
    SLuint32                        deviceId = 0;

    SLresult                        res;

    SLDataSource                    audioSource;
    SLDataLocator_BufferQueue       bufferQueue;
    SLDataFormat_PCM                pcm;

    SLDataSink                      audioSink;
    SLDataLocator_OutputMix         locator_outputmix;

    SLObjectItf                     player;
    SLPlayItf                       playItf;
    SLBufferQueueItf                bufferQueueItf;
    SLBufferQueueState              state;

    SLObjectItf                     OutputMix;
    SLVolumeItf                     volumeItf;
    SLEnvironmentalReverbItf        outputMixEnvironmentalReverb;

    int                             i;

    SLboolean required[MAX_NUMBER_INTERFACES];
    SLInterfaceID iidArray[MAX_NUMBER_INTERFACES];

    CallbackCntxt cntxt;

    res = (*sl)->GetInterface(sl, SL_IID_ENGINE, (void*)&EngineItf);
    CheckErr(res,2);

    for(i = 0; i < MAX_NUMBER_INTERFACES; ++i) {
        required[i] = SL_BOOLEAN_FALSE;
        iidArray[i] = SL_IID_NULL;
    }
    //设置required和iidArray为音量接口
    required[0] = SL_BOOLEAN_FALSE;
    iidArray[0] = SL_IID_ENVIRONMENTALREVERB;

    res = (*EngineItf)->CreateOutputMix(EngineItf, &OutputMix, 0, iidArray, required);
    CheckErr(res,3);

    res = (*OutputMix)->Realize(OutputMix,SL_BOOLEAN_FALSE);
    CheckErr(res,4);

    // res = (*OutputMix)->GetInterface(OutputMix, SL_IID_ENVIRONMENTALREVERB, &outputMixEnvironmentalReverb);
    // CheckErr(res,5);
    // if (SL_RESULT_SUCCESS == res) {
    //     res = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
    //             outputMixEnvironmentalReverb, &reverbSettings);
    //     CheckErr(res,6);
    // }
    bufferQueue.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
    bufferQueue.numBuffers = 4;

    // pcm.samplesPerSec = SL_SAMPLINGRATE_16;
    // pcm.channelMask = SL_SPEAKER_FRONT_LEFT;
    // pcm.numChannels = 1;
    pcm.samplesPerSec = SL_SAMPLINGRATE_48;
    pcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    pcm.numChannels = 2;
    pcm.formatType = SL_DATAFORMAT_PCM;
    pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
    pcm.containerSize = 16;
    pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;

    audioSource.pFormat = (void*)&pcm;
    audioSource.pLocator = (void*)&bufferQueue;


    locator_outputmix.locatorType = SL_DATALOCATOR_OUTPUTMIX;
    locator_outputmix.outputMix = OutputMix;
    audioSink.pLocator = (void*)&locator_outputmix;
    audioSink.pFormat = NULL;

    //初始化缓冲队列回调上下文
    cntxt.pDataBase = pcmData;
    cntxt.pData = cntxt.pDataBase;
    cntxt.size = sizeof(pcmData);
    // ALOGD("cntxt.size IS= %d",sizeof(pcmData));
    //for SEEK interface 
    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_EFFECTSEND,
                                    /*SL_IID_MUTESOLO,*/};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
                                   /*SL_BOOLEAN_TRUE,*/ };

    res = (*EngineItf)->CreateAudioPlayer(EngineItf, &player, &audioSource, &audioSink, 1, ids, req);
    CheckErr(res,7);

    res = (*player)->Realize(player, SL_BOOLEAN_FALSE);
    CheckErr(res,8);

    res = (*player)->GetInterface(player, SL_IID_PLAY, (void*)&playItf);
    CheckErr(res,9);

    res = (*player)->GetInterface(player, SL_IID_BUFFERQUEUE, (void*)&bufferQueueItf);
    CheckErr(res,10);

    res = (*bufferQueueItf)->RegisterCallback(bufferQueueItf, BufferQueueCallback, (void*)&cntxt);

    res = (*player)->GetInterface(player, SL_IID_VOLUME, &volumeItf);
    CheckErr(res,11);
    // res = (*volumeItf)->SetVolumeLevel(volumeItf, -300);
    // ALOGD("TestPlayMusicBufferQueue() xj 8");
    // CheckErr(res);

    res = (*bufferQueueItf)->Enqueue(bufferQueueItf, cntxt.pData, 4* AUDIO_DATA_BUFFER_SIZE);/* Size given in bytes */
    CheckErr(res,12);
    cntxt.pData += AUDIO_DATA_BUFFER_SIZE*2;

    res = (*bufferQueueItf)->Enqueue(bufferQueueItf, cntxt.pData, 4 * AUDIO_DATA_BUFFER_SIZE); /* Size given in bytes. */
    CheckErr(res,13);
    cntxt.pData += AUDIO_DATA_BUFFER_SIZE*2;

    res = (*bufferQueueItf)->Enqueue(bufferQueueItf, cntxt.pData, 4 * AUDIO_DATA_BUFFER_SIZE); /* Size given in bytes. */
    CheckErr(res,14);
    cntxt.pData += AUDIO_DATA_BUFFER_SIZE*2;

    //使用队列播放PCM示例
    res = (*playItf)->SetPlayState(playItf, SL_PLAYSTATE_PLAYING);
    CheckErr(res,15);

    res = (*bufferQueueItf)->GetState(bufferQueueItf, &state);
    CheckErr(res,16);

    while(state.count) {
        (*bufferQueueItf)->GetState(bufferQueueItf, &state);
    }

    res = (*playItf)->SetPlayState(playItf, SL_PLAYSTATE_STOPPED);
    CheckErr(res,17);

    (*player)->Destroy(player);

    (*OutputMix)->Destroy(OutputMix);
}

int main(int argc, char** argv) {
    SLresult res;
    SLObjectItf sl;
    SLEngineOption EngineOption[] = {
        (SLuint32) SL_ENGINEOPTION_THREADSAFE,
        (SLuint32) SL_BOOLEAN_TRUE};

    getPcmData();

    res = slCreateEngine(&sl, 1, EngineOption, 0, NULL, NULL);
    CheckErr(res,18);

    res = (*sl)->Realize(sl, SL_BOOLEAN_FALSE);
    CheckErr(res,19);
    TestPlayMusicBufferQueue(sl);

    //shutdown OpenSl
    (*sl)->Destroy(sl);
    if(file != NULL) {
       fclose(file); 
    }
    return 0;
}