#import "GLView.h"
#import "sphere.h"

enum
{
    CG_ATTRIBUTE_VERTEX_POSITION = 0,
	CG_ATTRIBUTE_COLOR,
	CG_ATTRIBUTE_NORMAL,
	CG_ATTRIBUTE_TEXTURE0,
};

CVReturn displayLinkCallBack(CVDisplayLinkRef displayLinkRef, const CVTimeStamp *now, const CVTimeStamp *outTime, CVOptionFlags inFlags, CVOptionFlags *outFlags, void *displayLinkContext);

const int numberOfRows = 6;
const int numberOfColumns = 4;

@implementation GLView
{
    BOOL isLightingEnabled;
    BOOL isAnimationEnabled;
    BOOL rotateLightOnXAxis;
    BOOL rotateLightOnYAxis;
    BOOL rotateLightOnZAxis;

    int viewportWidth;
    int viewportHeight;
    int viewportInitialX;
    int viewportInitialY;

    CVDisplayLinkRef displayLinkRef;
    GLuint vertexShaderObject;
    GLuint fragmentShaderObject;
    GLuint shaderProgramObject;

    GLuint vaoSphere;
    GLuint vboSpherePosition;
    GLuint vboSphereNormal;
    GLuint vboSphereElement;
    GLuint modelMatrixUniform;
    GLuint viewMatrixUniform;
    GLuint projectionMatrixUniform;
    GLuint laZeroUniform;
    GLuint ldZeroUniform;
    GLuint lsZeroUniform;
    GLuint laOneUniform;
    GLuint kaUniform;
    GLuint kdUniform;
    GLuint ksUniform;
    GLuint materialShininessUniform;
    GLuint lightZeroPositionUniform;
    GLuint isLightingEnabledUniform;

    vmath::mat4 perspectiveProjectionMatrix;

    GLfloat angleLightRotation;
    GLfloat speed;

    GLfloat lightZeroAmbient[4];
    GLfloat lightZeroDiffuse[4];
    GLfloat lightZeroSpecular[4];
    GLfloat lightZeroPosition[4];

    GLfloat materialAmbient[numberOfColumns][numberOfRows][4];
    GLfloat materialDiffuse[numberOfColumns][numberOfRows][4];
    GLfloat materialSpecular[numberOfColumns][numberOfRows][4];
    GLfloat materialShininess[numberOfColumns][numberOfRows];

    SphereData sphereData;
}

-(id)initWithFrame:(NSRect)rect
{
    self = [super initWithFrame:rect];

    if(self)
    {
        NSOpenGLPixelFormatAttribute pixelFormatAttribute[] = {
            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core, // OpenGL 4.1 core profile.
            NSOpenGLPFAScreenMask, CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay), // The display attached to OpenGL context, main display.
            NSOpenGLPFANoRecovery,
            NSOpenGLPFAAccelerated,
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFADepthSize, 24,
            NSOpenGLPFAAlphaSize, 8,
            NSOpenGLPFADoubleBuffer,
            0
        };

        NSOpenGLPixelFormat *pixelFormat = [[[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAttribute] autorelease];

        if(!pixelFormat)
        {
            NSLog(@"Error | Not able to get valid OpenGL pixel format.");
            [self release];
            [NSApp terminate:self];
        }

        NSOpenGLContext *glContext = [[[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil] autorelease];

        [self setPixelFormat:pixelFormat];
        [self setOpenGLContext:glContext];

        displayLinkRef = 0;
        vertexShaderObject = 0;
        fragmentShaderObject = 0;
        shaderProgramObject = 0;

        vaoSphere = 0;
        vboSpherePosition = 0;
        vboSphereNormal = 0;
        vboSphereElement = 0;
        modelMatrixUniform = 0;
        viewMatrixUniform = 0;
        projectionMatrixUniform = 0;
        laZeroUniform = 0;
        ldZeroUniform = 0;
        lsZeroUniform = 0;
        kaUniform = 0;
        kdUniform = 0;
        ksUniform = 0;
        materialShininessUniform = 0;
        lightZeroPositionUniform = 0;
        isLightingEnabledUniform = 0;

        viewportWidth = 1;
        viewportHeight = 1;
        viewportInitialX = 0;
        viewportInitialY = 0;
        angleLightRotation = 0.0f;
        speed = 0.1f;

        GLfloat _lightZeroAmbient[] = {0.5f, 0.5f, 0.5f, 1.0f};
        GLfloat _lightZeroDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
        GLfloat _lightZeroSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
        GLfloat _lightZeroPosition[] = {0.0f, 0.0f, 1.0f, 0.0f};

        GLfloat _materialAmbient[numberOfColumns][numberOfRows][4] = {
            {   // Column 1
                {0.0215f, 0.1745f, 0.0215f, 1.0f },
                {0.135f, 0.2225f, 0.1575f, 1.0f },
                {0.05375f, 0.05f, 0.06625f, 1.0f },
                {0.25f, 0.20725f, 0.20725f, 1.0f },
                {0.1745f, 0.01175f, 0.01175f, 1.0f },
                {0.1f, 0.18725f, 0.1745f, 1.0f }
            },
            {   // Column 2
                {0.329412f, 0.223529f, 0.027451f, 1.0f },
                {0.2125f, 0.1275f, 0.054f, 1.0f },
                {0.25f, 0.25f, 0.25f, 1.0f },
                {0.19125f, 0.0735f, 0.0225f, 1.0f },
                {0.24725f, 0.1995f, 0.0745f, 1.0f },
                {0.19225f, 0.19225f, 0.19225f, 1.0f }
            },
            {   // Column 3
                {0.0f, 0.0f, 0.0f, 1.0f },
                {0.0f, 0.1f, 0.06f, 1.0f },
                {0.0f, 0.0f, 0.0f, 1.0f },
                {0.0f, 0.0f, 0.0f, 1.0f },
                {0.0f, 0.0f, 0.0f, 1.0f },
                {0.0f, 0.0f, 0.0f, 1.0f }
            },
            {   // Column 4
                {0.02f, 0.02f, 0.02f, 1.0f },
                {0.0f, 0.05f, 0.05f, 1.0f },
                {0.0f, 0.05f, 0.0f, 1.0f },
                {0.05f, 0.0f, 0.0f, 1.0f },
                {0.05f, 0.05f, 0.05f, 1.0f },
                {0.05f, 0.05f, 0.0f, 1.0f }
            }
        };

        GLfloat _materialDiffuse[numberOfColumns][numberOfRows][4] = {
            {   // Column 1
                {0.07568f, 0.61424f, 0.07568f, 1.0f},
                {0.54f, 0.89f, 0.63f, 1.0f},
                {0.18275f, 0.17f, 0.22525f, 1.0f},
                {1.0f, 0.829f, 0.829f, 1.0f},
                {0.61424f, 0.04136f, 0.04136f, 1.0f},
                {0.396f, 0.74151f, 0.69102f, 1.0f},
            },
            {   // Column 2
                {0.780392f, 0.568627f, 0.113725f, 1.0f},
                {0.714f, 0.4284f, 0.18144f, 1.0f},
                {0.4f, 0.4f, 0.4f, 1.0f},
                {0.7038f, 0.27048f, 0.0828f, 1.0f},
                {0.75164f, 0.60648f, 0.22648f, 1.0f},
                {0.50754f, 0.50754f, 0.50754f, 1.0f},
            },
            {   // Column 3
                {0.01f, 0.01f, 0.01f, 1.0f},
                {0.0f, 0.50980392f, 0.50980392f, 1.0f},
                {0.1f, 0.35f, 0.1f, 1.0f},
                {0.5f, 0.0f, 0.0f, 1.0f},
                {0.55f, 0.55f, 0.55f, 1.0f},
                {0.5f, 0.5f, 0.0f, 1.0f},
            },
            {   // Column 4
                {0.01f, 0.01f, 0.01f, 1.0f},
                {0.4f, 0.5f, 0.5f, 1.0f},
                {0.4f, 0.5f, 0.4f, 1.0f},
                {0.5f, 0.4f, 0.4f, 1.0f},
                {0.5f, 0.5f, 0.5f, 1.0f},
                {0.5f, 0.5f, 0.4f, 1.0f},
            },
        };

        GLfloat _materialSpecular[numberOfColumns][numberOfRows][4] = {
            {   // Column 1
                {0.633f, 0.727811f, 0.633f, 1.0f},
                {0.316228f, 0.316228f, 0.316228f, 1.0f},
                {0.332741f, 0.328634f, 0.346435f, 1.0f},
                {0.296648f, 0.296648f, 0.296648f, 1.0f},
                {0.727811f, 0.626959f, 0.626959f, 1.0f},
                {0.297254f, 0.30829f, 0.306678f, 1.0f},
            },
            {   // Column 2
                {0.992157f, 0.941176f, 0.807843f, 1.0f},
                {0.393548f, 0.271906f, 0.166721f, 1.0f},
                {0.774597f, 0.774597f, 0.774597f, 1.0f},
                {0.256777f, 0.137622f, 0.086014f, 1.0f},
                {0.628281f, 0.555802f, 0.366065f, 1.0f},
                {0.508273f, 0.508273f, 0.508273f, 1.0f},
            },
            {   // Column 3
                {0.50f, 0.50f, 0.50f, 1.0f},
                {0.50196078f, 0.50196078f, 0.50196078f, 1.0f},
                {0.45f, 0.55f, 0.45f, 1.0f},
                {0.7f, 0.6f, 0.6f, 1.0f},
                {0.70f, 0.70f, 0.70f, 1.0f},
                {0.60f, 0.60f, 0.50f, 1.0f},
            },
            {   // Column 4
                {0.4f, 0.4f, 0.4f, 1.0f},
                {0.04f, 0.7f, 0.7f, 1.0f},
                {0.04f, 0.7f, 0.04f, 1.0f},
                {0.7f, 0.04f, 0.04f, 1.0f},
                {0.7f, 0.7f, 0.7f, 1.0f},
                {0.7f, 0.7f, 0.04f, 1.0f},
            }
        };

        GLfloat _materialShininess[numberOfColumns][numberOfRows] = {
            {   // Column 1
                0.6f * 128.0f,
                0.1f * 128.0f,
                0.3f * 128.0f,
                0.088f * 128.0f,
                0.6f * 128.0f,
                0.1f * 128.0f
            },
            {   // Column 2
                0.21794872f * 128.0f,
                0.2f * 128.0f,
                0.6f * 128.0f,
                0.1f * 128.0f,
                0.4f * 128.0f,
                0.4f * 128.0f
            },
            {   // Column 3
                0.25f * 128.0f,
                0.25f * 128.0f,
                0.25f * 128.0f,
                0.25f * 128.0f,
                0.25f * 128.0f,
                0.25f * 128.0f
            },
            {   // Column 4
                0.078125f * 128.0f,
                0.078125f * 128.0f,
                0.078125f * 128.0f,
                0.078125f * 128.0f,
                0.078125f * 128.0f,
                0.078125f * 128.0f
            }
        };

        memset(lightZeroAmbient, 0, sizeof(lightZeroAmbient));
        memset(lightZeroDiffuse, 0, sizeof(lightZeroDiffuse));
        memset(lightZeroSpecular, 0, sizeof(lightZeroSpecular));
        memset(lightZeroPosition, 0, sizeof(lightZeroPosition));
        memset(materialAmbient, 0, sizeof(materialAmbient));
        memset(materialDiffuse, 0, sizeof(materialDiffuse));
        memset(materialSpecular, 0, sizeof(materialSpecular));
        memset(materialShininess, 0, sizeof(materialShininess));

        memcpy(lightZeroAmbient, _lightZeroAmbient, sizeof(lightZeroAmbient));
        memcpy(lightZeroDiffuse, _lightZeroDiffuse, sizeof(lightZeroDiffuse));
        memcpy(lightZeroSpecular, _lightZeroSpecular, sizeof(lightZeroSpecular));
        memcpy(lightZeroPosition, _lightZeroPosition, sizeof(lightZeroPosition));
        memcpy(materialAmbient, _materialAmbient, sizeof(materialAmbient));
        memcpy(materialDiffuse, _materialDiffuse, sizeof(materialDiffuse));
        memcpy(materialSpecular, _materialSpecular, sizeof(materialSpecular));
        memcpy(materialShininess, _materialShininess, sizeof(materialShininess));
    }

    return self;
}

-(BOOL)acceptsFirstResponder
{
    [[self window] makeFirstResponder:self];
    return YES;
}

-(void)keyDown:(NSEvent *)event
{
    int key= (int)[[event characters] characterAtIndex:0];

    switch (key) {
            // Esc key code
        case 27:
            [self release];
            [NSApp terminate:self];
            break;

        case 'A':
        case 'a':
            isAnimationEnabled = !isAnimationEnabled;
            break;

        case 'F':
        case 'f':
            [[self window] toggleFullScreen:self]; // This will auto invoke repainting.
            break;

        case 'L':
        case 'l':
            isLightingEnabled = !isLightingEnabled;
            break;

        case 'X':
        case 'x':
            rotateLightOnXAxis = true;
            rotateLightOnYAxis = false;
            rotateLightOnZAxis = false;
            break;

        case 'Y':
        case 'y':
            rotateLightOnXAxis = false;
            rotateLightOnYAxis = true;
            rotateLightOnZAxis = false;
            break;

        case 'Z':
        case 'z':
            rotateLightOnXAxis = false;
            rotateLightOnYAxis = false;
            rotateLightOnZAxis = true;
            break;

        default:
            break;
    }

    if(key > '0' && key <= '9')
    {
        speed = 0.1f * (key - '0');
    }
}

-(void)mouseDown:(NSEvent *)event
{
    [self setNeedsDisplay:YES]; // specifies that repainting is needed.
}

-(void)rightMouseDown:(NSEvent *)event
{
    [self setNeedsDisplay:YES]; // specifies that repainting is needed.
}

-(void)mouseDragged:(NSEvent *)event
{

}

-(void)prepareOpenGL
{
    NSLog(@"OpenGL Version: %s", glGetString(GL_VERSION));
    NSLog(@"GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    [[self openGLContext] makeCurrentContext];

    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

    [self listExtensions];

    Sphere *sphere = new Sphere();
    sphere->getSphereVertexData(&sphereData);

    delete sphere;
    sphere = NULL;

    // Initialize the shaders and shader program object.
    [self initializeVertexShader];
    [self initializeFragmentShader];
    [self initializeShaderProgram];
    [self initializeSphereBuffers];

    glClearColor(0.25f, 0.25f, 0.25f, 0.25f);
    glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);

    // Disable face culling to see back side of object when rotated.
    // glEnable(GL_CULL_FACE);

    glDepthFunc(GL_LEQUAL);

    perspectiveProjectionMatrix = vmath::mat4::identity();

    isLightingEnabled = false;
    rotateLightOnXAxis = true;
    rotateLightOnYAxis = false;
    rotateLightOnZAxis = false;

    CVDisplayLinkCreateWithActiveCGDisplays(&displayLinkRef);
    CVDisplayLinkSetOutputCallback(displayLinkRef, displayLinkCallBack, self);

    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat] CGLPixelFormatObj];

    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLinkRef, cglContext, cglPixelFormat);
    CVDisplayLinkStart(displayLinkRef);
}

-(void)listExtensions
{
    GLint extensionCount = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCount);

    NSLog(@"Number of extensions: %d\n", extensionCount);

    for(int counter = 0; counter < extensionCount; ++counter)
    {
        NSLog(@"%d] Extension name: %s\n", counter + 1, (const char*)glGetStringi(GL_EXTENSIONS, counter));
    }
}

-(void)initializeVertexShader
{
    vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

    const GLchar *vertexShaderCode = "#version 410" \
        "\n" \
        "in vec4 vertexPosition;" \
        "in vec3 vertexNormal;" \
        "\n" \
        "out vec3 tNormal;" \
		"out vec3 lightZeroDirection;" \
		"out vec4 eyeCoordinates;" \
		"out vec3 viewVector;" \
		"\n" \
        "uniform mat4 modelMatrix;" \
        "uniform mat4 viewMatrix;" \
        "uniform mat4 projectionMatrix;" \
        "uniform vec4 lightZeroPosition;" \
        "uniform int isLightingEnabled;" \
        "\n" \
        "void main(void)" \
        "{" \
        "   if(isLightingEnabled == 1)" \
        "   {" \
        "       eyeCoordinates = viewMatrix * modelMatrix * vertexPosition;" \
        "       tNormal = mat3(viewMatrix * modelMatrix) * vertexNormal;" \
        "       lightZeroDirection = vec3(lightZeroPosition);" \
        "       viewVector = -eyeCoordinates.xyz;" \
        "   }" \
        "\n" \
        "   gl_Position = projectionMatrix * viewMatrix * modelMatrix * vertexPosition;" \
        "}";

    glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderCode, NULL);
    glCompileShader(vertexShaderObject);

    GLint infoLogLength = 0;
    GLint shaderCompileStatus = 0;
    char *infoLog = NULL;

    glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &shaderCompileStatus);

    if(shaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

        if(infoLogLength > 0)
        {
            infoLog = (char *)malloc(infoLogLength);

            if(infoLog != NULL)
            {
                GLsizei written = 0;
                glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, infoLog);
                NSLog(@"CG: Vertex shader compilation log: %s\n", infoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
}

-(void)initializeFragmentShader
{
    fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar *fragmentShaderCode = "#version 410" \
        "\n" \
		"in vec3 tNormal;" \
		"in vec3 lightZeroDirection;" \
		"in vec4 eyeCoordinates;" \
		"in vec3 viewVector;" \
        "\n" \
        "out vec4 fragmentColor;" \
        "\n" \
        "uniform int isLightingEnabled;" \
        "uniform vec3 laZero;" \
        "uniform vec3 ldZero;" \
        "uniform vec3 lsZero;" \
        "uniform vec3 ka;" \
        "uniform vec3 kd;" \
        "uniform vec3 ks;" \
        "uniform float materialShininess;" \
        "\n" \
        "void main(void)" \
        "{" \
        "   vec3 phongAdsColor = vec3(0.0, 0.0, 0.0);" \
        "   if(isLightingEnabled == 1)" \
        "   {" \
        "       vec3 normalizedTNormal = normalize(tNormal);" \
        "       vec3 normalizedViewVector = normalize(viewVector);" \
        "\n" \
        "       vec3 normalizedLightZeroDirection = normalize(lightZeroDirection - eyeCoordinates.xyz);" \
        "       float tNormalDotLightZeroDirection = max(dot(normalizedTNormal, normalizedLightZeroDirection), 0.0);" \
        "       vec3 ambientLightZero = laZero * ka;" \
        "       vec3 diffuseLightZero = ldZero * kd * tNormalDotLightZeroDirection;" \
        "       vec3 reflectionVectorLightZero = reflect(-normalizedLightZeroDirection, normalizedTNormal);" \
        "       vec3 specularLightZero = lsZero * ks * pow(max(dot(reflectionVectorLightZero, normalizedViewVector), 0.0), materialShininess);" \
        "\n" \
        "       phongAdsColor = ambientLightZero + diffuseLightZero + specularLightZero;" \
        "   }" \
        "   else" \
        "   {" \
        "       phongAdsColor = vec3(1.0, 1.0, 1.0);" \
        "   }" \
        "\n" \
        "   fragmentColor = vec4(phongAdsColor, 1.0);" \
        "}";

    glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderCode, NULL);
    glCompileShader(fragmentShaderObject);

    GLint infoLogLength = 0;
    GLint shaderCompileStatus = 0;
    char *infoLog = NULL;

    glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &shaderCompileStatus);

    if(shaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

        if(infoLogLength > 0)
        {
            infoLog = (char *)malloc(infoLogLength);

            if(infoLog != NULL)
            {
                GLsizei written = 0;
                glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, infoLog);
                NSLog(@"CG: Fragment shader compilation log: %s\n", infoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
}

-(void)initializeShaderProgram
{
    shaderProgramObject = glCreateProgram();

    glAttachShader(shaderProgramObject, vertexShaderObject);
    glAttachShader(shaderProgramObject, fragmentShaderObject);

    // Bind the position attribute location before linking.
    glBindAttribLocation(shaderProgramObject, CG_ATTRIBUTE_VERTEX_POSITION, "vertexPosition");

    // Bind the normal attribute location before linking.
    glBindAttribLocation(shaderProgramObject, CG_ATTRIBUTE_NORMAL, "vertexNormal");

    // Now link and check for error.
    glLinkProgram(shaderProgramObject);

    GLint infoLogLength = 0;
    GLint shaderProgramLinkStatus = 0;
    char *infoLog = NULL;

    glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &shaderProgramLinkStatus);

    if(shaderProgramLinkStatus == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);

        if(infoLogLength > 0)
        {
            infoLog = (char *)malloc(infoLogLength);

            if(infoLog != NULL)
            {
                GLsizei written = 0;
                glGetProgramInfoLog(shaderProgramObject, infoLogLength, &written, infoLog);
                NSLog(@"CG: Shader program link log: %s\n", infoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }

    // After linking get the value of modelView and projection uniform location from the shader program.
    modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "modelMatrix");
    viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "viewMatrix");
    projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "projectionMatrix");

    isLightingEnabledUniform = glGetUniformLocation(shaderProgramObject, "isLightingEnabled");

    laZeroUniform = glGetUniformLocation(shaderProgramObject, "laZero");
    ldZeroUniform = glGetUniformLocation(shaderProgramObject, "ldZero");
    lsZeroUniform = glGetUniformLocation(shaderProgramObject, "lsZero");
    kaUniform = glGetUniformLocation(shaderProgramObject, "ka");
    kdUniform = glGetUniformLocation(shaderProgramObject, "kd");
    ksUniform = glGetUniformLocation(shaderProgramObject, "ks");
    lightZeroPositionUniform = glGetUniformLocation(shaderProgramObject, "lightZeroPosition");
    materialShininessUniform = glGetUniformLocation(shaderProgramObject, "materialShininess");
}

-(void)initializeSphereBuffers
{
    glGenVertexArrays(1, &vaoSphere);
    glBindVertexArray(vaoSphere);

    glGenBuffers(1, &vboSpherePosition);
    glBindBuffer(GL_ARRAY_BUFFER, vboSpherePosition);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 1146, sphereData.vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(CG_ATTRIBUTE_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(CG_ATTRIBUTE_VERTEX_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vboSphereNormal);
    glBindBuffer(GL_ARRAY_BUFFER, vboSphereNormal);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 1146, sphereData.normals, GL_STATIC_DRAW);
    glVertexAttribPointer(CG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(CG_ATTRIBUTE_NORMAL);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vboSphereElement);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboSphereElement);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * 2280, sphereData.elements, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

-(CVReturn)getFrameForTime:(const CVTimeStamp *)outTime
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    [self render];

    [pool release];

    return kCVReturnSuccess;
}

-(void)drawRect:(NSRect)rect
{
    [self render];
}

-(void)updateScene
{
    if(!isAnimationEnabled)
    {
        return;
    }

    angleLightRotation += speed;

    if(angleLightRotation >= 360.0f)
    {
        angleLightRotation = 0.0f;
    }
}

-(void)render
{
    NSLog(@"Render");
    [[self openGLContext] makeCurrentContext];

    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];

    CGLLockContext(cglContext);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    for(int columnCounter = 0; columnCounter < numberOfColumns; ++columnCounter)
    {
        for(int rowCounter = 0; rowCounter < numberOfRows; ++rowCounter)
        {
            glViewport(viewportInitialX + (columnCounter * viewportWidth), viewportInitialY + (rowCounter * viewportHeight), viewportWidth, viewportHeight);

            glUseProgram(shaderProgramObject);

            if(isLightingEnabled)
            {
                glUniform1i(isLightingEnabledUniform, 1);

                glUniform3fv(laZeroUniform, 1, lightZeroAmbient);
                glUniform3fv(ldZeroUniform, 1, lightZeroDiffuse);
                glUniform3fv(lsZeroUniform, 1, lightZeroSpecular);

                if(rotateLightOnXAxis)
                {
                    lightZeroPosition[0] = 0.0f;
                    lightZeroPosition[1] = sinf(angleLightRotation) * 100.0f - 3.0f;
                    lightZeroPosition[2] = cosf(angleLightRotation) * 100.0f - 3.0f;

                    NSLog(@"lightZeroPosition: %f %f %f", lightZeroPosition[0],lightZeroPosition[1], lightZeroPosition[2]);
                }
                else if(rotateLightOnYAxis)
                {
                    lightZeroPosition[0] = sinf(angleLightRotation) * 100.0f - 3.0f;
                    lightZeroPosition[1] = 0.0f;
                    lightZeroPosition[2] = cosf(angleLightRotation) * 100.0f - 3.0f;
                }
                else if(rotateLightOnZAxis)
                {
                    lightZeroPosition[0] = sinf(angleLightRotation) * 100.0f - 3.0f;
                    lightZeroPosition[1] = cosf(angleLightRotation) * 100.0f - 3.0f;
                    lightZeroPosition[2] = 0.0f;
                }

                glUniform4fv(lightZeroPositionUniform, 1, lightZeroPosition);
                glUniform3fv(kaUniform, 1, materialAmbient[columnCounter][rowCounter]);
                glUniform3fv(kdUniform, 1, materialDiffuse[columnCounter][rowCounter]);
                glUniform3fv(ksUniform, 1, materialSpecular[columnCounter][rowCounter]);
                glUniform1f(materialShininessUniform, materialShininess[columnCounter][rowCounter]);
            }
            else
            {
                glUniform1i(isLightingEnabledUniform, 0);
            }

            [self drawSphere];

            glUseProgram(0);
        }
    }

    CGLFlushDrawable(cglContext);
    CGLUnlockContext(cglContext);

    [self updateScene];
}

-(void)drawSphere
{
    vmath::mat4 modelMatrix = vmath::mat4::identity();
    vmath::mat4 viewMatrix = vmath::mat4::identity();

    // Translate the modal matrix.
    modelMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
    // Pass modelMatrix to vertex shader in 'modelMatrix' variable defined in shader.
    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

    // Pass viewMatrix to vertex shader in 'viewMatrix' variable defined in shader.
    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);

    // Pass perspectiveProjectionMatrix to vertex shader in 'projectionMatrix' variable defined in shader.
    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

    // Now bind the VAO to which we want to use
    glBindVertexArray(vaoSphere);

    // Draw the sphere
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboSphereElement);
    glDrawElements(GL_TRIANGLES, sphereData.numberOfElements, GL_UNSIGNED_SHORT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // unbind the vao
    glBindVertexArray(0);
}

-(void)reshape
{
    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
    CGLLockContext(cglContext);

    NSRect rect = [self bounds];
    GLfloat width = rect.size.width;
    GLfloat height = rect.size.height;

    if(height == 0)
    {
        height = 1;
    }

    viewportWidth = width / numberOfRows;
    viewportHeight = height / numberOfRows;
    viewportInitialX = (width - (viewportWidth * numberOfColumns)) / 2;
    viewportInitialY = (height - (viewportHeight * numberOfRows)) / 2;
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f);
    CGLUnlockContext(cglContext);
}

-(void)dealloc
{
    CVDisplayLinkStop(displayLinkRef);
    CVDisplayLinkRelease(displayLinkRef);

    if(vaoSphere)
    {
        glDeleteVertexArrays(1, &vaoSphere);
        vaoSphere = 0;
    }

    if(vboSpherePosition)
    {
        glDeleteBuffers(1, &vboSpherePosition);
        vboSpherePosition = 0;
    }

    if(vboSphereNormal)
    {
        glDeleteBuffers(1, &vboSphereNormal);
        vboSphereNormal = 0;
    }

    if(vboSphereElement)
    {
        glDeleteBuffers(1, &vboSphereElement);
        vboSphereElement = 0;
    }

    if(shaderProgramObject)
    {
        if(vertexShaderObject)
        {
            glDetachShader(shaderProgramObject, vertexShaderObject);
        }

        if(fragmentShaderObject)
        {
            glDetachShader(shaderProgramObject, fragmentShaderObject);
        }
    }

    if(vertexShaderObject)
    {
        glDeleteShader(vertexShaderObject);
        vertexShaderObject = 0;
    }

    if(fragmentShaderObject)
    {
        glDeleteShader(fragmentShaderObject);
        fragmentShaderObject = 0;
    }

    if(shaderProgramObject)
    {
        glDeleteProgram(shaderProgramObject);
        shaderProgramObject = 0;
    }

    glUseProgram(0);

    free(sphereData.vertices);
    free(sphereData.normals);
    free(sphereData.textureCoordinates);
    free(sphereData.elements);

    sphereData.vertices = NULL;
    sphereData.normals = NULL;
    sphereData.textureCoordinates = NULL;
    sphereData.elements = NULL;

    [super dealloc];
}

@end

CVReturn displayLinkCallBack(CVDisplayLinkRef displayLinkRef, const CVTimeStamp *now, const CVTimeStamp *outTime, CVOptionFlags inFlags, CVOptionFlags *outFlags, void *displayLinkContext)
{
    return [(GLView *)displayLinkContext getFrameForTime:outTime];
}
