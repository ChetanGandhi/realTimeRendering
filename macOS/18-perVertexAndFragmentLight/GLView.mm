#import "GLView.h"
#import "sphere.cpp"

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
    BOOL isPerFragmentLightEnabled;

    CVDisplayLinkRef displayLinkRef;
    GLuint vertexShaderObjectPerVertex;
    GLuint fragmentShaderObjectPerVertex;
    GLuint shaderProgramObjectPerVertex;

    GLuint vaoSpherePerVertex;
    GLuint vboSpherePositionPerVertex;
    GLuint vboSphereNormalPerVertex;
    GLuint vboSphereElementPerVertex;
    GLuint modelMatrixUniformPerVertex;
    GLuint viewMatrixUniformPerVertex;
    GLuint projectionMatrixUniformPerVertex;
    GLuint laUniformPerVertex;
    GLuint ldUniformPerVertex;
    GLuint lsUniformPerVertex;
    GLuint kaUniformPerVertex;
    GLuint kdUniformPerVertex;
    GLuint ksUniformPerVertex;
    GLuint materialShininessUniformPerVertex;
    GLuint lightPositionUniformPerVertex;
    GLuint isLightingEnabledUniformPerVertex;

    GLuint vertexShaderObjectPerFragment;
    GLuint fragmentShaderObjectPerFragment;
    GLuint shaderProgramObjectPerFragment;

    GLuint vaoSpherePerFragment;
    GLuint vboSpherePositionPerFragment;
    GLuint vboSphereNormalPerFragment;
    GLuint vboSphereElementPerFragment;
    GLuint modelMatrixUniformPerFragment;
    GLuint viewMatrixUniformPerFragment;
    GLuint projectionMatrixUniformPerFragment;
    GLuint laUniformPerFragment;
    GLuint ldUniformPerFragment;
    GLuint lsUniformPerFragment;
    GLuint kaUniformPerFragment;
    GLuint kdUniformPerFragment;
    GLuint ksUniformPerFragment;
    GLuint materialShininessUniformPerFragment;
    GLuint lightPositionUniformPerFragment;
    GLuint isLightingEnabledUniformPerFragment;

    vmath::mat4 perspectiveProjectionMatrix;

    GLfloat lightAmbient[4];
    GLfloat lightDiffuse[4];
    GLfloat lightSpecular[4];
    GLfloat lightPosition[4];

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
        vertexShaderObjectPerVertex = 0;
        fragmentShaderObjectPerVertex = 0;
        shaderProgramObjectPerVertex = 0;

        vaoSpherePerVertex = 0;
        vboSpherePositionPerVertex = 0;
        vboSphereNormalPerVertex = 0;
        vboSphereElementPerVertex = 0;
        modelMatrixUniformPerVertex = 0;
        viewMatrixUniformPerVertex = 0;
        projectionMatrixUniformPerVertex = 0;
        laUniformPerVertex = 0;
        ldUniformPerVertex = 0;
        lsUniformPerVertex = 0;
        kaUniformPerVertex = 0;
        kdUniformPerVertex = 0;
        ksUniformPerVertex = 0;
        materialShininessUniformPerVertex = 0;
        lightPositionUniformPerVertex = 0;
        isLightingEnabledUniformPerVertex = 0;

        vertexShaderObjectPerFragment = 0;
        fragmentShaderObjectPerFragment = 0;
        shaderProgramObjectPerFragment = 0;

        vaoSpherePerFragment = 0;
        vboSpherePositionPerFragment = 0;
        vboSphereNormalPerFragment = 0;
        vboSphereElementPerFragment = 0;
        modelMatrixUniformPerFragment = 0;
        viewMatrixUniformPerFragment = 0;
        projectionMatrixUniformPerFragment = 0;
        laUniformPerFragment = 0;
        ldUniformPerFragment = 0;
        lsUniformPerFragment = 0;
        kaUniformPerFragment = 0;
        kdUniformPerFragment = 0;
        ksUniformPerFragment = 0;
        materialShininessUniformPerFragment = 0;
        lightPositionUniformPerFragment = 0;
        isLightingEnabledUniformPerFragment = 0;

        GLfloat _lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
        GLfloat _lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
        GLfloat _lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
        GLfloat _lightPosition[] = { 100.0f,100.0f,100.0f,1.0f };

        GLfloat _materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
        GLfloat _materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
        GLfloat _materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };

        memset(lightAmbient, 0, sizeof(lightAmbient));
        memset(lightDiffuse, 0, sizeof(lightDiffuse));
        memset(lightSpecular, 0, sizeof(lightSpecular));
        memset(lightPosition, 0, sizeof(lightPosition));
        memset(materialAmbient, 0, sizeof(materialAmbient));
        memset(materialDiffuse, 0, sizeof(materialDiffuse));
        memset(materialSpecular, 0, sizeof(materialSpecular));

        memcpy(lightAmbient, _lightAmbient, sizeof(lightAmbient));
        memcpy(lightDiffuse, _lightDiffuse, sizeof(lightDiffuse));
        memcpy(lightSpecular, _lightSpecular, sizeof(lightSpecular));
        memcpy(lightPosition, _lightPosition, sizeof(lightPosition));
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

        case 'F':
        case 'f':
            isPerFragmentLightEnabled = true;
            break;


        case 'L':
        case 'l':
            isLightingEnabled = !isLightingEnabled;
            break;

        case 'V':
        case 'v':
            isPerFragmentLightEnabled = false;
            break;

        case 'T':
        case 't':
            [[self window] toggleFullScreen:self]; // This will auto invoke repainting.
            break;

        default:
            break;
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
    [self initializeVertexShaderPerVertex];
    [self initializeFragmentShaderPerVertex];
    [self initializeShaderProgramPerVertex];
    [self initializeSphereBuffersPerVertex];
    [self initializeVertexShaderPerFragment];
    [self initializeFragmentShaderPerFragment];
    [self initializeShaderProgramPerFragment];
    [self initializeSphereBuffersPerFragment];

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);

    // Disable face culling to see back side of object when rotated.
    // glEnable(GL_CULL_FACE);

    glDepthFunc(GL_LEQUAL);

    perspectiveProjectionMatrix = vmath::mat4::identity();

    isLightingEnabled = false;
    isPerFragmentLightEnabled = false;

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

-(void)initializeVertexShaderPerVertex
{
    vertexShaderObjectPerVertex = glCreateShader(GL_VERTEX_SHADER);

    const GLchar *vertexShaderCode = "#version 410" \
        "\n" \
        "in vec4 vertexPosition;" \
        "in vec3 vertexNormal;" \
        "\n" \
        "out vec3 phongAdsColor;" \
        "\n" \
        "uniform mat4 modelMatrix;" \
        "uniform mat4 viewMatrix;" \
        "uniform mat4 projectionMatrix;" \
        "uniform int isLightingEnabled;" \
        "uniform vec3 la;" \
        "uniform vec3 ld;" \
        "uniform vec3 ls;" \
        "uniform vec3 ka;" \
        "uniform vec3 kd;" \
        "uniform vec3 ks;" \
        "uniform vec4 lightPosition;" \
        "uniform float materialShininess;" \
        "\n" \
        "void main(void)" \
        "{" \
        "   if(isLightingEnabled == 1)" \
        "   {" \
        "       vec4 eyeCoordinates = viewMatrix * modelMatrix * vertexPosition;" \
        "       vec3 tNormal = normalize(mat3(viewMatrix * modelMatrix) * vertexNormal);" \
        "       vec3 source = normalize(vec3(lightPosition) - eyeCoordinates.xyz);" \
        "       float tNormalDotLightDirection = max(dot(tNormal, source), 0.0);" \
        "       vec3 ambient = la * ka;" \
        "       vec3 diffuse = ld * kd * tNormalDotLightDirection;" \
        "       vec3 reflectionVector = reflect(-source, tNormal);" \
        "       vec3 viewVector = normalize(-eyeCoordinates.xyz);" \
        "       vec3 specular = ls * ks * pow(max(dot(reflectionVector, viewVector), 0.0), materialShininess);" \
        "       phongAdsColor = ambient + diffuse + specular;"
        "   }" \
        "   else" \
        "   {" \
        "       phongAdsColor = vec3(1.0, 1.0, 1.0);" \
        "   }" \
        "\n" \
        "   gl_Position = projectionMatrix * viewMatrix * modelMatrix * vertexPosition;" \
        "}";

    glShaderSource(vertexShaderObjectPerVertex, 1, (const GLchar**)&vertexShaderCode, NULL);
    glCompileShader(vertexShaderObjectPerVertex);

    GLint infoLogLength = 0;
    GLint shaderCompileStatus = 0;
    char *infoLog = NULL;

    glGetShaderiv(vertexShaderObjectPerVertex, GL_COMPILE_STATUS, &shaderCompileStatus);

    if(shaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObjectPerVertex, GL_INFO_LOG_LENGTH, &infoLogLength);

        if(infoLogLength > 0)
        {
            infoLog = (char *)malloc(infoLogLength);

            if(infoLog != NULL)
            {
                GLsizei written = 0;
                glGetShaderInfoLog(vertexShaderObjectPerVertex, infoLogLength, &written, infoLog);
                NSLog(@"CG: Vertex shader compilation log: %s\n", infoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
}

-(void)initializeFragmentShaderPerVertex
{
    fragmentShaderObjectPerVertex = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar *fragmentShaderCode = "#version 410" \
        "\n" \
        "in vec3 phongAdsColor;" \
        "\n" \
        "out vec4 fragmentColor;" \
        "\n" \
        "void main(void)" \
        "{" \
        "   fragmentColor = vec4(phongAdsColor, 1.0);" \
        "}";

    glShaderSource(fragmentShaderObjectPerVertex, 1, (const GLchar**)&fragmentShaderCode, NULL);
    glCompileShader(fragmentShaderObjectPerVertex);

    GLint infoLogLength = 0;
    GLint shaderCompileStatus = 0;
    char *infoLog = NULL;

    glGetShaderiv(fragmentShaderObjectPerVertex, GL_COMPILE_STATUS, &shaderCompileStatus);

    if(shaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObjectPerVertex, GL_INFO_LOG_LENGTH, &infoLogLength);

        if(infoLogLength > 0)
        {
            infoLog = (char *)malloc(infoLogLength);

            if(infoLog != NULL)
            {
                GLsizei written = 0;
                glGetShaderInfoLog(fragmentShaderObjectPerVertex, infoLogLength, &written, infoLog);
                NSLog(@"CG: Fragment shader compilation log: %s\n", infoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
}

-(void)initializeShaderProgramPerVertex
{
    shaderProgramObjectPerVertex = glCreateProgram();

    glAttachShader(shaderProgramObjectPerVertex, vertexShaderObjectPerVertex);
    glAttachShader(shaderProgramObjectPerVertex, fragmentShaderObjectPerVertex);

    // Bind the position attribute location before linking.
    glBindAttribLocation(shaderProgramObjectPerVertex, CG_ATTRIBUTE_VERTEX_POSITION, "vertexPosition");

    // Bind the normal attribute location before linking.
    glBindAttribLocation(shaderProgramObjectPerVertex, CG_ATTRIBUTE_NORMAL, "vertexNormal");

    // Now link and check for error.
    glLinkProgram(shaderProgramObjectPerVertex);

    GLint infoLogLength = 0;
    GLint shaderProgramLinkStatus = 0;
    char *infoLog = NULL;

    glGetProgramiv(shaderProgramObjectPerVertex, GL_LINK_STATUS, &shaderProgramLinkStatus);

    if(shaderProgramLinkStatus == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObjectPerVertex, GL_INFO_LOG_LENGTH, &infoLogLength);

        if(infoLogLength > 0)
        {
            infoLog = (char *)malloc(infoLogLength);

            if(infoLog != NULL)
            {
                GLsizei written = 0;
                glGetProgramInfoLog(shaderProgramObjectPerVertex, infoLogLength, &written, infoLog);
                NSLog(@"CG: Shader program link log: %s\n", infoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }

    // After linking get the value of modelView and projection uniform location from the shader program.
    modelMatrixUniformPerVertex = glGetUniformLocation(shaderProgramObjectPerVertex, "modelMatrix");
    viewMatrixUniformPerVertex = glGetUniformLocation(shaderProgramObjectPerVertex, "viewMatrix");
    projectionMatrixUniformPerVertex = glGetUniformLocation(shaderProgramObjectPerVertex, "projectionMatrix");

    isLightingEnabledUniformPerVertex = glGetUniformLocation(shaderProgramObjectPerVertex, "isLightingEnabled");

    laUniformPerVertex = glGetUniformLocation(shaderProgramObjectPerVertex, "la");
    ldUniformPerVertex = glGetUniformLocation(shaderProgramObjectPerVertex, "ld");
    lsUniformPerVertex = glGetUniformLocation(shaderProgramObjectPerVertex, "ls");
    kaUniformPerVertex = glGetUniformLocation(shaderProgramObjectPerVertex, "ka");
    kdUniformPerVertex = glGetUniformLocation(shaderProgramObjectPerVertex, "kd");
    ksUniformPerVertex = glGetUniformLocation(shaderProgramObjectPerVertex, "ks");
    lightPositionUniformPerVertex = glGetUniformLocation(shaderProgramObjectPerVertex, "lightPosition");
    materialShininessUniformPerVertex = glGetUniformLocation(shaderProgramObjectPerVertex, "materialShininess");
}

-(void)initializeSphereBuffersPerVertex
{
    glGenVertexArrays(1, &vaoSpherePerVertex);
    glBindVertexArray(vaoSpherePerVertex);

    glGenBuffers(1, &vboSpherePositionPerVertex);
    glBindBuffer(GL_ARRAY_BUFFER, vboSpherePositionPerVertex);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 1146, sphereData.vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(CG_ATTRIBUTE_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(CG_ATTRIBUTE_VERTEX_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vboSphereNormalPerVertex);
    glBindBuffer(GL_ARRAY_BUFFER, vboSphereNormalPerVertex);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 1146, sphereData.normals, GL_STATIC_DRAW);
    glVertexAttribPointer(CG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(CG_ATTRIBUTE_NORMAL);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vboSphereElementPerVertex);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboSphereElementPerVertex);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * 2280, sphereData.elements, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

-(void)initializeVertexShaderPerFragment
{
    vertexShaderObjectPerFragment = glCreateShader(GL_VERTEX_SHADER);

    const GLchar *vertexShaderCode = "#version 410" \
        "\n" \
        "in vec4 vertexPosition;" \
        "in vec3 vertexNormal;" \
        "\n" \
        "out vec3 tNormal;" \
		"out vec3 source;" \
		"out vec3 viewVector;" \
		"\n" \
        "uniform mat4 modelMatrix;" \
        "uniform mat4 viewMatrix;" \
        "uniform mat4 projectionMatrix;" \
        "uniform vec4 lightPosition;" \
        "uniform int isLightingEnabled;" \
        "\n" \
        "void main(void)" \
        "{" \
        "   if(isLightingEnabled == 1)" \
        "   {" \
        "       vec4 eyeCoordinates = viewMatrix * modelMatrix * vertexPosition;" \
        "       tNormal = mat3(viewMatrix * modelMatrix) * vertexNormal;" \
        "       source = vec3(lightPosition) - eyeCoordinates.xyz;" \
        "       viewVector = -eyeCoordinates.xyz;" \
        "   }" \
        "\n" \
        "   gl_Position = projectionMatrix * viewMatrix * modelMatrix * vertexPosition;" \
        "}";

    glShaderSource(vertexShaderObjectPerFragment, 1, (const GLchar**)&vertexShaderCode, NULL);
    glCompileShader(vertexShaderObjectPerFragment);

    GLint infoLogLength = 0;
    GLint shaderCompileStatus = 0;
    char *infoLog = NULL;

    glGetShaderiv(vertexShaderObjectPerFragment, GL_COMPILE_STATUS, &shaderCompileStatus);

    if(shaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObjectPerFragment, GL_INFO_LOG_LENGTH, &infoLogLength);

        if(infoLogLength > 0)
        {
            infoLog = (char *)malloc(infoLogLength);

            if(infoLog != NULL)
            {
                GLsizei written = 0;
                glGetShaderInfoLog(vertexShaderObjectPerFragment, infoLogLength, &written, infoLog);
                NSLog(@"CG: Vertex shader compilation log: %s\n", infoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
}

-(void)initializeFragmentShaderPerFragment
{
    fragmentShaderObjectPerFragment = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar *fragmentShaderCode = "#version 410" \
        "\n" \
		"in vec3 tNormal;" \
		"in vec3 source;" \
		"in vec3 viewVector;" \
        "\n" \
        "out vec4 fragmentColor;" \
        "\n" \
        "uniform int isLightingEnabled;" \
        "uniform vec3 la;" \
        "uniform vec3 ld;" \
        "uniform vec3 ls;" \
        "uniform vec3 ka;" \
        "uniform vec3 kd;" \
        "uniform vec3 ks;" \
        "uniform float materialShininess;" \
        "\n" \
        "void main(void)" \
        "{" \
        "   vec3 phongAdsColor;" \
        "   if(isLightingEnabled == 1)" \
        "   {" \
        "       vec3 normalizedTNormal = normalize(tNormal);" \
        "       vec3 normalizedSource = normalize(source);" \
        "       vec3 normalizedViewVector = normalize(viewVector);" \
        "       float tNormalDotLightDirection = max(dot(normalizedTNormal, normalizedSource), 0.0);" \
        "       vec3 ambient = la * ka;" \
        "       vec3 diffuse = ld * kd * tNormalDotLightDirection;" \
        "       vec3 reflectionVector = reflect(-normalizedSource, normalizedTNormal);" \
        "       vec3 specular = ls * ks * pow(max(dot(reflectionVector, normalizedViewVector), 0.0), materialShininess);" \
        "       phongAdsColor = ambient + diffuse + specular;"
        "   }" \
        "   else" \
        "   {" \
        "       phongAdsColor = vec3(1.0, 1.0, 1.0);" \
        "   }" \
        "\n" \
        "   fragmentColor = vec4(phongAdsColor, 1.0);" \
        "}";

    glShaderSource(fragmentShaderObjectPerFragment, 1, (const GLchar**)&fragmentShaderCode, NULL);
    glCompileShader(fragmentShaderObjectPerFragment);

    GLint infoLogLength = 0;
    GLint shaderCompileStatus = 0;
    char *infoLog = NULL;

    glGetShaderiv(fragmentShaderObjectPerFragment, GL_COMPILE_STATUS, &shaderCompileStatus);

    if(shaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObjectPerFragment, GL_INFO_LOG_LENGTH, &infoLogLength);

        if(infoLogLength > 0)
        {
            infoLog = (char *)malloc(infoLogLength);

            if(infoLog != NULL)
            {
                GLsizei written = 0;
                glGetShaderInfoLog(fragmentShaderObjectPerFragment, infoLogLength, &written, infoLog);
                NSLog(@"CG: Fragment shader compilation log: %s\n", infoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
}

-(void)initializeShaderProgramPerFragment
{
    shaderProgramObjectPerFragment = glCreateProgram();

    glAttachShader(shaderProgramObjectPerFragment, vertexShaderObjectPerFragment);
    glAttachShader(shaderProgramObjectPerFragment, fragmentShaderObjectPerFragment);

    // Bind the position attribute location before linking.
    glBindAttribLocation(shaderProgramObjectPerFragment, CG_ATTRIBUTE_VERTEX_POSITION, "vertexPosition");

    // Bind the normal attribute location before linking.
    glBindAttribLocation(shaderProgramObjectPerFragment, CG_ATTRIBUTE_NORMAL, "vertexNormal");

    // Now link and check for error.
    glLinkProgram(shaderProgramObjectPerFragment);

    GLint infoLogLength = 0;
    GLint shaderProgramLinkStatus = 0;
    char *infoLog = NULL;

    glGetProgramiv(shaderProgramObjectPerFragment, GL_LINK_STATUS, &shaderProgramLinkStatus);

    if(shaderProgramLinkStatus == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObjectPerFragment, GL_INFO_LOG_LENGTH, &infoLogLength);

        if(infoLogLength > 0)
        {
            infoLog = (char *)malloc(infoLogLength);

            if(infoLog != NULL)
            {
                GLsizei written = 0;
                glGetProgramInfoLog(shaderProgramObjectPerFragment, infoLogLength, &written, infoLog);
                NSLog(@"CG: Shader program link log: %s\n", infoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }

    // After linking get the value of modelView and projection uniform location from the shader program.
    modelMatrixUniformPerFragment = glGetUniformLocation(shaderProgramObjectPerFragment, "modelMatrix");
    viewMatrixUniformPerFragment = glGetUniformLocation(shaderProgramObjectPerFragment, "viewMatrix");
    projectionMatrixUniformPerFragment = glGetUniformLocation(shaderProgramObjectPerFragment, "projectionMatrix");

    isLightingEnabledUniformPerFragment = glGetUniformLocation(shaderProgramObjectPerFragment, "isLightingEnabled");

    laUniformPerFragment = glGetUniformLocation(shaderProgramObjectPerFragment, "la");
    ldUniformPerFragment = glGetUniformLocation(shaderProgramObjectPerFragment, "ld");
    lsUniformPerFragment = glGetUniformLocation(shaderProgramObjectPerFragment, "ls");
    kaUniformPerFragment = glGetUniformLocation(shaderProgramObjectPerFragment, "ka");
    kdUniformPerFragment = glGetUniformLocation(shaderProgramObjectPerFragment, "kd");
    ksUniformPerFragment = glGetUniformLocation(shaderProgramObjectPerFragment, "ks");
    lightPositionUniformPerFragment = glGetUniformLocation(shaderProgramObjectPerFragment, "lightPosition");
    materialShininessUniformPerFragment = glGetUniformLocation(shaderProgramObjectPerFragment, "materialShininess");
}

-(void)initializeSphereBuffersPerFragment
{
    glGenVertexArrays(1, &vaoSpherePerFragment);
    glBindVertexArray(vaoSpherePerFragment);

    glGenBuffers(1, &vboSpherePositionPerFragment);
    glBindBuffer(GL_ARRAY_BUFFER, vboSpherePositionPerFragment);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 1146, sphereData.vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(CG_ATTRIBUTE_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(CG_ATTRIBUTE_VERTEX_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vboSphereNormalPerFragment);
    glBindBuffer(GL_ARRAY_BUFFER, vboSphereNormalPerFragment);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 1146, sphereData.normals, GL_STATIC_DRAW);
    glVertexAttribPointer(CG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(CG_ATTRIBUTE_NORMAL);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vboSphereElementPerFragment);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboSphereElementPerFragment);

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

-(void)render
{
    [[self openGLContext] makeCurrentContext];

    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];

    CGLLockContext(cglContext);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if(isPerFragmentLightEnabled)
    {
        [self usePerFragmentLight];
    }
    else
    {
        [self usePerVertexLight];
    }

    CGLFlushDrawable(cglContext);
    CGLUnlockContext(cglContext);
}

-(void)usePerVertexLight
{
    glUseProgram(shaderProgramObjectPerVertex);

    if(isLightingEnabled)
    {
        glUniform1i(isLightingEnabledUniformPerVertex, 1);
        glUniform3fv(laUniformPerVertex, 1, lightAmbient);
        glUniform3fv(ldUniformPerVertex, 1, lightDiffuse);
        glUniform3fv(lsUniformPerVertex, 1, lightSpecular);
        glUniform4fv(lightPositionUniformPerVertex, 1, lightPosition);

        glUniform3fv(kaUniformPerVertex, 1, materialAmbient);
        glUniform3fv(kdUniformPerVertex, 1, materialDiffuse);
        glUniform3fv(ksUniformPerVertex, 1, materialSpecular);
        glUniform1f(materialShininessUniformPerVertex, materialShininess);
    }
    else
    {
        glUniform1i(isLightingEnabledUniformPerVertex, 0);
    }

    [self drawSpherePerVertex];

    glUseProgram(0);
}

-(void)usePerFragmentLight
{
    glUseProgram(shaderProgramObjectPerFragment);

    if(isLightingEnabled)
    {
        glUniform1i(isLightingEnabledUniformPerFragment, 1);
        glUniform3fv(laUniformPerFragment, 1, lightAmbient);
        glUniform3fv(ldUniformPerFragment, 1, lightDiffuse);
        glUniform3fv(lsUniformPerFragment, 1, lightSpecular);
        glUniform4fv(lightPositionUniformPerFragment, 1, lightPosition);

        glUniform3fv(kaUniformPerFragment, 1, materialAmbient);
        glUniform3fv(kdUniformPerFragment, 1, materialDiffuse);
        glUniform3fv(ksUniformPerFragment, 1, materialSpecular);
        glUniform1f(materialShininessUniformPerFragment, materialShininess);
    }
    else
    {
        glUniform1i(isLightingEnabledUniformPerFragment, 0);
    }

    [self drawSpherePerFragment];

    glUseProgram(0);
}

-(void)drawSpherePerVertex
{
    vmath::mat4 modelMatrix = vmath::mat4::identity();
    vmath::mat4 viewMatrix = vmath::mat4::identity();

    // Translate the modal view matrix.
    modelMatrix = vmath::translate(0.0f, 0.0f, -3.0f);

    // Pass modelMatrix to vertex shader in 'modelMatrix' variable defined in shader.
    glUniformMatrix4fv(modelMatrixUniformPerVertex, 1, GL_FALSE, modelMatrix);

    // Pass viewMatrix to vertex shader in 'viewMatrix' variable defined in shader.
    glUniformMatrix4fv(viewMatrixUniformPerVertex, 1, GL_FALSE, viewMatrix);

    // Pass perspectiveProjectionMatrix to vertex shader in 'projectionMatrix' variable defined in shader.
    glUniformMatrix4fv(projectionMatrixUniformPerVertex, 1, GL_FALSE, perspectiveProjectionMatrix);

    // Now bind the VAO to which we want to use
    glBindVertexArray(vaoSpherePerVertex);

    // Draw the sphere
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboSphereElementPerVertex);
    glDrawElements(GL_TRIANGLES, sphereData.numberOfElements, GL_UNSIGNED_SHORT, 0);

    // unbind the vao
    glBindVertexArray(0);
}

-(void)drawSpherePerFragment
{
    vmath::mat4 modelMatrix = vmath::mat4::identity();
    vmath::mat4 viewMatrix = vmath::mat4::identity();

    // Translate the modal view matrix.
    modelMatrix = vmath::translate(0.0f, 0.0f, -3.0f);

    // Pass modelMatrix to vertex shader in 'modelMatrix' variable defined in shader.
    glUniformMatrix4fv(modelMatrixUniformPerFragment, 1, GL_FALSE, modelMatrix);

    // Pass viewMatrix to vertex shader in 'viewMatrix' variable defined in shader.
    glUniformMatrix4fv(viewMatrixUniformPerFragment, 1, GL_FALSE, viewMatrix);

    // Pass perspectiveProjectionMatrix to vertex shader in 'projectionMatrix' variable defined in shader.
    glUniformMatrix4fv(projectionMatrixUniformPerFragment, 1, GL_FALSE, perspectiveProjectionMatrix);

    // Now bind the VAO to which we want to use
    glBindVertexArray(vaoSpherePerFragment);

    // Draw the sphere
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboSphereElementPerFragment);
    glDrawElements(GL_TRIANGLES, sphereData.numberOfElements, GL_UNSIGNED_SHORT, 0);

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

    if(vaoSpherePerVertex)
    {
        glDeleteVertexArrays(1, &vaoSpherePerVertex);
        vaoSpherePerVertex = 0;
    }

    if(vboSpherePositionPerVertex)
    {
        glDeleteBuffers(1, &vboSpherePositionPerVertex);
        vboSpherePositionPerVertex = 0;
    }

    if(vboSphereNormalPerVertex)
    {
        glDeleteBuffers(1, &vboSphereNormalPerVertex);
        vboSphereNormalPerVertex = 0;
    }

    if(vboSphereElementPerVertex)
    {
        glDeleteBuffers(1, &vboSphereElementPerVertex);
        vboSphereElementPerVertex = 0;
    }

    if(shaderProgramObjectPerVertex)
    {
        if(vertexShaderObjectPerVertex)
        {
            glDetachShader(shaderProgramObjectPerVertex, vertexShaderObjectPerVertex);
        }

        if(fragmentShaderObjectPerVertex)
        {
            glDetachShader(shaderProgramObjectPerVertex, fragmentShaderObjectPerVertex);
        }
    }

    if(vertexShaderObjectPerVertex)
    {
        glDeleteShader(vertexShaderObjectPerVertex);
        vertexShaderObjectPerVertex = 0;
    }

    if(fragmentShaderObjectPerVertex)
    {
        glDeleteShader(fragmentShaderObjectPerVertex);
        fragmentShaderObjectPerVertex = 0;
    }

    if(shaderProgramObjectPerVertex)
    {
        glDeleteProgram(shaderProgramObjectPerVertex);
        shaderProgramObjectPerVertex = 0;
    }

    if(vaoSpherePerFragment)
    {
        glDeleteVertexArrays(1, &vaoSpherePerFragment);
        vaoSpherePerFragment = 0;
    }

    if(vboSpherePositionPerFragment)
    {
        glDeleteBuffers(1, &vboSpherePositionPerFragment);
        vboSpherePositionPerFragment = 0;
    }

    if(vboSphereNormalPerFragment)
    {
        glDeleteBuffers(1, &vboSphereNormalPerFragment);
        vboSphereNormalPerFragment = 0;
    }

    if(vboSphereElementPerFragment)
    {
        glDeleteBuffers(1, &vboSphereElementPerFragment);
        vboSphereElementPerFragment = 0;
    }

    if(shaderProgramObjectPerFragment)
    {
        if(vertexShaderObjectPerFragment)
        {
            glDetachShader(shaderProgramObjectPerFragment, vertexShaderObjectPerFragment);
        }

        if(fragmentShaderObjectPerFragment)
        {
            glDetachShader(shaderProgramObjectPerFragment, fragmentShaderObjectPerFragment);
        }
    }

    if(vertexShaderObjectPerFragment)
    {
        glDeleteShader(vertexShaderObjectPerFragment);
        vertexShaderObjectPerFragment = 0;
    }

    if(fragmentShaderObjectPerFragment)
    {
        glDeleteShader(fragmentShaderObjectPerFragment);
        fragmentShaderObjectPerFragment = 0;
    }

    if(shaderProgramObjectPerFragment)
    {
        glDeleteProgram(shaderProgramObjectPerFragment);
        shaderProgramObjectPerFragment = 0;
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
