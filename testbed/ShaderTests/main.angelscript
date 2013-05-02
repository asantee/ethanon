void main()
{
	LoadScene("scenes/scene.esc", "", "onSceneUpdate");
	SetFixedHeight(512.0f);
}

void onSceneUpdate()
{
	ETHInput@ input = GetInputHandle();
	const vector3 accelerometer(input.GetAccelerometerData());
	SetParallaxOrigin(normalize(vector2(accelerometer.x, accelerometer.y)));	
}

void ETHCallback_barrel_blue_light(ETHEntity@ thisEntity)
{
	ETHInput@ input = GetInputHandle();
	thisEntity.AddToPositionXY(input.GetTouchMove(0));
	thisEntity.AddToPositionZ(input.GetTouchMove(1).y);
}
