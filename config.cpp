class CfgPatches
{
	class KOTH_SAVIOR
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Scripts"
		};
	};
};
class CfgMods
{
	class KOTH_SAVIOR_Mod
	{
		dir="KOTH-SAVIOR";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="KOTH-SAVIOR";
		credits="terry1877";
		author="terry1877";
		authorID="0";
		version="1.2";
		extra=0;
		type="mod";
		dependencies[]=
		{
			"World",
			"Game",
			"Mission"
		};
		class defs
		{
			class gameScriptModule
			{
				value="";
				files[]=
				{
					"KOTH-SAVIOR/scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"KOTH-SAVIOR/scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"KOTH-SAVIOR/scripts/5_Mission"
				};
			};
		};
	};
};
class CfgSoundShaders
{
	class KOTH_Alarm_SoundShader
	{
		samples[]=
		{
			
			{
				"KOTH-SAVIOR\sounds\alarm",
				1
			}
		};
		volume=1;
		range=500;
	};
	class KOTH_VehicleWin_SoundShader
	{
		samples[]=
		{
			
			{
				"KOTH-SAVIOR\sounds\vehicle_win",
				1
			}
		};
		volume=1;
		range=100;
	};
};
class CfgSoundSets
{
	class KOTH_Alarm_SoundSet
	{
		soundShaders[]=
		{
			"KOTH_Alarm_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		loop=0;
	};
	class KOTH_VehicleWin_SoundSet
	{
		soundShaders[]=
		{
			"KOTH_VehicleWin_SoundShader"
		};
		volumeFactor=1;
		frequencyFactor=1;
		spatial=1;
		loop=0;
	};
};
