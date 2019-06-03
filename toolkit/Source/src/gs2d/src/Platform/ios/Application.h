//
//  Application.h
//  Application
//
//  Created by Andre Santee on 4/12/14.
//  Copyright (c) 2014 Asantee Games. All rights reserved.
//

#ifndef GS2D_IOS_APPLICATION_H_
#define GS2D_IOS_APPLICATION_H_

#import <Foundation/Foundation.h>

#import <UIKit/UIKit.h>

#import <CoreMotion/CoreMotion.h>

#import <GLKit/GLKit.h>

#import "../../Math/GameMath.h"

#import "../NativeCommandManager.h"

class ApplicationWrapper
{
	float m_pixelDensity;
	
	NSMutableArray* m_touches;
	NSLock* m_arrayLock;

public:
	ApplicationWrapper();

	void Start(GLKView *view);
	void Update();
	void RenderFrame(GLKView *view);
	void Destroy();
	static void Restore();

	void DetectJoysticks();
	void ForceGamepadPause();

	static gs2d::math::Vector2 GetScreenSize(GLKView* view);

	void TouchesBegan(UIView* thisView, NSSet* touches, UIEvent* event);
	void TouchesMoved(UIView* thisView, NSSet* touches, UIEvent* event);
	void TouchesEnded(UIView* thisView, NSSet* touches, UIEvent* event);
	void TouchesCancelled(UIView* thisView, NSSet* touches, UIEvent* event);
	
	void UpdateAccelerometer(CMAccelerometerData *accelerometerData);

	static Platform::NativeCommandManager m_commandManager;
};

#endif
