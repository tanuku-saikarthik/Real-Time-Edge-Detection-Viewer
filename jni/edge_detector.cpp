#include "edge_detector.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <android/log.h>
#include <cstring>

#define LOG_TAG "EdgeDetector"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

using namespace cv;

EdgeDetector::EdgeDetector(): w(0), h(0), showEdges(true), buffer(nullptr) {}
EdgeDetector::~EdgeDetector() {
    if (buffer) delete[] buffer;
}

void EdgeDetector::init(int width, int height) {
    if (width <= 0 || height <= 0) return;
    w = width;
    h = height;
    if (buffer) delete[] buffer;
    buffer = new unsigned char[w * h];
    LOGI("EdgeDetector init %d x %d", w, h);
}

void EdgeDetector::setShowEdges(bool v) { showEdges = v; }

void EdgeDetector::processARGB(const int* pixels, int width, int height) {
    if (!pixels) return;
    if (width != w || height != h) {
        init(width, height);
    }
    if (w == 0 || h == 0 || !buffer) return;

    // Fill a BGRA Mat (OpenCV expects B G R A)
    Mat src(h, w, CV_8UC4);
    for (int y = 0; y < h; ++y) {
        Vec4b* row = src.ptr<Vec4b>(y);
        for (int x = 0; x < w; ++x) {
            int idx = y * w + x;
            int pix = pixels[idx];
            unsigned char A = (pix >> 24) & 0xFF;
            unsigned char R = (pix >> 16) & 0xFF;
            unsigned char G = (pix >> 8) & 0xFF;
            unsigned char B = (pix) & 0xFF;
            row[x][0] = B;
            row[x][1] = G;
            row[x][2] = R;
            row[x][3] = A;
        }
    }

    Mat gray;
    cvtColor(src, gray, COLOR_BGRA2GRAY);

    if (showEdges) {
        Mat edges;
        Canny(gray, edges, 80, 160);
        memcpy(buffer, edges.data, w * h);
    } else {
        memcpy(buffer, gray.data, w * h);
    }
}

unsigned char* EdgeDetector::getProcessedMatData() {
    return buffer;
}

int EdgeDetector::getWidth() { return w; }
int EdgeDetector::getHeight() { return h; }
