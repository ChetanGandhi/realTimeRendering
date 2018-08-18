#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "GLESView.h"
#import "vmath.h"

#define checkImageWidth 64
#define checkImageHeight 64

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
    
    GLuint vertexShaderObject;
    GLuint fragmentShaderObject;
    GLuint shaderProgramObject;
    
    GLfloat angleCube;
    GLfloat anglePyramid;
    GLfloat speed;
    
    GLuint vaoSquare;
    GLuint vboSquarePosition;
    GLuint vboSquareTexture;
    GLuint mvpUniform;
    GLuint textureSamplerUniform;
    GLuint textureCheckerboard;
    
    GLubyte checkImage[checkImageHeight][checkImageWidth][4];
    vmath::mat4 perspectiveProjectionMatrix;

    CADisplayLink *displayLink;

    NSInteger aimationFrameInterval;

    BOOL isRendering;
}

-(instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];

    if(self)
    {
        vertexShaderObject = 0;
        fragmentShaderObject = 0;
        shaderProgramObject = 0;
        
        vaoSquare = 0;
        vboSquarePosition = 0;
        vboSquareTexture = 0;
        mvpUniform = 0;
        textureSamplerUniform = 0;
        textureCheckerboard = 0;
        
        angleCube = 0.0f;
        anglePyramid = 0.0f;
        speed = 0.5f;

        if(![self initializeContext])
        {
            // Return nil as this will release 'self'
            return nil;
        }
        
        isRendering = NO;
        aimationFrameInterval = 60;

        // Initialize the shaders and shader program object.
        
        [self initializeGestures];
        [self initializeVertexShader];
        [self initializeFragmentShader];
        [self initializeShaderProgram];
        [self initializeSquareBuffers];
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        
        // Disable face culling to see back side of object when rotated.
        // glEnable(GL_CULL_FACE);
        
        glDepthFunc(GL_LEQUAL);
        
        perspectiveProjectionMatrix = vmath::mat4::identity();
        
        [self loadCheckerboardTexture];
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
    [self setNeedsDisplay];
}

-(void)onLongPress:(UILongPressGestureRecognizer *)gestureRecognizer
{
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

-(void)initializeVertexShader
{
    vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
    
    const GLchar *vertexShaderCode = "#version 300 es" \
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
                exit(EXIT_FAILURE);
            }
        }
    }
}

-(void)initializeFragmentShader
{
    fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    
    const GLchar *fragmentShaderCode = "#version 300 es" \
    "\n" \
    "precision highp float;"\
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
                exit(EXIT_FAILURE);
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
                exit(EXIT_FAILURE);
            }
        }
    }
    
    // After linking get the value of MVP uniform location from the shader program.
    mvpUniform = glGetUniformLocation(shaderProgramObject, "mvpMatrix");
    
    // Get the texture sampler uniform location from shader program.
    textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "textureSampler0");
}

-(void)initializeSquareBuffers
{
    const GLfloat squareTextureCoordinates[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };
    
    glGenVertexArrays(1, &vaoSquare);
    glBindVertexArray(vaoSquare);
    
    glGenBuffers(1, &vboSquarePosition);
    glBindBuffer(GL_ARRAY_BUFFER, vboSquarePosition);
    
    // As we are using dynamic draw, pass the size hardcoded
    // i.e. 12 * sizeof(GLfloat), 12 is number of coordinates in position array.
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(CG_ATTRIBUTE_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(CG_ATTRIBUTE_VERTEX_POSITION);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glGenBuffers(1, &vboSquareTexture);
    glBindBuffer(GL_ARRAY_BUFFER, vboSquareTexture);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareTextureCoordinates), squareTextureCoordinates, GL_STATIC_DRAW);
    glVertexAttribPointer(CG_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(CG_ATTRIBUTE_TEXTURE0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindVertexArray(0);
}

-(void)render:(id)sender
{
    [EAGLContext setCurrentContext:context];

    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    glUseProgram(shaderProgramObject);
    
    // Translation is same for both the shape, hence we are doing it in display.
    vmath::mat4 modelViewMatrix = vmath::mat4::identity();
    vmath::mat4 modelViewProjectionMatrix = vmath::mat4::identity();
    
    // Translate the modal view matrix.
    modelViewMatrix = vmath::translate(0.0f, 0.0f, -3.6f);
    
    // Multiply modelViewMatrix and perspectiveProjectionMatrix to get modelViewProjectionMatrix
    // Oder of multiplication is very important projectionMatrix * modelMatrix * viewMatrix
    // As we have model and view matrix combined, we just have to multiply projectionMatrix and modelViewMatrix
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
    
    // Pass modelViewProjectionMatrix to vertex shader in 'mvpMatrix' variable defined in shader.
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    
    [self drawStraightSquare];
    [self drawTiltedSquare];
    
    glUseProgram(0);
    
    [context presentRenderbuffer:GL_RENDERBUFFER];
}

-(void)drawStraightSquare
{
    // Now bind the VAO to which we want to use
    glBindVertexArray(vaoSquare);
    
    GLfloat squareVertices[] = {
        0.0f, 1.0f, 0.0f,
        -2.0f, 1.0f, 0.0f,
        -2.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f
    };
    
    glBindBuffer(GL_ARRAY_BUFFER, vboSquarePosition);
    
    // bind the above position coordinates here.
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // Enable 0th texture
    glActiveTexture(GL_TEXTURE0);
    
    // Bind with square texture
    glBindTexture(GL_TEXTURE_2D, textureCheckerboard);
    
    // Enable 0th sampler.
    glUniform1i(textureSamplerUniform, 0);
    
    // Draw the square
    // 4 is number of vertices in the array i.e. element count in squareVertices divide by 3 (x, y, z) component
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    // unbind the vao
    glBindVertexArray(0);
}

-(void)drawTiltedSquare
{
    // Now bind the VAO to which we want to use
    glBindVertexArray(vaoSquare);
    
    GLfloat tiltedSquareVertices[] = {
        2.41421f, 1.0f, -1.41421f,
        1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        2.41421f, -1.0f, -1.41421f
    };
    
    glBindBuffer(GL_ARRAY_BUFFER, vboSquarePosition);
    
    // bind the above position coordinates here.
    glBufferData(GL_ARRAY_BUFFER, sizeof(tiltedSquareVertices), tiltedSquareVertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // Enable 0th texture
    glActiveTexture(GL_TEXTURE0);
    
    // Bind with square texture
    glBindTexture(GL_TEXTURE_2D, textureCheckerboard);
    
    // Enable 0th sampler.
    glUniform1i(textureSamplerUniform, 0);
    
    // Draw the tilted square
    // 4 is number of vertices in the array for each face
    // i.e. element count in tiltedSquareVertices for each face divide by 3 (x, y, z) component
    // 2nd parameter is the index from which we need to use vertices.
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    // unbind the vao
    glBindVertexArray(0);
}

-(void)loadCheckerboardTexture
{
    [self makeCheckImage];
    glGenTextures(1, &textureCheckerboard);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glBindTexture(GL_TEXTURE_2D, textureCheckerboard);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
    
    // Unbind the texture else the last loaded texture will be shown in display
    // if we fo not forgot to specify to which texture to bind in display.
    glBindTexture(GL_TEXTURE_2D, 0);
}

-(void)makeCheckImage
{
    int heightCounter = 0;
    int widthCounter = 0;
    int colorWhite = 0;
    
    for (heightCounter = 0; heightCounter < checkImageHeight; ++heightCounter)
    {
        for (widthCounter = 0; widthCounter < checkImageWidth; ++widthCounter)
        {
            colorWhite = (((heightCounter & 0x8) == 0) ^ ((widthCounter & 0x8) == 0)) * 255;
            checkImage[heightCounter][widthCounter][0] = (GLubyte)colorWhite;
            checkImage[heightCounter][widthCounter][1] = (GLubyte)colorWhite;
            checkImage[heightCounter][widthCounter][2] = (GLubyte)colorWhite;
            checkImage[heightCounter][widthCounter][3] = (GLubyte)255;
        }
    }
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
    perspectiveProjectionMatrix = vmath::perspective(60.0f, (GLfloat)width / (GLfloat)height, 1.0f, 30.0f);
    
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
    
    if(vaoSquare)
    {
        glDeleteVertexArrays(1, &vaoSquare);
        vaoSquare = 0;
    }
    
    if(vboSquarePosition)
    {
        glDeleteBuffers(1, &vboSquarePosition);
        vboSquarePosition = 0;
    }
    
    if(vboSquarePosition)
    {
        glDeleteBuffers(1, &vboSquarePosition);
        vboSquarePosition = 0;
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
    
    if (textureCheckerboard)
    {
        glDeleteTextures(1, &textureCheckerboard);
        textureCheckerboard = 0;
    }
    
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
