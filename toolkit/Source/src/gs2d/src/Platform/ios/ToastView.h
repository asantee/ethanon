//
//	Code originally written by Scarmysun user @ Stackoverflow
//	http://stackoverflow.com/questions/3522866/what-is-the-android-widget-toast-equivalent-for-ios-applications
//

#import <UIKit/UIKit.h>

@interface ToastView : UIView

@property (strong, nonatomic) NSString *text;

+ (void)showToastInParentView: (UIView *)parentView withText:(NSString *)text withDuaration:(float)duration;

@end
