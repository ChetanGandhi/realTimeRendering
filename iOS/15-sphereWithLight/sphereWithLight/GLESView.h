#import <UIKit/UIKit.h>

@interface GLESView : UIView<UIGestureRecognizerDelegate>

-(void)startRendering;
-(void)stopRendering;

@end
