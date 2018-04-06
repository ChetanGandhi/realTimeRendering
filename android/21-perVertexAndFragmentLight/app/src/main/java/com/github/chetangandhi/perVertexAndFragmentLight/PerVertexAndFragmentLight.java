package com.github.chetangandhi.perVertexAndFragmentLight;

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

public class PerVertexAndFragmentLight extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener {

    private final Context context;
    private GestureDetector gestureDetector = null;

    private boolean isPerFragmentLightEnabled = false;
    private boolean isLightingEnabled = false;

    private int singleTapCounter = 1;
    private int numberOfSphereVertices = 0;
    private int numberOfSphereElements = 0;

    private int vertexShaderObjectPerVertex = 0;
    private int fragmentShaderObjectPerVertex = 0;
    private int shaderProgramObjectPerVertex = 0;
    private int modelMatrixUniformPerVertex = 0;
    private int viewMatrixUniformPerVertex = 0;
    private int projectionMatrixUniformPerVertex = 0;
    private int laUniformPerVertex = 0;
    private int ldUniformPerVertex = 0;
    private int lsUniformPerVertex = 0;
    private int kaUniformPerVertex = 0;
    private int kdUniformPerVertex = 0;
    private int ksUniformPerVertex = 0;
    private int materialShininessUniformPerVertex = 0;
    private int lightPositionUniformPerVertex = 0;
    private int isLightingEnabledUniformPerVertex = 0;

    private int[] vaoSpherePerVertex = new int[1];
    private int[] vboSpherePositionPerVertex = new int[1];
    private int[] vboSphereNormalPerVertex = new int[1];
    private int[] vboSphereElementPerVertex = new int[1];

    private int vertexShaderObjectPerFragment = 0;
    private int fragmentShaderObjectPerFragment = 0;
    private int shaderProgramObjectPerFragment = 0;
    private int modelMatrixUniformPerFragment = 0;
    private int viewMatrixUniformPerFragment = 0;
    private int projectionMatrixUniformPerFragment = 0;
    private int laUniformPerFragment = 0;
    private int ldUniformPerFragment = 0;
    private int lsUniformPerFragment = 0;
    private int kaUniformPerFragment = 0;
    private int kdUniformPerFragment = 0;
    private int ksUniformPerFragment = 0;
    private int materialShininessUniformPerFragment = 0;
    private int lightPositionUniformPerFragment = 0;
    private int isLightingEnabledUniformPerFragment = 0;

    private int[] vaoSpherePerFragment = new int[1];
    private int[] vboSpherePositionPerFragment = new int[1];
    private int[] vboSphereNormalPerFragment = new int[1];
    private int[] vboSphereElementPerFragment = new int[1];

    private float[] perspectiveProjectionMatrix = new float[16];

    private float[] lightAmbient = { 0.0f,0.0f,0.0f,1.0f };
    private float[] lightDiffuse = { 1.0f,1.0f,1.0f,1.0f };
    private float[] lightSpecular = { 1.0f,1.0f,1.0f,1.0f };
    private float[] lightPosition = { 100.0f,100.0f,100.0f,1.0f };

    private float[] materialAmbient = { 0.0f,0.0f,0.0f,1.0f };
    private float[] materialDiffuse = { 1.0f,1.0f,1.0f,1.0f };
    private float[] materialSpecular = { 1.0f,1.0f,1.0f,1.0f };
    private float materialShininess = 50.0f;

    private float[] sphereVertices = new float[1146];
    private float[] sphereNormals = new float[1146];
    private float[] sphereTextures = new float[764];
    private short[] sphereElements = new short[2280];

    PerVertexAndFragmentLight(Context context) {
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

        this.isPerFragmentLightEnabled = !this.isPerFragmentLightEnabled;

        return true;
    }

    @Override
    public boolean onDoubleTapEvent(MotionEvent event) {
        // Do nothing here as we have implemented onDoubleTap method, just return true.
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

        this.initializeVertexShaderObjectPerVertex();
        this.initializeFragmentShaderObjectPerVertex();
        this.initializeShaderProgramObjectPerVertex();
        this.initializeSphereBuffersPerVertex();
        this.initializeVertexShaderObjectPerFragment();
        this.initializeFragmentShaderObjectPerFragment();
        this.initializeShaderProgramObjectPerFragment();
        this.initializeSphereBuffersPerFragment();

        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        GLES32.glEnable(GLES32.GL_DEPTH_TEST);

        // Disable face culling to see back side of object when rotated.
        // GLES32.glEnable(GL_CULL_FACE);

        GLES32.glDepthFunc(GLES32.GL_LEQUAL);

        Matrix.setIdentityM(this.perspectiveProjectionMatrix, 0);

        this.isLightingEnabled = true;
    }

    private void initializeVertexShaderObjectPerVertex() {
        this.vertexShaderObjectPerVertex = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
        final String vertexShaderCode = String.format(
            "#version 320 es"
            + "\n"
            + "in vec4 vertexPosition;"
            + "in vec3 vertexNormal;"
            + "\n"
            + "out vec3 phongAdsColor;"
            + "\n"
            + "uniform mat4 modelMatrix;"
            + "uniform mat4 viewMatrix;"
            + "uniform mat4 projectionMatrix;"
            + "uniform mediump int isLightingEnabled;"
            + "uniform vec3 la;"
            + "uniform vec3 ld;"
            + "uniform vec3 ls;"
            + "uniform vec3 ka;"
            + "uniform vec3 kd;"
            + "uniform vec3 ks;"
            + "uniform vec4 lightPosition;"
            + "uniform float materialShininess;"
            + "\n"
            + "void main(void)"
            + "{"
            + "   if(isLightingEnabled == 1)"
            + "   {"
            + "       vec4 eyeCoordinates = viewMatrix * modelMatrix * vertexPosition;"
            + "       vec3 tNormal = normalize(mat3(viewMatrix * modelMatrix) * vertexNormal);"
            + "       vec3 source = normalize(vec3(lightPosition) - eyeCoordinates.xyz);"
            + "       float tNormalDotLightDirection = max(dot(tNormal, source), 0.0);"
            + "       vec3 ambient = la * ka;"
            + "       vec3 diffuse = ld * kd * tNormalDotLightDirection;"
            + "       vec3 reflectionVector = reflect(-source, tNormal);"
            + "       vec3 viewVector = normalize(-eyeCoordinates.xyz);"
            + "       vec3 specular = ls * ks * pow(max(dot(reflectionVector, viewVector), 0.0), materialShininess);"
            + "       phongAdsColor = ambient + diffuse + specular;"
            + "   }"
            + "   else"
            + "   {"
            + "       phongAdsColor = vec3(1.0, 1.0, 1.0);"
            + "   }"
            + "\n"
            + "   gl_Position = projectionMatrix * viewMatrix * modelMatrix * vertexPosition;"
            + "}"
        );

        GLES32.glShaderSource(this.vertexShaderObjectPerVertex, vertexShaderCode);
        GLES32.glCompileShader(this.vertexShaderObjectPerVertex);

        int[] infoLogLength = new int[1];
        int[] shaderCompileStatus = new int[1];
        String infoLog = null;

        GLES32.glGetShaderiv(this.vertexShaderObjectPerVertex, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);

        if(shaderCompileStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetShaderiv(this.vertexShaderObjectPerVertex, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);

            if(infoLogLength[0] > 0) {
                infoLog = GLES32.glGetShaderInfoLog(this.vertexShaderObjectPerVertex);
                System.out.println("CG | Vertex shader compilation log: " + infoLog);
                cleanUp();
                System.exit(1);
            }
        }
    }

    private void initializeFragmentShaderObjectPerVertex() {
        this.fragmentShaderObjectPerVertex = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

        final String fragmentShaderCode = String.format(
            "#version 320 es"
            + "\n"
            + "precision highp float;"
            + "\n"
            + "in vec3 phongAdsColor;"
            + "\n"
            + "out vec4 fragmentColor;"
            + "\n"
            + "void main(void)"
            + "{"
            + "   fragmentColor = vec4(phongAdsColor, 1.0);"
            + "}"
        );

        GLES32.glShaderSource(this.fragmentShaderObjectPerVertex, fragmentShaderCode);
        GLES32.glCompileShader(this.fragmentShaderObjectPerVertex);

        int[] infoLogLength = new int[1];
        int[] shaderCompileStatus = new int[1];
        String infoLog = null;

        GLES32.glGetShaderiv(this.fragmentShaderObjectPerVertex, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);

        if(shaderCompileStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetShaderiv(this.fragmentShaderObjectPerVertex, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);

            if(infoLogLength[0] > 0) {
                infoLog = GLES32.glGetShaderInfoLog(this.fragmentShaderObjectPerVertex);
                System.out.println("CG | Fragment shader compilation log: " + infoLog);
                cleanUp();
                System.exit(1);
            }
        }
    }

    private void initializeShaderProgramObjectPerVertex() {
        this.shaderProgramObjectPerVertex = GLES32.glCreateProgram();
        GLES32.glAttachShader(this.shaderProgramObjectPerVertex, this.vertexShaderObjectPerVertex);
        GLES32.glAttachShader(this.shaderProgramObjectPerVertex, this.fragmentShaderObjectPerVertex);

        // Bind the position attribute location before linking.
        GLES32.glBindAttribLocation(this.shaderProgramObjectPerVertex, GLESMacros.CG_ATTRIBUTE_VERTEX_POSITION, "vertexPosition");

        // Bind the normal attribute location before linking.
        GLES32.glBindAttribLocation(shaderProgramObjectPerVertex, GLESMacros.CG_ATTRIBUTE_NORMAL, "vertexNormal");

        // Now link and check for error.
        GLES32.glLinkProgram(this.shaderProgramObjectPerVertex);

        int[] infoLogLength = new int[1];
        int[] shaderProgramLinkStatus = new int[1];
        String infoLog = null;

        GLES32.glGetProgramiv(this.shaderProgramObjectPerVertex, GLES32.GL_LINK_STATUS, shaderProgramLinkStatus, 0);

        if(shaderProgramLinkStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetProgramiv(this.shaderProgramObjectPerVertex, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);

            if(infoLogLength[0] > 0) {
                infoLog = GLES32.glGetProgramInfoLog(this.shaderProgramObjectPerVertex);
                System.out.println("CG | Shader program link log: " + infoLog);
                cleanUp();
                System.exit(1);
            }
        }

        // After linking get the value of modelView and projection uniform location from the shader program.
        this.modelMatrixUniformPerVertex = GLES32.glGetUniformLocation(this.shaderProgramObjectPerVertex, "modelMatrix");
        this.viewMatrixUniformPerVertex = GLES32.glGetUniformLocation(this.shaderProgramObjectPerVertex, "viewMatrix");
        this.projectionMatrixUniformPerVertex = GLES32.glGetUniformLocation(this.shaderProgramObjectPerVertex, "projectionMatrix");
        this.isLightingEnabledUniformPerVertex = GLES32.glGetUniformLocation(this.shaderProgramObjectPerVertex, "isLightingEnabled");

        this.laUniformPerVertex = GLES32.glGetUniformLocation(this.shaderProgramObjectPerVertex, "la");
        this.ldUniformPerVertex = GLES32.glGetUniformLocation(this.shaderProgramObjectPerVertex, "ld");
        this.lsUniformPerVertex = GLES32.glGetUniformLocation(this.shaderProgramObjectPerVertex, "ls");
        this.kaUniformPerVertex = GLES32.glGetUniformLocation(this.shaderProgramObjectPerVertex, "ka");
        this.kdUniformPerVertex = GLES32.glGetUniformLocation(this.shaderProgramObjectPerVertex, "kd");
        this.ksUniformPerVertex = GLES32.glGetUniformLocation(this.shaderProgramObjectPerVertex, "ks");
        this.lightPositionUniformPerVertex = GLES32.glGetUniformLocation(this.shaderProgramObjectPerVertex, "lightPosition");
        this.materialShininessUniformPerVertex = GLES32.glGetUniformLocation(this.shaderProgramObjectPerVertex, "materialShininess");
    }

    private void initializeSphereBuffersPerVertex() {
        GLES32.glGenVertexArrays(1, this.vaoSpherePerVertex, 0);
        GLES32.glBindVertexArray(this.vaoSpherePerVertex[0]);

        GLES32.glGenBuffers(1, this.vboSpherePositionPerVertex, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vboSpherePositionPerVertex[0]);

        ByteBuffer verticesByteBuffer = ByteBuffer.allocateDirect(this.sphereVertices.length * 4);
        verticesByteBuffer.order(ByteOrder.nativeOrder());

        FloatBuffer verticesBuffer = verticesByteBuffer.asFloatBuffer();
        verticesBuffer.put(this.sphereVertices);
        verticesBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, this.sphereVertices.length * 4, verticesBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.CG_ATTRIBUTE_VERTEX_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.CG_ATTRIBUTE_VERTEX_POSITION);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        GLES32.glGenBuffers(1, this.vboSphereNormalPerVertex, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vboSphereNormalPerVertex[0]);

        ByteBuffer sphereNormalsByteBuffer = ByteBuffer.allocateDirect(this.sphereNormals.length * 4);
        sphereNormalsByteBuffer.order(ByteOrder.nativeOrder());

        FloatBuffer sphereNormalsBuffer = sphereNormalsByteBuffer.asFloatBuffer();
        sphereNormalsBuffer.put(this.sphereNormals);
        sphereNormalsBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, this.sphereNormals.length * 4, sphereNormalsBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.CG_ATTRIBUTE_NORMAL, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.CG_ATTRIBUTE_NORMAL);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        GLES32.glGenBuffers(1, this.vboSphereElementPerVertex, 0);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, this.vboSphereElementPerVertex[0]);

        ByteBuffer sphereElementByteBuffer = ByteBuffer.allocateDirect(this.sphereElements.length * 2);
        sphereElementByteBuffer.order(ByteOrder.nativeOrder());

        ShortBuffer sphereElementsBuffer = sphereElementByteBuffer.asShortBuffer();
        sphereElementsBuffer.put(this.sphereElements);
        sphereElementsBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER, this.sphereElements.length * 2, sphereElementsBuffer, GLES32.GL_STATIC_DRAW);

        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER,0);

        GLES32.glBindVertexArray(0);
    }

    private void initializeVertexShaderObjectPerFragment() {
        this.vertexShaderObjectPerFragment = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
        final String vertexShaderCode = String.format(
            "#version 320 es"
            + "\n"
            + "in vec4 vertexPosition;"
            + "in vec3 vertexNormal;"
            + "\n"
            + "out vec3 tNormal;"
            + "out vec3 source;"
            + "out vec3 viewVector;"
            + "\n"
            + "uniform mat4 modelMatrix;"
            + "uniform mat4 viewMatrix;"
            + "uniform mat4 projectionMatrix;"
            + "uniform vec4 lightPosition;"
            + "uniform mediump int isLightingEnabled;"
            + "\n"
            + "void main(void)"
            + "{"
            + "   if(isLightingEnabled == 1)"
            + "   {"
            + "       vec4 eyeCoordinates = viewMatrix * modelMatrix * vertexPosition;"
            + "       tNormal = mat3(viewMatrix * modelMatrix) * vertexNormal;"
            + "       source = vec3(lightPosition) - eyeCoordinates.xyz;"
            + "       viewVector = -eyeCoordinates.xyz;"
            + "   }"
            + "\n"
            + "   gl_Position = projectionMatrix * viewMatrix * modelMatrix * vertexPosition;"
            + "}"
        );

        GLES32.glShaderSource(this.vertexShaderObjectPerFragment, vertexShaderCode);
        GLES32.glCompileShader(this.vertexShaderObjectPerFragment);

        int[] infoLogLength = new int[1];
        int[] shaderCompileStatus = new int[1];
        String infoLog = null;

        GLES32.glGetShaderiv(this.vertexShaderObjectPerFragment, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);

        if(shaderCompileStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetShaderiv(this.vertexShaderObjectPerFragment, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);

            if(infoLogLength[0] > 0) {
                infoLog = GLES32.glGetShaderInfoLog(this.vertexShaderObjectPerFragment);
                System.out.println("CG | Vertex shader compilation log: " + infoLog);
                cleanUp();
                System.exit(1);
            }
        }
    }

    private void initializeFragmentShaderObjectPerFragment() {
        this.fragmentShaderObjectPerFragment = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

        final String fragmentShaderCode = String.format(
            "#version 320 es"
            + "\n"
            + "precision highp float;"
            + "\n"
            + "in vec3 tNormal;"
            + "in vec3 source;"
            + "in vec3 viewVector;"
            + "\n"
            + "out vec4 fragmentColor;"
            + "\n"
            + "uniform int isLightingEnabled;"
            + "uniform vec3 la;"
            + "uniform vec3 ld;"
            + "uniform vec3 ls;"
            + "uniform vec3 ka;"
            + "uniform vec3 kd;"
            + "uniform vec3 ks;"
            + "uniform float materialShininess;"
            + "\n"
            + "void main(void)"
            + "{"
            + "   vec3 phongAdsColor;"
            + "   if(isLightingEnabled == 1)"
            + "   {"
            + "       vec3 normalizedTNormal = normalize(tNormal);"
            + "       vec3 normalizedSource = normalize(source);"
            + "       vec3 normalizedViewVector = normalize(viewVector);"
            + "       float tNormalDotLightDirection = max(dot(normalizedTNormal, normalizedSource), 0.0);"
            + "       vec3 ambient = la * ka;"
            + "       vec3 diffuse = ld * kd * tNormalDotLightDirection;"
            + "       vec3 reflectionVector = reflect(-normalizedSource, normalizedTNormal);"
            + "       vec3 specular = ls * ks * pow(max(dot(reflectionVector, normalizedViewVector), 0.0), materialShininess);"
            + "       phongAdsColor = ambient + diffuse + specular;"
            + "   }"
            + "   else"
            + "   {"
            + "       phongAdsColor = vec3(1.0, 1.0, 1.0);"
            + "   }"
            + "\n"
            + "   fragmentColor = vec4(phongAdsColor, 1.0);"
            + "}"
        );

        GLES32.glShaderSource(this.fragmentShaderObjectPerFragment, fragmentShaderCode);
        GLES32.glCompileShader(this.fragmentShaderObjectPerFragment);

        int[] infoLogLength = new int[1];
        int[] shaderCompileStatus = new int[1];
        String infoLog = null;

        GLES32.glGetShaderiv(this.fragmentShaderObjectPerFragment, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);

        if(shaderCompileStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetShaderiv(this.fragmentShaderObjectPerFragment, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);

            if(infoLogLength[0] > 0) {
                infoLog = GLES32.glGetShaderInfoLog(this.fragmentShaderObjectPerFragment);
                System.out.println("CG | Fragment shader compilation log: " + infoLog);
                cleanUp();
                System.exit(1);
            }
        }
    }

    private void initializeShaderProgramObjectPerFragment() {
        this.shaderProgramObjectPerFragment = GLES32.glCreateProgram();
        GLES32.glAttachShader(this.shaderProgramObjectPerFragment, this.vertexShaderObjectPerFragment);
        GLES32.glAttachShader(this.shaderProgramObjectPerFragment, this.fragmentShaderObjectPerFragment);

        // Bind the position attribute location before linking.
        GLES32.glBindAttribLocation(this.shaderProgramObjectPerFragment, GLESMacros.CG_ATTRIBUTE_VERTEX_POSITION, "vertexPosition");

        // Bind the normal attribute location before linking.
        GLES32.glBindAttribLocation(shaderProgramObjectPerFragment, GLESMacros.CG_ATTRIBUTE_NORMAL, "vertexNormal");

        // Now link and check for error.
        GLES32.glLinkProgram(this.shaderProgramObjectPerFragment);

        int[] infoLogLength = new int[1];
        int[] shaderProgramLinkStatus = new int[1];
        String infoLog = null;

        GLES32.glGetProgramiv(this.shaderProgramObjectPerFragment, GLES32.GL_LINK_STATUS, shaderProgramLinkStatus, 0);

        if(shaderProgramLinkStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetProgramiv(this.shaderProgramObjectPerFragment, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);

            if(infoLogLength[0] > 0) {
                infoLog = GLES32.glGetProgramInfoLog(this.shaderProgramObjectPerFragment);
                System.out.println("CG | Shader program link log: " + infoLog);
                cleanUp();
                System.exit(1);
            }
        }

        // After linking get the value of modelView and projection uniform location from the shader program.
        this.modelMatrixUniformPerFragment = GLES32.glGetUniformLocation(this.shaderProgramObjectPerFragment, "modelMatrix");
        this.viewMatrixUniformPerFragment = GLES32.glGetUniformLocation(this.shaderProgramObjectPerFragment, "viewMatrix");
        this.projectionMatrixUniformPerFragment = GLES32.glGetUniformLocation(this.shaderProgramObjectPerFragment, "projectionMatrix");
        this.isLightingEnabledUniformPerFragment = GLES32.glGetUniformLocation(this.shaderProgramObjectPerFragment, "isLightingEnabled");

        this.laUniformPerFragment = GLES32.glGetUniformLocation(this.shaderProgramObjectPerFragment, "la");
        this.ldUniformPerFragment = GLES32.glGetUniformLocation(this.shaderProgramObjectPerFragment, "ld");
        this.lsUniformPerFragment = GLES32.glGetUniformLocation(this.shaderProgramObjectPerFragment, "ls");
        this.kaUniformPerFragment = GLES32.glGetUniformLocation(this.shaderProgramObjectPerFragment, "ka");
        this.kdUniformPerFragment = GLES32.glGetUniformLocation(this.shaderProgramObjectPerFragment, "kd");
        this.ksUniformPerFragment = GLES32.glGetUniformLocation(this.shaderProgramObjectPerFragment, "ks");
        this.lightPositionUniformPerFragment = GLES32.glGetUniformLocation(this.shaderProgramObjectPerFragment, "lightPosition");
        this.materialShininessUniformPerFragment = GLES32.glGetUniformLocation(this.shaderProgramObjectPerFragment, "materialShininess");
    }

    private void initializeSphereBuffersPerFragment() {
        GLES32.glGenVertexArrays(1, this.vaoSpherePerFragment, 0);
        GLES32.glBindVertexArray(this.vaoSpherePerFragment[0]);

        GLES32.glGenBuffers(1, this.vboSpherePositionPerFragment, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vboSpherePositionPerFragment[0]);

        ByteBuffer verticesByteBuffer = ByteBuffer.allocateDirect(this.sphereVertices.length * 4);
        verticesByteBuffer.order(ByteOrder.nativeOrder());

        FloatBuffer verticesBuffer = verticesByteBuffer.asFloatBuffer();
        verticesBuffer.put(this.sphereVertices);
        verticesBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, this.sphereVertices.length * 4, verticesBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.CG_ATTRIBUTE_VERTEX_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.CG_ATTRIBUTE_VERTEX_POSITION);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        GLES32.glGenBuffers(1, this.vboSphereNormalPerFragment, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vboSphereNormalPerFragment[0]);

        ByteBuffer sphereNormalsByteBuffer = ByteBuffer.allocateDirect(this.sphereNormals.length * 4);
        sphereNormalsByteBuffer.order(ByteOrder.nativeOrder());

        FloatBuffer sphereNormalsBuffer = sphereNormalsByteBuffer.asFloatBuffer();
        sphereNormalsBuffer.put(this.sphereNormals);
        sphereNormalsBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, this.sphereNormals.length * 4, sphereNormalsBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.CG_ATTRIBUTE_NORMAL, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.CG_ATTRIBUTE_NORMAL);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        GLES32.glGenBuffers(1, this.vboSphereElementPerFragment, 0);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, this.vboSphereElementPerFragment[0]);

        ByteBuffer sphereElementByteBuffer = ByteBuffer.allocateDirect(this.sphereElements.length * 2);
        sphereElementByteBuffer.order(ByteOrder.nativeOrder());

        ShortBuffer sphereElementsBuffer = sphereElementByteBuffer.asShortBuffer();
        sphereElementsBuffer.put(this.sphereElements);
        sphereElementsBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER, this.sphereElements.length * 2, sphereElementsBuffer, GLES32.GL_STATIC_DRAW);

        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER,0);

        GLES32.glBindVertexArray(0);
    }

    void display() {
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT | GLES32.GL_STENCIL_BUFFER_BIT);

        if(this.isPerFragmentLightEnabled) {
            this.usePerFragmentLight();
        }
        else {
            this.usePerVertexLight();
        }

        this.requestRender();
    }

    private void usePerVertexLight() {
        GLES32.glUseProgram(this.shaderProgramObjectPerVertex);

        if(this.isLightingEnabled) {
            GLES32.glUniform1i(this.isLightingEnabledUniformPerVertex, 1);
            GLES32.glUniform3fv(this.laUniformPerVertex, 1, this.lightAmbient, 0);
            GLES32.glUniform3fv(this.ldUniformPerVertex, 1, this.lightDiffuse, 0);
            GLES32.glUniform3fv(this.lsUniformPerVertex, 1, this.lightSpecular, 0);
            GLES32.glUniform4fv(this.lightPositionUniformPerVertex, 1, this.lightPosition, 0);

            GLES32.glUniform3fv(this.kaUniformPerVertex, 1, this.materialAmbient, 0);
            GLES32.glUniform3fv(this.kdUniformPerVertex, 1, this.materialDiffuse, 0);
            GLES32.glUniform3fv(this.ksUniformPerVertex, 1, this.materialSpecular, 0);
            GLES32.glUniform1f(this.materialShininessUniformPerVertex, this.materialShininess);
        }
        else {
            GLES32.glUniform1i(this.isLightingEnabledUniformPerVertex, 0);
        }

        this.drawSpherePerVertex();

        GLES32.glUseProgram(0);
    }

    private void usePerFragmentLight() {
        GLES32.glUseProgram(this.shaderProgramObjectPerFragment);

        if(this.isLightingEnabled) {
            GLES32.glUniform1i(this.isLightingEnabledUniformPerFragment, 1);
            GLES32.glUniform3fv(this.laUniformPerFragment, 1, this.lightAmbient, 0);
            GLES32.glUniform3fv(this.ldUniformPerFragment, 1, this.lightDiffuse, 0);
            GLES32.glUniform3fv(this.lsUniformPerFragment, 1, this.lightSpecular, 0);
            GLES32.glUniform4fv(this.lightPositionUniformPerFragment, 1, this.lightPosition, 0);

            GLES32.glUniform3fv(this.kaUniformPerFragment, 1, this.materialAmbient, 0);
            GLES32.glUniform3fv(this.kdUniformPerFragment, 1, this.materialDiffuse, 0);
            GLES32.glUniform3fv(this.ksUniformPerFragment, 1, this.materialSpecular, 0);
            GLES32.glUniform1f(this.materialShininessUniformPerFragment, this.materialShininess);
        }
        else {
            GLES32.glUniform1i(this.isLightingEnabledUniformPerFragment, 0);
        }

        this.drawSpherePerFragment();

        GLES32.glUseProgram(0);
    }

    private void drawSpherePerVertex() {
        float[] modelMatrix = new float[16];
        float[] viewMatrix = new float[16];

        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);

        // Translate the modal matrix.
        Matrix.translateM(modelMatrix, 0, 0.0f, 0.0f, -3.0f);

        // Pass modelMatrix and viewProjectionMatrix to vertex shader in 'modelMatrixUniformPerVertex' and 'viewMatrixUniformPerVertex'
        // and projectionMatrixUniformPerVertex variable defined in shader.
        GLES32.glUniformMatrix4fv(this.modelMatrixUniformPerVertex, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(this.viewMatrixUniformPerVertex, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(this.projectionMatrixUniformPerVertex, 1, false, this.perspectiveProjectionMatrix, 0);

        // Now bind the vaoSpherePerVertex to which we want to use
        GLES32.glBindVertexArray(vaoSpherePerVertex[0]);

        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vboSphereElementPerVertex[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, this.numberOfSphereElements, GLES32.GL_UNSIGNED_SHORT, 0);

        // unbind the vaoSpherePerVertex
        GLES32.glBindVertexArray(0);
    }

    private void drawSpherePerFragment() {
        float[] modelMatrix = new float[16];
        float[] viewMatrix = new float[16];

        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);

        // Translate the modal matrix.
        Matrix.translateM(modelMatrix, 0, 0.0f, 0.0f, -3.0f);

        // Pass modelMatrix and viewProjectionMatrix to vertex shader in 'modelMatrixUniformPerFragment' and 'viewMatrixUniformPerFragment'
        // and projectionMatrixUniformPerFragment variable defined in shader.
        GLES32.glUniformMatrix4fv(this.modelMatrixUniformPerFragment, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(this.viewMatrixUniformPerFragment, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(this.projectionMatrixUniformPerFragment, 1, false, this.perspectiveProjectionMatrix, 0);

        // Now bind the vaoSpherePerFragment to which we want to use
        GLES32.glBindVertexArray(vaoSpherePerFragment[0]);

        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vboSphereElementPerFragment[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, this.numberOfSphereElements, GLES32.GL_UNSIGNED_SHORT, 0);

        // unbind the vaoSpherePerFragment
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
        if(this.vaoSpherePerVertex[0] != 0) {
            GLES32.glDeleteVertexArrays(1, this.vaoSpherePerVertex, 0);
            this.vaoSpherePerVertex[0] = 0;
        }

        if(this.vboSpherePositionPerVertex[0] != 0) {
            GLES32.glDeleteBuffers(1, this.vboSpherePositionPerVertex, 0);
            this.vboSpherePositionPerVertex[0] = 0;
        }

       if(this.vboSphereNormalPerVertex[0] != 0) {
            GLES32.glDeleteBuffers(1, this.vboSphereNormalPerVertex, 0);
            this.vboSphereNormalPerVertex[0] = 0;
        }

        if(this.shaderProgramObjectPerVertex != 0) {
            if(this.vertexShaderObjectPerVertex != 0) {
                GLES32.glDetachShader(this.shaderProgramObjectPerVertex, this.vertexShaderObjectPerVertex);
            }

            if(this.fragmentShaderObjectPerVertex != 0) {
                GLES32.glDetachShader(this.shaderProgramObjectPerVertex, this.fragmentShaderObjectPerVertex);
            }
        }

        if(this.vertexShaderObjectPerVertex != 0) {
            GLES32.glDeleteShader(this.vertexShaderObjectPerVertex);
            this.vertexShaderObjectPerVertex = 0;
        }

        if(this.fragmentShaderObjectPerVertex != 0) {
            GLES32.glDeleteShader(this.fragmentShaderObjectPerVertex);
            this.fragmentShaderObjectPerVertex = 0;
        }

        if(this.shaderProgramObjectPerVertex != 0) {
            GLES32.glDeleteProgram(this.shaderProgramObjectPerVertex);
            this.shaderProgramObjectPerVertex = 0;
        }

        if(this.vaoSpherePerFragment[0] != 0) {
            GLES32.glDeleteVertexArrays(1, this.vaoSpherePerFragment, 0);
            this.vaoSpherePerFragment[0] = 0;
        }

        if(this.vboSpherePositionPerFragment[0] != 0) {
            GLES32.glDeleteBuffers(1, this.vboSpherePositionPerFragment, 0);
            this.vboSpherePositionPerFragment[0] = 0;
        }

       if(this.vboSphereNormalPerFragment[0] != 0) {
            GLES32.glDeleteBuffers(1, this.vboSphereNormalPerFragment, 0);
            this.vboSphereNormalPerFragment[0] = 0;
        }

        if(this.shaderProgramObjectPerFragment != 0) {
            if(this.vertexShaderObjectPerFragment != 0) {
                GLES32.glDetachShader(this.shaderProgramObjectPerFragment, this.vertexShaderObjectPerFragment);
            }

            if(this.fragmentShaderObjectPerFragment != 0) {
                GLES32.glDetachShader(this.shaderProgramObjectPerFragment, this.fragmentShaderObjectPerFragment);
            }
        }

        if(this.vertexShaderObjectPerFragment != 0) {
            GLES32.glDeleteShader(this.vertexShaderObjectPerFragment);
            this.vertexShaderObjectPerFragment = 0;
        }

        if(this.fragmentShaderObjectPerFragment != 0) {
            GLES32.glDeleteShader(this.fragmentShaderObjectPerFragment);
            this.fragmentShaderObjectPerFragment = 0;
        }

        if(this.shaderProgramObjectPerFragment != 0) {
            GLES32.glDeleteProgram(this.shaderProgramObjectPerFragment);
            this.shaderProgramObjectPerFragment = 0;
        }

        GLES32.glUseProgram(0);
    }
}
