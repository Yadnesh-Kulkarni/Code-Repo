package com.astromedicomp.FullScreenClass;

    import android.content.Context;
    import android.widget.TextView;
    import android.graphics.Color;
    import android.view.Gravity;
public class MyView extends TextView 
{
    MyView(Context context)
    {
        super(context);

        setTextSize(60);
        setTextColor(Color.rgb(0,255,0));
        setText("Hello World with Class!!!");
        setGravity(Gravity.CENTER);
    }
}