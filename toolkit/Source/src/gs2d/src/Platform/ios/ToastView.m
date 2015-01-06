//
//	Code originally written by Scarmysun user @ Stackoverflow
//	http://stackoverflow.com/questions/3522866/what-is-the-android-widget-toast-equivalent-for-ios-applications
//

#import "ToastView.h"

@interface ToastView ()
@property (strong, nonatomic, readonly) UILabel *textLabel;
@end
@implementation ToastView
@synthesize textLabel = _textLabel;

float const ToastHeight = 50.0f;
float const ToastGap = 10.0f;

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}

-(UILabel *)textLabel
{
    if (!_textLabel) {
        _textLabel = [[UILabel alloc] initWithFrame:CGRectMake(5.0, 5.0, self.frame.size.width - 10.0, self.frame.size.height - 10.0)];
        _textLabel.backgroundColor = [UIColor clearColor];
        _textLabel.textAlignment = NSTextAlignmentCenter;
        _textLabel.textColor = [UIColor whiteColor];
        _textLabel.numberOfLines = 2;
        _textLabel.font = [UIFont systemFontOfSize:13.0];
        _textLabel.lineBreakMode = NSLineBreakByCharWrapping;
        [self addSubview:_textLabel];

    }
    return _textLabel;
}

- (void)setText:(NSString *)text
{
    _text = text;
    self.textLabel.text = text;
}

+ (void)showToastInParentView: (UIView *)parentView withText:(NSString *)text withDuaration:(float)duration;
{

    //Count toast views are already showing on parent. Made to show several toasts one above another
    int toastsAlreadyInParent = 0;
    for (UIView *subView in [parentView subviews]) {
        if ([subView isKindOfClass:[ToastView class]])
        {
            toastsAlreadyInParent++;
        }
    }

    CGRect parentFrame = parentView.frame;

    float yOrigin = parentFrame.size.height - (70.0 + ToastHeight * toastsAlreadyInParent + ToastGap * toastsAlreadyInParent);

    CGRect selfFrame = CGRectMake(parentFrame.origin.x + 20.0, yOrigin, parentFrame.size.width - 40.0, ToastHeight);
    ToastView *toast = [[ToastView alloc] initWithFrame:selfFrame];

    toast.backgroundColor = [UIColor darkGrayColor];
    toast.alpha = 0.0f;
    toast.layer.cornerRadius = 4.0;
    toast.text = text;

    [parentView addSubview:toast];

    [UIView animateWithDuration:0.4 animations:^{
        toast.alpha = 0.9f;
        toast.textLabel.alpha = 0.9f;
    }completion:^(BOOL finished) {
        if(finished){

        }
    }];


    [toast performSelector:@selector(hideSelf) withObject:nil afterDelay:duration];

}

- (void)hideSelf
{

    [UIView animateWithDuration:0.4 animations:^{
        self.alpha = 0.0;
        self.textLabel.alpha = 0.0;
    }completion:^(BOOL finished) {
        if(finished){
            [self removeFromSuperview];
        }
    }];
}

@end
