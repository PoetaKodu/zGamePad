// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
bool* keyevent = (bool*)ZenDef(0x0086CCC8, 0x008B27C0, 0x008C3020, 0x008D1678);
bool* keytoggle = (bool*)ZenDef(0x0086CED0, 0x008B29D4, 0x008C3260, 0x008D18B8);
zCArray<int>& keybuffer = *(zCArray<int>*)ZenDef(0x0086D2DC, 0x008B2E00, 0x008C36F0, 0x008D1D50);



void SetKeyStateAndInsert(DXKEY keyID, bool_t state) {
	bool keyEventBefore = keyevent[keyID];
	keyevent[keyID] = state != 0;
	if (keyevent[keyID]) {
		keybuffer.InsertEnd(keyID);
		keytoggle[keyID] = !keyEventBefore;
	}
}



DXKEY GetBinded(uint16 bind) {
	zCArray<uint16> keys;
	if (zinput->GetBinding(bind, keys))
		if (keys.GetNum())
			return keys[0];

	return Invalid;
}



Combination::Combination() {
	enabled = false;
	help = Null;
	clear();
}



bool Combination::operator == (const Combination& other) const {
	return combination.GetNum() == other.combination.GetNum();
}



bool Combination::operator < (const Combination& other) const {
	return combination.GetNum() > other.combination.GetNum();
}



bool Combination::operator > (const Combination& other) const {
	return combination.GetNum() < other.combination.GetNum();
}



void Combination::detectDisable(JOYKEY keys)
{
	setHelpEnabled(false);
	for (uint i = 0; i < combination.GetNum(); i++)
	{
		if (!(keys & combination[i]))
			return disable();
	}
}



void Combination::disable() {
	if (!enabled)
		return;

	enabled = false;
	if (!toggled)
		return;

	toggled = false;
	setEmulationState(False);
}



bool HasFlag(int value, int flag) {
	return (value & flag) == flag;
}



bool Combination::checkAllConditions() {
	for (uint i = 0; i < allowConditions.GetNum(); i++)
		if (!Gamepad_GetStaticCondition(allowConditions[i]))
			return false;

	for (uint i = 0; i < denyConditions.GetNum(); i++)
		if (Gamepad_GetStaticCondition(denyConditions[i]))
			return false;

	for (uint i = 0; i < allowButtons.GetNum(); i++)
		if (!zinput->KeyPressed(allowButtons[i]))
			return false;

	for (uint i = 0; i < denyButtons.GetNum(); i++)
		if (zinput->KeyPressed(denyButtons[i]))
			return false;

	setHelpEnabled(true);

	if (!checkKeyStateConditions())
		return false;

	return true;
}



bool Combination::checkKeyStateConditions() {
	for (uint i = 0; i < allowCombinations.GetNum(); i++)
		if (!HasFlag(keyStates, allowCombinations[i]))
			return false;

	for (uint i = 0; i < denyCombinations.GetNum(); i++)
		if (HasFlag(keyStates, denyCombinations[i]))
			return false;

	return true;
}



bool Combination::detectEnable(JOYKEY& keys) {
	if (!checkAllConditions())
		return false;

	if (emulation.GetNum() == 0)
		return false;

	JOYKEY flags = keys;
	bool success = true;

	for (uint i = 0; i < combination.GetNum(); i++) {
		JOYKEY key = keys & combination[i];
		flags ^= combination[i];

		if (!key) {
			success = false;
			break;
		}
	}

	if (success) {
		keys = flags;
		enable();
	}

	return success;
}



void Combination::enable() {
	if (enabled) {
		if (toggleMode && toggled) {
			toggled = false;
			setEmulationState(False);
		}

		return;
	}
	else if (hasKeysToggled())
		return;

	enabled = true;
	toggled = true;
	setEmulationState(True);
}



void Combination::setHelpEnabled(bool enable) {
	if (help != Null)
		help->Enabled = enable;
}



bool Combination::hasKeysToggled() {
	JOYKEY joyKeys = 0;
	for (uint i = 0; i < combination.GetNum(); i++)
		joyKeys |= combination[i];

	return xinputDevice.hasKeysToggled(joyKeys);
}



void Combination::addCombination(JOYKEY keys_first, ...) {
	JOYKEY* keys = &keys_first;

	for (uint i = 0; true; i++) {
		if (keys[i] == 0)
			break;

		combination += keys[i];
	}
}



void Combination::addEmulation(DXKEY keys_first, ...) {
	DXKEY* keys = &keys_first;

	for (uint i = 0; true; i++) {
		if (keys[i] == 0)
			break;

		emulation += keys[i];
	}
}



void Combination::addAllowFunctions(LPCONDFUNC conditions, ...) {
	auto* keys = &conditions;

	for (uint i = 0; true; i++) {
		if (keys[i] == 0)
			break;

		allowConditions += keys[i];
	}
}



void Combination::addDenyFunctions(LPCONDFUNC conditions, ...) {
	auto* keys = &conditions;

	for (uint i = 0; true; i++) {
		if (keys[i] == 0)
			break;

		denyConditions += keys[i];
	}
}



void Combination::addAllowButtons(DXKEY keys, ...) {
	auto* __keys = &keys;

	for (uint i = 0; true; i++) {
		if (__keys[i] == 0)
			break;

		allowButtons += __keys[i];
	}
}



void Combination::addDenyButtons(DXKEY keys, ...) {
	auto* __keys = &keys;

	for (uint i = 0; true; i++) {
		if (__keys[i] == 0)
			break;

		denyButtons += __keys[i];
	}
}



void Combination::addAllowCombinations(JOYKEY keys, ...) {
	// FIXME: this is an unsafe code
	// TODO: use variadic function template:
	// 
	// template <std::same_as<JOYKEY>... TKey>
	// addAllowCombinations(TKey... key)

	// Iterate over the next arguments
	for (auto it = &keys; ; it++)
	{
		if (*it == 0)
			break;

		allowCombinations += *it;
	}
}



void Combination::addDenyCombinations(JOYKEY keys, ...)
{
	// FIXME: this is an unsafe code
	// TODO: use variadic function template:
	// 
	// template <std::same_as<JOYKEY>... TKey>
	// addAllowCombinations(TKey... key)

	// Iterate over the next arguments
	for (auto it = &keys; ; it++)
	{
		if (*it == 0)
			break;

		denyCombinations += *it;
	}
}



void Combination::setEmulationState(bool_t state)
{
	for (uint i = 0; i < emulation.GetNum(); i++)
	{
		SetKeyStateAndInsert(emulation[i], state);
	}
}



void Combination::clear() {
	combination.Clear();
	emulation.Clear();
	allowConditions.Clear();
	denyConditions.Clear();
	allowButtons.Clear();
	denyButtons.Clear();
	allowCombinations.Clear();
	denyCombinations.Clear();
	toggleMode = false;
	toggled = false;
	keyStates = None;
	help = Null;
}



Combination::~Combination() {
}









void XInputDevice::init() {
	InitializeCompatibleXInput();
	xinputState = XINPUT_STATE();
	capabilities = PXINPUT_CAPABILITIES();
	ZeroMemory(&xinputState, sizeof(XINPUT_STATE));
	keyStates = (XInputKey)0;
	updateControls();
}



void XInputDevice::updateControls() {
	keyCombinations.Clear();
	initCombinations();

	XINPUTGETSTATE(0, &xinputState);
	for (uint i = 0; i < keyCombinations.GetNum(); i++)
		keyCombinations[i].keyStates = xinputState.Gamepad.wButtons;
}



void XInputDevice::updateVibration() {
	if (!Opt_Vibration || !vibrationIsEnabled) {
		if (vibrationMessage.Index != Invalid) {
			vibrationMessage.Index = Invalid;
			vibration.wLeftMotorSpeed = 0;
			vibration.wRightMotorSpeed = 0;
			XINPUTSETSTATE(0, &vibration);
		}

		return;
	}

	if (vibrationMessage.Index == Invalid)
		return;

	vibrationMessage.Timer.ClearUnused();
	if (vibrationMessage.Timer[0u].Await(150, true)) {

		string strength = vibrationMessage.Pattern.GetWordSmart(++vibrationMessage.Index);
		if (strength.IsEmpty()) {
			vibrationMessage.Index = Invalid;
			vibration.wLeftMotorSpeed = 0;
			vibration.wRightMotorSpeed = 0;
		}
		else {
			vibration.wLeftMotorSpeed = strength.ToInt32(); // big motor
			vibration.wRightMotorSpeed = 0;                  // small motor
		}

		XINPUTSETSTATE(0, &vibration);
	}
}



int sqrti(int a) {
	return (int)sqrt((float)a);
}



void XInputDevice::updateLeftSticksState() {
	static bool runActive = false;
	static Timer timer;
	timer.ClearUnused();

	// Maximum of Triggers or Sticks - 65536
	if (!deviceConnected)
	{
		leftStick.x = DS4Device.getLeftStick().x;
		leftStick.y = DS4Device.getLeftStick().y;
	}
	else
	{
		leftStick.x = xinputState.Gamepad.sThumbLX;
		leftStick.y = xinputState.Gamepad.sThumbLY;
	}

	int length = sqrti(leftStick.x * leftStick.x + leftStick.y * leftStick.y);
	strafePressed = false;

	if (length > DEADZONE_L) {
		strafePressed = abs(leftStick.x * 5) > abs(leftStick.y * 7);

		// Check strafe
		if (Opt_MotionType == 0 && strafePressed) {
			keyStates |= leftStick.x > 0 ?
				GameRightStrafe :
				GameLeftStrafe;
		}
		else
		{
			// Check run
			if (leftStick.y > 0 || strafePressed) {
				walkBack = false;
				if (runActive || timer[0u].Await(20)) {
					runActive = true;

					// Walk situations
					bool canWalk = length <= 25000 && !(player && player->fmode) && !Cond_CanSneaking();

					keyStates |= canWalk ?
						GameWalk :
						GameForward;
				}
			}
			else {
				keyStates |= GameBackward;
				walkBack = true;
			}
		}
	}
	else if (runActive) {
		runActive = false;
		timer[0u].Delete();
	}
}



void XInputDevice::updateRightSticksState() {

	if (!deviceConnected)
	{
		leftTrigger = (int)DS4Device.getLeftTrigger();
		rightTrigger = (int)DS4Device.getRightTrigger();
	}
	else
	{
		leftTrigger = xinputState.Gamepad.bLeftTrigger;
		rightTrigger = xinputState.Gamepad.bRightTrigger;
	}

	if (leftTrigger > 50)
		keyStates |= GameParade;

	if (rightTrigger > 50)
		keyStates |= GamePunch;
}













void XInputDevice::updateSticksState() {
	bool diveMode = Cond_Diving();
	updateLeftSticksState();

	if (!diveMode)
		updateRightSticksState();

	// Dive inverse
	int stickValX, stickValY;
	int invY = Opt_InvertY ? -1 : 1;

	if (!deviceConnected) {
		stickValX = (diveMode ? DS4Device.getLeftStick().x : DS4Device.getRightStick().x);
		stickValY = (diveMode ? DS4Device.getLeftStick().y : DS4Device.getRightStick().y * invY);
	}
	else {
		stickValX = (diveMode ? xinputState.Gamepad.sThumbLX : xinputState.Gamepad.sThumbRX);
		stickValY = (diveMode ? xinputState.Gamepad.sThumbLY : xinputState.Gamepad.sThumbRY * invY);
	}

	rightStick.x = abs(stickValX) > DEADZONE_R ? stickValX : 0;
	rightStick.y = abs(stickValY) > DEADZONE_R ? stickValY : 0;

	if (rightStick.x) rightStick.x += stickValX > 0 ? (int)-DEADZONE_R : (int)+DEADZONE_R;
	if (rightStick.y) rightStick.y += stickValY > 0 ? (int)-DEADZONE_R : (int)+DEADZONE_R;
}



bool XInputDevice::forceVideoSkipping() {
	static bool skipReady = false;

	if (!ActiveVideo) {
		if (skipReady)
			skipReady = false;

		return false;
	}

	if (!keyStates)
		skipReady = true;

	if (keyStates && skipReady) {
		SetKeyStateAndInsert(KEY_ESCAPE, True);
		skipReady = false;
	}

	return true;
}



void XInputDevice::displayDisconnect() {
	static zCMenu* menu = Null;
	if (menu == Null) {
		menu = zCMenu::Create("ZGAMEPAD_MENU_DISCONNECTED");
		if (menu) {
			ogame->Pause();
			menu->Run();
			menu->Release();
			ogame->Unpause();
			menu = Null;
		}
	}
}



void XInputDevice::updateKeyState() {
	if (!zinput)
		return;

	bool DSInputDisconnected = !DS4Device.checkConnection();
	bool XInputDisconnected = XINPUTGETSTATE(Opt_ControllerID, &xinputState) == ERROR_DEVICE_NOT_CONNECTED;

	deviceConnected = !XInputDisconnected;
	DS4Device.connected = !DSInputDisconnected;

	if (XInputDisconnected && DSInputDisconnected) {
		if (deviceConnected || DS4Device.connected) {
			displayDisconnect();
		}
		return;
	}

	if (!deviceConnected) {
		DS4Device.update();
		keyStates = DS4Device.getKeyState();
	}
	else {
		keyStates = xinputState.Gamepad.wButtons;
	}

	updateSticksState();
	keyStatesReal = keyStates;

	if (forceVideoSkipping() || !ogame)
		return;

	if (!Opt_GamepadEnabled)
		return;

#if 0
	static uint lastBattery = 0;
	uint currentbattery = getBatteryLevel();

	if (currentbattery == 0 && lastBattery > 0) {
		static zCMenu* menu = Null;
		if (menu == Null) {
			menu = zCMenu::Create("ZGAMEPAD_MENU_DISCONNECTED");
			if (menu) {
				ogame->Pause();
				menu->Run();
				menu->Release();
				ogame->Unpause();
				menu = Null;
			}
		}
	}

	lastBattery = currentbattery;
#endif

	// Important: Update a statick condition
	// information for the faster access !!!
	Gamepad_UpdateStaticConditions();

	for (uint i = 0; i < keyCombinations.GetNum(); i++)
		keyCombinations[i].detectDisable(keyStates);

	// For correclty hints drawing this
	// condition should be always true
	if (keyStates || Opt_HintsEnabled)
		for (uint i = 0; i < keyCombinations.GetNum(); i++)
			keyCombinations[i].detectEnable(keyStates);

	updateLastKeyState();

	if (player) {
		static Timer helper;
		helper.ClearUnused();
		keyStates = xinputState.Gamepad.wButtons;

		if ((keyStates & JOY_LSTICK) && (keyStates & JOY_RSTICK)) {
			if (helper[0u].Await(2000))
			{
				auto pos = player->GetPositionWorld();
				player->EmergencyResetPos(pos);
			}
		}
		else
			helper[0u].Delete();
	}
}


void XInputDevice::updateLastKeyState() {
	if (!deviceConnected)
	{
		WORD DSKeyState = DS4Device.getKeyState();
		for (uint i = 0; i < keyCombinations.GetNum(); i++)
			keyCombinations[i].keyStates = DSKeyState;
	}
	else
	{
		for (uint i = 0; i < keyCombinations.GetNum(); i++)
			keyCombinations[i].keyStates = xinputState.Gamepad.wButtons;
	}
}

void XInputDevice::updateDevice() {
	updateKeyState();
	updateVibration();
}

void XInputDevice::startVibration(string ptr) {
	vibrationMessage.Index = 0;
	vibrationMessage.Pattern = ptr;
	vibrationMessage.Timer[0u].Delete();
}

uint XInputDevice::getBatteryLevel() const {
	XINPUT_BATTERY_INFORMATION batteryInformation;
	if (!XINPUTGETBATTERYINFORMATION(Opt_ControllerID, BATTERY_DEVTYPE_GAMEPAD, &batteryInformation) != ERROR_SUCCESS)
		/*return BATTERY_LEVEL_FAKE*/;

	return batteryInformation.BatteryLevel;
}

bool XInputDevice::IsBatteryLow() const {
	return getBatteryLevel() <= BATTERY_LEVEL_LOW;
}

bool XInputDevice::isConnected() const {
	return deviceConnected;
}

bool XInputDevice::hasKeysToggled(JOYKEY keys) const
{
	for (uint i = 0; i < keyCombinations.GetNum(); i++)
	{
		auto& combo = keyCombinations[i];
		if (combo.toggleMode && combo.enabled)
		{
			auto& comboKeys = combo.combination;
			for (uint j = 0; j < comboKeys.GetNum(); j++)
			{
				if ((keys & comboKeys[j]) != 0)
					return true;
			}
		}
	}

	return false;
}



bool XInputDevice::joyPressed(JOYKEY keys) const {
	return keyStatesReal & keys;
}



bool XInputDevice::isStrafeButtonPressed() const {
	return strafePressed;
}



bool XInputDevice::isBackWalk() const {
	return walkBack;
}



void XInputDevice::getStickStatesSquare(StickState& stateLeft, StickState& stateRight) {
	stateLeft = leftStick;
	stateRight = rightStick;
}



void XInputDevice::getStickStatesCircle(StickState& stateLeft, StickState& stateRight) {
	double squareX = double(leftStick.x) / STICK_MAX_D;
	double squareY = double(leftStick.y) / STICK_MAX_D;
	stateLeft.x = int(squareX * sqrt(1.0 - pow(squareY, 2) * 0.5) * STICK_MAX_D);
	stateLeft.y = int(squareY * sqrt(1.0 - pow(squareX, 2) * 0.5) * STICK_MAX_D);

	squareX = double(rightStick.x) / STICK_MAX_D;
	squareY = double(rightStick.y) / STICK_MAX_D;
	stateRight.x = int(squareX * sqrt(1.0 - pow(squareY, 2) * 0.5) * STICK_MAX_D);
	stateRight.y = int(squareY * sqrt(1.0 - pow(squareX, 2) * 0.5) * STICK_MAX_D);
}



bool XInputDevice::hasGamepadInput() const {
	if (keyStatesReal != 0)
		return true;

	if (leftTrigger != 0 || rightTrigger != 0)
		return true;

	int leftStickLength = sqrti(leftStick.x * leftStick.x + leftStick.y * leftStick.y);
	if (leftStickLength > DEADZONE_L)
		return true;

	int rightStickLength = sqrti(rightStick.x * rightStick.x + rightStick.y * rightStick.y);
	if (rightStickLength > DEADZONE_R)
		return true;

	return false;
}



XInputDevice::~XInputDevice() {
	startVibration("0");
	updateVibration();
}
}