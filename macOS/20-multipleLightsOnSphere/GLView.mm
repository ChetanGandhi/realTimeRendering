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

@implementation GLView
{
    BOOL isLightingEnabled;
    bool isAnimationEnabled;

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
    GLuint ldOneUniform;
    GLuint lsOneUniform;
    GLuint laTwoUniform;
    GLuint ldTwoUniform;
    GLuint lsTwoUniform;
    GLuint kaUniform;
    GLuint kdUniform;
    GLuint ksUniform;
    GLuint materialShininessUniform;
    GLuint lightZeroPositionUniform;
    GLuint lightOnePositionUniform;
    GLuint lightTwoPositionUniform;
    GLuint isLightingEnabledUniform;

    vmath::mat4 perspectiveProjectionMatrix;

    GLfloat angleRedLight;
    GLfloat angleGreenLight;
    GLfloat angleBlueLight;
    GLfloat speed;

    GLfloat lightZeroAmbient[4];
    GLfloat lightZeroDiffuse[4];
    GLfloat lightZeroSpecular[4];
    GLfloat lightZeroPosition[4];

    GLfloat lightOneAmbient[4];
    GLfloat lightOneDiffuse[4];
    GLfloat lightOneSpecular[4];
    GLfloat lightOnePosition[4];

    GLfloat lightTwoAmbient[4];
    GLfloat lightTwoDiffuse[4];
    GLfloat lightTwoSpecular[4];
    GLfloat lightTwoPosition[4];


    GLfloat materialAmbient[4];
    GLfloat materialDiffuse[4];
    GLfloat materialSpecular[4];
    GLfloat materialShininess;

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
        laOneUniform = 0;
        ldOneUniform = 0;
        lsOneUniform = 0;
        laTwoUniform = 0;
        ldTwoUniform = 0;
        lsTwoUniform = 0;
        kaUniform = 0;
        kdUniform = 0;
        ksUniform = 0;
        materialShininessUniform = 0;
        lightZeroPositionUniform = 0;
        lightOnePositionUniform = 0;
        lightTwoPositionUniform = 0;
        isLightingEnabledUniform = 0;

        angleRedLight = 0.0f;
        angleGreenLight = 0.0f;
        angleBlueLight = 0.0f;
        speed = 0.1f;

        GLfloat _lightZeroAmbient[] = {0.0f, 0.0f, 0.0f, 0.0f};
        GLfloat _lightZeroDiffuse[] = {1.0f, 0.0f, 0.0f, 0.0f};
        GLfloat _lightZeroSpecular[] = {1.0f, 0.0f, 0.0f, 0.0f};
        GLfloat _lightZeroPosition[] = {0.0f, 0.0f, 0.0f, 0.0f};

        GLfloat _lightOneAmbient[] = {0.0f, 0.0f, 0.0f, 0.0f};
        GLfloat _lightOneDiffuse[] = {0.0f, 1.0f, 0.0f, 0.0f};
        GLfloat _lightOneSpecular[] = {0.0f, 1.0f, 0.0f, 0.0f};
        GLfloat _lightOnePosition[] = {0.0f, 0.0f, 0.0f, 0.0f};

        GLfloat _lightTwoAmbient[] = {0.0f, 0.0f, 0.0f, 0.0f};
        GLfloat _lightTwoDiffuse[] = {0.0f, 0.0f, 1.0f, 0.0f};
        GLfloat _lightTwoSpecular[] = {0.0f, 0.0f, 1.0f, 0.0f};
        GLfloat _lightTwoPosition[] = {0.0f, 0.0f, 0.0f, 0.0f};

        GLfloat _materialAmbient[] = {0.0f, 0.0f, 0.0f, 0.0f};
        GLfloat _materialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
        GLfloat _materialSpecular[] = {1.0f, 1.0f, 1.0f, 0.0f};

        memset(lightZeroAmbient, 0, sizeof(lightZeroAmbient));
        memset(lightZeroDiffuse, 0, sizeof(lightZeroDiffuse));
        memset(lightZeroSpecular, 0, sizeof(lightZeroSpecular));
        memset(lightZeroPosition, 0, sizeof(lightZeroPosition));
        memset(lightOneAmbient, 0, sizeof(lightOneAmbient));
        memset(lightOneDiffuse, 0, sizeof(lightOneDiffuse));
        memset(lightOneSpecular, 0, sizeof(lightOneSpecular));
        memset(lightOnePosition, 0, sizeof(lightOnePosition));
        memset(lightTwoAmbient, 0, sizeof(lightTwoAmbient));
        memset(lightTwoDiffuse, 0, sizeof(lightTwoDiffuse));
        memset(lightTwoSpecular, 0, sizeof(lightTwoSpecular));
        memset(lightTwoPosition, 0, sizeof(lightTwoPosition));
        memset(materialAmbient, 0, sizeof(materialAmbient));
        memset(materialDiffuse, 0, sizeof(materialDiffuse));
        memset(materialSpecular, 0, sizeof(materialSpecular));

        memcpy(lightZeroAmbient, _lightZeroAmbient, sizeof(lightZeroAmbient));
        memcpy(lightZeroDiffuse, _lightZeroDiffuse, sizeof(lightZeroDiffuse));
        memcpy(lightZeroSpecular, _lightZeroSpecular, sizeof(lightZeroSpecular));
        memcpy(lightZeroPosition, _lightZeroPosition, sizeof(lightZeroPosition));
        memcpy(lightOneAmbient, _lightOneAmbient, sizeof(lightOneAmbient));
        memcpy(lightOneDiffuse, _lightOneDiffuse, sizeof(lightOneDiffuse));
        memcpy(lightOneSpecular, _lightOneSpecular, sizeof(lightOneSpecular));
        memcpy(lightOnePosition, _lightOnePosition, sizeof(lightOnePosition));
        memcpy(lightTwoAmbient, _lightTwoAmbient, sizeof(lightTwoAmbient));
        memcpy(lightTwoDiffuse, _lightTwoDiffuse, sizeof(lightTwoDiffuse));
        memcpy(lightTwoSpecular, _lightTwoSpecular, sizeof(lightTwoSpecular));
        memcpy(lightTwoPosition, _lightTwoPosition, sizeof(lightTwoPosition));
        memcpy(materialAmbient, _materialAmbient, sizeof(materialAmbient));
        memcpy(materialDiffuse, _materialDiffuse, sizeof(materialDiffuse));
        memcpy(materialSpecular, _materialSpecular, sizeof(materialSpecular));

        materialShininess = 50.0f;
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

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);

    // Disable face culling to see back side of object when rotated.
    // glEnable(GL_CULL_FACE);

    glDepthFunc(GL_LEQUAL);

    perspectiveProjectionMatrix = vmath::mat4::identity();

    isLightingEnabled = false;

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
		"out vec3 lightOneDirection;" \
		"out vec3 lightTwoDirection;" \
		"out vec4 eyeCoordinates;" \
		"out vec3 viewVector;" \
		"\n" \
        "uniform mat4 modelMatrix;" \
        "uniform mat4 viewMatrix;" \
        "uniform mat4 projectionMatrix;" \
        "uniform vec4 lightZeroPosition;" \
        "uniform vec4 lightOnePosition;" \
        "uniform vec4 lightTwoPosition;" \
        "uniform int isLightingEnabled;" \
        "\n" \
        "void main(void)" \
        "{" \
        "   if(isLightingEnabled == 1)" \
        "   {" \
        "       eyeCoordinates = viewMatrix * modelMatrix * vertexPosition;" \
        "       tNormal = mat3(viewMatrix * modelMatrix) * vertexNormal;" \
        "       lightZeroDirection = vec3(lightZeroPosition);" \
        "       lightOneDirection = vec3(lightOnePosition);" \
        "       lightTwoDirection = vec3(lightTwoPosition);" \
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
                free(infoLog);
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
		"in vec3 lightOneDirection;" \
		"in vec3 lightTwoDirection;" \
		"in vec4 eyeCoordinates;" \
		"in vec3 viewVector;" \
        "\n" \
        "out vec4 fragmentColor;" \
        "\n" \
        "uniform int isLightingEnabled;" \
        "uniform vec3 laZero;" \
        "uniform vec3 ldZero;" \
        "uniform vec3 lsZero;" \
        "uniform vec3 laOne;" \
        "uniform vec3 ldOne;" \
        "uniform vec3 lsOne;" \
        "uniform vec3 laTwo;" \
        "uniform vec3 ldTwo;" \
        "uniform vec3 lsTwo;" \
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
        "       vec3 normalizedLightOneDirection = normalize(lightOneDirection - eyeCoordinates.xyz);" \
        "       float tNormalDotLightOneDirection = max(dot(normalizedTNormal, normalizedLightOneDirection), 0.0);" \
        "       vec3 ambientLightOne = laOne * ka;" \
        "       vec3 diffuseLightOne = ldOne * kd * tNormalDotLightOneDirection;" \
        "       vec3 reflectionVectorLightOne = reflect(-normalizedLightOneDirection, normalizedTNormal);" \
        "       vec3 specularLightOne = lsOne * ks * pow(max(dot(reflectionVectorLightOne, normalizedViewVector), 0.0), materialShininess);" \
        "\n" \
        "       vec3 normalizedLightTwoDirection = normalize(lightTwoDirection - eyeCoordinates.xyz);" \
        "       float tNormalDotLightTwoDirection = max(dot(normalizedTNormal, normalizedLightTwoDirection), 0.0);" \
        "       vec3 ambientLightTwo = laTwo * ka;" \
        "       vec3 diffuseLightTwo = ldTwo * kd * tNormalDotLightTwoDirection;" \
        "       vec3 reflectionVectorLightTwo = reflect(-normalizedLightTwoDirection, normalizedTNormal);" \
        "       vec3 specularLightTwo = lsTwo * ks * pow(max(dot(reflectionVectorLightTwo, normalizedViewVector), 0.0), materialShininess);" \
        "\n" \
        "       phongAdsColor = ambientLightZero + diffuseLightZero + specularLightZero;" \
        "       phongAdsColor += ambientLightOne + diffuseLightOne + specularLightOne;" \
        "       phongAdsColor += ambientLightTwo + diffuseLightTwo + specularLightTwo;" \
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
                free(infoLog);
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
                free(infoLog);
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
    laOneUniform = glGetUniformLocation(shaderProgramObject, "laOne");
    ldOneUniform = glGetUniformLocation(shaderProgramObject, "ldOne");
    lsOneUniform = glGetUniformLocation(shaderProgramObject, "lsOne");
    laTwoUniform = glGetUniformLocation(shaderProgramObject, "laTwo");
    ldTwoUniform = glGetUniformLocation(shaderProgramObject, "ldTwo");
    lsTwoUniform = glGetUniformLocation(shaderProgramObject, "lsTwo");
    kaUniform = glGetUniformLocation(shaderProgramObject, "ka");
    kdUniform = glGetUniformLocation(shaderProgramObject, "kd");
    ksUniform = glGetUniformLocation(shaderProgramObject, "ks");
    lightZeroPositionUniform = glGetUniformLocation(shaderProgramObject, "lightZeroPosition");
    lightOnePositionUniform = glGetUniformLocation(shaderProgramObject, "lightOnePosition");
    lightTwoPositionUniform = glGetUniformLocation(shaderProgramObject, "lightTwoPosition");
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

    angleRedLight += speed;
    angleGreenLight += speed;
    angleBlueLight += speed;

    if(angleRedLight >= 360.0f)
    {
        angleRedLight = 0.0f;
    }

    if(angleGreenLight >= 360.0f)
    {
        angleGreenLight = 0.0f;
    }

    if(angleBlueLight >= 360.0f)
    {
        angleBlueLight = 0.0f;
    }
}

-(void)render
{
    [[self openGLContext] makeCurrentContext];

    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];

    CGLLockContext(cglContext);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glUseProgram(shaderProgramObject);

    if(isLightingEnabled)
    {
        glUniform1i(isLightingEnabledUniform, 1);

        glUniform3fv(laZeroUniform, 1, lightZeroAmbient);
        glUniform3fv(ldZeroUniform, 1, lightZeroDiffuse);
        glUniform3fv(lsZeroUniform, 1, lightZeroSpecular);

        glUniform3fv(laOneUniform, 1, lightOneAmbient);
        glUniform3fv(ldOneUniform, 1, lightOneDiffuse);
        glUniform3fv(lsOneUniform, 1, lightOneSpecular);

        glUniform3fv(laTwoUniform, 1, lightTwoAmbient);
        glUniform3fv(ldTwoUniform, 1, lightTwoDiffuse);
        glUniform3fv(lsTwoUniform, 1, lightTwoSpecular);

        lightZeroPosition[0] = 0.0f;
        lightZeroPosition[1] = sinf(angleRedLight) * 100.0f;
        lightZeroPosition[2] = cosf(angleRedLight) * 100.0f - 3.0f;
        glUniform4fv(lightZeroPositionUniform, 1, lightZeroPosition);

        lightOnePosition[0] = sinf(angleGreenLight) * 100.0f;
        lightOnePosition[1] = 0.0f;
        lightOnePosition[2] = cosf(angleGreenLight) * 100.0f - 3.0f;
        glUniform4fv(lightOnePositionUniform, 1, lightOnePosition);

        lightTwoPosition[0] = cosf(angleBlueLight) * 100.0f;
        lightTwoPosition[1] = sinf(angleBlueLight) * 100.0f;
        lightTwoPosition[2] = -3.0f;
        glUniform4fv(lightTwoPositionUniform, 1, lightTwoPosition);

        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDiffuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        glUniform1f(materialShininessUniform, materialShininess);
    }
    else
    {
        glUniform1i(isLightingEnabledUniform, 0);
    }

    [self drawSphere];

    glUseProgram(0);

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

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    perspectiveProjectionMatrix = vmath::perspective(45.0f, width/height, 1.0f, 100.0f);
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
