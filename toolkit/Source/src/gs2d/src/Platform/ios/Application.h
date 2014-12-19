//
//  Application.h
//  Application
//
//  Created by Andre Santee on 4/12/14.
//  Copyright (c) 2014 Asantee Games. All rights reserved.
//

#import <Foundation/Foundation.h>

#import <UIKit/UIKit.h>

#import <CoreMotion/CoreMotion.h>

#import "../NativeCommandManager.h"

class ApplicationWrapper
{
	float m_pixelDensity;

public:
	ApplicationWrapper();

	void Start();
	void RenderFrame();
	void Destroy();

	void TouchesBegan(UIView* thisView, NSSet* touches, UIEvent* event);
	void TouchesMoved(UIView* thisView, NSSet* touches, UIEvent* event);
	void TouchesEnded(UIView* thisView, NSSet* touches, UIEvent* event);
	void TouchesCancelled(NSSet* touches, UIEvent* event);
	
	void UpdateAccelerometer(CMAccelerometerData *accelerometerData);

	static Platform::NativeCommandManager m_commandManager;
};
