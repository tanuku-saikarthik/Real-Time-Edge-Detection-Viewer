#include <jni.h>
#include <android/log.h>
#include <opencv2/opencv.hpp>
#include "edge_detector.h"
#include "gl_renderer.h"

#define LOG_TAG "native-lib"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

static EdgeDetector* gDetector = nullptr;
static GLRenderer* gRenderer = nullptr;

extern "C" JNIEXPORT void JNICALL
Java_com_example_edgedemo_MainActivity_setupCameraAndNative(JNIEnv *env, jobject thiz, jobject surfaceTexture, jint width, jint height) {
    if (!gDetector) {
        gDetector = new EdgeDetector();
    }
    gDetector->init(width, height);

    if (!gRenderer) {
        gRenderer = new GLRenderer();
    }
    gRenderer->init(width, height);

    LOGI("setupCameraAndNative %d x %d", width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_edgedemo_MainActivity_setShowEdgesNative(JNIEnv *env, jobject thiz, jint flag) {
    if (gDetector) gDetector->setShowEdges(flag != 0);
}

// pixels: int[] ARGB, width, height
extern "C" JNIEXPORT void JNICALL
Java_com_example_edgedemo_MainActivity_processFrameNative(JNIEnv *env, jobject thiz, jintArray pixels, jint width, jint height) {
    if (!pixels) return;
    jint *arr = env->GetIntArrayElements(pixels, nullptr);
    if (!arr) return;

    if (!gDetector || !gRenderer) {
        env->ReleaseIntArrayElements(pixels, arr, JNI_ABORT);
        return;
    }

    gDetector->processARGB((const int*)arr, width, height);

    env->ReleaseIntArrayElements(pixels, arr, JNI_ABORT);

    unsigned char* data = gDetector->getProcessedMatData();
    int w = gDetector->getWidth();
    int h = gDetector->getHeight();
    if (data && w > 0 && h > 0) {
        gRenderer->uploadProcessed(data, w, h);
    }
}

// GL hooks
extern "C" JNIEXPORT void JNICALL
Java_com_example_edgedemo_GLRenderer_nativeOnSurfaceCreated(JNIEnv *env, jobject thiz) {
    if (!gRenderer) gRenderer = new GLRenderer();
    gRenderer->onSurfaceCreated();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_edgedemo_GLRenderer_nativeOnSurfaceChanged(JNIEnv *env, jobject thiz, jint width, jint height) {
    if (!gRenderer) gRenderer = new GLRenderer();
    gRenderer->onSurfaceChanged(width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_edgedemo_GLRenderer_nativeOnDrawFrame(JNIEnv *env, jobject thiz) {
    if (gRenderer) gRenderer->onDrawFrame();
}
