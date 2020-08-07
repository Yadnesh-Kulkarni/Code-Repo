package com.astromedicomp.FullScreen;

        import android.app.Activity;
        import android.os.Bundle;
        import android.view.Window;
        import android.view.WindowManager;
        import android.content.pm.ActivityInfo;
        import android.widget.TextView;
        import android.graphics.Color;
        import android.view.Gravity;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,WindowManager.LayoutParams.FLAG_FULLSCREEN);
        this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        getWindow().getDecorView().setBackgroundColor(Color.rgb(0,0,0));
        TextView myTextView = new TextView(this);
        myTextView.setTextSize(60);
        myTextView.setTextColor(Color.GREEN);
        myTextView.setGravity(Gravity.CENTER);
        myTextView.setText("Hello World");
        setContentView(myTextView);
    }
}
