namespace MQ2Globals
{

	
enum ePVPServer
{
	PVP_NONE = 0,
	PVP_TEAM = 1,
	PVP_RALLOS = 2,
	PVP_SULLON = 3,
};


EQLIB_VAR DWORD EQADDR_MEMCHECK0;
EQLIB_VAR DWORD EQADDR_MEMCHECK2;
EQLIB_VAR DWORD EQADDR_MEMCHECK3;
EQLIB_VAR DWORD EQADDR_MEMCHECK4;

EQLIB_VAR PBYTE EQADDR_ENCRYPTPAD0;
EQLIB_VAR PBYTE EQADDR_ENCRYPTPAD2;
EQLIB_VAR PBYTE EQADDR_ENCRYPTPAD3;
EQLIB_VAR PBYTE EQADDR_ENCRYPTPAD4;

typedef struct _MOUSEINFO {
    DWORD X;
    DWORD Y;
    DWORD SpeedX;
    DWORD SpeedY;
} MOUSEINFO, *PMOUSEINFO;

//EQLIB_VAR

EQLIB_VAR DWORD ManaGained;
EQLIB_VAR DWORD HealthGained;
EQLIB_VAR DWORD EnduranceGained;

EQLIB_VAR DWORD gGameState;


EQLIB_VAR DWORD EQADDR_CONVERTITEMTAGS;

EQLIB_VAR DWORD EQADDR_MEMCHECK0;
EQLIB_VAR DWORD EQADDR_MEMCHECK2;
EQLIB_VAR DWORD EQADDR_MEMCHECK3;
EQLIB_VAR DWORD EQADDR_MEMCHECK4;
EQLIB_VAR PCHAR EQADDR_SERVERHOST;
EQLIB_VAR PCHAR EQADDR_SERVERNAME;
EQLIB_VAR DWORD EQADDR_HWND;

EQLIB_VAR PSKILL *SkillDict;

EQLIB_VAR PEQFRIENDSLIST pFriendsList;
EQLIB_VAR PEQFRIENDSLIST pIgnoreList;

EQLIB_VAR PCMDLIST EQADDR_CMDLIST;

EQLIB_VAR PBYTE EQADDR_ATTACK;
EQLIB_VAR PBYTE EQADDR_NOTINCHATMODE;
EQLIB_VAR PBYTE EQADDR_RUNWALKSTATE;
EQLIB_VAR PCHAR EQADDR_LASTTELL;
EQLIB_VAR PVOID EQADDR_GWORLD;
EQLIB_VAR PDWORD EQADDR_DOABILITYLIST;
EQLIB_VAR PBYTE EQADDR_DOABILITYAVAILABLE;
EQLIB_VAR PBYTE pTributeActive;

EQLIB_VAR PBYTE EQADDR_ENCRYPTPAD0;
EQLIB_VAR PBYTE EQADDR_ENCRYPTPAD2;
EQLIB_VAR PBYTE EQADDR_ENCRYPTPAD3;
EQLIB_VAR PBYTE EQADDR_ENCRYPTPAD4;

EQLIB_VAR PMOUSEINFO EQADDR_MOUSE;
EQLIB_VAR PMOUSECLICK EQADDR_MOUSECLICK;

EQLIB_VAR PMAPLABEL *ppCurrentMapLabel;
#define pCurrentMapLabel (*ppCurrentMapLabel)

EQLIB_VAR PCHAR gpbRangedAttackReady;
#define gbRangedAttackReady (*gpbRangedAttackReady)
EQLIB_VAR PCHAR gpbAltTimerReady;
#define gbAltTimerReady (*gpbAltTimerReady)
EQLIB_VAR PCHAR gpbShowNetStatus;
#define gbShowNetStatus (*gpbShowNetStatus)
EQLIB_VAR DWORD *gpShowNames;
#define gShowNames (*gpShowNames)
EQLIB_VAR DWORD *gpPCNames;
#define gPCNames (*gpPCNames)


EQLIB_VAR DWORD *g_ppDrawHandler;
#define g_pDrawHandler (*g_ppDrawHandler)

EQLIB_VAR PCHAR *EQMappableCommandList;
EQLIB_VAR DWORD gnNormalEQMappableCommands;

#ifndef ISXEQ
EQLIB_VAR DWORD EQADDR_DIMAIN;
EQLIB_VAR IDirectInputDevice8A **EQADDR_DIKEYBOARD;
EQLIB_VAR IDirectInputDevice8A **EQADDR_DIMOUSE;
#endif

EQLIB_VAR DWORD EQADDR_EQLABELS;

EQLIB_VAR BOOL gMouseLeftClickInProgress;
EQLIB_VAR BOOL gMouseRightClickInProgress;
EQLIB_VAR BOOL bDetMouse;

// ***************************************************************************
// String arrays
// ***************************************************************************
EQLIB_VAR PCHAR szEQMappableCommands[nEQMappableCommands];

EQLIB_VAR PCHAR szHeading[];
EQLIB_VAR PCHAR szHeadingShort[];
EQLIB_VAR PCHAR szHeadingNormal[];
EQLIB_VAR PCHAR szHeadingNormalShort[];
EQLIB_VAR PCHAR szSize[];
EQLIB_VAR PCHAR szSpawnType[];
EQLIB_VAR PCHAR szGuildStatus[];
EQLIB_VAR PCHAR szGender[];
EQLIB_VAR PCHAR szDeityTeam[];
EQLIB_VAR PCHAR szLights[];
EQLIB_VAR BYTE LightBrightness[];
EQLIB_VAR PCHAR szSkills[];
EQLIB_VAR PCHAR szInnates[];
EQLIB_VAR PCHAR szCombineTypes[];
EQLIB_VAR PCHAR szItemTypes[];

EQLIB_VAR PCHAR szWornLoc[];

EQLIB_VAR PCHAR szItemName[];
EQLIB_VAR PCHAR szItemName4xx[];
//EQLIB_VAR PCHAR szTheme[]; 
EQLIB_VAR PCHAR szDmgBonusType[];
EQLIB_VAR PCHAR szBodyType[];
EQLIB_VAR PCHAR szAugRestrictions[];
EQLIB_VAR PCHAR szItemSlot[];

EQLIB_VAR StringTable **ppStringTable;
#define pStringTable (*ppStringTable)

EQLIB_VAR CDBStr **ppCDBStr;
#define pCDBStr (*ppCDBStr)

EQLIB_VAR CEverQuest **ppEverQuest;
#define pEverQuest (*ppEverQuest)
EQLIB_VAR CDisplay **ppDisplay;
#define pDisplay (*ppDisplay)
EQLIB_VAR EQ_PC **ppPCData;
#define pPCData (*ppPCData)
EQLIB_VAR EQ_Character **ppCharData;
#define pCharData (*ppCharData)
#define pCharData1 ((EQ_Character1 *)(((char *)(*ppCharData))+0xc280))
EQLIB_VAR EQPlayer **ppCharSpawn;
#define pCharSpawn (*ppCharSpawn)
EQLIB_VAR EQPlayer **ppActiveMerchant;
#define pActiveMerchant (*ppActiveMerchant)
EQLIB_VAR EQPlayer **ppSpawnList;
#define pSpawnList (*ppSpawnList)

EQLIB_VAR EQPlayer **ppLocalPlayer;
#define pLocalPlayer (*ppLocalPlayer)
EQLIB_VAR EQPlayer **ppControlledPlayer;
#define pControlledPlayer (*ppControlledPlayer)



EQLIB_VAR EQWorldData **ppWorldData;
#define pWorldData (*ppWorldData)
EQLIB_VAR SpellManager **ppSpellMgr;
#define pSpellMgr (*ppSpellMgr)
EQLIB_VAR CInvSlot **ppSelectedItem;
#define pSelectedItem (*ppSelectedItem)
EQLIB_VAR EQGROUP *pGroup;
EQLIB_VAR EQPlayer **ppTarget;
#define pTarget (*ppTarget)
EQLIB_VAR EqSwitchManager **ppSwitchMgr;
#define pSwitchMgr (*ppSwitchMgr)
EQLIB_VAR EQItemList **ppItemList;
#define pItemList (*ppItemList)
EQLIB_VAR EQZoneInfo *pZoneInfo;
EQLIB_VAR PGUILDS pGuildList;
EQLIB_VAR PEQSOCIAL   pSocialList;

EQLIB_VAR PBYTE pgHotkeyPage;
#define gHotkeyPage (*pgHotkeyPage)

EQLIB_VAR EQPlayer **ppTradeTarget;
#define pTradeTarget (*ppTradeTarget)
EQLIB_VAR EQPlayer **ppActiveBanker;
#define pActiveBanker (*ppActiveBanker)
EQLIB_VAR EQPlayer **ppActiveGMaster;
#define pActiveGMaster (*ppActiveGMaster)
EQLIB_VAR EQPlayer **ppActiveCorpse;
#define pActiveCorpse (*ppActiveCorpse)

EQLIB_VAR CSidlManager **ppSidlMgr;
#define pSidlMgr (*ppSidlMgr)

EQLIB_VAR CXWndManager **ppWndMgr;
#define pWndMgr (*ppWndMgr)

EQLIB_VAR KeypressHandler **ppKeypressHandler;
#define pKeypressHandler (*ppKeypressHandler)

EQLIB_VAR PEQRAID pRaid;

EQLIB_VAR PINT pgCurrentSocial;
typedef VOID    (__cdecl *fEQSendMessage)(PVOID,DWORD,PVOID,DWORD,BOOL);
EQLIB_VAR fEQSendMessage     send_message;
#define gCurrentSocial (*pgCurrentSocial)

EQLIB_VAR DWORD *pScreenX;
#define ScreenX (*pScreenX)
EQLIB_VAR DWORD *pScreenY;
#define ScreenY (*pScreenY)
EQLIB_VAR DWORD *pScreenMode;
#define ScreenMode (*pScreenMode)
EQLIB_VAR CHAR  *pMouseLook;
#define bMouseLook (*pMouseLook)

EQLIB_VAR SPELLFAVORITE *pSpellSets;
EQLIB_VAR AltAdvManager** ppAltAdvManager;
#define pAltAdvManager (*ppAltAdvManager)

/* WINDOW INSTANCES */ 
EQLIB_VAR CContextMenuManager **ppContextMenuManager;
EQLIB_VAR CCursorAttachment **ppCursorAttachment;
EQLIB_VAR CSocialEditWnd **ppSocialEditWnd;
EQLIB_VAR CContainerMgr **ppContainerMgr;
EQLIB_VAR CChatManager **ppChatManager;
EQLIB_VAR CConfirmationDialog **ppConfirmationDialog;
EQLIB_VAR CFacePick **ppFacePick;
EQLIB_VAR CInvSlotMgr **ppInvSlotMgr;
//EQLIB_VAR CPopupWndManager **ppPopupWndManager;
EQLIB_VAR CNoteWnd **ppNoteWnd;
EQLIB_VAR CHelpWnd **ppHelpWnd;
EQLIB_VAR CTipWnd **ppTipWnd;
EQLIB_VAR CTipWnd **ppTipWnd;
EQLIB_VAR CBookWnd **ppBookWnd;
EQLIB_VAR CFriendsWnd **ppFriendsWnd;
EQLIB_VAR CMusicPlayerWnd **ppMusicPlayerWnd;
EQLIB_VAR CAlarmWnd **ppAlarmWnd;
EQLIB_VAR CLoadskinWnd **ppLoadskinWnd;
EQLIB_VAR CPetInfoWnd **ppPetInfoWnd;
EQLIB_VAR CTrainWnd **ppTrainWnd;
EQLIB_VAR CSkillsWnd **ppSkillsWnd;
EQLIB_VAR CSkillsSelectWnd **ppSkillsSelectWnd;
//EQLIB_VAR CCombatSkillSelectWnd **ppCombatSkillSelectWnd;
EQLIB_VAR CAAWnd **ppAAWnd;
EQLIB_VAR CGroupWnd **ppGroupWnd;
//EQLIB_VAR CSystemInfoDialogBox **ppSystemInfoDialogBox;
EQLIB_VAR CGroupSearchWnd **ppGroupSearchWnd;
EQLIB_VAR CGroupSearchFiltersWnd **ppGroupSearchFiltersWnd;
EQLIB_VAR CRaidWnd **ppRaidWnd;
EQLIB_VAR CRaidOptionsWnd **ppRaidOptionsWnd;
EQLIB_VAR CBreathWnd **ppBreathWnd;
EQLIB_VAR CMapToolbarWnd **ppMapToolbarWnd;
EQLIB_VAR CMapViewWnd **ppMapViewWnd;
EQLIB_VAR CEditLabelWnd **ppEditLabelWnd;
EQLIB_VAR COptionsWnd **ppOptionsWnd;
EQLIB_VAR CBuffWindow **ppBuffWindow;
EQLIB_VAR CBuffWindow **ppBuffWindow;
EQLIB_VAR CTargetWnd **ppTargetWnd;
EQLIB_VAR CColorPickerWnd **ppColorPickerWnd;
EQLIB_VAR CHotButtonWnd **ppHotButtonWnd;
EQLIB_VAR CPlayerWnd **ppPlayerWnd;
EQLIB_VAR CCastingWnd **ppCastingWnd;
EQLIB_VAR CCastSpellWnd **ppCastSpellWnd;
EQLIB_VAR CSpellBookWnd **ppSpellBookWnd;
EQLIB_VAR CInventoryWnd **ppInventoryWnd;
EQLIB_VAR CBankWnd **ppBankWnd;
EQLIB_VAR CQuantityWnd **ppQuantityWnd;
EQLIB_VAR CTextEntryWnd **ppTextEntryWnd;
EQLIB_VAR CFileSelectionWnd **ppFileSelectionWnd;
EQLIB_VAR CLootWnd **ppLootWnd;
EQLIB_VAR CPetInfoWnd **ppPetInfoWnd;
EQLIB_VAR CActionsWnd **ppActionsWnd;
//EQLIB_VAR CCombatAbilityWnd **ppCombatAbilityWnd;
EQLIB_VAR CMerchantWnd **ppMerchantWnd;
EQLIB_VAR CTradeWnd **ppTradeWnd;
EQLIB_VAR CBazaarWnd **ppBazaarWnd;
EQLIB_VAR CBazaarSearchWnd **ppBazaarSearchWnd;
EQLIB_VAR CGiveWnd **ppGiveWnd;
EQLIB_VAR CSelectorWnd **ppSelectorWnd;
EQLIB_VAR CTrackingWnd **ppTrackingWnd;
EQLIB_VAR CInspectWnd **ppInspectWnd;
EQLIB_VAR CFeedbackWnd **ppFeedbackWnd;
EQLIB_VAR CBugReportWnd **ppBugReportWnd;
EQLIB_VAR CVideoModesWnd **ppVideoModesWnd;
EQLIB_VAR CCompassWnd **ppCompassWnd;
EQLIB_VAR CPlayerNotesWnd **ppPlayerNotesWnd;
EQLIB_VAR CGemsGameWnd **ppGemsGameWnd;
EQLIB_VAR CStoryWnd **ppStoryWnd;
//EQLIB_VAR CFindLocationWnd **ppFindLocationWnd;
//EQLIB_VAR CAdventureRequestWnd **ppAdventureRequestWnd;
//EQLIB_VAR CAdventureMerchantWnd **ppAdventureMerchantWnd;
//EQLIB_VAR CAdventureStatsWnd **ppAdventureStatsWnd;
//EQLIB_VAR CAdventureLeaderboardWnd **ppAdventureLeaderboardWnd;
//EQLIB_VAR CLeadershipWindow **ppLeadershipWindow;
EQLIB_VAR CBodyTintWnd **ppBodyTintWnd;
EQLIB_VAR CGuildMgmtWnd **ppGuildMgmtWnd;
EQLIB_VAR CJournalTextWnd **ppJournalTextWnd;
EQLIB_VAR CJournalCatWnd **ppJournalCatWnd;
//EQLIB_VAR CTributeBenefitWnd **ppTributeBenefitWnd;
//EQLIB_VAR CTributeMasterWnd **ppTributeMasterWnd;
EQLIB_VAR CPetitionQWnd **ppPetitionQWnd;
EQLIB_VAR CSoulmarkWnd **ppSoulmarkWnd;
EQLIB_VAR CTimeLeftWnd **ppTimeLeftWnd;
EQLIB_VAR CTextOverlay **ppTextOverlay;


#define pContextMenuManager (*ppContextMenuManager)
#define pCursorAttachment (*ppCursorAttachment)
#define pSocialEditWnd (*ppSocialEditWnd)
#define pContainerMgr (*ppContainerMgr)
#define pChatManager (*ppChatManager)
#define pConfirmationDialog (*ppConfirmationDialog)
#define pFacePick (*ppFacePick)
#define pInvSlotMgr (*ppInvSlotMgr)
#define pPopupWndManager (*ppPopupWndManager)
#define pNoteWnd (*ppNoteWnd)
#define pHelpWnd (*ppHelpWnd)
#define pTipWnd (*ppTipWnd)
#define pTipWnd (*ppTipWnd)
#define pBookWnd (*ppBookWnd)
#define pFriendsWnd (*ppFriendsWnd)
#define pMusicPlayerWnd (*ppMusicPlayerWnd)
#define pAlarmWnd (*ppAlarmWnd)
#define pLoadskinWnd (*ppLoadskinWnd)
#define pPetInfoWnd (*ppPetInfoWnd)
#define pTrainWnd (*ppTrainWnd)
#define pSkillsWnd (*ppSkillsWnd)
#define pSkillsSelectWnd (*ppSkillsSelectWnd)
#define pCombatSkillSelectWnd (*ppCombatSkillSelectWnd)
#define pAAWnd (*ppAAWnd)
#define pGroupWnd (*ppGroupWnd)
#define pSystemInfoDialogBox (*ppSystemInfoDialogBox)
#define pGroupSearchWnd (*ppGroupSearchWnd)
#define pGroupSearchFiltersWnd (*ppGroupSearchFiltersWnd)
#define pRaidWnd (*ppRaidWnd)
#define pRaidOptionsWnd (*ppRaidOptionsWnd)
#define pBreathWnd (*ppBreathWnd)
#define pMapToolbarWnd (*ppMapToolbarWnd)
#define pMapViewWnd (*ppMapViewWnd)
#define pEditLabelWnd (*ppEditLabelWnd)
#define pOptionsWnd (*ppOptionsWnd)
#define pBuffWindow (*ppBuffWindow)
#define pBuffWindow (*ppBuffWindow)
#define pTargetWnd (*ppTargetWnd)
#define pColorPickerWnd (*ppColorPickerWnd)
#define pHotButtonWnd (*ppHotButtonWnd)
#define pPlayerWnd (*ppPlayerWnd)
#define pCastingWnd (*ppCastingWnd)
#define pCastSpellWnd (*ppCastSpellWnd)
#define pSpellBookWnd (*ppSpellBookWnd)
#define pInventoryWnd (*ppInventoryWnd)
#define pBankWnd (*ppBankWnd)
#define pQuantityWnd (*ppQuantityWnd)
#define pTextEntryWnd (*ppTextEntryWnd)
#define pFileSelectionWnd (*ppFileSelectionWnd)
#define pLootWnd (*ppLootWnd)
#define pPetInfoWnd (*ppPetInfoWnd)
#define pActionsWnd (*ppActionsWnd)
#define pCombatAbilityWnd (*ppCombatAbilityWnd)
#define pMerchantWnd (*ppMerchantWnd)
#define pTradeWnd (*ppTradeWnd)
#define pBazaarWnd (*ppBazaarWnd)
#define pBazaarSearchWnd (*ppBazaarSearchWnd)
#define pGiveWnd (*ppGiveWnd)
#define pSelectorWnd (*ppSelectorWnd)
#define pTrackingWnd (*ppTrackingWnd)
#define pInspectWnd (*ppInspectWnd)
#define pFeedbackWnd (*ppFeedbackWnd)
#define pBugReportWnd (*ppBugReportWnd)
#define pVideoModesWnd (*ppVideoModesWnd)
#define pCompassWnd (*ppCompassWnd)
#define pPlayerNotesWnd (*ppPlayerNotesWnd)
#define pGemsGameWnd (*ppGemsGameWnd)
#define pStoryWnd (*ppStoryWnd)
#define pFindLocationWnd (*ppFindLocationWnd)
#define pAdventureRequestWnd (*ppAdventureRequestWnd)
#define pAdventureMerchantWnd (*ppAdventureMerchantWnd)
#define pAdventureStatsWnd (*ppAdventureStatsWnd)
#define pAdventureLeaderboardWnd (*ppAdventureLeaderboardWnd)
#define pLeadershipWindow (*ppLeadershipWindow)
#define pBodyTintWnd (*ppBodyTintWnd)
#define pGuildMgmtWnd (*ppGuildMgmtWnd)
#define pJournalTextWnd (*ppJournalTextWnd)
#define pJournalCatWnd (*ppJournalCatWnd)
#define pTributeBenefitWnd (*ppTributeBenefitWnd)
#define pTributeMasterWnd (*ppTributeMasterWnd)
#define pPetitionQWnd (*ppPetitionQWnd)
#define pSoulmarkWnd (*ppSoulmarkWnd)
#define pTimeLeftWnd (*ppTimeLeftWnd)
#define pTextOverlay (*ppTextOverlay)

/*
#define pContextMenuManager (*ppContextMenuManager)
#define pCursorAttachment (*ppCursorAttachment)
#define pSocialEditWnd (*ppSocialEditWnd)
#define pInvSlotMgr (*ppInvSlotMgr)
#define pContainerMgr (*ppContainerMgr)
#define pChatManager (*ppChatManager)
#define pConfirmationDialog (*ppConfirmationDialog)
#define pFacePick (*ppFacePick)
#define pItemDisplayMgr (*ppItemDisplayMgr)
#define pSpellDisplayMgr (*ppSpellDisplayMgr)
#define pNoteWnd (*ppNoteWnd)
#define pHelpWnd (*ppHelpWnd)
#define pTipWnd (*ppTipWnd)
#define pTipWnd (*ppTipWnd)
#define pBookWnd (*ppBookWnd)
#define pFriendsWnd (*ppFriendsWnd)
#define pMusicPlayerWnd (*ppMusicPlayerWnd)
#define pAlarmWnd (*ppAlarmWnd)
#define pLoadskinWnd (*ppLoadskinWnd)
#define pPetInfoWnd (*ppPetInfoWnd)
#define pTrainWnd (*ppTrainWnd)
#define pSkillsWnd (*ppSkillsWnd)
#define pSkillsSelectWnd (*ppSkillsSelectWnd)
#define pAAWnd (*ppAAWnd)
#define pGroupWnd (*ppGroupWnd)
#define pJournalNPCWnd (*ppJournalNPCWnd)
#define pGroupSearchWnd (*ppGroupSearchWnd)
#define pGroupSearchFiltersWnd (*ppGroupSearchFiltersWnd)
#define pRaidWnd (*ppRaidWnd)
#define pRaidOptionsWnd (*ppRaidOptionsWnd)
#define pBreathWnd (*ppBreathWnd)
#define pMapToolbarWnd (*ppMapToolbarWnd)
#define pMapViewWnd (*ppMapViewWnd)
#define pEditLabelWnd (*ppEditLabelWnd)
#define pOptionsWnd (*ppOptionsWnd)
#define pBuffWindow (*ppBuffWindow)
#define pBuffWindow (*ppBuffWindow)
#define pTargetWnd (*ppTargetWnd)
#define pColorPickerWnd (*ppColorPickerWnd)
#define pHotButtonWnd (*ppHotButtonWnd)
#define pPlayerWnd (*ppPlayerWnd)
#define pCastingWnd (*ppCastingWnd)
#define pCastSpellWnd (*ppCastSpellWnd)
#define pSpellBookWnd (*ppSpellBookWnd)
#define pInventoryWnd (*ppInventoryWnd)
#define pBankWnd (*ppBankWnd)
#define pQuantityWnd (*ppQuantityWnd)
#define pTextEntryWnd (*ppTextEntryWnd)
#define pFileSelectionWnd (*ppFileSelectionWnd)
#define pLootWnd (*ppLootWnd)
#define pActionsWnd (*ppActionsWnd)
#define pMerchantWnd (*ppMerchantWnd)
#define pTradeWnd (*ppTradeWnd)
#define pBazaarWnd (*ppBazaarWnd)
#define pBazaarSearchWnd (*ppBazaarSearchWnd)
#define pGiveWnd (*ppGiveWnd)
#define pSelectorWnd (*ppSelectorWnd)
#define pTrackingWnd (*ppTrackingWnd)
#define pInspectWnd (*ppInspectWnd)
#define pFeedbackWnd (*ppFeedbackWnd)
#define pBugReportWnd (*ppBugReportWnd)
#define pVideoModesWnd (*ppVideoModesWnd)
#define pCompassWnd (*ppCompassWnd)
#define pPlayerNotesWnd (*ppPlayerNotesWnd)
#define pGemsGameWnd (*ppGemsGameWnd)
#define pStoryWnd (*ppStoryWnd)
#define pFindLocationWnd (*ppFindLocationWnd)
#define pAdventureRequestWnd (*ppAdventureRequestWnd)
#define pAdventureStatsWnd (*ppAdventureStatsWnd)
#define pAdventureLeaderboardWnd (*ppAdventureLeaderboardWnd)
#define pBodyTintWnd (*ppBodyTintWnd)
#define pGuildMgmtWnd (*ppGuildMgmtWnd)
#define pJournalTextWnd (*ppJournalTextWnd)
#define pJournalCatWnd (*ppJournalCatWnd)
#define pPetitionQWnd (*ppPetitionQWnd)
#define pSoulmarkWnd (*ppSoulmarkWnd)
#define pTimeLeftWnd (*ppTimeLeftWnd)
/**/
}
using namespace MQ2Globals;
