#import "MyView.h"

@implementation MyView
{
    NSString *message;
}

-(id)initWithFrame:(NSRect)rect
{
    self = [super initWithFrame:rect];
    
    if(self)
    {
        [[self window] setContentView:self];
        message = @"Hello World!!!";
    }
    
    return self;
}

-(void)drawRect:(NSRect)rect
{
    NSColor *color = [NSColor blackColor];
    [color set];
    
    NSRectFill(rect);
    
    NSDictionary *textAttributes = [NSDictionary dictionaryWithObjectsAndKeys:
                                    [NSFont fontWithName:@"Helvetica" size:32], NSFontAttributeName,
                                    [NSColor greenColor], NSForegroundColorAttributeName,
                                    nil];
    
    NSSize textSize = [message sizeWithAttributes:textAttributes];
    NSPoint point = NSMakePoint((rect.size.width - textSize.width) / 2.0, (rect.size.height - textSize.height) / 2.0);
    
    [message drawAtPoint:point withAttributes:textAttributes];
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
            message = @"F or f key is pressed...";
            [[self window] toggleFullScreen:self]; // This will auto invoke repainting.
            break;
            
        default:
            break;
    }
}

-(void)mouseDown:(NSEvent *)event
{
    message = @"Left mouse button was clicked...";
    [self setNeedsDisplay:YES]; // specifies that repainting is needed.
}

-(void)rightMouseDown:(NSEvent *)event
{
    message = @"Right mouse button was clicked...";
    [self setNeedsDisplay:YES]; // specifies that repainting is needed.
}

-(void)mouseDragged:(NSEvent *)event
{
    
}

-(void)dealloc
{
    [super dealloc];
}

@end
