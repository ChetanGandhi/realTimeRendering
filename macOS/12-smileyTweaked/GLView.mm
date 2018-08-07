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

    GLuint vaoSmiley;
    GLuint vboSmileyPosition;
    GLuint vboSmileyTexture;
    GLuint mvpUniform;
    GLuint textureSamplerUniform;
    GLuint textureSmiley;
    GLuint textureWhiteColor;
    GLuint textureMode;

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

        vaoSmiley = 0;
        vboSmileyPosition = 0;
        vboSmileyTexture = 0;
        mvpUniform = 0;
        textureSamplerUniform = 0;
        textureSmiley = 0;
        textureWhiteColor = 0;
        textureMode = 0;
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

    // 0 to 4 number
    if(key >= '0' && key <= '4')
    {
        textureMode = (key - '0');
    }
    else
    {
        textureMode = 0;
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
    [self initializeSmileyBuffers];

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    // Disable face culling to see back side of object when rotated.
    // glEnable(GL_CULL_FACE);

    glDepthFunc(GL_LEQUAL);

    perspectiveProjectionMatrix = vmath::mat4::identity();

    textureSmiley = [self loadGLTextures:@"resources/smiley.bmp"];
    [self makeWhiteColorTexture];

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
        "in vec2 vertexTextureCoordinate0;" \
        "\n" \
        "out vec2 outVertexTextureCoordinate0;" \
        "\n" \
        "uniform mat4 mvpMatrix;" \
        "\n" \
        "void main(void)" \
        "{" \
        "   gl_Position = mvpMatrix * vertexPosition;" \
        "   outVertexTextureCoordinate0 = vertexTextureCoordinate0;" \
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
        "in vec2 outVertexTextureCoordinate0;" \
        "\n" \
        "out vec4 fragmentColor;" \
        "\n" \
        "uniform sampler2D textureSampler0;"
        "\n" \
        "void main(void)" \
        "{" \
        "   fragmentColor = texture(textureSampler0, outVertexTextureCoordinate0);" \
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

    // Bind the texture attribute location before linking.
    glBindAttribLocation(shaderProgramObject, CG_ATTRIBUTE_TEXTURE0, "vertexTextureCoordinate0");

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

    // After linking get the value of MVP uniform location from the shader program.
    mvpUniform = glGetUniformLocation(shaderProgramObject, "mvpMatrix");

    // Get the texture sampler uniform location from shader program.
    textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "textureSampler0");
}

-(void)initializeSmileyBuffers
{
    const GLfloat smileyVertices[] = {
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f
    };

    glGenVertexArrays(1, &vaoSmiley);
    glBindVertexArray(vaoSmiley);

    glGenBuffers(1, &vboSmileyPosition);
    glBindBuffer(GL_ARRAY_BUFFER, vboSmileyPosition);

    glBufferData(GL_ARRAY_BUFFER, sizeof(smileyVertices), smileyVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(CG_ATTRIBUTE_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(CG_ATTRIBUTE_VERTEX_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vboSmileyTexture);
    glBindBuffer(GL_ARRAY_BUFFER, vboSmileyTexture);

    // As we are using dynamic draw, pass the size hardcoded
    // i.e. 8 * sizeof(GLfloat), 8 is number of coordinates in array.
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(CG_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(CG_ATTRIBUTE_TEXTURE0);

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

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glUseProgram(shaderProgramObject);

    [self drawSmiley];

    glUseProgram(0);

    CGLFlushDrawable(cglContext);
    CGLUnlockContext(cglContext);
}

-(void)drawSmiley
{
    vmath::mat4 modelViewMatrix = vmath::mat4::identity();
    vmath::mat4 modelViewProjectionMatrix = vmath::mat4::identity();

    // Translate the modal view matrix.
    modelViewMatrix = vmath::translate(0.0f, 0.0f, -6.0f);

	// Multiply modelViewMatrix and perspectiveProjectionMatrix to get modelViewProjectionMatrix
    // Oder of multiplication is very important projectionMatrix * modelMatrix * viewMatrix
    // As we have model and view matrix combined, we just have to multiply projectionMatrix and modelViewMatrix
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    // Pass modelViewProjectionMatrix to vertex shader in 'mvpMatrix' variable defined in shader.
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    // Now bind the VAO to which we want to use
    glBindVertexArray(vaoSmiley);

    // As we are using dynamic draw, pass the coordinates here as per texture mode.
    GLfloat smileyTextureCoordinates[8] = {0.0f};
    GLuint currentTexture = textureSmiley;

    if(textureMode == 1)
    {
        smileyTextureCoordinates[0] = 0.5f;
        smileyTextureCoordinates[1] = 0.5f;
        smileyTextureCoordinates[2] = 0.0f;
        smileyTextureCoordinates[3] = 0.5f;
        smileyTextureCoordinates[4] = 0.0f;
        smileyTextureCoordinates[5] = 0.0f;
        smileyTextureCoordinates[6] = 0.5f;
        smileyTextureCoordinates[7] = 0.0f;
    }
    else if(textureMode == 2)
    {
        smileyTextureCoordinates[0] = 1.0f;
        smileyTextureCoordinates[1] = 0.0f;
        smileyTextureCoordinates[2] = 0.0f;
        smileyTextureCoordinates[3] = 0.0f;
        smileyTextureCoordinates[4] = 0.0f;
        smileyTextureCoordinates[5] = 1.0f;
        smileyTextureCoordinates[6] = 1.0f;
        smileyTextureCoordinates[7] = 1.0f;
    }
    else if(textureMode == 3)
    {
        smileyTextureCoordinates[0] = 2.0f;
        smileyTextureCoordinates[1] = 2.0f;
        smileyTextureCoordinates[2] = 0.0f;
        smileyTextureCoordinates[3] = 2.0f;
        smileyTextureCoordinates[4] = 0.0f;
        smileyTextureCoordinates[5] = 0.0f;
        smileyTextureCoordinates[6] = 2.0f;
        smileyTextureCoordinates[7] = 0.0f;
    }
    else if(textureMode == 4)
    {
        smileyTextureCoordinates[0] = 0.5f;
        smileyTextureCoordinates[1] = 0.5f;
        smileyTextureCoordinates[2] = 0.5f;
        smileyTextureCoordinates[3] = 0.5f;
        smileyTextureCoordinates[4] = 0.5f;
        smileyTextureCoordinates[5] = 0.5f;
        smileyTextureCoordinates[6] = 0.5f;
        smileyTextureCoordinates[7] = 0.5f;
    }
    else
    {
        currentTexture = textureWhiteColor;
        smileyTextureCoordinates[0] = 1.0f;
        smileyTextureCoordinates[1] = 1.0f;
        smileyTextureCoordinates[2] = 0.0f;
        smileyTextureCoordinates[3] = 1.0f;
        smileyTextureCoordinates[4] = 0.0f;
        smileyTextureCoordinates[5] = 0.0f;
        smileyTextureCoordinates[6] = 1.0f;
        smileyTextureCoordinates[7] = 0.0f;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vboSmileyTexture);

    // bind the above texture coordinates here.
    glBufferData(GL_ARRAY_BUFFER, sizeof(smileyTextureCoordinates), smileyTextureCoordinates, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Enable 0th texture
    glActiveTexture(GL_TEXTURE0);

    // Bind with smiley texture
    glBindTexture(GL_TEXTURE_2D, currentTexture);

    // Enable 0th sampler.
    glUniform1i(textureSamplerUniform, 0);

    // Draw the square
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // unbind the vao
    glBindVertexArray(0);
}

-(GLuint)loadGLTextures:(NSString *)textureFileName
{
	NSBundle *mainBundle = [NSBundle mainBundle];
    NSString *applicationPath = [mainBundle bundlePath];
    NSString *parentFolderPath = [applicationPath stringByDeletingLastPathComponent];
    NSString *textureFilePath = [parentFolderPath stringByAppendingPathComponent:textureFileName];
    NSImage *image = [[NSImage alloc] initWithContentsOfFile:textureFilePath];

	GLuint texture = 0;

    if(image)
	{
        CGImageRef imageRef = [image CGImageForProposedRect:nil context:nil hints:nil];
        int width = (int)CGImageGetWidth(imageRef);
        int height = (int)CGImageGetHeight(imageRef);

        CGDataProviderRef imageDataProviderRef = CGImageGetDataProvider(imageRef);
        CFDataRef imageDataRef = CGDataProviderCopyData(imageDataProviderRef);
        void *pixels = (void *)CFDataGetBytePtr(imageDataRef);

	    glGenTextures(1, &texture);
        // For programmable pipeline set 1 instead of 4 for better performation.
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		// Generate the mipmapped texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
        CFRelease(imageDataRef);

        // Unbind the texture else the last loaded texture will be shown in display
        // if we forgot to specify to which texture to bind in display.
        glBindTexture(GL_TEXTURE_2D, 0);
	}
    else
    {
        NSLog(@"Not able to load texture: %@", textureFilePath);
    }

	return texture;
}

-(void)makeWhiteColorTexture
{
    GLubyte whiteColorImage[4] = { (GLubyte)255, (GLubyte)255, (GLubyte)255, (GLubyte)255 };

    glGenTextures(1, &textureWhiteColor);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glBindTexture(GL_TEXTURE_2D, textureWhiteColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whiteColorImage);
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

    if(vaoSmiley)
    {
        glDeleteVertexArrays(1, &vaoSmiley);
        vaoSmiley = 0;
    }

    if(vboSmileyPosition)
    {
        glDeleteBuffers(1, &vboSmileyPosition);
        vboSmileyPosition = 0;
    }

    if(vboSmileyTexture)
    {
        glDeleteBuffers(1, &vboSmileyTexture);
        vboSmileyTexture = 0;
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

    if (textureSmiley)
	{
		glDeleteTextures(1, &textureSmiley);
		textureSmiley = 0;
	}

    if (textureWhiteColor)
	{
		glDeleteTextures(1, &textureWhiteColor);
		textureWhiteColor = 0;
	}


    [super dealloc];
}

@end

CVReturn displayLinkCallBack(CVDisplayLinkRef displayLinkRef, const CVTimeStamp *now, const CVTimeStamp *outTime, CVOptionFlags inFlags, CVOptionFlags *outFlags, void *displayLinkContext)
{
    return [(GLView *)displayLinkContext getFrameForTime:outTime];
}
