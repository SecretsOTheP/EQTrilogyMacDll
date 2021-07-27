/*****************************************************************************
    MQ2Main.dll: MacroQuest2's extension DLL for EverQuest
    Copyright (C) 2002-2003 Plazmic, 2003-2005 Lax

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License, version 2, as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
******************************************************************************/

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x510
#define DIRECTINPUT_VERSION 0x800

#define DBG_SPEW

#include "MQ2Main.h"
namespace MQ2Globals
{

DWORD EQADDR_MEMCHECK0=__MemChecker0;
DWORD EQADDR_MEMCHECK2=__MemChecker2;
DWORD EQADDR_MEMCHECK3=__MemChecker3;
DWORD EQADDR_MEMCHECK4=__MemChecker4;
PBYTE EQADDR_ENCRYPTPAD0=(PBYTE)__EncryptPad0;
PBYTE EQADDR_ENCRYPTPAD2=(PBYTE)__EncryptPad2;
PBYTE EQADDR_ENCRYPTPAD3=(PBYTE)__EncryptPad3;
PBYTE EQADDR_ENCRYPTPAD4=(PBYTE)__EncryptPad4;
fEQSendMessage    send_message = (fEQSendMessage)__SendMessage;
StringTable **ppStringTable=(StringTable**)pinstStringTable;
CDBStr **ppCDBStr = (CDBStr**)pinstCDBStr;

EQPlayer **ppEQP_IDArray=(EQPlayer**)__EQP_IDArray;

CEverQuest **ppEverQuest=(CEverQuest**)pinstCEverQuest;
CDisplay **ppDisplay=(CDisplay**)pinstCDisplay;
EQ_PC **ppPCData=(EQ_PC**)pinstPCData;
EQ_Character **ppCharData=(EQ_Character**)pinstCharData;
EQPlayer **ppCharSpawn=(EQPlayer**)pinstCharSpawn;
EQPlayer **ppActiveMerchant=(EQPlayer**)pinstActiveMerchant;
EQPlayer **ppSpawnList=(EQPlayer**)pinstSpawnList;
EQPlayer **ppLocalPlayer=(EQPlayer**)pinstLocalPlayer;
EQPlayer **ppControlledPlayer=(EQPlayer**)pinstControlledPlayer;
EQWorldData **ppWorldData=(EQWorldData**)pinstWorldData;
SpellManager **ppSpellMgr=(SpellManager**)pinstSpellManager;
CInvSlot **ppSelectedItem=(CInvSlot **)pinstSelectedItem;
EQGROUP *pGroup=(EQGROUP*)instGroup;
EQPlayer **ppTarget=(EQPlayer**)pinstTarget;
EqSwitchManager **ppSwitchMgr=(EqSwitchManager**)pinstSwitchManager;
EQItemList **ppItemList=(EQItemList**)pinstEQItemList;
SPELLFAVORITE *pSpellSets=(SPELLFAVORITE *)pinstSpellSets;
EQZoneInfo *pZoneInfo=(EQZoneInfo*)instEQZoneInfo;
AltAdvManager** ppAltAdvManager=(AltAdvManager**)pinstAltAdvManager;

EQPlayer **ppTradeTarget=(EQPlayer **)pinstTradeTarget;
EQPlayer **ppActiveBanker=(EQPlayer **)pinstActiveBanker;
EQPlayer **ppActiveGMaster=(EQPlayer **)pinstActiveGMaster;
EQPlayer **ppActiveCorpse=(EQPlayer **)pinstActiveCorpse;

DWORD gGameState = 0;
/* WINDOW INSTANCES */

CContextMenuManager **ppContextMenuManager=(CContextMenuManager**)pinstCContextMenuManager;
CCursorAttachment **ppCursorAttachment=(CCursorAttachment**)pinstCCursorAttachment;
CSocialEditWnd **ppSocialEditWnd=(CSocialEditWnd**)pinstCSocialEditWnd;
CContainerMgr **ppContainerMgr=(CContainerMgr**)pinstCContainerMgr;
CChatManager **ppChatManager=(CChatManager**)pinstCChatManager;
CFacePick **ppFacePick=(CFacePick**)pinstCFacePick;
CInvSlotMgr **ppInvSlotMgr=(CInvSlotMgr**)pinstCInvSlotMgr;
//CPopupWndManager **ppPopupWndManager=(CPopupWndManager**)pinstCPopupWndManager;
CNoteWnd **ppNoteWnd=(CNoteWnd**)pinstCNoteWnd;
CTipWnd **ppTipWndOFDAY=(CTipWnd**)pinstCTipWndOFDAY;
CTipWnd **ppTipWndCONTEXT=(CTipWnd**)pinstCTipWndCONTEXT;
CBookWnd **ppBookWnd=(CBookWnd**)pinstCBookWnd;
CFriendsWnd **ppFriendsWnd=(CFriendsWnd**)pinstCFriendsWnd;
CMusicPlayerWnd **ppMusicPlayerWnd=(CMusicPlayerWnd**)pinstCMusicPlayerWnd;
CAlarmWnd **ppAlarmWnd=(CAlarmWnd**)pinstCAlarmWnd;
CLoadskinWnd **ppLoadskinWnd=(CLoadskinWnd**)pinstCLoadskinWnd;
CPetInfoWnd **ppPetInfoWnd=(CPetInfoWnd**)pinstCPetInfoWnd;
CTrainWnd **ppTrainWnd=(CTrainWnd**)pinstCTrainWnd;
CSkillsWnd **ppSkillsWnd=(CSkillsWnd**)pinstCSkillsWnd;
CSkillsSelectWnd **ppSkillsSelectWnd=(CSkillsSelectWnd**)pinstCSkillsSelectWnd;
//CCombatSkillSelectWnd **ppCombatSkillSelectWnd=(CCombatSkillSelectWnd**)pinstCCombatSkillSelectWnd;
CAAWnd **ppAAWnd=(CAAWnd**)pinstCAAWnd;
CGroupWnd **ppGroupWnd=(CGroupWnd**)pinstCGroupWnd;
//CSystemInfoDialogBox **ppSystemInfoDialogBox=(CSystemInfoDialogBox**)pinstCSystemInfoDialogBox;
CGroupSearchWnd **ppGroupSearchWnd=(CGroupSearchWnd**)pinstCGroupSearchWnd;
CGroupSearchFiltersWnd **ppGroupSearchFiltersWnd=(CGroupSearchFiltersWnd**)pinstCGroupSearchFiltersWnd;
CRaidWnd **ppRaidWnd=(CRaidWnd**)pinstCRaidWnd;
CRaidOptionsWnd **ppRaidOptionsWnd=(CRaidOptionsWnd**)pinstCRaidOptionsWnd;
CBreathWnd **ppBreathWnd=(CBreathWnd**)pinstCBreathWnd;
CMapToolbarWnd **ppMapToolbarWnd=(CMapToolbarWnd**)pinstCMapToolbarWnd;
CMapViewWnd **ppMapViewWnd=(CMapViewWnd**)pinstCMapViewWnd;
CEditLabelWnd **ppEditLabelWnd=(CEditLabelWnd**)pinstCEditLabelWnd;
COptionsWnd **ppOptionsWnd=(COptionsWnd**)pinstCOptionsWnd;
CBuffWindow **ppBuffWindowSHORT=(CBuffWindow**)pinstCBuffWindowSHORT;
CBuffWindow **ppBuffWindowNORMAL=(CBuffWindow**)pinstCBuffWindowNORMAL;
CTargetWnd **ppTargetWnd=(CTargetWnd**)pinstCTargetWnd;
CColorPickerWnd **ppColorPickerWnd=(CColorPickerWnd**)pinstCColorPickerWnd;
CHotButtonWnd **ppHotButtonWnd=(CHotButtonWnd**)pinstCHotButtonWnd;
CPlayerWnd **ppPlayerWnd=(CPlayerWnd**)pinstCPlayerWnd;
CCastingWnd **ppCastingWnd=(CCastingWnd**)pinstCCastingWnd;
CCastSpellWnd **ppCastSpellWnd=(CCastSpellWnd**)pinstCCastSpellWnd;
CSpellBookWnd **ppSpellBookWnd=(CSpellBookWnd**)pinstCSpellBookWnd;
CInventoryWnd **ppInventoryWnd=(CInventoryWnd**)pinstCInventoryWnd;
CBankWnd **ppBankWnd=(CBankWnd**)pinstCBankWnd;
CQuantityWnd **ppQuantityWnd=(CQuantityWnd**)pinstCQuantityWnd;
CTextEntryWnd **ppTextEntryWnd=(CTextEntryWnd**)pinstCTextEntryWnd;
CFileSelectionWnd **ppFileSelectionWnd=(CFileSelectionWnd**)pinstCFileSelectionWnd;
CLootWnd **ppLootWnd=(CLootWnd**)pinstCLootWnd;
CActionsWnd **ppActionsWnd=(CActionsWnd**)pinstCActionsWnd;
//CCombatAbilityWnd **ppCombatAbilityWnd=(CCombatAbilityWnd**)pinstCCombatAbilityWnd;
CMerchantWnd **ppMerchantWnd=(CMerchantWnd**)pinstCMerchantWnd;
CTradeWnd **ppTradeWnd=(CTradeWnd**)pinstCTradeWnd;
CBazaarWnd **ppBazaarWnd=(CBazaarWnd**)pinstCBazaarWnd;
CBazaarSearchWnd **ppBazaarSearchWnd=(CBazaarSearchWnd**)pinstCBazaarSearchWnd;
CGiveWnd **ppGiveWnd=(CGiveWnd**)pinstCGiveWnd;
CSelectorWnd **ppSelectorWnd=(CSelectorWnd**)pinstCSelectorWnd;
CTrackingWnd **ppTrackingWnd=(CTrackingWnd**)pinstCTrackingWnd;
CInspectWnd **ppInspectWnd=(CInspectWnd**)pinstCInspectWnd;
CFeedbackWnd **ppFeedbackWnd=(CFeedbackWnd**)pinstCFeedbackWnd;
CBugReportWnd **ppBugReportWnd=(CBugReportWnd**)pinstCBugReportWnd;
CVideoModesWnd **ppVideoModesWnd=(CVideoModesWnd**)pinstCVideoModesWnd;
CCompassWnd **ppCompassWnd=(CCompassWnd**)pinstCCompassWnd;
CPlayerNotesWnd **ppPlayerNotesWnd=(CPlayerNotesWnd**)pinstCPlayerNotesWnd;
CGemsGameWnd **ppGemsGameWnd=(CGemsGameWnd**)pinstCGemsGameWnd;
CStoryWnd **ppStoryWnd=(CStoryWnd**)pinstCStoryWnd;
//CFindLocationWnd **ppFindLocationWnd=(CFindLocationWnd**)pinstCFindLocationWnd;
//CAdventureRequestWnd **ppAdventureRequestWnd=(CAdventureRequestWnd**)pinstCAdventureRequestWnd;
//CAdventureMerchantWnd **ppAdventureMerchantWnd=(CAdventureMerchantWnd**)pinstCAdventureMerchantWnd;
//CAdventureStatsWnd **ppAdventureStatsWnd=(CAdventureStatsWnd**)pinstCAdventureStatsWnd;
//CAdventureLeaderboardWnd **ppAdventureLeaderboardWnd=(CAdventureLeaderboardWnd**)pinstCAdventureLeaderboardWnd;
//CLeadershipWindow **ppLeadershipWindow=(CLeadershipWindow**)pinstCLeadershipWindow;
CBodyTintWnd **ppBodyTintWnd=(CBodyTintWnd**)pinstCBodyTintWnd;
CGuildMgmtWnd **ppGuildMgmtWnd=(CGuildMgmtWnd**)pinstCGuildMgmtWnd;
CJournalTextWnd **ppJournalTextWnd=(CJournalTextWnd**)pinstCJournalTextWnd;
CJournalCatWnd **ppJournalCatWnd=(CJournalCatWnd**)pinstCJournalCatWnd;
//CTributeBenefitWnd **ppTributeBenefitWnd=(CTributeBenefitWnd**)pinstCTributeBenefitWnd;
//CTributeMasterWnd **ppTributeMasterWnd=(CTributeMasterWnd**)pinstCTributeMasterWnd;
CPetitionQWnd **ppPetitionQWnd=(CPetitionQWnd**)pinstCPetitionQWnd;
CSoulmarkWnd **ppSoulmarkWnd=(CSoulmarkWnd**)pinstCSoulmarkWnd;
CTimeLeftWnd **ppTimeLeftWnd=(CTimeLeftWnd**)pinstCTimeLeftWnd;

CTextOverlay **ppTextOverlay=(CTextOverlay**)pinstCTextOverlay;

CSidlManager **ppSidlMgr=(CSidlManager **)pinstCSidlManager;
CXWndManager **ppWndMgr=(CXWndManager**)pinstCXWndManager;
KeypressHandler **ppKeypressHandler=(KeypressHandler**)instKeypressHandler;
PEQRAID pRaid=(PEQRAID)instCRaid;
DWORD ManaGained=0;
DWORD HealthGained=0;
DWORD EnduranceGained=0;
};
