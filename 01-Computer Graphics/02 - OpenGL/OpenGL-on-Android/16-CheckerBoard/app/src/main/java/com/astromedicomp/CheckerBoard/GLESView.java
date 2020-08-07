package com.astromedicomp.CheckerBoard;

import android.content.Context; // for drawing context related
import android.opengl.GLSurfaceView; // for OpenGL Surface View and all related
import javax.microedition.khronos.opengles.GL10; // for OpenGLES 1.0 needed as param type GL10
import javax.microedition.khronos.egl.EGLConfig; // for EGLConfig needed as param type EGLConfig
import android.opengl.GLES32; // for OpenGLES 3.0
import android.view.MotionEvent; // for "MotionEvent"
import android.view.GestureDetector; // for GestureDetector
import android.view.GestureDetector.OnGestureListener; // OnGestureListener
import android.view.GestureDetector.OnDoubleTapListener; // for OnDoubleTapListener

// for vbo
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import android.opengl.Matrix; // for Matrix math

import android.graphics.Bitmap; // for PNG image
import android.opengl.GLUtils; // for texImage2D()

// A view for OpenGLES3 graphics which also receives touch events
public class GLESView extends GLSurfaceView implements
GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
    private final Context context;
    
    private GestureDetector gestureDetector;
    
    private int vertexShaderObject;
    private int fragmentShaderObject;
    private int shaderProgramObject;
    
    private int[] vao = new int[1];
    private int[] vbo_position = new int[1];
    private int[] vbo_texture = new int[1];
    
    private int mvpUniform;
    private int texture0_sampler_uniform;
    
    private int[] texture_checkerboard = new int[1];

    private int checkImageWidth = 64;
    private int checkImageHeight = 64;
    
    int[] checkImage = new int[checkImageHeight * checkImageWidth];
    
    private float perspectiveProjectionMatrix[] = new float[16]; // 4x4 matrix
    
    public GLESView(Context drawingContext)
    {
        super(drawingContext);
        
        context = drawingContext;
        
        // accordingly set EGLContext to current supported version of OpenGL-ES
        setEGLContextClientVersion(3); // version negotiation this is NDK call
        
        // set Renderer for drawing on the GLSurfaceView
        setRenderer(this); // this call elss to call OnSurfaceCreated()
        
        // Render the view only when there is a chnage in the drawing data
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        
        gestureDetector = new GestureDetector(context, this, null, false); // this means 'handler' i.e. who is going to handle
        gestureDetector.setOnDoubleTapListener(this); // this means 'handler' i.e. who is going to handle
    }
    
    // overriden method of GLSurfaceView.Renderer (Init code)
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config)
    {
        // get OpenGL-ES version
        String glesVersion = gl.glGetString(GL10.GL_VERSION);
        System.out.println("YDK: OpenGL-ES Version = "+glesVersion);
        // get GLSL version
        String glslVersion = gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION);
        System.out.println("YDK: GLSL Version = "+glslVersion);
        
        initialize(gl);
    }
    
    // overriden method of GLSurfaceView.Renderer (Change Size code)
    @Override
    public void onSurfaceChanged(GL10 unused, int width, int height)
    {
        resize(width, height);
    }
    
    // Overriden method of GLSurfaceView.Renderer (Rendering Code)
    @Override
    public void onDrawFrame(GL10 unused)
    {
        display();
    }
    
    // Handling 'onTouchEvent' Is The Most IMPORTANT,
    // Because It Triggers All Gesture And Tap Events
    @Override
    public boolean onTouchEvent(MotionEvent e)
    {
        // code
        int eventaction = e.getAction();
        if (!gestureDetector.onTouchEvent(e))
            super.onTouchEvent(e);
        return(true);
    }
    
    // abstract method from onDoubleTapListener so must be implemented
    @Override
    public boolean onDoubleTap(MotionEvent e)
    {
        return(true);
    }
    
    // abstract method from onDoubleTapListener so must be implemented
    @Override
    public boolean onDoubleTapEvent(MotionEvent e)
    {
        // Do not Write Any code Here Because Already Written 'onDoubleTap'
        return(true);
    }
    
    // abstract method from onDoubleTapListener so must be implemented
    @Override
    public boolean onSingleTapConfirmed(MotionEvent e)
    {
        return(true);
    }
    
    // abstract method from onGestureListener so must be implemented
    @Override
    public boolean onDown(MotionEvent e)
    {
        // Do not Write Any code Here Because Already Written 'onSingleTapConfirmed'
        return(true);
    }
    
    // abstract method from onGestureListener so must be implemented
    @Override
    public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY)
    {
        return(true);
    }
    
    // abstract method from onGestureListener so must be implemented
    @Override
    public void onLongPress(MotionEvent e)
    {
    }
    
    // abstract method from onGestureListener so must be implemented
    @Override
    public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
    {
        uninitialize();
        System.exit(0);
        return(true);
    }
    
    // abstract method from onGestureListener so must be implemented
    @Override
    public void onShowPress(MotionEvent e)
    {
    }
    
    // abstract method from onGestureListener so must be implemented
    @Override
    public boolean onSingleTapUp(MotionEvent e)
    {
        return(true);
    }
    
    private void initialize(GL10 gl)
    {
        // *************************************
        // Vertex Shader
        // *************************************
        // create shader
        vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
        
        // vertex shader source code
        final String vertexShaderSourceCode = String.format
        (
        "#version 300 es"+
        "\n"+
        "in vec4 vPosition;"+
        "in vec2 vTexture0_Coord;"+
        "out vec2 out_texture0_coord;"+
        "uniform mat4 u_mvp_matrix;"+
        "void main(void)"+
        "{"+
        "gl_Position = u_mvp_matrix * vPosition;"+
        "out_texture0_coord = vTexture0_Coord;"+
        "}"
        );
        
        // provide source code to shader
        GLES32.glShaderSource(vertexShaderObject, vertexShaderSourceCode);
        
        // compile shader & check for errors
        GLES32.glCompileShader(vertexShaderObject);
        int[] iShaderCompiledStatus = new int[1];
        int[] iInfoLogLength = new int[1];
        String szInfoLog = null;
        GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0); // last 0 tells that start from 0 index
        if (iShaderCompiledStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if (iInfoLogLength[0] > 0)
            {
                szInfoLog = GLES32.glGetShaderInfoLog(vertexShaderObject);
                System.out.println("YDK: Vertex Shader Compilation Log = "+szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }
        
        // *************************************
        // Fragment Shader
        // *************************************
        // create shader
        fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
        
        // fragment shader source code
        final String fragmentShaderSourceCode = String.format
        (
        "#version 300 es"+
        "\n"+
        "precision highp float;"+
        "in vec2 out_texture0_coord;"+
        "uniform highp sampler2D u_texture0_sampler;"+
        "out vec4 FragColor;"+
        "void main(void)"+
        "{"+
        "FragColor = texture(u_texture0_sampler, out_texture0_coord);"+
        "}"
        );
        
        // provide source code to shader
        GLES32.glShaderSource(fragmentShaderObject, fragmentShaderSourceCode);
        
        // compile shader & check for errors
        GLES32.glCompileShader(fragmentShaderObject);
        iShaderCompiledStatus[0] = 0; // re-initialize
        iInfoLogLength[0] = 0; // re-initialize
        szInfoLog = null; // re-initialize
        GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompiledStatus, 0);
        if (iShaderCompiledStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if (iInfoLogLength[0] > 0)
            {
                szInfoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject);
                System.out.println("YDK: Fragment Shader Compilation Log = "+szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }
        
        // create shader program
        shaderProgramObject = GLES32.glCreateProgram();
        
        // attach vertex shader to shader program
        GLES32.glAttachShader(shaderProgramObject, vertexShaderObject);
        
        // attach fragment shader to shader program
        GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);
        
        // pre-link binding of shader program object with vertex shader position attributes
        GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.YDK_ATTRIBUTE_VERTEX, "vPosition");
        // pre-link binding of shader program object with vertex shader texture attributes
        GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.YDK_ATTRIBUTE_TEXTURE0, "vTexture0_Coord");
        
        // link the two shaders together to shader program object
        GLES32.glLinkProgram(shaderProgramObject);
        int[] iShaderProgramLinkStatus = new int[1];
        iInfoLogLength[0] = 0; // re-initialize
        szInfoLog = null; // re-initialize
        GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_LINK_STATUS, iShaderProgramLinkStatus, 0);
        if (iShaderProgramLinkStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if (iInfoLogLength[0] > 0)
            {
                szInfoLog = GLES32.glGetProgramInfoLog(shaderProgramObject);
                System.out.println("YDK: Shader Program Link Log = "+szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }
        
        // get MVP uniform location
        mvpUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_mvp_matrix");
        
        // get texture sampler uniform location
        texture0_sampler_uniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_texture0_sampler");
        
        loadGLTexture();
        
        // *** vertices, colors, shader attribs, vbo, vao, initializations ***
        final float squareTexcoords[] = new float[]
        {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
        };
        
        float squareVertices[] = new float[12];
        
        // vao
        GLES32.glGenVertexArrays(1, vao, 0);
        GLES32.glBindVertexArray(vao[0]);
        
        // vbo_position
        GLES32.glGenBuffers(1, vbo_position, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);
        
        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(squareVertices.length * 4);
        FloatBuffer verticesBuffer = byteBuffer.asFloatBuffer();
        verticesBuffer.put(0);
        verticesBuffer.position(0); // start from 0 index
        
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, verticesBuffer, GLES32.GL_DYNAMIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.YDK_ATTRIBUTE_VERTEX, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.YDK_ATTRIBUTE_VERTEX);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0); // vbo_position unbind
        
        // vbo_texture
        GLES32.glGenBuffers(1, vbo_texture, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_texture[0]);
        
        byteBuffer = ByteBuffer.allocateDirect(squareTexcoords.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer texcoordsBuffer = byteBuffer.asFloatBuffer();
        texcoordsBuffer.put(squareTexcoords);
        texcoordsBuffer.position(0); // start from 0 index
        
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, squareTexcoords.length * 4, texcoordsBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.YDK_ATTRIBUTE_TEXTURE0, 2, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.YDK_ATTRIBUTE_TEXTURE0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0); // vbo_position unbind
        
        GLES32.glBindVertexArray(0); // vao unbind
        
        // enable depth testing
        GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        // depth test to do
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);
        // We will always cull back faces for better performance
        //GLES32.glEnable(GLES32.GL_CULL_FACE);
        
        // Set the background frame color
        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // black
        
        // set projectionMatrix to identity matrix
        Matrix.setIdentityM(perspectiveProjectionMatrix, 0);
    }
    
    private void makeCheckImage()
    {
        // code
        int i, j, c;
        
        for (i = 0; i < checkImageHeight; i++)
        {
            for (j = 0; j < checkImageWidth; j++)
            {
                boolean x = (((i & 0x8) == 0) ^ ((j & 0x8) == 0));
                
                if (x == true)
                {
                    c = 255;
                }
                else
                {
                    c = 0;
                }
                
                int r = c;
                int g = c;
                int b = c;
                int a = 255;
                checkImage[i * checkImageWidth + j] = (a << 24) | (r << 16) | (g << 8) | b;
            }
        }
    }
    
    private void loadGLTexture()
    {
        makeCheckImage();
        
        Bitmap bitmap = Bitmap.createBitmap(checkImage, 0, checkImageWidth, checkImageWidth, checkImageHeight, Bitmap.Config.ARGB_8888);
        
        // create a texture object to apply to model
        GLES32.glGenTextures(1, texture_checkerboard, 0);
        
        // indicate that pixel rows are tightly packed(defaults to stride of 4 which is kind of only good for RGBA or FLOAT dta types)
        GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 1);
        
        // bind with the texture
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture_checkerboard[0]);
        
        // set up filter and wrap modes for this texture object
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_S, GLES32.GL_REPEAT);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_T, GLES32.GL_REPEAT);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_NEAREST);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_NEAREST);
        
        // load the bitmap into the bound texture
        GLUtils.texImage2D(GLES32.GL_TEXTURE_2D, 0, bitmap, 0);
        
        // generate mipmap
        GLES32.glGenerateMipmap(GLES32.GL_TEXTURE_2D);
    }
    
    private void resize(int width, int height)
    {
        // Adjust the viewport based on geometry changes,
        // such as screen rotation
        if (height == 0)
            height = 1;
        GLES32.glViewport(0, 0, width, height);
        
        Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 60.0f, (float)width/(float)height, 1.0f, 30.0f);
    }
    
    public void display()
    {
        // code
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
        
        // use shader program
        GLES32.glUseProgram(shaderProgramObject);
        
        // OpenGL-ES drawing
        float modelViewMatrix[] = new float[16];
        float modelViewProjectionMatrix[] = new float[16];
        
        // ***** SQUARE *****
        // set modelview & modelviewprojection matrices to identity
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);
        
        Matrix.translateM(modelViewMatrix, 0, 0.0f, 0.0f, -3.6f);
        
        // multiply the modelview and projection matrix to get modelviewprojection matrix
        Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);
        
        // pass above modelviewprojection matrix to the vertex shader in 'u_mvp_matrix' shader variable
        // whose position value we already calculated in initWithFrame() by using glGetUniformLocation()
        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);
        
        // bind vao
        GLES32.glBindVertexArray(vao[0]);
        
        float squareVertices[] = new float[12];
        
        squareVertices[0] = -2.0f;
        squareVertices[1] = -1.0f;
        squareVertices[2] = 0.0f;
        squareVertices[3] = -2.0f;
        squareVertices[4] = 1.0f;
        squareVertices[5] = 0.0f;
        squareVertices[6] = 0.0f;
        squareVertices[7] = 1.0f;
        squareVertices[8] = 0.0f;
        squareVertices[9] = 0.0f;
        squareVertices[10] = -1.0f;
        squareVertices[11] = 0.0f;
        
        // bind with vbo_position
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);
        
        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(squareVertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer verticesBuffers = byteBuffer.asFloatBuffer();
        verticesBuffers.put(squareVertices);
        verticesBuffers.position(0); // start from 0 index
        
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, squareVertices.length * 4, verticesBuffers, GLES32.GL_DYNAMIC_DRAW);
        // unbind with vbo_position
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        
        // bind with texture
        GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture_checkerboard[0]);
        // 0th sampler enable (as we have only 1 texture sampler in fragment shader)
        GLES32.glUniform1i(texture0_sampler_uniform, 0);
        
        // draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements();
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4); // 4 (each with its x, y, z) vertices in squareVertices array
        
        squareVertices[0] = 1.0f;
        squareVertices[1] = -1.0f;
        squareVertices[2] = 0.0f;
        squareVertices[3] = 1.0f;
        squareVertices[4] = 1.0f;
        squareVertices[5] = 0.0f;
        squareVertices[6] = 2.41421f;
        squareVertices[7] = 1.0f;
        squareVertices[8] = -1.41421f;
        squareVertices[9] = 2.41421f;
        squareVertices[10] = -1.0f;
        squareVertices[11] = -1.41421f;
        
        // bind with vbo_position
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);
        
        byteBuffer = ByteBuffer.allocateDirect(squareVertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        verticesBuffers = byteBuffer.asFloatBuffer();
        verticesBuffers.put(squareVertices);
        verticesBuffers.position(0); // start from 0 index
        
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, squareVertices.length * 4, verticesBuffers, GLES32.GL_DYNAMIC_DRAW);
        // unbind with vbo_position
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        
        // bind with texture
        GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture_checkerboard[0]);
        // 0th sampler enable (as we have only 1 texture sampler in fragment shader)
        GLES32.glUniform1i(texture0_sampler_uniform, 0);
        
        // draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements();
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4); // 4 (each with its x, y, z) vertices in squareVertices array
        
        // unbind vao
        GLES32.glBindVertexArray(0);
        
        // un-use shader program
        GLES32.glUseProgram(0);
        
        // render/flush
        requestRender(); // same line SwapBuffers() in Windows
    }
    
    void uninitialize()
    {
        // code
        
        // SQUARE
        // destroy vbo_position
        if (vbo_position[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_position, 0);
            vbo_position[0] = 0;
        }
        
        // destroy vbo_texture
        if (vbo_texture[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_texture, 0);
            vbo_texture[0] = 0;
        }
        
        // destroy vao
        if (vao[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vao, 0);
            vao[0] = 0;
        }
        
        if (shaderProgramObject != 0)
        {
            if (vertexShaderObject != 0)
            {
                // detach vertex shader from program object
                GLES32.glDetachShader(shaderProgramObject, vertexShaderObject);
                // delete vertex shader object
                GLES32.glDeleteShader(vertexShaderObject);
                vertexShaderObject = 0;
            }
        
            if (fragmentShaderObject != 0)
            {
                // detach fragment shader from shader program object
                GLES32.glDetachShader(shaderProgramObject, fragmentShaderObject);
                // delete fragment shader object
                GLES32.glDeleteShader(fragmentShaderObject);
                fragmentShaderObject = 0;
            }
        }
    
        // delete shader program object
        if (shaderProgramObject != 0)
        {
            GLES32.glDeleteProgram(shaderProgramObject);
            shaderProgramObject = 0;
        }
        
        // delete texture object
        if (texture_checkerboard[0] != 0)
        {
            GLES32.glDeleteTextures(1, texture_checkerboard, 0);
            texture_checkerboard[0] = 0;
        }
    }
}
