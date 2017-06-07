package gcalero.demo.lasersabervr;

import android.app.Activity;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.WindowManager;

import com.google.vr.ndk.base.AndroidCompat;
import com.google.vr.ndk.base.GvrLayout;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MainActivity extends Activity {

    private static final String TAG = "MainActivity";

    static {
        // Load our JNI code.
        System.loadLibrary("lasersaber_jni");
    }

    // Opaque native pointer to the DemoApp C++ object.
    // This object is owned by the MainActivity instance and passed to the native methods.
    private long nativeLaserSaber;

    private GvrLayout gvrLayout;
    private GLSurfaceView surfaceView;
    private AssetManager assetManager;

    // Note that pause and resume signals to the native app are performed on the GL thread, ensuring
    // thread-safety.
    private final Runnable pauseNativeRunnable =
            new Runnable() {
                @Override
                public void run() {
                    nativeOnPause(nativeLaserSaber);
                }
            };

    private final Runnable resumeNativeRunnable =
            new Runnable() {
                @Override
                public void run() {
                    nativeOnResume(nativeLaserSaber);
                }
            };


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        setImmersiveSticky();
        getWindow()
                .getDecorView()
                .setOnSystemUiVisibilityChangeListener(
                        new View.OnSystemUiVisibilityChangeListener() {
                            @Override
                            public void onSystemUiVisibilityChange(int visibility) {
                                if ((visibility & View.SYSTEM_UI_FLAG_FULLSCREEN) == 0) {
                                    setImmersiveSticky();
                                }
                            }
                        });

        // Enable VR mode, if the device supports it.
        AndroidCompat.setVrModeEnabled(this, true);

        // Get the GvrLayout.
        gvrLayout = new GvrLayout(this);

        // Enable async reprojection, if possible.
        if (gvrLayout.setAsyncReprojectionEnabled(true)) {
            Log.d(TAG, "Successfully enabled async reprojection.");
            // Async reprojection decouples the app framerate from the display framerate,
            // allowing immersive interaction even at the throttled clockrates set by
            // sustained performance mode.
            AndroidCompat.setSustainedPerformanceMode(this, true);
        } else {
            Log.w(TAG, "Failed to enable async reprojection.");
        }

        // Configure the GLSurfaceView.
        surfaceView = new GLSurfaceView(this);
        surfaceView.setEGLContextClientVersion(2);
        surfaceView.setEGLConfigChooser(8, 8, 8, 0, 0, 0);
        surfaceView.setRenderer(renderer);

        // Note that we are not setting setPreserveEGLContextOnPause(true) here,
        // even though it is recommended.  This is done so that we have at least
        // one demo that provides some testing coverage for no-preserve contexts.

        // Set the GLSurfaceView as the GvrLayout's presentation view.
        gvrLayout.setPresentationView(surfaceView);

        // Add the GvrLayout to the View hierarchy.
        setContentView(gvrLayout);

        assetManager = getResources().getAssets();
        nativeLaserSaber =
                nativeOnCreate(assetManager, gvrLayout.getGvrApi().getNativeGvrContext());

        // Prevent screen from dimming/locking.
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        // Destruction order is important; shutting down the GvrLayout will detach
        // the GLSurfaceView and stop the GL thread, allowing safe shutdown of
        // native resources from the UI thread.
        gvrLayout.shutdown();
        nativeOnDestroy(nativeLaserSaber);
        nativeLaserSaber = 0;
    }

    @Override
    protected void onPause() {
        surfaceView.queueEvent(pauseNativeRunnable);
        surfaceView.onPause();
        gvrLayout.onPause();
        super.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        gvrLayout.onResume();
        surfaceView.onResume();
        surfaceView.queueEvent(resumeNativeRunnable);
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus) {
            setImmersiveSticky();
        }
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        // Avoid accidental volume key presses while the phone is in the VR headset.
        if (event.getKeyCode() == KeyEvent.KEYCODE_VOLUME_UP
                || event.getKeyCode() == KeyEvent.KEYCODE_VOLUME_DOWN) {
            return true;
        }
        return super.dispatchKeyEvent(event);
    }

    private void setImmersiveSticky() {
        getWindow()
                .getDecorView()
                .setSystemUiVisibility(
                        View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                                | View.SYSTEM_UI_FLAG_FULLSCREEN
                                | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
    }

    private final GLSurfaceView.Renderer renderer =
            new GLSurfaceView.Renderer() {
                @Override
                public void onSurfaceCreated(GL10 gl, EGLConfig config) {
                    loadSkyboxImages();
                    nativeOnSurfaceCreated(nativeLaserSaber);
                }

                @Override
                public void onSurfaceChanged(GL10 gl, int width, int height) {
                    nativeOnSurfaceChanged(width, height, nativeLaserSaber);
                }

                @Override
                public void onDrawFrame(GL10 gl) {
                    nativeOnDrawFrame(nativeLaserSaber);
                }
            };


    private void loadSkyboxImages() {
        String[] faces =  {
            "skybox/skybox_right.png", "skybox/skybox_left.png",
            "skybox/skybox_up.png", "skybox/skybox_down.png",
            "skybox/skybox_back.png", "skybox/skybox_front.png"
        };

        PngManager pngManager = new PngManager();
        AssetManager assetManager = getAssets();
        int pos = 0;
        for (String f: faces) {
            Bitmap bitmap = pngManager.open(f, assetManager);
            int width = pngManager.getWidth(bitmap);
            int height = pngManager.getHeight(bitmap);
            int[] bytes= pngManager.getBytes(bitmap);
            for(int i=0;i<bytes.length;i++) {
                // original format is ARGB
                int b = (bytes[i] & 0x000000ff);
                int g = (bytes[i] & 0x0000ff00) >> 8;
                int r = (bytes[i] & 0x00ff0000) >> 16;
                int a = (bytes[i] & 0xff000000) >>> 24;
                // and I build ABGR
                 bytes[i] = a << 24 | b << 16 | g << 8 | r;
            }
            nativeOnSkyboxImageReady(nativeLaserSaber, width, height, pos++, bytes);
        }
    }

    private native long nativeOnCreate(AssetManager assetManager, long gvrContextPtr);
    private native void nativeOnDestroy(long laserSaberJptr);
    private native void nativeOnResume(long laserSaberJptr);
    private native void nativeOnPause(long laserSaberJptr);
    private native void nativeOnSurfaceCreated(long laserSaberJptr);
    private native void nativeOnSurfaceChanged(int width, int height, long laserSaberJptr);
    private native void nativeOnDrawFrame(long controllerPaintJptr);
    private native void nativeOnSkyboxImageReady(long laser_saber_jptr, int width, int height, int skyboxPos, int[] img);
}
