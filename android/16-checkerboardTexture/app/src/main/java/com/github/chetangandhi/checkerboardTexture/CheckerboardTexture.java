package com.github.chetangandhi.checkerboardTexture;

import android.content.Context;

import android.opengl.GLSurfaceView;
import android.opengl.GLES10;
import android.opengl.GLES32;
import android.opengl.Matrix;
import android.opengl.GLUtils;

import android.graphics.BitmapFactory;
import android.graphics.Bitmap;

import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;

import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

public class CheckerboardTexture extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener {

    private final Context context;
    private GestureDetector gestureDetector = null;

    private final int checkImageWidth = 64;
    private final int checkImageHeight = 64;

    private int vertexShaderObject = 0;
    private int fragmentShaderObject = 0;
    private int shaderProgramObject = 0;
    private int mvpUniform = 0;
    private int textureSamplerUniform = 0;

    private int[] vaoSquare = new int[1];
    private int[] vboSquarePosition = new int[1];
    private int[] vboSquareTexture = new int[1];
    private int[] textureCheckerboard = new int[1];
    private int[] checkImage = new int[checkImageHeight * checkImageWidth];

    private float[] perspectiveProjectionMatrix = new float[16];

    CheckerboardTexture(Context context) {
        super(context);

        this.context = context;

        this.setEGLContextClientVersion(3);
        this.setRenderer(this);
        this.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

        this.gestureDetector = new GestureDetector(context, this, null, false);
        this.gestureDetector.setOnDoubleTapListener(this);
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        String version = gl.glGetString(GL10.GL_VERSION);
        System.out.println("CG | OpenGL version: " + version);

        String glslVersion = gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION);
        System.out.println("CG | GLSL version: " + glslVersion);

        this.initialize(gl);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        this.resize(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        this.display();
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        int eventAction = event.getAction();

        if (!this.gestureDetector.onTouchEvent(event)) {
            super.onTouchEvent(event);
        }

        return true;
    }

    @Override
    public boolean onDoubleTap(MotionEvent event) {
        System.out.println("CG | " + this.context.getString(R.string.text_double_tap));
        return true;
    }

    @Override
    public boolean onDoubleTapEvent(MotionEvent event) {
        // Do nothing here as we have implemented onDoubleTap methid, just return true.
        return true;
    }

    @Override
    public boolean onSingleTapConfirmed(MotionEvent event) {
        System.out.println("CG | " + this.context.getString(R.string.text_single_tap_confirmed));
        return true;
    }

    @Override
    public boolean onDown(MotionEvent event) {
        // Do nothing here as we have implemented onSingleTapConfirmed method, just return true.
        return true;
    }

    @Override
    public boolean onFling(MotionEvent eventStart, MotionEvent eventEnd, float velocityX, float velocityY) {
        System.out.println("CG | " + this.context.getString(R.string.text_fling));
        return true;
    }

    @Override
    public void onLongPress(MotionEvent event) {
        System.out.println("CG | " + this.context.getString(R.string.text_long_press));
    }

    @Override
    public void onShowPress(MotionEvent event) {
    }

    @Override
    public boolean onScroll(MotionEvent eventStart, MotionEvent eventEnd, float distanceX, float distanceY) {
        System.out.println("CG | " + this.context.getString(R.string.text_scroll));
        this.cleanUp();
        System.exit(0);
        return true;
    }

    @Override
    public boolean onSingleTapUp(MotionEvent event) {
        return true;
    }

    private void initialize(GL10 gl) {
        this.initializeVertexShaderObject();
        this.initializeFragmentShaderObject();
        this.initializeShaderProgramObject();
        this.initializeSquareBuffers();

        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        GLES32.glEnable(GLES32.GL_DEPTH_TEST);

        // Disable face culling to see back side of object when rotated.
        // GLES32.glEnable(GL_CULL_FACE);

        GLES32.glDepthFunc(GLES32.GL_LEQUAL);

        Matrix.setIdentityM(this.perspectiveProjectionMatrix, 0);

        this.loadCheckerboardTexture();
    }

    private void initializeVertexShaderObject() {
        this.vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
        final String vertexShaderCode = String.format(
            "#version 320 es"
            + "\n"
            + "in vec4 vertexPosition;"
            + "in vec2 vertexTextureCoordinate0;"
            + "\n"
            + "out vec2 outVertexTextureCoordinate0;"
            + "\n"
            + "uniform mat4 mvpMatrix;"
            + "\n"
            + "void main(void)"
            + "{"
            + "   gl_Position = mvpMatrix * vertexPosition;"
            + "   outVertexTextureCoordinate0 = vertexTextureCoordinate0;"
            + "}"
        );

        GLES32.glShaderSource(this.vertexShaderObject, vertexShaderCode);
        GLES32.glCompileShader(this.vertexShaderObject);

        int[] infoLogLength = new int[1];
        int[] shaderCompileStatus = new int[1];
        String infoLog = null;

        GLES32.glGetShaderiv(this.vertexShaderObject, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);

        if(shaderCompileStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(this.vertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);

            if(infoLogLength[0] > 0)
            {
                infoLog = GLES32.glGetShaderInfoLog(this.vertexShaderObject);
                System.out.println("CG | Vertex shader compilation log: " + infoLog);
                cleanUp();
                System.exit(1);
            }
        }
    }

    private void initializeFragmentShaderObject() {
        this.fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

        final String fragmentShaderCode = String.format(
            "#version 320 es"
            + "\n"
            + "precision highp float;"
            + "\n"
            + "in vec2 outVertexTextureCoordinate0;"
            + "\n"
            + "out vec4 fragmentColor;"
            + "\n"
            + "uniform sampler2D textureSampler0;"
            + "\n"
            + "void main(void)"
            + "{"
            + "   fragmentColor = texture(textureSampler0, outVertexTextureCoordinate0);"
            + "}"
        );

        GLES32.glShaderSource(this.fragmentShaderObject, fragmentShaderCode);
        GLES32.glCompileShader(this.fragmentShaderObject);

        int[] infoLogLength = new int[1];
        int[] shaderCompileStatus = new int[1];
        String infoLog = null;

        GLES32.glGetShaderiv(this.fragmentShaderObject, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);

        if(shaderCompileStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(this.fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);

            if(infoLogLength[0] > 0)
            {
                infoLog = GLES32.glGetShaderInfoLog(this.fragmentShaderObject);
                System.out.println("CG | Fragment shader compilation log: " + infoLog);
                cleanUp();
                System.exit(1);
            }
        }
    }

    private void initializeShaderProgramObject() {
        this.shaderProgramObject = GLES32.glCreateProgram();
        GLES32.glAttachShader(this.shaderProgramObject, this.vertexShaderObject);
        GLES32.glAttachShader(this.shaderProgramObject, this.fragmentShaderObject);

        // Bind the position attribute location before linking.
        GLES32.glBindAttribLocation(this.shaderProgramObject, GLESMacros.CG_ATTRIBUTE_VERTEX_POSITION, "vertexPosition");

        // Bind the texture attribute location before linking.
        GLES32.glBindAttribLocation(this.shaderProgramObject, GLESMacros.CG_ATTRIBUTE_TEXTURE0, "vertexTextureCoordinate0");

        // Now link and check for error.
        GLES32.glLinkProgram(this.shaderProgramObject);

        int[] infoLogLength = new int[1];
        int[] shaderProgramLinkStatus = new int[1];
        String infoLog = null;

        GLES32.glGetProgramiv(this.shaderProgramObject, GLES32.GL_LINK_STATUS, shaderProgramLinkStatus, 0);

        if(shaderProgramLinkStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetProgramiv(this.shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);

            if(infoLogLength[0] > 0)
            {
                infoLog = GLES32.glGetProgramInfoLog(this.shaderProgramObject);
                System.out.println("CG | Shader program link log: " + infoLog);
                cleanUp();
                System.exit(1);
            }
        }

        // After linking get the value of MVP uniform location from the shader program.
        this.mvpUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "mvpMatrix");

        // Get the texture sampler uniform location from shader program.
        this.textureSamplerUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "textureSampler0");
    }

    private void initializeSquareBuffers() {
        final float[] squareTextureCoordinates = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
        };

        GLES32.glGenVertexArrays(1, this.vaoSquare, 0);
        GLES32.glBindVertexArray(this.vaoSquare[0]);

        GLES32.glGenBuffers(1, this.vboSquarePosition, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vboSquarePosition[0]);

        // As we are using dynamic draw, pass the size hard coded
        // i.e. 12 * 4, 12 is number of coordinates in position array.
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 12 * 4, null, GLES32.GL_DYNAMIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.CG_ATTRIBUTE_VERTEX_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.CG_ATTRIBUTE_VERTEX_POSITION);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        GLES32.glGenBuffers(1, this.vboSquareTexture, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vboSquareTexture[0]);

        ByteBuffer textureCoordinatesByteBuffer = ByteBuffer.allocateDirect(squareTextureCoordinates.length * 4);
        textureCoordinatesByteBuffer.order(ByteOrder.nativeOrder());

        FloatBuffer textureCoordinatesBuffer = textureCoordinatesByteBuffer.asFloatBuffer();
        textureCoordinatesBuffer.put(squareTextureCoordinates);
        textureCoordinatesBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, squareTextureCoordinates.length * 4, textureCoordinatesBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.CG_ATTRIBUTE_TEXTURE0, 2, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.CG_ATTRIBUTE_TEXTURE0);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        GLES32.glBindVertexArray(0);
    }

    private void display() {
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT | GLES32.GL_STENCIL_BUFFER_BIT);
        GLES32.glUseProgram(shaderProgramObject);

        // Translation is same for both the shape, hence we are doing it in display.
        float[] modelViewMatrix = new float[16];
        float[] modelViewProjectionMatrix = new float[16];

        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);

        // Translate the modal view matrix.
        Matrix.translateM(modelViewMatrix, 0, 0.0f, 0.0f, -3.6f);

        // Multiply modelViewMatrix and perspectiveProjectionMatrix to get modelViewProjectionMatrix
        // Oder of multiplication is very important projectionMatrix * modelMatrix * viewMatrix
        // As we have model and view matrix combined, we just have to multiply projectionMatrix and modelViewMatrix
        Matrix.multiplyMM(modelViewProjectionMatrix, 0, this.perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

        // Pass modelViewProjectionMatrix to vertex shader in 'mvpMatrix' variable defined in shader.
        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

        this.drawStraightSquare();
        this.drawTiltedSquare();

        GLES32.glUseProgram(0);

        this.requestRender();
    }

    private void drawStraightSquare() {
        // Now bind the vaoSquare to which we want to use
        GLES32.glBindVertexArray(vaoSquare[0]);

        float[] squareVertices = {
            0.0f, 1.0f, 0.0f,
            -2.0f, 1.0f, 0.0f,
            -2.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f
        };

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vboSquarePosition[0]);

        // bind the above position coordinates here.
        ByteBuffer verticesByteBuffer = ByteBuffer.allocateDirect(squareVertices.length * 4);
        verticesByteBuffer.order(ByteOrder.nativeOrder());

        FloatBuffer verticesBuffer = verticesByteBuffer.asFloatBuffer();
        verticesBuffer.put(squareVertices);
        verticesBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, squareVertices.length * 4, verticesBuffer, GLES32.GL_DYNAMIC_DRAW);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        // Enable 0th texture
        GLES32.glActiveTexture(GLES32.GL_TEXTURE0);

        // Bind with Square texture
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, this.textureCheckerboard[0]);

        // Enable 0th sampler.
        GLES32.glUniform1i(this.textureSamplerUniform, 0);

        // Draw the Square
        // 4 is number of vertices in the array
        // i.e. element count in squareVertices for each face divide by 3 (x, y, z) component
        // 2nd parameter is the index from which we need to use vertices.
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);

        // unbind the vaoSquare
        GLES32.glBindVertexArray(0);
    }

    private void drawTiltedSquare() {
        // Now bind the vaoSquare to which we want to use
        GLES32.glBindVertexArray(vaoSquare[0]);

        float[] tiltedSquareVertices = {
            2.41421f, 1.0f, -1.41421f,
            1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            2.41421f, -1.0f, -1.41421f
        };

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vboSquarePosition[0]);

        // bind the above position coordinates here.
        ByteBuffer verticesByteBuffer = ByteBuffer.allocateDirect(tiltedSquareVertices.length * 4);
        verticesByteBuffer.order(ByteOrder.nativeOrder());

        FloatBuffer verticesBuffer = verticesByteBuffer.asFloatBuffer();
        verticesBuffer.put(tiltedSquareVertices);
        verticesBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, tiltedSquareVertices.length * 4, verticesBuffer, GLES32.GL_DYNAMIC_DRAW);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        // Enable 0th texture
        GLES32.glActiveTexture(GLES32.GL_TEXTURE0);

        // Bind with Square texture
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, this.textureCheckerboard[0]);

        // Enable 0th sampler.
        GLES32.glUniform1i(this.textureSamplerUniform, 0);

        // Draw the Square
        // 4 is number of vertices in the array
        // i.e. element count in squareVertices for each face divide by 3 (x, y, z) component
        // 2nd parameter is the index from which we need to use vertices.
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);

        // unbind the vaoSquare
        GLES32.glBindVertexArray(0);
    }

    private void loadCheckerboardTexture() {
        this.makeCheckImage();

        Bitmap bitmap = Bitmap.createBitmap(this.checkImage, this.checkImageWidth, this.checkImageHeight, Bitmap.Config.ARGB_8888);

        GLES32.glGenTextures(1, this.textureCheckerboard, 0);
        GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 1);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, this.textureCheckerboard[0]);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_S, GLES32.GL_REPEAT);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_T, GLES32.GL_REPEAT);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_NEAREST);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_NEAREST);

        GLUtils.texImage2D(GLES32.GL_TEXTURE_2D, 0, bitmap, 0);

        GLES10.glTexEnvf(GLES10.GL_TEXTURE_ENV, GLES10.GL_TEXTURE_ENV_MODE, GLES10.GL_REPLACE);

        // Unbind the texture else the last loaded texture will be shown in display
        // if we forgot to specify to which texture to bind in display.
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, 0);
    }

    void makeCheckImage() {
        int heightCounter = 0;
        int widthCounter = 0;

        for (heightCounter = 0; heightCounter < this.checkImageHeight; ++heightCounter)
        {
            for (widthCounter = 0; widthCounter < this.checkImageWidth; ++widthCounter)
            {
                if(((heightCounter & 0x8) ^ (widthCounter & 0x8)) != 0) {
                    this.checkImage[heightCounter * this.checkImageHeight + widthCounter] = android.graphics.Color.WHITE;
                }
                else {
                    this.checkImage[heightCounter * this.checkImageHeight + widthCounter] = android.graphics.Color.BLACK;
                }
            }
        }
    }

    private void resize(int width, int height) {
        if(height == 0) {
            height = 1;
        }

        GLES32.glViewport(0, 0, width, height);
        Matrix.perspectiveM(this.perspectiveProjectionMatrix, 0, 60.0f, (float)width / (float)height, 1.0f, 30.0f);
    }

    private void cleanUp() {
        if(this.vaoSquare[0] != 0) {
            GLES32.glDeleteVertexArrays(1, this.vaoSquare, 0);
            this.vaoSquare[0] = 0;
        }

        if(this.vboSquarePosition[0] != 0) {
            GLES32.glDeleteBuffers(1, this.vboSquarePosition, 0);
            this.vboSquarePosition[0] = 0;
        }

        if(this.vboSquareTexture[0] != 0) {
            GLES32.glDeleteBuffers(1, this.vboSquareTexture, 0);
            this.vboSquareTexture[0] = 0;
        }

        if(this.shaderProgramObject != 0) {
            if(this.vertexShaderObject != 0) {
                GLES32.glDetachShader(this.shaderProgramObject, this.vertexShaderObject);
            }

            if(this.fragmentShaderObject != 0) {
                GLES32.glDetachShader(this.shaderProgramObject, this.fragmentShaderObject);
            }
        }

        if(this.vertexShaderObject != 0) {
            GLES32.glDeleteShader(this.vertexShaderObject);
            this.vertexShaderObject = 0;
        }

        if(this.fragmentShaderObject != 0) {
            GLES32.glDeleteShader(this.fragmentShaderObject);
            this.fragmentShaderObject = 0;
        }

        if(this.shaderProgramObject != 0) {
            GLES32.glDeleteProgram(this.shaderProgramObject);
            this.shaderProgramObject = 0;
        }

        GLES32.glUseProgram(0);

        if (this.textureCheckerboard[0] != 0) {
            GLES32.glDeleteTextures(1, this.textureCheckerboard, 0);
            this.textureCheckerboard[0] = 0;
        }
    }
}
