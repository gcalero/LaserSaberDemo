/*
 * Copyright 2017 Google Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "app_jni.h"  // NOLINT

#include <memory>

#include "lasersaberapp.h"
#include "utils.h"  // NOLINT

namespace {
inline jlong jptr(LaserSaberApp* ptr) { return reinterpret_cast<intptr_t>(ptr); }

inline LaserSaberApp* ptr(jlong jptr) { return reinterpret_cast<LaserSaberApp*>(jptr); }

}  // namespace

NATIVE_METHOD(jlong, nativeOnCreate)
(JNIEnv* env, jobject obj, jobject asset_mgr, jlong gvr_context_ptr) {
  return jptr(new LaserSaberApp(env, asset_mgr, gvr_context_ptr));
}

NATIVE_METHOD(void, nativeOnResume)
(JNIEnv* env, jobject obj, jlong laser_saber_jptr) {
  ptr(laser_saber_jptr)->OnResume();
}

NATIVE_METHOD(void, nativeOnPause)
(JNIEnv* env, jobject obj, jlong laser_saber_jptr) {
  ptr(laser_saber_jptr)->OnPause();
}

  NATIVE_METHOD(void, nativeOnSurfaceCreated)
(JNIEnv* env, jobject obj, jlong laser_saber_jptr) {
  ptr(laser_saber_jptr)->OnSurfaceCreated();
}

NATIVE_METHOD(void, nativeOnSurfaceChanged)
(JNIEnv* env, jobject obj, jint width, jint height,
 jlong laser_saber_jptr) {
  ptr(laser_saber_jptr)
      ->OnSurfaceChanged(static_cast<int>(width), static_cast<int>(height));
}

NATIVE_METHOD(void, nativeOnDrawFrame)
(JNIEnv* env, jobject obj, jlong laser_saber_jptr) {
  ptr(laser_saber_jptr)->OnDrawFrame();
}

NATIVE_METHOD(void, nativeOnDestroy)
(JNIEnv* env, jobject obj, jlong laser_saber_jptr) {
  delete ptr(laser_saber_jptr);
}

NATIVE_METHOD(void, nativeOnSkyboxImageReady)
(JNIEnv* env, jobject obj, jlong laser_saber_jptr, jint width, jint height, jint skybox_pos, jintArray img) {
  jboolean isCopy;
  jint *cData = env->GetIntArrayElements(img, &isCopy);
  ptr(laser_saber_jptr)->OnSkyboxImageReady(width, height, skybox_pos, (u_char *) cData);
  env->ReleaseIntArrayElements(img, cData, JNI_ABORT);
}

