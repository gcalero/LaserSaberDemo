//
// Created by Gabriel Calero on 5/29/17.
//

#include <glm/detail/type_mat4x4.hpp>
#include "skybox.h"
#include "utils.h"
#include "glm/gtc/type_ptr.hpp"



Skybox::Skybox(_JNIEnv *jEnv, AAssetManager *asset_mgr) :
    jEnv_(jEnv),
    asset_mgr_(asset_mgr),
    shader_(0),
    viewLocation_(0),
    projectionLocation_(0),
    loadedParts(0) {
};

void Skybox::init() {
    int vp = Utils::BuildShader(GL_VERTEX_SHADER, loadShader("shaders/skybox.vs"));
    int fp = Utils::BuildShader(GL_FRAGMENT_SHADER, loadShader("shaders/skybox.fs"));
    shader_ = Utils::BuildProgram(vp, fp);
    viewLocation_ = glGetUniformLocation(shader_, "view");
    projectionLocation_ = glGetUniformLocation(shader_, "projection");
    skyboxTextureLocation_ =glGetUniformLocation(shader_, "skybox");


    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        LOGE("*** GL CHECK FAILED at %s:%d: with error %x", __FILE__, __LINE__, error);
        abort();
    }


    glGenVertexArrays(1, &skyboxVAO_);
    glGenBuffers(1, &skyboxVBO_);
    glBindVertexArray(skyboxVAO_);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices_), &skyboxVertices_, GL_STATIC_DRAW);

    error = glGetError();
    if (error != GL_NO_ERROR) {
        LOGE("*** GL CHECK FAILED at %s:%d: with error %x", __FILE__, __LINE__, error);
        abort();
    }

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);

    error = glGetError();
    if (error != GL_NO_ERROR) {
        LOGE("*** GL CHECK FAILED at %s:%d: with error %x", __FILE__, __LINE__, error);
        abort();
    }

    if (loadedParts == 6) {
        cubemapTexture_ = initCubemapTexture();
    } else {
        LOGE("SKYBOX could not init textures. %d loaded", loadedParts);
    }

    error = glGetError();
    if (error != GL_NO_ERROR) {
        LOGE("*** GL CHECK FAILED at %s:%d: with error %x", __FILE__, __LINE__, error);
        abort();
    }

}

void Skybox::draw(gvr::Eye which_eye,
                  glm::mat4 projection,
                  glm::mat4 view,
                  const gvr::BufferViewport& viewport,
                  gvr::Sizei framebuf_size) {

    Utils::SetUpViewportAndScissor(framebuf_size, viewport);

    glUseProgram(shader_);
    glActiveTexture(GL_TEXTURE_CUBE_MAP);

    glUniformMatrix4fv(viewLocation_, 1, GL_FALSE, glm::value_ptr(view) );
    glUniformMatrix4fv(projectionLocation_, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(skyboxVAO_);
    CHECK(glGetError() == GL_NO_ERROR);

    //LOGD("bind GL_TEXTURE_CUBE_MAP %d", cubemapTexture_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture_);
    CHECK(glGetError() == GL_NO_ERROR);
    //LOGD("bind GL_TEXTURE_CUBE_MAP done");

    //glUniform1f(skyboxTextureLocation_, 0);
    //CHECK(glGetError() == GL_NO_ERROR);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    CHECK(glGetError() == GL_NO_ERROR);
    glBindVertexArray(0);
    CHECK(glGetError() == GL_NO_ERROR);
}

char * Skybox::loadShader(const char * filename) {
    CHECK(asset_mgr_);
    AAsset* asset = AAssetManager_open(asset_mgr_, filename, AASSET_MODE_UNKNOWN);
    if (NULL == asset) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "_ASSET_NOT_FOUND_");
        return JNI_FALSE;
    }
    long size = AAsset_getLength(asset);

    char* buffer = (char*) malloc (sizeof(char)*size);
    AAsset_read (asset,buffer,size);
    //__android_log_print(ANDROID_LOG_ERROR, TAG, buffer);
    AAsset_close(asset);
    return buffer;
}

GLuint Skybox::initCubemapTexture() {
    GLuint textureID;
    LOGD("initCubemapTexture");
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &textureID);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        LOGE("*** GL CHECK FAILED at %s:%d: with error %x", __FILE__, __LINE__, error);
        abort();
    }

    int width,height;

    LOGD("initCubemapTexture glBindTexture");
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    CHECK(glGetError() == GL_NO_ERROR);
    for(GLuint i = 0; i < loadedParts; i++)
    {
        //image = parts[i].data;
        width = parts[i].width;
        height = parts[i].height;
        glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &parts[i].data[0]
        );
        CHECK(glGetError() == GL_NO_ERROR);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    error = glGetError();
    if (error != GL_NO_ERROR) {
        LOGE("*** GL CHECK FAILED at %s:%d: with error %x", __FILE__, __LINE__, error);
        abort();
    }

    //glUniform1i(skyboxTextureLocation_, textureID);
    error = glGetError();
    if (error != GL_NO_ERROR) {
        LOGE("*** GL CHECK FAILED at %s:%d: with error %x", __FILE__, __LINE__, error);
        abort();
    }
    //glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return textureID;
}

void Skybox::addImage(int width, int height, jint pos, u_char *image) {
    CHECK(pos >= 0 && pos < 6);
    if (parts[pos].height == 0 && parts[pos].width == 0) loadedParts++;
    parts[pos].width = width;
    parts[pos].height = height;
    parts[pos].data = image;
    LOGD("add skybox image %d ", pos);
}







