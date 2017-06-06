package gcalero.demo.lasersabervr;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;

public class PngManager
{
    public Bitmap open(String path, AssetManager amgr)
    {
        try
        {
            return BitmapFactory.decodeStream(amgr.open(path));
        }
        catch (Exception e) {
            Log.e("SKYBOX", "Could not decode " + path, e);
        }
        return null;
    }

    public int getWidth(Bitmap bmp) { return bmp.getWidth(); }
    public int getHeight(Bitmap bmp) { return bmp.getHeight(); }

    public int[] getBytes(Bitmap bmp)
    {
        int w = bmp.getWidth();
        int h = bmp.getHeight();
        int[] pixels = new int[w*h];
        bmp.getPixels(pixels, 0, w, 0, 0, w, h);
        return pixels;

    }

    public void close(Bitmap bmp)
    {
        bmp.recycle();
    }
}