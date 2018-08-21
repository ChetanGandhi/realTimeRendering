#import "GLView.h"

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
    CVDisplayLinkRef displayLinkRef;
    GLuint vertexShaderObject;
    GLuint fragmentShaderObject;
    GLuint shaderProgramObject;

    GLuint vaoTriangle;
    GLuint vboTriangle;
    GLuint vaoSquare;
    GLuint vboSquare;
    GLuint mvpUniform;

    vmath::mat4 perspectiveProjectionMatrix;
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

        vaoTriangle = 0;
        vboTriangle = 0;
        vaoSquare = 0;
        vboSquare = 0;
        mvpUniform = 0;
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

    // Initialize the shaders and shader program object.
    [self initializeVertexShader];
    [self initializeFragmentShader];
    [self initializeShaderProgram];
    [self initializeTriangleBuffers];
    [self initializeSquareBuffers];

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);

    perspectiveProjectionMatrix = vmath::mat4::identity();

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
        "uniform mat4 mvpMatrix;" \
        "\n" \
        "void main(void)" \
        "{" \
        "   gl_Position = mvpMatrix * vertexPosition;" \
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
        "out vec4 fragmentColor;" \
        "\n" \
        "void main(void)" \
        "{" \
        "   fragmentColor = vec4(1.0, 1.0, 1.0, 1.0);" \
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

    // After linking get the value of MVP uniform location from the shader program.
    mvpUniform = glGetUniformLocation(shaderProgramObject, "mvpMatrix");
}

-(void)initializeTriangleBuffers
{
    const GLfloat triangleVertices[] = {
        0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f
    };

    glGenVertexArrays(1, &vaoTriangle);
    glBindVertexArray(vaoTriangle);

    glGenBuffers(1, &vboTriangle);
    glBindBuffer(GL_ARRAY_BUFFER, vboTriangle);

    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(CG_ATTRIBUTE_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(CG_ATTRIBUTE_VERTEX_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

-(void)initializeSquareBuffers
{
    const GLfloat squareVertices[] = {
        -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &vaoSquare);
    glBindVertexArray(vaoSquare);

    glGenBuffers(1, &vboSquare);
    glBindBuffer(GL_ARRAY_BUFFER, vboSquare);

    glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(CG_ATTRIBUTE_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(CG_ATTRIBUTE_VERTEX_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
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

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramObject);

    [self drawTriangle];
    [self drawSquare];

    glUseProgram(0);

    CGLFlushDrawable(cglContext);
    CGLUnlockContext(cglContext);
}

-(void)drawTriangle
{
    vmath::mat4 modelViewMatrix = vmath::mat4::identity();
    vmath::mat4 modelViewProjectionMatrix = vmath::mat4::identity();

    // Translate the modal view matrix.
    modelViewMatrix = vmath::translate(-1.5f, 0.0f, -6.0f);

	// Multiply modelViewMatrix and perspectiveProjectionMatrix to get modelViewProjectionMatrix
    // Oder of multiplication is very important projectionMatrix * modelMatrix * viewMatrix
    // As we have model and view matrix combined, we just have to multiply projectionMatrix and modelViewMatrix
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    // Pass modelViewProjectionMatrix to vertex shader in 'mvpMatrix' variable defined in shader.
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    // Now bind the VAO to which we want to use
    glBindVertexArray(vaoTriangle);

    // Draw the triangle
    // 3 is number of vertices in the array i.e. element count in triangleVertices divide by 3 (x, y, z) component
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // unbind the vao
    glBindVertexArray(0);
}

-(void)drawSquare
{
    vmath::mat4 modelViewMatrix = vmath::mat4::identity();
    vmath::mat4 modelViewProjectionMatrix = vmath::mat4::identity();

    // Translate the modal view matrix.
    modelViewMatrix = vmath::translate(1.5f, 0.0f, -6.0f);

	// Multiply modelViewMatrix and perspectiveProjectionMatrix to get modelViewProjectionMatrix
    // Oder of multiplication is very important projectionMatrix * modelMatrix * viewMatrix
    // As we have model and view matrix combined, we just have to multiply projectionMatrix and modelViewMatrix
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    // Pass modelViewProjectionMatrix to vertex shader in 'mvpMatrix' variable defined in shader.
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    // Now bind the VAO to which we want to use
    glBindVertexArray(vaoSquare);

    // Draw the square
    // 4 is number of vertices in the array i.e. element count in squareVertices divide by 3 (x, y, z) component
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

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
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f);
    CGLUnlockContext(cglContext);
}

-(void)dealloc
{
    CVDisplayLinkStop(displayLinkRef);
    CVDisplayLinkRelease(displayLinkRef);

    if(vaoTriangle)
    {
        glDeleteVertexArrays(1, &vaoTriangle);
        vaoTriangle = 0;
    }

    if(vboTriangle)
    {
        glDeleteBuffers(1, &vboTriangle);
        vboTriangle = 0;
    }

    if(vaoSquare)
    {
        glDeleteVertexArrays(1, &vaoSquare);
        vaoSquare = 0;
    }

    if(vboSquare)
    {
        glDeleteBuffers(1, &vboSquare);
        vboSquare = 0;
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

    [super dealloc];
}

@end

CVReturn displayLinkCallBack(CVDisplayLinkRef displayLinkRef, const CVTimeStamp *now, const CVTimeStamp *outTime, CVOptionFlags inFlags, CVOptionFlags *outFlags, void *displayLinkContext)
{
    return [(GLView *)displayLinkContext getFrameForTime:outTime];
}
