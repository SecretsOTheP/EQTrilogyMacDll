/*	EQEMu: Everquest Server Emulator
	Copyright (C) 2001-2006 EQEMu Development Team (http://eqemulator.net)

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; version 2 of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY except by those people which sell it, which
	are required to give you total support for your newly bought product;
	without even the implied warranty of MERCHANTABILITY or FITNESS FOR
	A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/
#ifndef __EQEmuConfig_H
#define __EQEmuConfig_H

#include "xml_parser.h"
#include "linked_list.h"

struct LoginConfig 
{
	std::string LoginHost;
	std::string LoginAccount;
	std::string LoginPassword;
	uint16 LoginPort;
	uint8 LoginType;
};

class EQEmuConfig : public XMLParser 
{
	public:
		virtual std::string GetByName(const std::string &var_name) const;

		// From <world/>
		std::string ShortName;
		std::string LongName;
		std::string WorldAddress;
		std::string LocalAddress;
		std::string LoginHost;
		std::string LoginAccount;
		std::string LoginPassword;
		uint8 LoginType;
		uint16 LoginPort;
		uint32 LoginCount;
		LinkedList<LoginConfig*> loginlist;
		bool Locked;
		uint16 WorldTCPPort;
		std::string WorldIP;
		bool TelnetEnabled;
		int32 MaxClients;
		bool WorldHTTPEnabled;
		uint16 WorldHTTPPort;
		std::string WorldHTTPMimeFile;
		std::string SharedKey;
		// From <chatserver/>
		std::string ChatHost;
		uint16 ChatPort;
		// From <mailserver/>
		std::string MailHost;
		uint16 MailPort;
		// From <database/>
		std::string DatabaseHost;
		std::string DatabaseUsername;
		std::string DatabasePassword;
		std::string DatabaseDB;
		uint16 DatabasePort;
		// From <qsdatabase> // QueryServ
		std::string QSDatabaseHost;
		std::string QSDatabaseUsername;
		std::string QSDatabasePassword;
		std::string QSDatabaseDB;
		uint16 QSDatabasePort;
		// from <web_interface>
		uint16 WebInterfacePort;
		bool WebInterfaceUseSSL;
		std::string WebInterfaceCert;
		std::string WebInterfacePrivKey;
		// From <files/>
		std::string SpellsFile;
		std::string OpCodesFile;
		// From <directories/>
		std::string MapDir;
		std::string QuestDir;
		std::string PluginDir;
		// From <launcher/>
		std::string LogPrefix;
		std::string LogSuffix;
		std::string ZoneExe;
		uint32 RestartWait;
		uint32 TerminateWait;
		uint32 InitialBootWait;
		uint32 ZoneBootInterval;
		// From <zones/>
		uint16 ZonePortLow;
		uint16 ZonePortHigh;
		uint8 DefaultStatus;
		//	uint16 DynamicCount;
		//	map<string,uint16> StaticZones;
	protected:
		static EQEmuConfig *_config;
		static std::string ConfigFile;

#define ELEMENT(name) \
	void do_##name(TiXmlElement *ele);
#include "eqemu_config_elements.h"

		EQEmuConfig() 
		{
			// import the needed handler prototypes
#define ELEMENT(name) \
		Handlers[#name]=(ElementHandler)&EQEmuConfig::do_##name;
#include "eqemu_config_elements.h"
			// Set sane defaults
			// Login server
			LoginHost="eqemulator.net";
			LoginPort=5998;
			LoginType=0;
			// World
			Locked=false;
			WorldTCPPort=9000;
			TelnetEnabled=false;
			WorldHTTPEnabled=false;
			WorldHTTPPort=9080;
			WorldHTTPMimeFile="mime.types";
			SharedKey = "";	//blank disables authentication
			// Mail
			ChatHost="eqchat.eqemulator.net";
			ChatPort=7778;
			// Mail
			MailHost="eqmail.eqemulator.net";
			MailPort=7779;
			// Mysql
			DatabaseHost="localhost";
			DatabasePort=3306;
			DatabaseUsername="eq";
			DatabasePassword="eq";
			DatabaseDB="eq";
			// QueryServ Database
			QSDatabaseHost="localhost";
			QSDatabasePort=3306;
			QSDatabaseUsername="eq";
			QSDatabasePassword="eq";
			QSDatabaseDB="eq";
			// web_interface
			WebInterfacePort = 9081;
			WebInterfaceUseSSL = false;
			WebInterfaceCert = "";
			WebInterfacePrivKey = "";
			// Files
			SpellsFile = "spells_us.txt";
			OpCodesFile = "opcodes.conf";
			// Dirs
			MapDir="Maps";
			QuestDir="quests";
			PluginDir="quests/plugins";
			// Launcher
			LogPrefix = "logs/zone-";
			LogSuffix = ".log";
			RestartWait = 10000;		//milliseconds
			TerminateWait = 10000;		//milliseconds
			InitialBootWait = 20000;	//milliseconds
			ZoneBootInterval = 2000;	//milliseconds
#ifdef WIN32
		ZoneExe = "zone.exe";
#else
		ZoneExe = "./zone";
#endif
			// Zones
			ZonePortLow=7000;
			ZonePortHigh=7999;
			DefaultStatus=0;
			// For where zones need to connect to.
			WorldIP="127.0.0.1";
			// Dynamics to start
			//DynamicCount=5;
			MaxClients=-1;
			LoginCount=0;

		}
		virtual ~EQEmuConfig() {}
	public:

		// Produce a const singleton
		static const EQEmuConfig *get() 
		{
			if (_config == nullptr)
				LoadConfig();

			return(_config);
		}
		// Allow the use to set the conf file to be used.
		static void SetConfigFile(std::string file) { EQEmuConfig::ConfigFile = file; }
		// Load the config
		static bool LoadConfig() 
		{
			if (_config != nullptr)
				delete _config;

			_config=new EQEmuConfig;
			return _config->ParseFile(EQEmuConfig::ConfigFile.c_str(),"server");
		}

		void Dump() const;
};

#endif
