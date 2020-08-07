package com.astromedicomp.FullScreenTemp;

        import android.app.Activity;
        import android.os.Bundle;
        import android.view.Window;
        import android.view.WindowManager;
        import android.content.pm.ActivityInfo;
        import android.graphics.Color;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,WindowManager.LayoutParams.FLAG_FULLSCREEN);
        this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        getWindow().getDecorView().setBackgroundColor(Color.rgb(0,0,0));

    }
}
