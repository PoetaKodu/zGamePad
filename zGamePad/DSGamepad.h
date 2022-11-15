// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {

class DSDevice {

	JOY_SHOCK_STATE		gamepadState;
	WORD				keyStates;
	zTStickState		leftStick;
	zTStickState		rightStick;
	float				leftTrigger;
	float				rightTrigger;
	int					deviceCount;

public:
	bool				connected;


	void init();
	void update();

	auto getKeyState() const -> WORD {
		return keyStates;
	}
	auto getLeftStick() const -> zTStickState {
		return leftStick;
	}
	auto getRightStick() const -> zTStickState {
		return rightStick;
	}
	auto getLeftTrigger() const -> float {
		return leftTrigger;
	}
	auto getRightTrigger() const -> float {
		return rightTrigger;
	}
	auto checkConnection() const -> bool {
		// NOTE: See .cpp file for complete description.
		return deviceCount > 0;
	}
};

auto DS4Device = DSDevice();

}