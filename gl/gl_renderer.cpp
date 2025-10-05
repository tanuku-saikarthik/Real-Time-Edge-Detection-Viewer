#include "gl_renderer.h"
#include <GLES2/gl2.h>
#include <android/log.h>
#include <cstring>
#include <cstdlib>

#define LOG_TAG "GLRenderer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

static const char* VERTEX_SHADER =
        "attribute vec4 aPosition;\n"
        "attribute vec2 aTexCoord;\n"
        "varying vec2 vTexCoord;\n"
        "void main() {\n"
        "    gl_Position = aPosition;\n"
        "    vTexCoord = aTexCoord;\n"
        "}\n";

static const char* FRAGMENT_SHADER =
        "precision mediump float;\n"
        "varying vec2 vTexCoord;\n"
        "uniform sampler2D uTexture;\n"
        "void main() {\n"
        "    float l = texture2D(uTexture, vTexCoord).r;\n"
        "    gl_FragColor = vec4(l,l,l,1.0);\n"
        "}\n";

static const GLfloat QUAD_VERTICES[] = {
    -1.f, -1.f,
     1.f, -1.f,
    -1.f,  1.f,
     1.f,  1.f
};

static const GLfloat QUAD_TEXCOORDS[] = {
    0.f, 1.f,
    1.f, 1.f,
    0.f, 0.f,
    1.f, 0.f
};

GLRenderer::GLRenderer(): texWidth(0), texHeight(0), textureId(0),
    pendingData(nullptr), pendingSize(0), pendingUpload(false),
    program(0), attribPos(-1), attribTex(-1), uniformTex(-1) {}

GLRenderer::~GLRenderer() {
    if (pendingData) delete[] pendingData;
    if (textureId) {
        GLuint t = textureId;
        glDeleteTextures(1, &t);
    }
    if (program) {
        glDeleteProgram(program);
    }
}

void GLRenderer::init(int w, int h) {
    texWidth = w;
    texHeight = h;
    pendingUpload = false;
    LOGI("GLRenderer init %d x %d", w, h);
}

void GLRenderer::createTextureIfNeeded(int w, int h) {
    if (textureId && texWidth == w && texHeight == h) return;

    if (textureId) {
        GLuint t = textureId;
        glDeleteTextures(1, &t);
        textureId = 0;
    }

    texWidth = w;
    texHeight = h;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, texWidth, texHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    LOGI("Created texture %u for %d x %d", textureId, texWidth, texHeight);
}

void GLRenderer::uploadProcessed(unsigned char* data, int w, int h) {
    if (!data || w <= 0 || h <= 0) return;
    int size = w * h;
    if (!pendingData) {
        pendingData = new unsigned char[size];
        pendingSize = size;
    } else if (pendingSize != size) {
        delete[] pendingData;
        pendingData = new unsigned char[size];
        pendingSize = size;
    }
    memcpy(pendingData, data, size);
    pendingUpload = true;
}

unsigned int GLRenderer::compileShader(unsigned int type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen) {
            char* buf = (char*)malloc(infoLen);
            glGetShaderInfoLog(shader, infoLen, nullptr, buf);
            LOGI("Shader compile error:\n%s", buf);
            free(buf);
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

unsigned int GLRenderer::linkProgram(unsigned int vsh, unsigned int fsh) {
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vsh);
    glAttachShader(prog, fsh);
    glBindAttribLocation(prog, 0, "aPosition");
    glBindAttribLocation(prog, 1, "aTexCoord");
    glLinkProgram(prog);
    GLint linked;
    glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint infoLen = 0;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen) {
            char* buf = (char*)malloc(infoLen);
            glGetProgramInfoLog(prog, infoLen, nullptr, buf);
            LOGI("Program link error:\n%s", buf);
            free(buf);
        }
        glDeleteProgram(prog);
        return 0;
    }
    return prog;
}

void GLRenderer::onSurfaceCreated() {
    LOGI("onSurfaceCreated");
    glClearColor(0.f, 0.f, 0.f, 1.f);

    unsigned int vsh = compileShader(GL_VERTEX_SHADER, VERTEX_SHADER);
    unsigned int fsh = compileShader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER);
    if (vsh && fsh) {
        program = linkProgram(vsh, fsh);
    }
    if (vsh) glDeleteShader(vsh);
    if (fsh) glDeleteShader(fsh);

    if (program) {
        attribPos = glGetAttribLocation(program, "aPosition");
        attribTex = glGetAttribLocation(program, "aTexCoord");
        uniformTex = glGetUniformLocation(program, "uTexture");
    }
}

void GLRenderer::onSurfaceChanged(int width, int height) {
    LOGI("onSurfaceChanged %d x %d", width, height);
    glViewport(0, 0, width, height);
}

void GLRenderer::onDrawFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (pendingUpload && pendingData && pendingSize > 0) {
        createTextureIfNeeded(texWidth, texHeight);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, texWidth, texHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pendingData);
        glBindTexture(GL_TEXTURE_2D, 0);
        pendingUpload = false;
    }

    if (program && textureId) {
        glUseProgram(program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glUniform1i(uniformTex, 0);

        glEnableVertexAttribArray((GLuint)attribPos);
        glEnableVertexAttribArray((GLuint)attribTex);

        glVertexAttribPointer((GLuint)attribPos, 2, GL_FLOAT, GL_FALSE, 0, QUAD_VERTICES);
        glVertexAttribPointer((GLuint)attribTex, 2, GL_FLOAT, GL_FALSE, 0, QUAD_TEXCOORDS);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glDisableVertexAttribArray((GLuint)attribPos);
        glDisableVertexAttribArray((GLuint)attribTex);
        glBindTexture(GL_TEXTURE_2D, 0);

        glUseProgram(0);
    }
}
