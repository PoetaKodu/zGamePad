// This file added in headers queue
// File: "Sources.h"

namespace GOTHIC_ENGINE {
  void Game_Entry() {
    DS4Device.init();
  }




  void Game_Init() {
    // ApplyGamepadOptions();
    xinputDevice.updateControls();
  }

  void Game_Exit() {
  }

  inline zSTRING GetInfoName() {
    switch( oCInformationManager::GetInformationManager().Mode ) {
      case oCInformationManager::INFO_MGR_MODE_IMPORTANT: return "INFO_MGR_MODE_IMPORTANT";
      case oCInformationManager::INFO_MGR_MODE_INFO:      return "INFO_MGR_MODE_INFO";
      case oCInformationManager::INFO_MGR_MODE_CHOICE:    return "INFO_MGR_MODE_CHOICE";
      case oCInformationManager::INFO_MGR_MODE_TRADE:     return "INFO_MGR_MODE_TRADE";
    }
   
    return "...";
  }

  void Game_PreLoop() {
  }

  static int s_bUseOldControls_Real = 0;

  void Game_Loop() {
    if( keybuffer.GetNum() ) {
      bool isGpInput = xinputDevice.hasGamepadInput();
      XInputDevice::vibrationIsEnabled = isGpInput;
#if ENGINE >= Engine_G2
      if( !s_bUseOldControls_Real )
        oCGame::s_bUseOldControls = isGpInput;
#endif
    }

    FocusNpcLoop();
    OverlaysLoop();
  }

  void Game_PostLoop() {
  }

  void Game_MenuLoop() {
  }

  // Information about current saving or loading world
  TSaveLoadGameInfo& SaveLoadGameInfo = UnionCore::SaveLoadGameInfo;

  void Game_SaveBegin() {
  }

  void Game_SaveEnd() {
    zLastSaveInfo::GetInstance().UpdateSaveSlotNr( SaveLoadGameInfo.slotID );

    zSTRING savePath = ogame->CreateSavePath( "GAMEPAD_QUICKBAR.SAV" );
    zCArchiver* archiver = zarcFactory->CreateArchiverWrite( savePath, zARC_MODE_ASCII, 0, 0 );
    if( !archiver )
      return;

    zCGamepadQuickBar_Items::GetInstance()->Archive( *archiver );
    archiver->Close();
    archiver->Release();

    if( oCNpc::s_bTargetLocked ) {
      player->SetEnemy( player->GetFocusNpc() );
    }
  }

  void LoadBegin() {
    static bool getMousePos_Commited = false;
    if( !getMousePos_Commited ) {
      Hook_GetMousePos.Commit();
      getMousePos_Commited = true;
    }
  }


  void LoadEnd() {
    zSTRING savePath = ogame->CreateSavePath( "GAMEPAD_QUICKBAR.SAV" );
    zCArchiver* archiver = zarcFactory->CreateArchiverRead( savePath, 0 );
    if( !archiver ) {
      zCGamepadQuickBar_Items::GetInstance()->ResetAll();
      return;
    }

    zCGamepadQuickBar_Items::GetInstance()->Unarchive( *archiver );
    archiver->Close();
    archiver->Release();
  }

  void Game_LoadBegin_NewGame() {
    LoadBegin();
  }

  void Game_LoadEnd_NewGame() {
    LoadEnd();
  }

  void Game_LoadBegin_SaveGame() {
    LoadBegin();
  }

  void Game_LoadEnd_SaveGame() {
    LoadEnd();
  }

  void Game_LoadBegin_ChangeLevel() {
    LoadBegin();
  }

  void Game_LoadEnd_ChangeLevel() {
    LoadEnd();
  }

  void Game_LoadBegin_Trigger() {
  }
  
  void Game_LoadEnd_Trigger() {
  }
  
  void Game_Pause() {
  }
  
  void Game_Unpause() {
  }
  
  void Game_DefineExternals() {
  }

  void Game_ApplyOptions() {
    ApplyGamepadOptions();
#if ENGINE >= Engine_G2
    s_bUseOldControls_Real = oCGame::s_bUseOldControls;
#endif
  }

  /*
  Functions call order on Game initialization:
    - Game_Entry           * Gothic entry point
    - Game_DefineExternals * Define external script functions
    - Game_Init            * After DAT files init
  
  Functions call order on Change level:
    - Game_LoadBegin_Trigger     * Entry in trigger
    - Game_LoadEnd_Trigger       *
    - Game_Loop                  * Frame call window
    - Game_LoadBegin_ChangeLevel * Load begin
    - Game_SaveBegin             * Save previous level information
    - Game_SaveEnd               *
    - Game_LoadEnd_ChangeLevel   *
  
  Functions call order on Save game:
    - Game_Pause     * Open menu
    - Game_Unpause   * Click on save
    - Game_Loop      * Frame call window
    - Game_SaveBegin * Save begin
    - Game_SaveEnd   *
  
  Functions call order on Load game:
    - Game_Pause              * Open menu
    - Game_Unpause            * Click on load
    - Game_LoadBegin_SaveGame * Load begin
    - Game_LoadEnd_SaveGame   *
  */

#define AppDefault True
  CApplication* lpApplication = !CHECK_THIS_ENGINE ? Null : CApplication::CreateRefApplication(
    Enabled( AppDefault ) Game_Entry,
    Enabled( AppDefault ) Game_Init,
    Enabled( AppDefault ) Game_Exit,
    Enabled( AppDefault ) Game_PreLoop,
    Enabled( AppDefault ) Game_Loop,
    Enabled( AppDefault ) Game_PostLoop,
    Enabled( AppDefault ) Game_MenuLoop,
    Enabled( AppDefault ) Game_SaveBegin,
    Enabled( AppDefault ) Game_SaveEnd,
    Enabled( AppDefault ) Game_LoadBegin_NewGame,
    Enabled( AppDefault ) Game_LoadEnd_NewGame,
    Enabled( AppDefault ) Game_LoadBegin_SaveGame,
    Enabled( AppDefault ) Game_LoadEnd_SaveGame,
    Enabled( AppDefault ) Game_LoadBegin_ChangeLevel,
    Enabled( AppDefault ) Game_LoadEnd_ChangeLevel,
    Enabled( AppDefault ) Game_LoadBegin_Trigger,
    Enabled( AppDefault ) Game_LoadEnd_Trigger,
    Enabled( AppDefault ) Game_Pause,
    Enabled( AppDefault ) Game_Unpause,
    Enabled( AppDefault ) Game_DefineExternals,
    Enabled( AppDefault ) Game_ApplyOptions
  );
}