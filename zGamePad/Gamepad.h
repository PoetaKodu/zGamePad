// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
#define STICK_MAX   32768
#define STICK_MAX_D 32768.0
#define STICK_MAX_F 32768.0f
#define DEADZONE_L  15000.0f
#define DEADZONE_R  10000.0f

struct zTGamepadControlInfo {
	string FileName;
	string StyleName;
	bool operator == (const string& fileName) const;

	static void CreateGamepadControlsList();
	static Array<zTGamepadControlInfo> GamepadControlsList;
	static void RegisterStyleInfo(const string& fileName, const string& styleName);
};

typedef bool(*LPCONDFUNC)();
typedef int JOYKEY, DXKEY;
struct zTCombination_Help;


struct zTCombination {
	string              Id;
	Array<JOYKEY>       Combination;
	Array<DXKEY>        Emulation;
	Array<bool (*)()>   AllowConditions;
	Array<DXKEY>        AllowButtons;
	Array<DXKEY>        AllowCombinations;
	Array<bool(*)()>   DenyConditions;
	Array<DXKEY>        DenyButtons;
	Array<DXKEY>        DenyCombinations;
	bool                Enabled;
	bool                ToggleMode;
	bool                Toggled;
	JOYKEY              KeyStates;
	zTCombination_Help* Help;

	zTCombination();
	bool operator == (const zTCombination& other) const;
	bool operator <  (const zTCombination& other) const;
	bool operator >  (const zTCombination& other) const;
	~zTCombination();

	void CheckDisable(JOYKEY& keys);
	void Disable();
	bool CheckEnable(JOYKEY& keys);
	bool hasKeysToggled();
	bool CheckAllConditions();
	bool CheckKeyStateConditions();
	void Enable();
	void SetHelpEnable(bool enable);
	void AddCombination(JOYKEY keys ...);
	void AddEmulation(DXKEY keys ...);
	void AddAllowFunctions(LPCONDFUNC conditions ...);
	void AddAllowButtons(DXKEY keys ...);
	void AddAllowCombinations(JOYKEY keys ...);
	void AddDenyFunctions(LPCONDFUNC conditions ...);
	void AddDenyButtons(DXKEY keys ...);
	void AddDenyCombinations(JOYKEY keys ...);
	void SetEmulationState(bool_t state);
	void Clear();
};



struct zTVibrationMessage {
	Timer  Timer;
	uint   Index;
	string Pattern;
};




enum XInputKey : int {
	ArrowUp = 1 << 0,
	ArrowDown = 1 << 1,
	ArrowLeft = 1 << 2,
	ArrowRight = 1 << 3,

	// Dualshock names
	ButtonStart = 1 << 4,
	ButtonSelect = 1 << 5,
	// xbox names
	ButtonMenu = ButtonStart,
	ButtonView = ButtonSelect,

	ButtonLeftStick = 1 << 6,
	ButtonRightStick = 1 << 7,
	ButtonL1 = 1 << 8,
	ButtonR1 = 1 << 9,

	// Dualshock names
	ButtonCross = 1 << 12,
	ButtonCircle = 1 << 13,
	ButtonSquare = 1 << 14,
	ButtonTriangle = 1 << 15,
	// xbox names
	ButtonA = ButtonCross,
	ButtonB = ButtonCircle,
	ButtonX = ButtonSquare,
	ButtonY = ButtonTriangle,

	GameWalk = 1 << 16,
	GameForward = 1 << 17,
	GameBackward = 1 << 18,
	GameLeftStrafe = 1 << 19,
	GameRightStrafe = 1 << 20,
	GamePunch = 1 << 21,
	GameParade = 1 << 22,

	ArrowPad_Full = 1 << 25,
	ArrowPad_UpDown = 1 << 26,
	ArrowPad_LeftRight = 1 << 27,
	LeftStick = 1 << 28,
	RightStick = 1 << 29,
};

struct StickState {
	int x;
	int y;
	int length() const {
		return static_cast<int>(std::sqrt(double(x * x + y * y)));
	}
};




class XInputDevice {
	friend class zCInput_Win32;
	friend class zCGamepadSpellBook;
	friend class zTGamepadControlInfo;

	string FileName;
	JOYKEY KeyStatesReal;
	JOYKEY KeyStates;
	StickState LeftStick;
	StickState RightStick;
	int LeftTrigger;
	int RightTrigger;
	bool DeviceConnected;
	bool StrafePressed;
	bool WalkBack;

	void InitCombinations();
	void UpdateVibration();
	void UpdateLeftSticksState();
	void UpdateRightSticksState();
	void UpdateSticksState();
	void UpdateKeyState();
	void UpdateLastKeyState();
	bool ForceVideoSkipping();
	bool ParseControlFile(const string& fileName);
	bool ParseControlFileStrings(const string& fileName);
	void ParseControlsId(zTCombination& combination, string row);
	void ParseControlsCombination(zTCombination& combination, string row);
	void ParseControlsEmulation(zTCombination& combination, string row);
	void ParseControlsEndRecord(zTCombination& combination);
	void ParseControlsCondition(zTCombination& combination, string row);
	void ParseControlsHelp(zTCombination& combination, string row);
	void ParseControlsStringName(string& stringName, string row);
	void ParseControlsStyleName(const string& fileName, string row);
	static void ParseControlsStringText(string& stringName, string row);
	void DisplayDisconnect();
	Array<zTCombination> KeyCombinations;
	zTVibrationMessage VibrationMessage;
public:

	XINPUT_STATE Gamepad;
	PXINPUT_CAPABILITIES Capabilities;
	XINPUT_VIBRATION Vibration;
	static bool VibrationIsEnabled;

	auto init() -> void;
	auto updateControls() -> void;
	auto updateDevice() -> void;
	auto startVibration(string ptr) -> void;
	auto getBatteryLevel() const->uint;
	auto IsBatteryLow() const -> bool;
	auto isConnected() const -> bool;
	auto hasKeysToggled(JOYKEY keys) const -> bool;
	auto joyPressed(JOYKEY keys) const -> bool;
	auto isStrafeButtonPressed() const -> bool;
	auto isBackWalk() const -> bool;
	auto getStickStatesSquare(StickState& stateLeft, StickState& stateRight) -> void;
	auto getStickStatesCircle(StickState& stateLeft, StickState& stateRight) -> void;
	auto hasGamepadInput() const -> bool;
	~XInputDevice();
};

auto xinputDevice = XInputDevice();



DXKEY GetBinded(uint16 bind);



using LPCONDITION = bool(*)();
void Gamepad_SetStaticCondition(LPCONDITION condition);
bool Gamepad_GetStaticCondition(LPCONDITION condition);
void Gamepad_UpdateStaticConditions();

bool Cond_FightMode();
bool Cond_FightModeMelee();
bool Cond_FightModeRange();
bool Cond_FightModeMagic();
bool Cond_CanShoot();
bool Cond_CanSneaking();
bool Cond_Diving();
bool Cond_HasFocusVob();
bool Cond_HasFocusNpc();
bool Cond_TargetIsLocked();
bool Cond_OnChooseWeapon();
bool Cond_InventoryIsOpen();
bool Cond_InTransformation();
bool Cond_CanQuickPotionDrink();
bool Cond_VideoIsOpen();
bool Cond_CanLockTarget();
bool Cond_G1();
bool Cond_G2();
bool Cond_IsDialogTop();
bool Cond_IsDocumentTop();
bool Cond_IsOverlayTop();
bool Cond_IsMenuTop();
bool Cond_OnSpellBook();
bool Cond_IsPlayerTalking();
bool Cond_InterfaceIsOpen();
bool Cond_HasLeftContainer();
bool Cond_UsesPicklock();
bool Cond_IsOnTrade();
bool Cond_ConsoleIsOpen();




#define KEYRECORD_BEGIN { zTCombination _comb;
#define COMBINATION                     _comb.AddCombination(
#define EMULATION                 , 0); _comb.AddEmulation(
#define ALLOWCONDITION            , 0); _comb.AddAllowFunctions(
#define DENYCONDITION             , 0); _comb.AddDenyFunctions(
#define KEYRECORD_END             , 0); KeyCombinations.InsertSorted( _comb ); }




static auto GetScaleMultiplier() -> float {
	float scale = 1.0f;
	Union.GetSysPackOption().Read(scale, "INTERFACE", "Scale");
	return scale;
}

static auto CalculateInterfaceScale() -> float {
	static float scale = GetScaleMultiplier();

	if (scale == 0)
		return 1.0f;

	if (scale != 1)
		return scale;

	int ydim = zrenderer->vid_ydim;
	if (ydim < 2048)
		return 1.0f;

	return (float)ydim / 1080.0f;
}

auto SetKeyStateAndInsert(DXKEY keyID, bool_t state) -> void;
auto GetBinded(uint16 bind) -> DXKEY;
auto Input_GetMousePosReal(float& x, float& y, float& z) -> void;






// XInput 1.3 - 1.4 compatible code
struct XINPUT_BATTERY_INFORMATION {
	BYTE BatteryType;
	BYTE BatteryLevel;
};

using LPXINPUTGETSTATE				= DWORD(WINAPI*)(DWORD, XINPUT_STATE*);
using LPXINPUTSETSTATE				= DWORD(WINAPI*)(DWORD, XINPUT_VIBRATION*);
using LPXINPUTGETBATTERYINFORMATION	= DWORD(WINAPI*)(DWORD, BYTE, XINPUT_BATTERY_INFORMATION*);

#ifndef BATTERY_DEVTYPE_GAMEPAD
	#define BATTERY_DEVTYPE_GAMEPAD    0x00
	#define BATTERY_DEVTYPE_HEADSET    0x01
#endif

#ifndef BATTERY_LEVEL_EMPTY
	#define BATTERY_LEVEL_EMPTY			0x00
	#define BATTERY_LEVEL_LOW			0x01
	#define BATTERY_LEVEL_MEDIUM		0x02
	#define BATTERY_LEVEL_FULL			0x03
#endif
#define BATTERY_LEVEL_FAKE				0x04

static DWORD WINAPI XInputgetBatteryInformation_Fake(DWORD device, BYTE flags, XINPUT_BATTERY_INFORMATION* info) {
	info->BatteryLevel = BATTERY_LEVEL_FAKE;
	return ERROR_SUCCESS;
}


LPXINPUTGETSTATE XINPUTGETSTATE = &XInputGetState;
LPXINPUTSETSTATE XINPUTSETSTATE = &XInputSetState;
LPXINPUTGETBATTERYINFORMATION XINPUTGETBATTERYINFORMATION = &XInputgetBatteryInformation_Fake;


static auto LoadHighestXInput() -> HMODULE {
	auto xinput = LoadLibrary("XInput1_4.dll");
	if (!xinput) {
		xinput = LoadLibrary("XInput1_3.dll");
	}

	return xinput;
}

static void InitializeCompatibleXInput() {
	auto xinput = LoadHighestXInput();

	if (xinput) {
		XINPUTGETSTATE = (LPXINPUTGETSTATE)GetProcAddress(xinput, "XInputGetState");
		XINPUTSETSTATE = (LPXINPUTSETSTATE)GetProcAddress(xinput, "XInputSetState");
		XINPUTGETBATTERYINFORMATION = (LPXINPUTGETBATTERYINFORMATION)GetProcAddress(xinput, "XInputGetBatteryInformation");
	}
}



struct MouseState {
	int posX;
	int posY;
	int posZ;
	int buttonPressedLeft;
	int buttonPressedMid;
	int buttonPressedRight;
};

inline static auto GetWrapperMouseState() -> MouseState&
{	
	if (auto zMouseFix = GetModuleHandle("zMouseFix.dll"))
	{
		auto mouseState = reinterpret_cast<MouseState*>( GetProcAddress(zMouseFix, "wrapperMouseState") );

		if (mouseState)
		{
			return *mouseState;
		}
	}

	// Default gothic mouse state
	return *(MouseState*)ZenDef(0x0086CCAC, 0x008B27A8, 0x008C3004, 0x008D165C);
}

static auto& wrapperMouseState = GetWrapperMouseState();
}