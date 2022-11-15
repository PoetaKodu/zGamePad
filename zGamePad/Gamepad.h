// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
#define STICK_MAX   32768
#define STICK_MAX_D 32768.0
#define STICK_MAX_F 32768.0f
#define DEADZONE_L  15000.0f
#define DEADZONE_R  10000.0f

struct GamepadControlInfo {
	string fileName;
	string styleName;
	auto operator==(const string& fileName) const -> bool;

	static auto createList() -> void;
	static auto registerStyleInfo(const string& fileName, const string& styleName) -> void;

	static Array<GamepadControlInfo> controlsList;
};

typedef bool(*LPCONDFUNC)();
typedef int JOYKEY, DXKEY;
struct zTCombination_Help;


struct Combination {
	string				id;
	Array<JOYKEY>		combination;
	Array<DXKEY>		emulation;
	Array<bool (*)()>	allowConditions;
	Array<DXKEY>		allowButtons;
	Array<DXKEY>		allowCombinations;
	Array<bool(*)()>	denyConditions;
	Array<DXKEY>		denyButtons;
	Array<DXKEY>		denyCombinations;
	bool				enabled;
	bool				toggleMode;
	bool				toggled;
	JOYKEY				keyStates;
	zTCombination_Help*	help;

	Combination();
	bool operator == (const Combination& other) const;
	bool operator <  (const Combination& other) const;
	bool operator >  (const Combination& other) const;
	~Combination();

	void enable();
	void disable();
	bool detectEnable(JOYKEY& keys);
	void detectDisable(JOYKEY keys);

	bool hasKeysToggled();
	bool checkAllConditions();
	bool checkKeyStateConditions();
	void setHelpEnabled(bool enable);
	void addCombination(JOYKEY keys, ...);
	void addEmulation(DXKEY keys, ...);
	void addAllowFunctions(LPCONDFUNC conditions, ...);
	void addAllowButtons(DXKEY keys, ...);
	void addAllowCombinations(JOYKEY keys, ...);
	void addDenyFunctions(LPCONDFUNC conditions, ...);
	void addDenyButtons(DXKEY keys, ...);
	void addDenyCombinations(JOYKEY keys, ...);
	void setEmulationState(bool_t state);
	void clear();
};



struct VibrationMessage {
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
	friend class GamepadControlInfo;

	string					fileName;
	JOYKEY					keyStatesReal;
	JOYKEY					keyStates;
	StickState				leftStick;
	StickState				rightStick;
	int						leftTrigger;
	int						rightTrigger;
	bool					deviceConnected;
	bool					strafePressed;
	bool					walkBack;
	Array<Combination>		keyCombinations;
	VibrationMessage		vibrationMessage;

	auto initCombinations() -> void;
	auto updateVibration() -> void;
	auto updateLeftSticksState() -> void;
	auto updateRightSticksState() -> void;
	auto updateSticksState() -> void;
	auto updateKeyState() -> void;
	auto updateLastKeyState() -> void;
	auto forceVideoSkipping() -> bool;
	auto parseControlFile(string const& fileName) -> bool;
	auto parseControlFileStrings(string const& fileName) -> bool;
	auto parseControlsId(Combination& combination, string row) -> void;
	auto parseControlsCombination(Combination& combination, string row) -> void;
	auto parseControlsEmulation(Combination& combination, string row) -> void;
	auto parseControlsEndRecord(Combination& combination) -> void;
	auto parseControlsCondition(Combination& combination, string row) -> void;
	auto parseControlsHelp(Combination& combination, string row) -> void;
	auto parseControlsStringName(string& stringName, string row) -> void;
	auto parseControlsStyleName(string const& fileName, string row) -> void;
	static auto parseControlsStringText(string& stringName, string row) -> void;

	void displayDisconnect();

	
public:

	XINPUT_STATE			xinputState;
	PXINPUT_CAPABILITIES	capabilities;
	XINPUT_VIBRATION		vibration;
	static bool				vibrationIsEnabled;

	~XInputDevice();

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




#define KEYRECORD_BEGIN { Combination _comb;
#define COMBINATION                     _comb.addCombination(
#define EMULATION                 , 0); _comb.addEmulation(
#define ALLOWCONDITION            , 0); _comb.addAllowFunctions(
#define DENYCONDITION             , 0); _comb.addDenyFunctions(
#define KEYRECORD_END             , 0); keyCombinations.InsertSorted( _comb ); }




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