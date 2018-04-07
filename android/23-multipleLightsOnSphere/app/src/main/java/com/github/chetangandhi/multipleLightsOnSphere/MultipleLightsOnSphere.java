package com.github.chetangandhi.multipleLightsOnSphere;

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
import java.nio.ShortBuffer;

import com.github.chetangandhi.sphere.Sphere;

public class MultipleLightsOnSphere extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener {

    private final Context context;
    private GestureDetector gestureDetector = null;

    private boolean isLightingEnabled = false;
    private boolean isAnimationEnabled = false;

    private float angleRedLight = 0.0f;
    private float angleGreenLight = 0.0f;
    private float angleBlueLight = 0.0f;
    private float speed = 0.01f;

    private int singleTapCounter = 1;
    private int numberOfSphereVertices = 0;
    private int numberOfSphereElements = 0;
    private int vertexShaderObject = 0;
    private int fragmentShaderObject = 0;
    private int shaderProgramObject = 0;
    private int modelMatrixUniform = 0;
    private int viewMatrixUniform = 0;
    private int projectionMatrixUniform = 0;
    private int laZeroUniform = 0;
    private int ldZeroUniform = 0;
    private int lsZeroUniform = 0;
    private int laOneUniform = 0;
    private int ldOneUniform = 0;
    private int lsOneUniform = 0;
    private int laTwoUniform = 0;
    private int ldTwoUniform = 0;
    private int lsTwoUniform = 0;
    private int kaUniform = 0;
    private int kdUniform = 0;
    private int ksUniform = 0;
    private int materialShininessUniform = 0;
    private int lightZeroPositionUniform = 0;
    private int lightOnePositionUniform = 0;
    private int lightTwoPositionUniform = 0;
    private int isLightingEnabledUniform = 0;

    private int[] vaoSphere = new int[1];
    private int[] vboSpherePosition = new int[1];
    private int[] vboSphereNormal = new int[1];
    private int[] vboSphereElement = new int[1];

    private float[] perspectiveProjectionMatrix = new float[16];

    private float[] lightZeroAmbient = {0.0f, 0.0f, 0.0f, 0.0f};
    private float[] lightZeroDiffuse = {1.0f, 0.0f, 0.0f, 0.0f};
    private float[] lightZeroSpecular = {1.0f, 0.0f, 0.0f, 0.0f};
    private float[] lightZeroPosition = {0.0f, 0.0f, 0.0f, 0.0f};

    private float[] lightOneAmbient = {0.0f, 0.0f, 0.0f, 0.0f};
    private float[] lightOneDiffuse = {0.0f, 1.0f, 0.0f, 0.0f};
    private float[] lightOneSpecular = {0.0f, 1.0f, 0.0f, 0.0f};
    private float[] lightOnePosition = {0.0f, 0.0f, 0.0f, 0.0f};

    private float[] lightTwoAmbient = {0.0f, 0.0f, 0.0f, 0.0f};
    private float[] lightTwoDiffuse = {0.0f, 0.0f, 1.0f, 0.0f};
    private float[] lightTwoSpecular = {0.0f, 0.0f, 1.0f, 0.0f};
    private float[] lightTwoPosition = {0.0f, 0.0f, 0.0f, 0.0f};

    private float[] materialAmbient = {0.0f, 0.0f, 0.0f, 0.0f};
    private float[] materialDiffuse = {1.0f, 1.0f, 1.0f, 1.0f};
    private float[] materialSpecular = {1.0f, 1.0f, 1.0f, 0.0f};
    private float materialShininess = 50.0f;

    private float[] sphereVertices = new float[1146];
    private float[] sphereNormals = new float[1146];
    private float[] sphereTextures = new float[764];
    private short[] sphereElements = new short[2280];

    MultipleLightsOnSphere(Context context) {
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

        singleTapCounter++;

        if(singleTapCounter > 9) {
            singleTapCounter = 1;
        }

        speed = 0.1f * (float)singleTapCounter;

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

        this.isAnimationEnabled = !this.isAnimationEnabled;

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

        this.isLightingEnabled = !this.isLightingEnabled;
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
        Sphere sphere=new Sphere();
        sphere.getSphereVertexData(this.sphereVertices, this.sphereNormals, this.sphereTextures, this.sphereElements);

        this.numberOfSphereVertices = sphere.getNumberOfSphereVertices();
        this.numberOfSphereElements = sphere.getNumberOfSphereElements();

        this.initializeVertexShaderObject();
        this.initializeFragmentShaderObject();
        this.initializeShaderProgramObject();
        this.initializeSphereBuffers();

        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        GLES32.glEnable(GLES32.GL_DEPTH_TEST);

        // Disable face culling to see back side of object when rotated.
        // GLES32.glEnable(GL_CULL_FACE);

        GLES32.glDepthFunc(GLES32.GL_LEQUAL);

        Matrix.setIdentityM(this.perspectiveProjectionMatrix, 0);

        this.isAnimationEnabled = true;
        this.isLightingEnabled = true;
    }

    private void initializeVertexShaderObject() {
        this.vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
        final String vertexShaderCode = String.format(
            "#version 320 es"
            + "\n"
            + "in vec4 vertexPosition;"
            + "in vec3 vertexNormal;"
            + "\n"
            + "out vec3 tNormal;"
            + "out vec3 lightZeroDirection;"
            + "out vec3 lightOneDirection;"
            + "out vec3 lightTwoDirection;"
            + "out vec4 eyeCoordinates;"
            + "out vec3 viewVector;"
            + "\n"
            + "uniform mat4 modelMatrix;"
            + "uniform mat4 viewMatrix;"
            + "uniform mat4 projectionMatrix;"
            + "uniform vec4 lightZeroPosition;"
            + "uniform vec4 lightOnePosition;"
            + "uniform vec4 lightTwoPosition;"
            + "uniform mediump int isLightingEnabled;"
            + "\n"
            + "void main(void)"
            + "{"
            + "   if(isLightingEnabled == 1)"
            + "   {"
            + "       eyeCoordinates = viewMatrix * modelMatrix * vertexPosition;"
            + "       tNormal = mat3(viewMatrix * modelMatrix) * vertexNormal;"
            + "       lightZeroDirection = vec3(lightZeroPosition);"
            + "       lightOneDirection = vec3(lightOnePosition);"
            + "       lightTwoDirection = vec3(lightTwoPosition);"
            + "       viewVector = -eyeCoordinates.xyz;"
            + "   }"
            + "\n"
            + "   gl_Position = projectionMatrix * viewMatrix * modelMatrix * vertexPosition;"
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
            + "in vec3 tNormal;"
            + "in vec3 lightZeroDirection;"
            + "in vec3 lightOneDirection;"
            + "in vec3 lightTwoDirection;"
            + "in vec4 eyeCoordinates;"
            + "in vec3 viewVector;"
            + "\n"
            + "out vec4 fragmentColor;"
            + "\n"
            + "uniform int isLightingEnabled;"
            + "uniform vec3 laZero;"
            + "uniform vec3 ldZero;"
            + "uniform vec3 lsZero;"
            + "uniform vec3 laOne;"
            + "uniform vec3 ldOne;"
            + "uniform vec3 lsOne;"
            + "uniform vec3 laTwo;"
            + "uniform vec3 ldTwo;"
            + "uniform vec3 lsTwo;"
            + "uniform vec3 ka;"
            + "uniform vec3 kd;"
            + "uniform vec3 ks;"
            + "uniform float materialShininess;"
            + "\n"
            + "void main(void)"
            + "{"
            + "   vec3 phongAdsColor = vec3(0.0, 0.0, 0.0);"
            + "   if(isLightingEnabled == 1)"
            + "   {"
            + "       vec3 normalizedTNormal = normalize(tNormal);"
            + "       vec3 normalizedViewVector = normalize(viewVector);"
            + "\n"
            + "       vec3 normalizedLightZeroDirection = normalize(lightZeroDirection - eyeCoordinates.xyz);"
            + "       float tNormalDotLightZeroDirection = max(dot(normalizedTNormal, normalizedLightZeroDirection), 0.0);"
            + "       vec3 ambientLightZero = laZero * ka;"
            + "       vec3 diffuseLightZero = ldZero * kd * tNormalDotLightZeroDirection;"
            + "       vec3 reflectionVectorLightZero = reflect(-normalizedLightZeroDirection, normalizedTNormal);"
            + "       vec3 specularLightZero = lsZero * ks * pow(max(dot(reflectionVectorLightZero, normalizedViewVector), 0.0), materialShininess);"
            + "\n"
            + "       vec3 normalizedLightOneDirection = normalize(lightOneDirection - eyeCoordinates.xyz);"
            + "       float tNormalDotLightOneDirection = max(dot(normalizedTNormal, normalizedLightOneDirection), 0.0);"
            + "       vec3 ambientLightOne = laOne * ka;"
            + "       vec3 diffuseLightOne = ldOne * kd * tNormalDotLightOneDirection;"
            + "       vec3 reflectionVectorLightOne = reflect(-normalizedLightOneDirection, normalizedTNormal);"
            + "       vec3 specularLightOne = lsOne * ks * pow(max(dot(reflectionVectorLightOne, normalizedViewVector), 0.0), materialShininess);"
            + "\n"
            + "       vec3 normalizedLightTwoDirection = normalize(lightTwoDirection - eyeCoordinates.xyz);"
            + "       float tNormalDotLightTwoDirection = max(dot(normalizedTNormal, normalizedLightTwoDirection), 0.0);"
            + "       vec3 ambientLightTwo = laTwo * ka;"
            + "       vec3 diffuseLightTwo = ldTwo * kd * tNormalDotLightTwoDirection;"
            + "       vec3 reflectionVectorLightTwo = reflect(-normalizedLightTwoDirection, normalizedTNormal);"
            + "       vec3 specularLightTwo = lsTwo * ks * pow(max(dot(reflectionVectorLightTwo, normalizedViewVector), 0.0), materialShininess);"
            + "\n"
            + "       phongAdsColor = ambientLightZero + diffuseLightZero + specularLightZero;"
            + "       phongAdsColor += ambientLightOne + diffuseLightOne + specularLightOne;"
            + "       phongAdsColor += ambientLightTwo + diffuseLightTwo + specularLightTwo;"
            + "   }"
            + "   else"
            + "   {"
            + "       phongAdsColor = vec3(1.0, 1.0, 1.0);"
            + "   }"
            + "\n"
            + "   fragmentColor = vec4(phongAdsColor, 1.0);"
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

        // Bind the normal attribute location before linking.
        GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.CG_ATTRIBUTE_NORMAL, "vertexNormal");

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

        // After linking get the value of model, view and projection uniform location from the shader program.
        this.modelMatrixUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "modelMatrix");
        this.viewMatrixUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "viewMatrix");
        this.projectionMatrixUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "projectionMatrix");

        this.isLightingEnabledUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "isLightingEnabled");
        this.laZeroUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "laZero");
        this.ldZeroUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "ldZero");
        this.lsZeroUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "lsZero");
        this.laOneUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "laOne");
        this.ldOneUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "ldOne");
        this.lsOneUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "lsOne");
        this.laTwoUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "laTwo");
        this.ldTwoUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "ldTwo");
        this.lsTwoUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "lsTwo");
        this.kaUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "ka");
        this.kdUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "kd");
        this.ksUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "ks");
        this.lightZeroPositionUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "lightZeroPosition");
        this.lightOnePositionUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "lightOnePosition");
        this.lightTwoPositionUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "lightTwoPosition");
        this.materialShininessUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "materialShininess");
    }

    private void initializeSphereBuffers() {
        GLES32.glGenVertexArrays(1, this.vaoSphere, 0);
        GLES32.glBindVertexArray(this.vaoSphere[0]);

        GLES32.glGenBuffers(1, this.vboSpherePosition, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vboSpherePosition[0]);

        ByteBuffer verticesByteBuffer = ByteBuffer.allocateDirect(sphereVertices.length * 4);
        verticesByteBuffer.order(ByteOrder.nativeOrder());

        FloatBuffer verticesBuffer = verticesByteBuffer.asFloatBuffer();
        verticesBuffer.put(this.sphereVertices);
        verticesBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, this.sphereVertices.length * 4, verticesBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.CG_ATTRIBUTE_VERTEX_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.CG_ATTRIBUTE_VERTEX_POSITION);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        GLES32.glGenBuffers(1, this.vboSphereNormal, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vboSphereNormal[0]);

        ByteBuffer sphereNormalsByteBuffer = ByteBuffer.allocateDirect(sphereNormals.length * 4);
        sphereNormalsByteBuffer.order(ByteOrder.nativeOrder());

        FloatBuffer sphereNormalsBuffer = sphereNormalsByteBuffer.asFloatBuffer();
        sphereNormalsBuffer.put(this.sphereNormals);
        sphereNormalsBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, this.sphereNormals.length * 4, sphereNormalsBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.CG_ATTRIBUTE_NORMAL, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.CG_ATTRIBUTE_NORMAL);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        GLES32.glGenBuffers(1, this.vboSphereElement, 0);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, this.vboSphereElement[0]);

        ByteBuffer sphereElementByteBuffer = ByteBuffer.allocateDirect(this.sphereElements.length * 2);
        sphereElementByteBuffer.order(ByteOrder.nativeOrder());

        ShortBuffer sphereElementsBuffer = sphereElementByteBuffer.asShortBuffer();
        sphereElementsBuffer.put(this.sphereElements);
        sphereElementsBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER, this.sphereElements.length * 2, sphereElementsBuffer, GLES32.GL_STATIC_DRAW);

        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER,0);

        GLES32.glBindVertexArray(0);
    }

    private void update() {
        if(!this.isAnimationEnabled) {
            return;
        }

        this.angleRedLight += speed;
        this.angleGreenLight += speed;
        this.angleBlueLight += speed;

        if(this.angleRedLight >= 360.0f) {
            this.angleRedLight = 0.0f;
        }

        if(this.angleGreenLight >= 360.0f) {
            this.angleGreenLight = 0.0f;
        }

        if(this.angleBlueLight >= 360.0f) {
            this.angleBlueLight = 0.0f;
        }
    }

    private void display() {
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT | GLES32.GL_STENCIL_BUFFER_BIT);
        GLES32.glUseProgram(shaderProgramObject);

        if(isLightingEnabled) {
            GLES32.glUniform1i(isLightingEnabledUniform, 1);

            GLES32.glUniform3fv(this.laZeroUniform, 1, this.lightZeroAmbient, 0);
            GLES32.glUniform3fv(this.ldZeroUniform, 1, this.lightZeroDiffuse, 0);
            GLES32.glUniform3fv(this.lsZeroUniform, 1, this.lightZeroSpecular, 0);

            GLES32.glUniform3fv(this.laOneUniform, 1, this.lightOneAmbient, 0);
            GLES32.glUniform3fv(this.ldOneUniform, 1, this.lightOneDiffuse, 0);
            GLES32.glUniform3fv(this.lsOneUniform, 1, this.lightOneSpecular, 0);

            GLES32.glUniform3fv(this.laTwoUniform, 1, this.lightTwoAmbient, 0);
            GLES32.glUniform3fv(this.ldTwoUniform, 1, this.lightTwoDiffuse, 0);
            GLES32.glUniform3fv(this.lsTwoUniform, 1, this.lightTwoSpecular, 0);

            lightZeroPosition[0] = 0.0f;
            lightZeroPosition[1] = (float)Math.sin(angleRedLight) * 100.0f;
            lightZeroPosition[2] = (float)Math.cos(angleRedLight) * 100.0f - 3.0f;
            GLES32.glUniform4fv(this.lightZeroPositionUniform, 1, this.lightZeroPosition, 0);

            lightOnePosition[0] = (float)Math.sin(angleRedLight) * 100.0f;
            lightOnePosition[1] = 0.0f;
            lightOnePosition[2] = (float)Math.cos(angleRedLight) * 100.0f - 3.0f;
            GLES32.glUniform4fv(this.lightOnePositionUniform, 1, this.lightOnePosition, 0);

            lightOnePosition[0] = (float)Math.cos(angleRedLight) * 100.0f - 3.0f;
            lightOnePosition[1] = (float)Math.sin(angleRedLight) * 100.0f;
            lightOnePosition[2] = -3.0f;
            GLES32.glUniform4fv(this.lightTwoPositionUniform, 1, this.lightOnePosition, 0);

            GLES32.glUniform3fv(this.kaUniform, 1, this.materialAmbient, 0);
            GLES32.glUniform3fv(this.kdUniform, 1, this.materialDiffuse, 0);
            GLES32.glUniform3fv(this.ksUniform, 1, this.materialSpecular, 0);
            GLES32.glUniform1f(this.materialShininessUniform, this.materialShininess);
        }
        else {
            GLES32.glUniform1i(isLightingEnabledUniform, 0);
        }

        this.drawSphere();

        GLES32.glUseProgram(0);

        this.update();
        this.requestRender();
    }

    private void drawSphere() {
        float[] modelMatrix = new float[16];
        float[] viewMatrix = new float[16];

        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);

        // Translate the modal matrix.
        Matrix.translateM(modelMatrix, 0, 0.0f, 0.0f, -3.0f);

        // Pass modelMatrix and viewProjectionMatrix to vertex shader in 'modelMatrixUniform' and 'viewMatrixUniform'
        // and projectionMatrixUniform variable defined in shader.
        GLES32.glUniformMatrix4fv(this.modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(this.viewMatrixUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(this.projectionMatrixUniform, 1, false, this.perspectiveProjectionMatrix, 0);

        // Now bind the vaoSphere to which we want to use
        GLES32.glBindVertexArray(vaoSphere[0]);

        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vboSphereElement[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, this.numberOfSphereElements, GLES32.GL_UNSIGNED_SHORT, 0);

        // unbind the vaoSphere
        GLES32.glBindVertexArray(0);
    }

    private void resize(int width, int height) {
        if(height == 0) {
            height = 1;
        }

        GLES32.glViewport(0, 0, width, height);
        Matrix.perspectiveM(this.perspectiveProjectionMatrix, 0, 45.0f, (float)width / (float)height, 1.0f, 100.0f);
    }

    private void cleanUp() {
        if(this.vaoSphere[0] != 0) {
            GLES32.glDeleteVertexArrays(1, this.vaoSphere, 0);
            this.vaoSphere[0] = 0;
        }

        if(this.vboSpherePosition[0] != 0) {
            GLES32.glDeleteBuffers(1, this.vboSpherePosition, 0);
            this.vboSpherePosition[0] = 0;
        }

        if(this.vboSphereNormal[0] != 0) {
            GLES32.glDeleteBuffers(1, this.vboSphereNormal, 0);
            this.vboSphereNormal[0] = 0;
        }

       if(this.vboSphereElement[0] != 0) {
            GLES32.glDeleteBuffers(1, this.vboSphereElement, 0);
            this.vboSphereElement[0] = 0;
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
    }
}
