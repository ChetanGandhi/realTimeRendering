#import "MyView.h"

@implementation MyView
{
    NSString *message;
}

-(instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    
    if(self)
    {
        [self setBackgroundColor:[UIColor whiteColor]];
        message = @"Hell World!!!";
        
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
    
    return self;
}

- (void)drawRect:(CGRect)rect
{
    UIColor *fillColor = [UIColor blackColor];
    [fillColor set];
    UIRectFill(rect);
    
    NSMutableDictionary *textAttributes = [[NSMutableDictionary alloc] init];
    [textAttributes setObject:[UIFont fontWithName:@"Helvetica" size:24] forKey:NSFontAttributeName];
    [textAttributes setObject:[UIColor greenColor] forKey:NSForegroundColorAttributeName];
    
    CGSize textSize = [message sizeWithAttributes:textAttributes];
    CGPoint textPosition = CGPointMake((rect.size.width - textSize.width) / 2.0f, (rect.size.height - textSize.height) / 2.0f);
    
    [message drawAtPoint:textPosition withAttributes:textAttributes];
    
    [textAttributes release];
    textAttributes = nil;
}

-(BOOL)acceptsFirstResponder
{
    return YES;
}

-(void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
//    message = @"touchBegan event.";
//    [self setNeedsDisplay];
}

-(void)onSingleTap:(UIGestureRecognizer *)gestureRecognizer
{
    message = @"Single Tap";
    [self setNeedsDisplay];
}

-(void)onDoubleTap:(UIGestureRecognizer *)gestureRecognizer
{
    message = @"Double Tap";
    [self setNeedsDisplay];
}

-(void)onLongPress:(UILongPressGestureRecognizer *)gestureRecognizer
{
    message = @"Long Press";
    [self setNeedsDisplay];
}

-(void)onSwip:(UISwipeGestureRecognizer *)gestureRecognizer
{
    [self release];
    exit(EXIT_SUCCESS);
}

-(void)dealloc
{
    [super dealloc];
}

@end
