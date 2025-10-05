#ifndef GL_RENDERER_H
#define GL_RENDERER_H

class GLRenderer {
public:
    GLRenderer();
    ~GLRenderer();
    void init(int w, int h);

    void uploadProcessed(unsigned char* data, int w, int h);

    void onSurfaceCreated();
    void onSurfaceChanged(int width, int height);
    void onDrawFrame();
private:
    int texWidth;
    int texHeight;
    unsigned int textureId;
    unsigned char* pendingData;
    int pendingSize;
    bool pendingUpload;

    unsigned int program;
    int attribPos;
    int attribTex;
    int uniformTex;

    void createTextureIfNeeded(int w, int h);
    unsigned int compileShader(unsigned int type, const char* source);
    unsigned int linkProgram(unsigned int vsh, unsigned int fsh);
};

#endif // GL_RENDERER_H
