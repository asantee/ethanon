
void ETHCallback_cursor(ETHEntity @thisEntity)
{
	ETHInput @input = GetInputHandle();
	const vector2 cursorPos = input.GetCursorPos();
	const vector2 cameraPos = GetCameraPos();
	thisEntity.SetPositionXY(cursorPos+cameraPos);
}