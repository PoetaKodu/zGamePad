// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {


void DSDevice::init()
{
	deviceCount = JslConnectDevices();
}

//bool DSDevice::checkConnection() const
//{
//	// FIXME: the following code doesn't work.
//	// It is equivalent to returning the same value we previously obtained
//	// from the JslConnectDevices() function.
//	// See: https://github.com/JibbSmart/JoyShockLibrary/blob/master/JoyShockLibrary/JoyShockLibrary.cpp#L336
//	// 
//	// constexpr auto MAX_JOYSTICKS = 16;
//	// std::array<int, MAX_JOYSTICKS> devices;
//	// return JslGetConnectedDeviceHandles(devices.data(), DeviceCount) > 0;
//
//	return deviceCount > 0;
//}

void DSDevice::update()
{
	constexpr auto StickScale	= 32767.f;
	constexpr auto TriggerScale	= 255.f;

	gamepadState = JslGetSimpleState(0);
	if (connected)
	{
		keyStates		= gamepadState.buttons;
		
		leftStick.x		= int(gamepadState.stickLX * StickScale);
		leftStick.y		= int(gamepadState.stickLY * StickScale);
		rightStick.x	= int(gamepadState.stickRX * StickScale);
		rightStick.y	= int(gamepadState.stickRY * StickScale);
		leftTrigger		= gamepadState.lTrigger * StickScale;
		rightTrigger	= gamepadState.rTrigger * StickScale;
	}
}

}