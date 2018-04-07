package com.github.chetangandhi.sphereWithLight;

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

public class SphereWithLight extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener {

    private final Context context;
    private GestureDetector gestureDetector = null;

    private boolean isLightingEnabled = false;

    private int singleTapCounter = 1;
    private int numberOfSphereVertices = 0;
    private int numberOfSphereElements = 0;
    private int vertexShaderObject = 0;
    private int fragmentShaderObject = 0;
    private int shaderProgramObject = 0;
    private int modelViewMatrixUniform = 0;
    private int projectionMatrixUniform = 0;
    private int ldUniform = 0;
    private int kdUniform = 0;
    private int lightPositionUniform = 0;
    private int isLightingEnabledUniform = 0;

    private int[] vaoSphere = new int[1];
    private int[] vboSpherePosition = new int[1];
    private int[] vboSphereNormal = new int[1];
    private int[] vboSphereElement = new int[1];

    private float[] perspectiveProjectionMatrix = new float[16];

    private float[] sphereVertices = new float[1146];
    private float[] sphereNormals = new float[1146];
    private float[] sphereTextures = new float[764];
    private short[] sphereElements = new short[2280];

    SphereWithLight(Context context) {
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
            + "out vec3 diffuseLight;"
            + "\n"
            + "uniform mediump int isLightingEnabled;"
            + "uniform mat4 modelViewMatrix;"
            + "uniform mat4 projectionMatrix;"
            + "uniform vec3 ld;"
            + "uniform vec3 kd;"
            + "uniform vec4 lightPosition;"
            + "\n"
            + "void main(void)"
            + "{"
            + "   if(isLightingEnabled == 1);"
            + "   {"
            + "       vec4 eyeCoordinates = modelViewMatrix * vertexPosition;"
            + "       vec3 tNormal = normalize(mat3(modelViewMatrix) * vertexNormal);"
            + "       vec3 source = normalize(vec3(lightPosition - eyeCoordinates));"
            + "       diffuseLight = ld * kd * max(dot(source, tNormal), 0.0);"
            + "   }"
            + "\n"
            + "   gl_Position = projectionMatrix * modelViewMatrix * vertexPosition;"
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
            + "in vec3 diffuseLight;"
            + "\n"
            + "out vec4 fragmentColor;"
            + "\n"
            + "uniform int isLightingEnabled;"
            + "\n"
            + "void main(void)"
            + "{"
            + "   if(isLightingEnabled == 1)"
            + "   {"
            + "       fragmentColor = vec4(diffuseLight, 1.0);"
            + "   }"
            + "   else"
            + "   {"
            + "       fragmentColor = vec4(1.0, 1.0, 1.0, 1.0);"
            + "   }"
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

        // After linking get the value of modelView and projection uniform location from the shader program.
        this.modelViewMatrixUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "modelViewMatrix");
        this.projectionMatrixUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "projectionMatrix");

        this.isLightingEnabledUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "isLightingEnabled");
        this.ldUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "ld");
        this.kdUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "kd");
        this.lightPositionUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "lightPositionUniform");
    }

    private void initializeSphereBuffers() {
        GLES32.glGenVertexArrays(1, this.vaoSphere, 0);
        GLES32.glBindVertexArray(this.vaoSphere[0]);

        GLES32.glGenBuffers(1, this.vboSpherePosition, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vboSpherePosition[0]);

        ByteBuffer verticesByteBuffer = ByteBuffer.allocateDirect(this.sphereVertices.length * 4);
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

        ByteBuffer sphereNormalsByteBuffer = ByteBuffer.allocateDirect(this.sphereNormals.length * 4);
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

    private void display() {
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT | GLES32.GL_STENCIL_BUFFER_BIT);
        GLES32.glUseProgram(this.shaderProgramObject);

        if(isLightingEnabled) {
            GLES32.glUniform1i(this.isLightingEnabledUniform, 1);
            GLES32.glUniform3f(this.ldUniform, 1.0f, 1.0f, 1.0f);
            GLES32.glUniform3f(this.kdUniform, 0.5f, 0.5f, 0.5f);

            float lightPosition[] = { 0.0f, 0.0f, 2.0f, 1.0f };
            GLES32.glUniform4fv(this.lightPositionUniform, 1, lightPosition, 0);
        }
        else {
            GLES32.glUniform1i(this.isLightingEnabledUniform, 0);
        }

        this.drawSphere();

        GLES32.glUseProgram(0);

        this.requestRender();
    }

    private void drawSphere() {
        float[] modelViewMatrix = new float[16];

        Matrix.setIdentityM(modelViewMatrix, 0);

        // Translate the modal view matrix.
        Matrix.translateM(modelViewMatrix, 0, 0.0f, 0.0f, -3.0f);

        // Pass modelViewMatrix and modelViewProjectionMatrix to vertex shader in 'modelViewUniform'
        // and projectionMatrixUniform variable defined in shader.
        GLES32.glUniformMatrix4fv(this.modelViewMatrixUniform, 1, false, modelViewMatrix, 0);
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
