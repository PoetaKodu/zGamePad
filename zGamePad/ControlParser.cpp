// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
Array<GamepadControlInfo> GamepadControlInfo::controlsList;

void GamepadControlInfo::createList() {
	Array<string> namesList;

	// Find physical control list
	char** fileTable = Null;
	long count = vdf_filelist_physical(fileTable);
	for (long i = 0; i < count; i++) {
		string fileName = fileTable[i];
		if (fileName.EndWith(".GAMEPAD"))
			namesList |= fileName.GetWord("\\");
	}

	delete[] fileTable;

	// Find virtual control list
	count = vdf_filelist_physical(fileTable);
	for (long i = 0; i < count; i++) {
		string fileName = fileTable[i];
		if (fileName.EndWith(".GAMEPAD"))
			namesList |= fileName.GetWord("\\");
	}

	delete[] fileTable;

	for (uint i = 0; i < namesList.GetNum(); i++) {
		xinputDevice.parseControlFileStrings(namesList[i]);
	}
	// for( string fileName : namesList )
	// xinputDevice.parseControlFileStrings( fileName );
}

void GamepadControlInfo::registerStyleInfo(const string& fileName, const string& styleName) {
	uint index = controlsList.SearchEqual(fileName);
	if (index == Invalid) {
		GamepadControlInfo& info = controlsList.Create();
		info.fileName = fileName;
		info.styleName = styleName;
	}
}

bool GamepadControlInfo::operator == (const string& fileName) const {
	return this->fileName == fileName;
}





Array<zTHelpString> zTHelpString::HelpStrings;

zTHelpString::zTHelpString() {
	// pass
}



inline TSystemLangID GetLangID(const string& lang) {
	if (lang == "RUS") return Lang_Rus;
	if (lang == "ENG") return Lang_Eng;
	if (lang == "GER") return Lang_Ger;
	if (lang == "DEU") return Lang_Ger;
	if (lang == "POL") return Lang_Pol;
	if (lang == "ROU") return Lang_Rou;
	if (lang == "ITA") return Lang_Ita;
	if (lang == "CZE") return Lang_Cze;
	if (lang == "ESP") return Lang_Esp;
	return Lang_Eng;
}



void zTHelpString::SetText(const string& text, const string& lang) {
	TSystemLangID langID = GetLangID(lang);

	for (uint i = 0; i < Items.GetNum(); i++) {
		if (Items[i].LangID == langID) {
			Items[i].Text = text;
			return;
		}
	}

	auto& item = Items.Create();
	item.LangID = langID;
	item.Text = text;
}



string zTHelpString::GetText() {
	TSystemLangID currentLandID = Union.GetSystemLanguage();
	TSystemLangID alterLandID = Lang_Eng;

	for (uint i = 0; i < Items.GetNum(); i++)
		if (Items[i].LangID == currentLandID)
			return Items[i].Text;

	for (uint i = 0; i < Items.GetNum(); i++)
		if (Items[i].LangID == alterLandID)
			return Items[i].Text;

	if (Items.GetNum() > 0)
		return Items.GetFirst().Text;

	return "";
}



void zTHelpString::CreateString(const string& name, const string& text, const string& lang) {
	for (uint i = 0; i < HelpStrings.GetNum(); i++) {
		zTHelpString& help = HelpStrings[i];
		if (help.Name == name) {
			help.SetText(text, lang);
			return;
		}
	}

	zTHelpString& help = HelpStrings.Create();
	help.Name = name;
	help.SetText(text, lang);
}



string zTHelpString::GetString(const string& name) {
	for (uint i = 0; i < HelpStrings.GetNum(); i++)
		if (HelpStrings[i].Name == name)
			return HelpStrings[i].GetText();

	return "???";
}



static string CurrentControlsNamespace = "";
static uint   CurrentCimbinationLine = 0;
static uint   CurrentControlsLine = 0;

void XInputDevice::parseControlsId(Combination& combination, string row) {
	string id = row.GetWordSmart(2);
	combination.id = string::Combine("%s.%s", CurrentControlsNamespace, id);
	for (uint i = 0; i < keyCombinations.GetNum(); i++)
		if (combination.id == keyCombinations[i].id)
			Message::Fatal(string::Combine("Key '%s' already defined in '%s' file.", id, CurrentControlsNamespace));
}



void XInputDevice::parseControlsCombination(Combination& combination, string row) {
	for (uint i = 2; true; i++) {
		string token = row.GetWordSmart(i);
		if (token.IsEmpty())
			break;

		// Token-separator
		if (token == ",")
			continue;

		int code = GetCombinationKeyCode(token);
		if (code == None)
			Message::Fatal("Unknown control combination: " + token);

		combination.addCombination(code, 0);
	}
}



void XInputDevice::parseControlsEmulation(Combination& combination, string row) {
	for (uint i = 2; true; i++) {
		string token = row.GetWordSmart(i);
		if (token.IsEmpty())
			break;

		// Token-separator
		if (token == ",")
			continue;

		int code = GetEmulationKeyCode(token);
		if (code == None)
			Message::Fatal("Unknown control emulation: " + token);

		combination.addEmulation(code, 0);
	}
}



void XInputDevice::parseControlsEndRecord(Combination& combination) {
	if (combination.id.IsEmpty())
		cmd << Col16(CMD_RED | CMD_INT) << string::Combine("Found unnamed combination in '%s' file.\nLine:%i", CurrentControlsNamespace, CurrentCimbinationLine) << Col16() << endl;
	// Message::Fatal( string::Combine( "Found unnamed combination in '%s' file.\nLine:%i", CurrentControlsNamespace, CurrentCimbinationLine ) );

	keyCombinations.InsertSorted(combination);
}



void XInputDevice::parseControlsCondition(Combination& combination, string row) {
	bool isNot = false;
	for (uint i = 2; true; i++) {
		string token = row.GetWordSmart(i);
		if (token.IsEmpty())
			break;

		// Is 'NOT' function ??
		if (token == "!") {
			isNot = !isNot;
			continue;
		}

		// Token-separator
		if (token == ",")
			continue;

		// Condition by function
		LPCONDFUNC func = GetConditionFunction(token);
		if (func != Null) {
			if (isNot) {
				combination.addDenyFunctions(func, 0);
				isNot = false;
			}
			else
				combination.addAllowFunctions(func, 0);

			continue;
		}

		// Condition by keyboard key
		DXKEY dxKey = GetEmulationKeyCode(token);
		if (dxKey != None) {
			if (isNot) {
				combination.addDenyButtons(dxKey, 0);
				isNot = false;
			}
			else
				combination.addAllowButtons(dxKey, 0);

			continue;
		}

		// Condition by gamepad key
		JOYKEY joyKey = GetCombinationKeyCode(token);
		if (joyKey != None) {
			if (isNot) {
				combination.addDenyCombinations(joyKey, 0);
				isNot = false;
			}
			else
				combination.addAllowCombinations(joyKey, 0);

			continue;
		}

		Message::Fatal("Unknown control condition: " + token);
	}
}



void XInputDevice::parseControlsHelp(Combination& combination, string row) {
	string text = row.GetWordSmart(2);
	if (text == "\"")
		text = row.GetPattern("\"", "\"");
	else
		text = zTHelpString::GetString(text);

	combination.help = zTCombination_Help::Create(text, combination.combination);
}



void XInputDevice::parseControlsStringName(string& stringName, string row) {
	string name = row.GetWordSmart(2);
	if (name.IsEmpty())
		Message::Fatal("Empty string name in Gamepad file.");

	stringName = name;
}



void XInputDevice::parseControlsStringText(string& stringName, string row) {
	if (stringName.IsEmpty())
		Message::Fatal("Empty string name in Gamepad file.");

	string lang = row.GetWordSmart(1);
	string text = row.GetWordSmart(2);
	if (text == "\"")
		text = row.GetPattern("\"", "\"");
	else
		text = zTHelpString::GetString(text);

	zTHelpString::CreateString(stringName, text, lang);
}



void XInputDevice::parseControlsStyleName(const string& fileName, string row) {
	string text = row.GetWordSmart(2);
	if (text == "\"")
		text = row.GetPattern("\"", "\"");
	else
		text = zTHelpString::GetString(text);

	GamepadControlInfo::registerStyleInfo(fileName, text);
}



inline string FileNamePart(const string& fileFullName) {
	string shortName = fileFullName.GetWord("\\", -1);
	string namePart = shortName.GetWord(".");
	return namePart;
}



bool XInputDevice::parseControlFile(string const& fileName)
{
	auto initialized = false;
	auto combination = Combination();
	auto currentStringName = string();

	auto controlsFile = string();
	if (!controlsFile.ReadFromVdf(fileName, VDF_DEFAULT | VDF_PHYSICALFIRST)) {
		cmd << "Controls '" << fileName << "' not found" << endl;
		return false;
	}

	CurrentControlsNamespace = FileNamePart(fileName);
	auto controlsRows = rowString(controlsFile);

	for (uint i = 0; i < controlsRows.GetNum(); i++) {
		CurrentControlsLine = i;

		// Skip empty line
		string& row = controlsRows[i];
		if (row.Shrink().IsEmpty())
			continue;

		// Skip comment line or blocks
		string command = row.GetWordSmart(1, true);
		if (command == "//" || command.StartWith("#"))
			continue;

		// Check command
		if (command == "KeyRecord") {

			// End record and start new
			if (initialized) {
				parseControlsEndRecord(combination);
				combination.clear();
				CurrentCimbinationLine = i;
			}

			// One press - one click ??
			string mode = row.GetWordSmart(2, true);
			if (mode == "Toggled")
				combination.toggleMode = true;

			initialized = true;
			continue;
		}
		else if (command == "KeyDisable") {
			if (initialized) {
				parseControlsEndRecord(combination);
				combination.clear();
				// initialized = false;
			}

			string keyPart1 = row.GetWordSmart(2, true);
			string keyPart2 = row.GetWordSmart(4, true);
			string keyName;
			if (keyPart2.IsEmpty())
				keyName = string::Combine("%s.%s", CurrentControlsNamespace, keyPart1);
			else
				keyName = string::Combine("%s.%s", keyPart1, keyPart2);

			// cmd << "want to disable: " << keyName << endl;
			for (uint i = 0; i < keyCombinations.GetNum(); i++) {
				if (keyName == keyCombinations[i].id) {
					keyCombinations.RemoveAt(i);
					break;
				}
			}
			continue;
		}

		// Parse commands
		if (command == "Id")				parseControlsId(combination, row);
		else if (command == "Combination")	parseControlsCombination(combination, row);
		else if (command == "Emulation")	parseControlsEmulation(combination, row);
		else if (command == "Condition")	parseControlsCondition(combination, row);
		else if (command == "Help")			parseControlsHelp(combination, row);
		else if (command == "String")		parseControlsStringName(currentStringName, row);
		else if (command == "Rus")			parseControlsStringText(currentStringName, row);
		else if (command == "Eng")			parseControlsStringText(currentStringName, row);
		else if (command == "Ger")			parseControlsStringText(currentStringName, row);
		else if (command == "Deu")			parseControlsStringText(currentStringName, row);
		else if (command == "Pol")			parseControlsStringText(currentStringName, row);
		else if (command == "Rou")			parseControlsStringText(currentStringName, row);
		else if (command == "Ita")			parseControlsStringText(currentStringName, row);
		else if (command == "Cze")			parseControlsStringText(currentStringName, row);
		else if (command == "Esp")			parseControlsStringText(currentStringName, row);
		else if (command == "ControlName")	continue;
		else
			// unknown command !!!
			Message::Fatal("Unknown controls command: " + command);
	}

	// End last record
	if (initialized)
		parseControlsEndRecord(combination);

	return true;
}






bool XInputDevice::parseControlFileStrings(const string& fileName)
{
	auto currentStringName = string();
	auto controlsFile = string();
	if (!controlsFile.ReadFromVdf(fileName, VDF_DEFAULT | VDF_PHYSICALFIRST)) {
		cmd << "Controls not found" << endl;
		return false;
	}

	rowString controlsRows = controlsFile;

	for (uint i = 0; i < controlsRows.GetNum(); i++) {
		// Skip empty line
		string& row = controlsRows[i];
		if (row.Shrink().IsEmpty())
			continue;

		// Skip comment line or blocks
		string command = row.GetWordSmart(1, true);
		if (command == "//" || command.StartWith("#"))
			continue;

		if (command == "String")			parseControlsStringName(currentStringName, row);
		else if (command == "Rus")			parseControlsStringText(currentStringName, row);
		else if (command == "Eng")			parseControlsStringText(currentStringName, row);
		else if (command == "Ger")			parseControlsStringText(currentStringName, row);
		else if (command == "Deu")			parseControlsStringText(currentStringName, row);
		else if (command == "Pol")			parseControlsStringText(currentStringName, row);
		else if (command == "Rou")			parseControlsStringText(currentStringName, row);
		else if (command == "Ita")			parseControlsStringText(currentStringName, row);
		else if (command == "Cze")			parseControlsStringText(currentStringName, row);
		else if (command == "Esp")			parseControlsStringText(currentStringName, row);
		else if (command == "ControlName")	parseControlsStyleName(fileName, row);
		else continue;
	}

	return true;
}
}