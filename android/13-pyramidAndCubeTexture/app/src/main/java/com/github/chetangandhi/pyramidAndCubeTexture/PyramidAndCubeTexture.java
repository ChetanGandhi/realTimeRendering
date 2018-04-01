package com.github.chetangandhi.pyramidAndCubeTexture;

import android.content.Context;

import android.opengl.GLSurfaceView;
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

public class PyramidAndCubeTexture extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener {

    private final Context context;
    private GestureDetector gestureDetector = null;

    private float angleCube = 0.0f;
    private float anglePyramid = 0.0f;
    private float speed = 0.1f;

    private int singleTapCounter = 1;
    private int vertexShaderObject = 0;
    private int fragmentShaderObject = 0;
    private int shaderProgramObject = 0;
    private int mvpUniform = 0;
    private int textureSamplerUniform = 0;

    private int[] vaoPyramid = new int[1];
    private int[] vboPyramidPosition = new int[1];
    private int[] vboPyramidTexture = new int[1];
    private int[] vaoCube = new int[1];
    private int[] vboCubePosition = new int[1];
    private int[] vboCubeTexture = new int[1];
    private int[] textureKundali = new int[1];
    private int[] textureStone = new int[1];

    private float[] perspectiveProjectionMatrix = new float[16];

    PyramidAndCubeTexture(Context context) {
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

        singleTapCounter++;

        if(singleTapCounter < 1 || singleTapCounter > 9) {
            singleTapCounter = 1;
        }

        speed = 0.1f * (float)singleTapCounter;

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
        this.initializePyramidBuffers();
        this.initializeCubeBuffers();

        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        GLES32.glEnable(GLES32.GL_DEPTH_TEST);

        // Disable face culling to see back side of object when rotated.
        // GLES32.glEnable(GL_CULL_FACE);

        GLES32.glDepthFunc(GLES32.GL_LEQUAL);

        Matrix.setIdentityM(this.perspectiveProjectionMatrix, 0);

        this.textureStone[0] = this.loadGLTexture(R.raw.stone);
        this.textureKundali[0] = this.loadGLTexture(R.raw.vijay_kundali_inverted);
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

    private void initializePyramidBuffers() {
        final float[] pyramidVertices = {
            // Front face
            0.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,

            // Right face
            0.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,

            // Back face
            0.0f, 1.0f, 0.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,

            // Left face
            0.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f
        };

        final float[] pyramidTextureCoordinates = {
            // Front face
            0.5f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,

            // Right face
            0.5f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,

            // Back face
            0.5f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,

            // Left face
            0.5f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f
        };

        GLES32.glGenVertexArrays(1, this.vaoPyramid, 0);
        GLES32.glBindVertexArray(this.vaoPyramid[0]);

        GLES32.glGenBuffers(1, this.vboPyramidPosition, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vboPyramidPosition[0]);

        ByteBuffer verticesByteBuffer = ByteBuffer.allocateDirect(pyramidVertices.length * 4);
        verticesByteBuffer.order(ByteOrder.nativeOrder());

        FloatBuffer verticesBuffer = verticesByteBuffer.asFloatBuffer();
        verticesBuffer.put(pyramidVertices);
        verticesBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramidVertices.length * 4, verticesBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.CG_ATTRIBUTE_VERTEX_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.CG_ATTRIBUTE_VERTEX_POSITION);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        GLES32.glGenBuffers(1, this.vboPyramidTexture, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vboPyramidTexture[0]);

        ByteBuffer textureCoordibatesByteBuffer = ByteBuffer.allocateDirect(pyramidTextureCoordinates.length * 4);
        textureCoordibatesByteBuffer.order(ByteOrder.nativeOrder());

        FloatBuffer textureCoordinatesBuffer = textureCoordibatesByteBuffer.asFloatBuffer();
        textureCoordinatesBuffer.put(pyramidTextureCoordinates);
        textureCoordinatesBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramidTextureCoordinates.length * 4, textureCoordinatesBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.CG_ATTRIBUTE_TEXTURE0, 2, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.CG_ATTRIBUTE_TEXTURE0);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        GLES32.glBindVertexArray(0);
    }

    private void initializeCubeBuffers() {
        final float[] cubeVertices = {
            // Top face
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,

            // Bottom face
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,

            // Front face
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,

            // Back face
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            // Right face
            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,

            // Left face
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f
        };

        final float[] cubeTextureCoordinates = {
            // Top face
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,

            // Bottom face
            1.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,

            // Front face
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,

            // Back face
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 0.0f,

            // right face
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 0.0f,

            // left face
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
        };

        GLES32.glGenVertexArrays(1, this.vaoCube, 0);
        GLES32.glBindVertexArray(this.vaoCube[0]);

        GLES32.glGenBuffers(1, this.vboCubePosition, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vboCubePosition[0]);

        ByteBuffer verticesByteBuffer = ByteBuffer.allocateDirect(cubeVertices.length * 4);
        verticesByteBuffer.order(ByteOrder.nativeOrder());

        FloatBuffer verticesBuffer = verticesByteBuffer.asFloatBuffer();
        verticesBuffer.put(cubeVertices);
        verticesBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cubeVertices.length * 4, verticesBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.CG_ATTRIBUTE_VERTEX_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.CG_ATTRIBUTE_VERTEX_POSITION);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        GLES32.glGenBuffers(1, this.vboCubeTexture, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vboCubeTexture[0]);

        ByteBuffer textureCoordinatesByteBuffer = ByteBuffer.allocateDirect(cubeTextureCoordinates.length * 4);
        textureCoordinatesByteBuffer.order(ByteOrder.nativeOrder());

        FloatBuffer textureCoordinatesBuffer = textureCoordinatesByteBuffer.asFloatBuffer();
        textureCoordinatesBuffer.put(cubeTextureCoordinates);
        textureCoordinatesBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cubeTextureCoordinates.length * 4, textureCoordinatesBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.CG_ATTRIBUTE_TEXTURE0, 2, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.CG_ATTRIBUTE_TEXTURE0);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        GLES32.glBindVertexArray(0);
    }

    private void update() {
        angleCube -= speed;
        anglePyramid += speed;

        if(angleCube <= -360.0f)
        {
            angleCube = 0.0f;
        }

        if(anglePyramid >= 360.0f)
        {
            anglePyramid = 0.0f;
        }
    }

    private void display() {
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT | GLES32.GL_STENCIL_BUFFER_BIT);
        GLES32.glUseProgram(shaderProgramObject);

        this.drawPyramid();
        this.drawCube();

        GLES32.glUseProgram(0);

        this.update();
        this.requestRender();
    }

    private void drawPyramid() {
        float[] modelViewMatrix = new float[16];
        float[] rotationMatrix = new float[16];
        float[] modelViewProjectionMatrix = new float[16];

        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(rotationMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);

        // Translate the modal view matrix.
        Matrix.translateM(modelViewMatrix, 0, -1.5f, 0.0f, -6.0f);
        Matrix.rotateM(rotationMatrix, 0, anglePyramid, 0.0f, 1.0f, 0.0f);

        // Rotate after transformation of modelViewMatrix.
        Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0, rotationMatrix, 0);

        // Multiply modelViewMatrix and perspectiveProjectionMatrix to get modelViewProjectionMatrix
        // Oder of multiplication is very important projectionMatrix * modelMatrix * viewMatrix
        // As we have model and view matrix combined, we just have to multiply projectionMatrix and modelViewMatrix
        Matrix.multiplyMM(modelViewProjectionMatrix, 0, this.perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

        // Pass modelViewProjectionMatrix to vertex shader in 'mvpMatrix' variable defined in shader.
        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

        // Now bind the voaPyramid to which we want to use
        GLES32.glBindVertexArray(vaoPyramid[0]);

        // Enable 0th texture
        GLES32.glActiveTexture(GLES32.GL_TEXTURE0);

        // Bind with pyramid texture
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, this.textureStone[0]);

        // Enable 0th sampler.
        GLES32.glUniform1i(this.textureSamplerUniform, 0);

        // Draw the pyramid
        // 12 is number of vertices in the array i.e. element count in pyramidVertices divide by 3 (x, y, z) component
        GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 12);

        // unbind the vaoPyramid
        GLES32.glBindVertexArray(0);
    }

    private void drawCube() {
        float[] modelViewMatrix = new float[16];
        float[] rotationMatrix = new float[16];
        float[] scaleMatrix = new float[16];
        float[] modelViewProjectionMatrix = new float[16];

        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(rotationMatrix, 0);
        Matrix.setIdentityM(scaleMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);

        // Scale, translate and rotate the modal view matrix.
        Matrix.scaleM(scaleMatrix, 0, 0.75f, 0.75f, 0.75f);
        Matrix.translateM(modelViewMatrix, 0, 1.5f, 0.0f, -6.0f);
        Matrix.rotateM(rotationMatrix, 0, angleCube, 1.0f, 0.0f, 0.0f);
        Matrix.rotateM(rotationMatrix, 0, angleCube, 0.0f, 1.0f, 0.0f);
        Matrix.rotateM(rotationMatrix, 0, angleCube, 0.0f, 0.0f, 1.0f);

        // Scale after transformation of modelViewMatrix.
        Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0, scaleMatrix, 0);

        // Rotate after transformation and scale of modelViewMatrix.
        Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0, rotationMatrix, 0);

        // Multiply modelViewMatrix and perspectiveProjectionMatrix to get modelViewProjectionMatrix
        // Oder of multiplication is very important projectionMatrix * modelMatrix * viewMatrix
        // As we have model and view matrix combined, we just have to multiply projectionMatrix and modelViewMatrix
        Matrix.multiplyMM(modelViewProjectionMatrix, 0, this.perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

        // Pass modelViewProjectionMatrix to vertex shader in 'mvpMatrix' variable defined in shader.
        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

        // Now bind the vaoCube to which we want to use
        GLES32.glBindVertexArray(vaoCube[0]);

        // Enable 0th texture
        GLES32.glActiveTexture(GLES32.GL_TEXTURE0);

        // Bind with pyramid texture
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, this.textureKundali[0]);

        // Enable 0th sampler.
        GLES32.glUniform1i(this.textureSamplerUniform, 0);

        // Draw the cube
        // 4 is number of vertices in the array for each face
        // i.e. element count in cubeVertices for each face divide by 3 (x, y, z) component
        // 2nd parameter is the index from which we need to use vertices.
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 4, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 8, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 12, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 16, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 20, 4);

        // unbind the vaoCube
        GLES32.glBindVertexArray(0);
    }

    private int loadGLTexture(int resourceId)
    {
        int[] texture = new int[1];

        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inScaled = false;

        Bitmap bitmap = BitmapFactory.decodeResource(this.context.getResources(), resourceId, options);

        GLES32.glGenTextures(1, texture, 0);
        GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 1);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture[0]);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER,GLES32.GL_LINEAR);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER,GLES32.GL_LINEAR_MIPMAP_LINEAR);

        GLUtils.texImage2D(GLES32.GL_TEXTURE_2D, 0, bitmap, 0);

        // Generate the mipmapped texture
        GLES32.glGenerateMipmap(GLES32.GL_TEXTURE_2D);

        // Unbind the texture else the last loaded texture will be shown in display
        // if we forgot to specify to which texture to bind in display.
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, 0);

        return texture[0];
    }

    private void resize(int width, int height) {
        if(height == 0) {
            height = 1;
        }

        GLES32.glViewport(0, 0, width, height);
        Matrix.perspectiveM(this.perspectiveProjectionMatrix, 0, 45.0f, (float)width / (float)height, 1.0f, 100.0f);
    }

    private void cleanUp() {
        if(this.vaoPyramid[0] != 0) {
            GLES32.glDeleteVertexArrays(1, this.vaoPyramid, 0);
            this.vaoPyramid[0] = 0;
        }

        if(this.vboPyramidPosition[0] != 0) {
            GLES32.glDeleteBuffers(1, this.vboPyramidPosition, 0);
            this.vboPyramidPosition[0] = 0;
        }

        if(this.vboPyramidTexture[0] != 0) {
            GLES32.glDeleteBuffers(1, this.vboPyramidTexture, 0);
            this.vboPyramidTexture[0] = 0;
        }

        if(this.vaoCube[0] != 0) {
            GLES32.glDeleteVertexArrays(1, this.vaoCube, 0);
            this.vaoCube[0] = 0;
        }

        if(this.vboCubePosition[0] != 0) {
            GLES32.glDeleteBuffers(1, this.vboCubePosition, 0);
            this.vboCubePosition[0] = 0;
        }

        if(this.vboCubeTexture[0] != 0) {
            GLES32.glDeleteBuffers(1, this.vboCubeTexture, 0);
            this.vboCubeTexture[0] = 0;
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

        if (this.textureStone[0] != 0) {
            GLES32.glDeleteTextures(1, this.textureStone, 0);
            this.textureStone[0] = 0;
        }

        if (this.textureKundali[0] != 0) {
            GLES32.glDeleteTextures(1, this.textureKundali, 0);
            this.textureKundali[0] = 0;
        }
    }
}
