#ifndef EDGE_DETECTOR_H
#define EDGE_DETECTOR_H

class EdgeDetector {
public:
    EdgeDetector();
    ~EdgeDetector();
    void init(int width, int height);
    void processARGB(const int* pixels, int width, int height); // Android ARGB int array
    void setShowEdges(bool v);
    unsigned char* getProcessedMatData();
    int getWidth();
    int getHeight();
private:
    int w, h;
    bool showEdges;
    unsigned char* buffer;
};

#endif // EDGE_DETECTOR_H
