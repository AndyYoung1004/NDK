#include <jni.h>
#include <string>
#include <media/NdkMediaCodec.h>
#include <media/NdkMediaExtractor.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/log.h>
#include <pthread.h>
#include <unistd.h>
extern "C" {
#include "libavutil/avutil.h"
}

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "keymatch", __VA_ARGS__)

AMediaExtractor* mediaExtractor = NULL;
AMediaCodec* mediaCodec = NULL;

std::string jstringTostring(JNIEnv* env, jstring jstr) {
    char *rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("GB2312");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char *) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    std::string stemp(rtn);
    free(rtn);
    return stemp;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_testndk_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = av_version_info();
    LOGD("yangliu output:%s", hello.c_str());
    return env->NewStringUTF(hello.c_str());
}

void* playThread(void* args) {
    int inputIndex;
    for(;;) {
        inputIndex = AMediaCodec_dequeueInputBuffer(mediaCodec, 10000);
        if (inputIndex >= 0) {
            size_t bufSize;
            auto buf = AMediaCodec_getInputBuffer(mediaCodec, inputIndex, &bufSize);
            auto sampleSize = AMediaExtractor_readSampleData(mediaExtractor, buf, bufSize);
            if (sampleSize < 0) {
                break;
            }
            auto pts = AMediaExtractor_getSampleTime(mediaExtractor);
            AMediaCodec_queueInputBuffer(mediaCodec, inputIndex, 0, sampleSize, pts, 0);
            AMediaExtractor_advance(mediaExtractor);
        }
        AMediaCodecBufferInfo info;
        auto status = AMediaCodec_dequeueOutputBuffer(mediaCodec, &info, 0);
        if (status >= 0) {
            LOGD("yangliu pts:%d",info.presentationTimeUs/1000);
            AMediaCodec_releaseOutputBuffer(mediaCodec, status, info.size != 0);
            usleep(30);
        }
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_testndk_MainActivity_playVideo(
        JNIEnv* env,
        jobject /* this */, jstring path, jobject surface) {
    const char *filePath = jstringTostring(env, path).c_str();
    mediaExtractor = AMediaExtractor_new();
    ANativeWindow* aNativeWindow = ANativeWindow_fromSurface(env, surface);
    pthread_t thread;
    media_status_t err = AMediaExtractor_setDataSource(mediaExtractor, filePath);
    if (err != AMEDIA_OK) {
        LOGD("set data source err");
        return;
    }
    int numTracks = AMediaExtractor_getTrackCount(mediaExtractor);
    for (int i = 0; i < numTracks; i++) {
        AMediaFormat *format = AMediaExtractor_getTrackFormat(mediaExtractor, i);
        const char *s = AMediaFormat_toString(format);
        LOGD("HWCodecPlayer::InitDecoder track %d format: %s", i, s);
        const char *mime;
        if (!AMediaFormat_getString(format, AMEDIAFORMAT_KEY_MIME, &mime)) {
            LOGD("HWCodecPlayer::InitDecoder no mime type");
            return;
        } else if (!strncmp(mime, "video/", 6)) {
            AMediaExtractor_selectTrack(mediaExtractor, i);
            mediaCodec = AMediaCodec_createDecoderByType(mime);
            AMediaCodec_configure(mediaCodec, format, aNativeWindow, NULL, 0);
            AMediaCodec_start(mediaCodec);
        }
        AMediaFormat_delete(format);
    }
    pthread_create(&thread, NULL, playThread, NULL);
}