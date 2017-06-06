//
// Created by Gabriel Calero on 5/29/17.
//

#include <android/asset_manager.h>
#include <GLES3/gl31.h>
#include <glm/glm.hpp>
#include "jni.h"
#include "vr/gvr/capi/include/gvr.h"
#ifndef LASERSABERVR_SKYBOX_H
#define LASERSABERVR_SKYBOX_H

class Skybox {

public:
    Skybox(_JNIEnv *jEnv, AAssetManager *asset_mgr);
    //~Skybox();
    void init();
    void draw(gvr::Eye which_eye,
              glm::mat4 projection,
              glm::mat4 view,
              const gvr::BufferViewport& viewport,
            gvr::Sizei framebuf_size);
    AAssetManager* asset_mgr_;
    jobject j_png_mgr;

    void addImage(int width, int height, jint pos, u_char *image);

private:
    const char * TAG = "SKYBOX";

    char * loadShader(const char * filename);
    GLuint initCubemapTexture();

    struct Image {
        int width;
        int height;
        u_char *data;
    };

    int loadedParts;

    Image parts[6];

    int shader_;
    JNIEnv *jEnv_;
    int viewLocation_;
    int projectionLocation_;
    int skyboxTextureLocation_;

    GLuint skyboxVAO_, skyboxVBO_;

    GLuint cubemapTexture_;

    const GLfloat skyboxVertices_[6*6*3] = {
            // Positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };

};

#endif //LASERSABERVR_SKYBOX_H
