#ifndef LASER_SABER_APP_H_  // NOLINT
#define LASER_SABER_APP_H_

#include <android/asset_manager.h>
#include <GLES3/gl31.h>
#include <jni.h>

#include <array>
#include <chrono>  // NOLINT
#include <memory>
#include <vector>

#include "vr/gvr/capi/include/gvr.h"
#include "vr/gvr/capi/include/gvr_controller.h"

#include "skybox.h"

#define NEAR_CLIP 0.08f
#define FAR_CLIP 16384.0f

class LaserSaberApp {
public:

    // Initializes the demo app.
    // |asset_manager| is the Android Asset Manager obtained from Java.
    // |gvr_context_ptr| a jlong representing a pointer to the GVR context
    //     obtained from Java.
    LaserSaberApp(JNIEnv* env, jobject asset_manager, jlong gvr_context_ptr);
    ~LaserSaberApp();
    // Must be called when the Activity gets onResume().
    // Must be called on the UI thread.
    void OnResume();
    // Must be called when the Activity gets onPause().
    // Must be called on the UI thread.
    void OnPause();
    // Must be called when the GL renderer gets onSurfaceCreated().
    // Must be called on the rendering thread.
    void OnSurfaceCreated();
    // Must be called when the GL renderer gets onSurfaceChanged().
    // Must be called on the rendering thread.
    void OnSurfaceChanged(int width, int height);
    // Must be called when the GL renderer gets onDrawFrame().
    // Must be called on the rendering thread.
    void OnDrawFrame();

    void OnSkyboxImageReady(int width, int height, jint pos, u_char * data);

private:

    // Prepares the GvrApi framebuffer for rendering, resizing if needed.
    void PrepareFramebuffer();

    // Clears the whole drawing.
    void ClearDrawing();

    // Gvr API entry point.
    gvr_context* gvr_context_;
    std::unique_ptr<gvr::GvrApi> gvr_api_;
    bool gvr_api_initialized_;

    Skybox skybox_;
    // Controller API entry point.
    std::unique_ptr<gvr::ControllerApi> controller_api_;

    // Handle to the swapchain. On every frame, we have to check if the buffers
    // are still the right size for the frame (since they can be resized at any
    // time). This is done by PrepareFramebuffer().
    std::unique_ptr<gvr::SwapChain> swapchain_;

    // List of rendering params (used to render each eye).
    gvr::BufferViewportList viewport_list_;
    gvr::BufferViewport scratch_viewport_;

    // Size of the offscreen framebuffer.
    gvr::Sizei framebuf_size_;

    // The shader we use to render our geometry. Since this is a very simple
    // demo, we use only one shader.
    //int shader_;

    // Uniform/attrib locations in the shader. These are looked up after we
    // compile/link the shader.
    /*int shader_u_color_;
    int shader_u_mvp_matrix_;  // Model-view-projection matrix.
    int shader_u_sampler_;
    int shader_a_position_;
    int shader_a_texcoords_;*/

    // Ground texture.
    //int ground_texture_;

    // Paint texture. This is the texture we use for painting.
    //int paint_texture_;

    // Android asset manager (we use it to load the texture).
    AAssetManager* asset_mgr_;

    // The last controller state (updated once per frame).
    gvr::ControllerState controller_state_;

    // The vertex and texture coordinates representing recently painted geometry.
    // As this array grows beyond a certain limit, we commit that geometry
    // to a VBO for performance. This is formatted for rendering, with
    // each group of 5 floats meaning vx, vy, vz, s, t, where (vx, vy, vz) are the
    // vertex coordinates in world space and s,t are the texture coordinates.
    std::vector<float> recent_geom_;

    // Count of vertices in recent_geom_.
    int recent_geom_vertex_count_;

    // Total vertices in the current brush stroke (the brush
    // stroke starts when the user first touches the touchpad and continues
    // until they release it).
    int brush_stroke_total_vertices_;

    // Currently selected color (index).
    int selected_color_;

    // If true, we are currently painting.
    bool painting_;

    // If painting_ == true, then this is the position where the last
    // paint segment ended (or the position where painting began, if no
    // segments were added yet).
    std::array<float, 3> paint_anchor_;

    // Indicates whether we have continuation points to continue the shape
    // from (for smooth drawing).
    bool has_continuation_;

    // If has_continuation_ == true, these are the continuation points.
    std::array<std::array<float, 3>, 2> continuation_points_;

    // This is the list of committed VBOs that contains the static parts
    // of the current drawing. As the drawing accumulates in painted_geom_,
    // we push it to a static VBO on the GPU for performance.
    struct VboInfo {
        GLuint vbo;
        int vertex_count;
        int color;
    };
    std::vector<VboInfo> committed_vbos_;

    // Touchpad coordinates where touch started. We use this to detect
    // the swipe gestures that cause the drawing color to change.
    float touch_down_x_;
    float touch_down_y_;

    // If true, a color switch already happened during this touch cycle.
    bool switched_color_;

    // Width of the painting stroke.
    float stroke_width_;

    // Width of the painting stroke when the user last started touching the
    // touchpad.
    float touch_down_stroke_width_;

};


#endif