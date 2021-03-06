/**
  * Touhou Community Reliant Automatic Patcher
  * Team Shanghai Alice support plugin
  *
  * ----
  *
  * Plugin setup.
  */

#include <thcrap.h>
// (for TL note removal hooks)
#include <tlnote.hpp>
#include <commctrl.h>
#include "thcrap_tsa.h"
#include "layout.h"

tsa_game_t game_id;

// Game iterator
// -------------
tsa_game_t operator ++(tsa_game_t &game)
{
	if(game == TH_FUTURE) {
		return game;
	}
	return (game = static_cast<tsa_game_t>(static_cast<int>(game) + 1));
}

tsa_game_t operator *(tsa_game_t game)
{
	return game;
}

tsa_game_t begin(tsa_game_t game)
{
	return TH06;
}

tsa_game_t end(tsa_game_t game)
{
	return TH_FUTURE;
}
// -------------

// Translate strings to IDs.
static tsa_game_t game_id_from_string(const char *game)
{
	if(game == NULL) {
		return TH_NONE;
	} else if(!strcmp(game, "th06")) {
		return TH06;
	} else if(!strcmp(game, "th07")) {
		return TH07;
	} else if(!strcmp(game, "th08")) {
		return TH08;
	} else if(!strcmp(game, "th09")) {
		return TH09;
	} else if(!strcmp(game, "th095")) {
		return TH095;
	} else if(!strcmp(game, "th10")) {
		return TH10;
	} else if(!strcmp(game, "alcostg")) {
		return ALCOSTG;
	} else if(!strcmp(game, "th11")) {
		return TH11;
	} else if(!strcmp(game, "th12")) {
		return TH12;
	} else if(!strcmp(game, "th125")) {
		return TH125;
	} else if(!strcmp(game, "th128")) {
		return TH128;
	} else if(!strcmp(game, "th13")) {
		return TH13;
	} else if(!strcmp(game, "th14")) {
		return TH14;
	} else if(!strcmp(game, "th143")) {
		return TH143;
	} else if(!strcmp(game, "th15")) {
		return TH15;
	} else if(!strcmp(game, "th16")) {
		return TH16;
	} else if(!strcmp(game, "th165")) {
		return TH165;
	} else if(!strcmp(game, "th17")) {
		return TH17;
	}
	return TH_FUTURE;
}

int game_is_trial(void)
{
	static int trial = -2;
	if(trial == -2) {
		json_t *build = json_object_get(runconfig_get(), "build");
		const char *build_str = json_string_value(build);
		size_t build_len = json_string_length(build);

		if(!build_str || build_len < 2) {
			return trial = -1;
		}
		assert(
			(build_str[0] == 'v' && (build_str[1] == '0' || build_str[1] == '1'))
			|| !"invalid build format?"
		);

		trial = build_str[1] == '0';
	}
	return trial;
}

extern "C" __declspec(dllexport) const char* steam_appid(void)
{
	int trial = game_is_trial();
	switch(game_id) {
	case TH125:
		return "1100170";
	case TH128:
		return "1100180";
	case TH13:
		return "1043230";
	case TH14:
		return "1043240";
	case TH143:
		return "937570";
	case TH15:
		return "937580";
	case TH16:
		return trial ? "752490" : "745880";
	case TH165:
		return "924650";
	case TH17:
		return trial ? "1079170" : "1079160";
	default: // -Wswitch...
		return NULL;
	}
}

size_t tlnote_remove_size_hook(const char *fn, json_t *patch, size_t patch_size)
{
	// We want these gone as early as possible
	tlnote_remove();
	return 0;
}

int __stdcall thcrap_plugin_init()
{
	if(stack_remove_if_unneeded("base_tsa")) {
		return 1;
	}

	const char *game = json_object_get_string(runconfig_get(), "game");
	game_id = game_id_from_string(game);

	// th06_msg
	patchhook_register("msg*.dat", patch_msg_dlg, NULL); // th06-08
	patchhook_register("*.end", patch_end_th06, NULL); // th06 endings
	patchhook_register("p*.msg", patch_msg_dlg, NULL); // th09
	patchhook_register("s*.msg", patch_msg_dlg, NULL); // lowest common denominator for th10+
	patchhook_register("msg*.msg", patch_msg_dlg, NULL); // th143
	patchhook_register("e*.msg", patch_msg_end, NULL); // th10+ endings

	patchhook_register("*.anm", patch_anm, tlnote_remove_size_hook);
	// Remove TL notes when retrying a stage
	patchhook_register("*.std", nullptr, tlnote_remove_size_hook);
	return 0;
}

int InitDll(HMODULE hDll)
{
	// custom.exe bugfix
	INITCOMMONCONTROLSEX icce = {
		sizeof(icce), ICC_STANDARD_CLASSES
	};
	InitCommonControlsEx(&icce);
	return 0;
}

// Yes, this _has_ to be included in every project.
// Visual C++ won't use it when imported from a library
// and just defaults to msvcrt's one in this case.
BOOL APIENTRY DllMain(HMODULE hDll, DWORD ulReasonForCall, LPVOID lpReserved)
{
	switch(ulReasonForCall) {
		case DLL_PROCESS_ATTACH:
			InitDll(hDll);
			break;
	}
	return TRUE;
}
