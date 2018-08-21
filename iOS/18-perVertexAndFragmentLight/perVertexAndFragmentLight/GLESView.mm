#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "GLESView.h"
#import "vmath.h"
#import "sphere.h"

enum
{
    CG_ATTRIBUTE_VERTEX_POSITION = 0,
    CG_ATTRIBUTE_COLOR,
    CG_ATTRIBUTE_NORMAL,
    CG_ATTRIBUTE_TEXTURE0,
};

@implementation GLESView
{
    EAGLContext *context;

    GLuint defaultFramebuffer;
    GLuint colorRenderbuffer;
    GLuint depthRenderbuffer;

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

    CADisplayLink *displayLink;
    SphereData sphereData;

    NSInteger aimationFrameInterval;

    BOOL isRendering;
    BOOL isLightingEnabled;
    BOOL isPerFragmentLightEnabled;
}

-(instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];

    if(self)
    {
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

        if(![self initializeContext])
        {
            // Return nil as this will release 'self'
            return nil;
        }

        isRendering = NO;
        aimationFrameInterval = 60;

        [self listExtensions];

        Sphere *sphere = new Sphere();
        sphere->getSphereVertexData(&sphereData);

        // Initialize the shaders and shader program object.

        [self initializeGestures];
        [self initializeVertexShaderPerVertex];
        [self initializeFragmentShaderPerVertex];
        [self initializeShaderProgramPerVertex];
        [self initializeSphereBuffersPerVertex];
        [self initializeVertexShaderPerFragment];
        [self initializeFragmentShaderPerFragment];
        [self initializeShaderProgramPerFragment];
        [self initializeSphereBuffersPerFragment];

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glEnable(GL_DEPTH_TEST);

        // Disable face culling to see back side of object when rotated.
        // glEnable(GL_CULL_FACE);

        glDepthFunc(GL_LEQUAL);

        perspectiveProjectionMatrix = vmath::mat4::identity();

        isLightingEnabled = NO;
        isPerFragmentLightEnabled = NO;
    }

    return self;
}

-(BOOL)acceptsFirstResponder
{
    return YES;
}

-(void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
//    [self setNeedsDisplay];
}

-(void)onSingleTap:(UIGestureRecognizer *)gestureRecognizer
{
    [self setNeedsDisplay];
}

-(void)onDoubleTap:(UIGestureRecognizer *)gestureRecognizer
{
    isPerFragmentLightEnabled = !isPerFragmentLightEnabled;
    [self setNeedsDisplay];
}

-(void)onLongPress:(UILongPressGestureRecognizer *)gestureRecognizer
{
    if([gestureRecognizer state] == UIGestureRecognizerStateBegan)
    {
        isLightingEnabled = !isLightingEnabled;
    }

    [self setNeedsDisplay];
}

-(void)onSwip:(UISwipeGestureRecognizer *)gestureRecognizer
{
    [self release];
    exit(EXIT_SUCCESS);
}

+(Class)layerClass
{
    return [CAEAGLLayer class];
}

-(void)initializeGestures
{
    UITapGestureRecognizer *doubleTapGestureRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onDoubleTap:)];
    [doubleTapGestureRecognizer setNumberOfTapsRequired:2];
    [doubleTapGestureRecognizer setNumberOfTouchesRequired:1];
    [doubleTapGestureRecognizer setDelegate:self];
    [self addGestureRecognizer:doubleTapGestureRecognizer];

    UITapGestureRecognizer *singleTapGestureRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onSingleTap:)];
    [singleTapGestureRecognizer setNumberOfTapsRequired:1];
    [singleTapGestureRecognizer setNumberOfTouchesRequired:1];
    [singleTapGestureRecognizer setDelegate:self];
    [singleTapGestureRecognizer requireGestureRecognizerToFail:doubleTapGestureRecognizer];
    [self addGestureRecognizer:singleTapGestureRecognizer];

    UILongPressGestureRecognizer *longPressGestureRecognizer = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(onLongPress:)];
    [self addGestureRecognizer:longPressGestureRecognizer];

    UISwipeGestureRecognizer *swipeGestureRecognizer = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(onSwip:)];
    [self addGestureRecognizer:swipeGestureRecognizer];
}

-(BOOL)initializeContext
{
    NSMutableDictionary *drawableProperties = [[NSMutableDictionary alloc] init];
    [drawableProperties setObject:[NSNumber numberWithBool:NO] forKey:kEAGLDrawablePropertyRetainedBacking];
    [drawableProperties setObject:kEAGLColorFormatRGBA8 forKey:kEAGLDrawablePropertyColorFormat];

    CAEAGLLayer *layer = (CAEAGLLayer *)super.layer;
    layer.opaque = NO;
    layer.drawableProperties = drawableProperties;

    context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];

    [drawableProperties release];
    drawableProperties = nil;

    if(context == nil)
    {
        NSLog(@"[Error] | Cannot create OpenGL ES 3.0 context");
        [self release];
        return NO;
    }

    [EAGLContext setCurrentContext:context];

    glGenFramebuffers(1, &defaultFramebuffer);
    glGenRenderbuffers(1, &colorRenderbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);

    [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:layer];

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);

    GLint backingWidth = 0;
    GLint backingHeight = 0;

    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);

    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        NSLog(@"[Error] | Cannot create complete framebuffer object, status: %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
        glDeleteFramebuffers(1, &defaultFramebuffer);
        glDeleteRenderbuffers(1, &colorRenderbuffer);
        glDeleteRenderbuffers(1, &depthRenderbuffer);

        return NO;
    }

    NSLog(@"[Info] | Renderer: %s", glGetString(GL_RENDERER));
    NSLog(@"[Info] | OpenGL ES version: %s", glGetString(GL_VERSION));
    NSLog(@"[Info] | GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    return YES;
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

    const GLchar *vertexShaderCode = "#version 300 es" \
        "\n" \
        "in vec4 vertexPosition;" \
        "in vec3 vertexNormal;" \
        "\n" \
        "out vec3 phongAdsColor;" \
        "\n" \
        "uniform mat4 modelMatrix;" \
        "uniform mat4 viewMatrix;" \
        "uniform mat4 projectionMatrix;" \
        "uniform mediump int isLightingEnabled;" \
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
                free(infoLog);
                [self release];
                exit(EXIT_FAILURE);
            }
        }
    }
}

-(void)initializeFragmentShaderPerVertex
{
    fragmentShaderObjectPerVertex = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar *fragmentShaderCode = "#version 300 es" \
        "\n" \
        "precision highp float;"\
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
                free(infoLog);
                [self release];
                exit(EXIT_FAILURE);
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
                free(infoLog);
                [self release];
                exit(EXIT_FAILURE);
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

    const GLchar *vertexShaderCode = "#version 300 es" \
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
        "uniform mediump int isLightingEnabled;" \
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
                free(infoLog);
                [self release];
                exit(EXIT_FAILURE);
            }
        }
    }
}

-(void)initializeFragmentShaderPerFragment
{
    fragmentShaderObjectPerFragment = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar *fragmentShaderCode = "#version 300 es" \
        "\n" \
        "precision highp float;"\
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
                free(infoLog);
                [self release];
                exit(EXIT_FAILURE);
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
                free(infoLog);
                [self release];
                exit(EXIT_FAILURE);
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

-(void)render:(id)sender
{
    [EAGLContext setCurrentContext:context];

    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if(isPerFragmentLightEnabled)
    {
        [self usePerFragmentLight];
    }
    else
    {
        [self usePerVertexLight];
    }

    [context presentRenderbuffer:GL_RENDERBUFFER];
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

-(void)layoutSubviews
{
    GLint width = 0;
    GLint height = 0;

    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];

    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);

    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

    if(height == 0)
    {
        height = 1;
    }

    glViewport(0, 0, width, height);
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        NSLog(@"[Error] | Cannot create complete framebuffer object, status: %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }

    [self render: nil];
}

-(void)startRendering
{
    if(!isRendering)
    {
        Class displayLinkClass = NSClassFromString(@"CADisplayLink");

        displayLink = [displayLinkClass displayLinkWithTarget:self selector:@selector(render:)];
        [displayLink setPreferredFramesPerSecond:aimationFrameInterval];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];

        isRendering = YES;
    }
}

-(void)stopRendering
{
    if(isRendering)
    {
        [displayLink invalidate];
        displayLink = nil;

        isRendering = NO;
    }
}

-(void)dealloc
{
    [self stopRendering];

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

    if(depthRenderbuffer)
    {
        glDeleteRenderbuffers(1, &depthRenderbuffer);
        depthRenderbuffer = 0;
    }

    if(colorRenderbuffer)
    {
        glDeleteRenderbuffers(1, &colorRenderbuffer);
        colorRenderbuffer = 0;
    }

    if(defaultFramebuffer)
    {
        glDeleteFramebuffers(1, &defaultFramebuffer);
        defaultFramebuffer = 0;
    }

    if([EAGLContext currentContext] == context)
    {
        [EAGLContext setCurrentContext:nil];
    }

    [context release];
    context = nil;

    [super dealloc];
}

@end
