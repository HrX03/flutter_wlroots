#include "flutter_embedder.h"

#define WLR_USE_UNSTABLE
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_xcursor_manager.h>
#include <wlr/types/wlr_pointer.h>

#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_input_device.h>
#include <wlr/types/wlr_keyboard.h>

#include <wlr/backend.h>
#include <wlr/util/log.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_xdg_shell.h>
#include <xkbcommon/xkbcommon.h>
#include <stdlib.h>


#include "input.h"
#include "shaders.h"
#include "instance.h"
#include "standard_message_codec.h"
#include "constants.h"
#include "handle_map.h"
#include "messages.h"
#include "platform_channel.h"

#define NS_PER_MS 1000000

/* std::map<uint64_t, uint64_t> xkb_to_physical_key_map = {
    {0x00000009, 0x00070029},  // escape
    {0x0000000a, 0x0007001e},  // digit1
    {0x0000000b, 0x0007001f},  // digit2
    {0x0000000c, 0x00070020},  // digit3
    {0x0000000d, 0x00070021},  // digit4
    {0x0000000e, 0x00070022},  // digit5
    {0x0000000f, 0x00070023},  // digit6
    {0x00000010, 0x00070024},  // digit7
    {0x00000011, 0x00070025},  // digit8
    {0x00000012, 0x00070026},  // digit9
    {0x00000013, 0x00070027},  // digit0
    {0x00000014, 0x0007002d},  // minus
    {0x00000015, 0x0007002e},  // equal
    {0x00000016, 0x0007002a},  // backspace
    {0x00000017, 0x0007002b},  // tab
    {0x00000018, 0x00070014},  // keyQ
    {0x00000019, 0x0007001a},  // keyW
    {0x0000001a, 0x00070008},  // keyE
    {0x0000001b, 0x00070015},  // keyR
    {0x0000001c, 0x00070017},  // keyT
    {0x0000001d, 0x0007001c},  // keyY
    {0x0000001e, 0x00070018},  // keyU
    {0x0000001f, 0x0007000c},  // keyI
    {0x00000020, 0x00070012},  // keyO
    {0x00000021, 0x00070013},  // keyP
    {0x00000022, 0x0007002f},  // bracketLeft
    {0x00000023, 0x00070030},  // bracketRight
    {0x00000024, 0x00070028},  // enter
    {0x00000025, 0x000700e0},  // controlLeft
    {0x00000026, 0x00070004},  // keyA
    {0x00000027, 0x00070016},  // keyS
    {0x00000028, 0x00070007},  // keyD
    {0x00000029, 0x00070009},  // keyF
    {0x0000002a, 0x0007000a},  // keyG
    {0x0000002b, 0x0007000b},  // keyH
    {0x0000002c, 0x0007000d},  // keyJ
    {0x0000002d, 0x0007000e},  // keyK
    {0x0000002e, 0x0007000f},  // keyL
    {0x0000002f, 0x00070033},  // semicolon
    {0x00000030, 0x00070034},  // quote
    {0x00000031, 0x00070035},  // backquote
    {0x00000032, 0x000700e1},  // shiftLeft
    {0x00000033, 0x00070031},  // backslash
    {0x00000034, 0x0007001d},  // keyZ
    {0x00000035, 0x0007001b},  // keyX
    {0x00000036, 0x00070006},  // keyC
    {0x00000037, 0x00070019},  // keyV
    {0x00000038, 0x00070005},  // keyB
    {0x00000039, 0x00070011},  // keyN
    {0x0000003a, 0x00070010},  // keyM
    {0x0000003b, 0x00070036},  // comma
    {0x0000003c, 0x00070037},  // period
    {0x0000003d, 0x00070038},  // slash
    {0x0000003e, 0x000700e5},  // shiftRight
    {0x0000003f, 0x00070055},  // numpadMultiply
    {0x00000040, 0x000700e2},  // altLeft
    {0x00000041, 0x0007002c},  // space
    {0x00000042, 0x00070039},  // capsLock
    {0x00000043, 0x0007003a},  // f1
    {0x00000044, 0x0007003b},  // f2
    {0x00000045, 0x0007003c},  // f3
    {0x00000046, 0x0007003d},  // f4
    {0x00000047, 0x0007003e},  // f5
    {0x00000048, 0x0007003f},  // f6
    {0x00000049, 0x00070040},  // f7
    {0x0000004a, 0x00070041},  // f8
    {0x0000004b, 0x00070042},  // f9
    {0x0000004c, 0x00070043},  // f10
    {0x0000004d, 0x00070053},  // numLock
    {0x0000004e, 0x00070047},  // scrollLock
    {0x0000004f, 0x0007005f},  // numpad7
    {0x00000050, 0x00070060},  // numpad8
    {0x00000051, 0x00070061},  // numpad9
    {0x00000052, 0x00070056},  // numpadSubtract
    {0x00000053, 0x0007005c},  // numpad4
    {0x00000054, 0x0007005d},  // numpad5
    {0x00000055, 0x0007005e},  // numpad6
    {0x00000056, 0x00070057},  // numpadAdd
    {0x00000057, 0x00070059},  // numpad1
    {0x00000058, 0x0007005a},  // numpad2
    {0x00000059, 0x0007005b},  // numpad3
    {0x0000005a, 0x00070062},  // numpad0
    {0x0000005b, 0x00070063},  // numpadDecimal
    {0x0000005d, 0x00070094},  // lang5
    {0x0000005e, 0x00070064},  // intlBackslash
    {0x0000005f, 0x00070044},  // f11
    {0x00000060, 0x00070045},  // f12
    {0x00000061, 0x00070087},  // intlRo
    {0x00000062, 0x00070092},  // lang3
    {0x00000063, 0x00070093},  // lang4
    {0x00000064, 0x0007008a},  // convert
    {0x00000065, 0x00070088},  // kanaMode
    {0x00000066, 0x0007008b},  // nonConvert
    {0x00000068, 0x00070058},  // numpadEnter
    {0x00000069, 0x000700e4},  // controlRight
    {0x0000006a, 0x00070054},  // numpadDivide
    {0x0000006b, 0x00070046},  // printScreen
    {0x0000006c, 0x000700e6},  // altRight
    {0x0000006e, 0x0007004a},  // home
    {0x0000006f, 0x00070052},  // arrowUp
    {0x00000070, 0x0007004b},  // pageUp
    {0x00000071, 0x00070050},  // arrowLeft
    {0x00000072, 0x0007004f},  // arrowRight
    {0x00000073, 0x0007004d},  // end
    {0x00000074, 0x00070051},  // arrowDown
    {0x00000075, 0x0007004e},  // pageDown
    {0x00000076, 0x00070049},  // insert
    {0x00000077, 0x0007004c},  // delete
    {0x00000079, 0x0007007f},  // audioVolumeMute
    {0x0000007a, 0x00070081},  // audioVolumeDown
    {0x0000007b, 0x00070080},  // audioVolumeUp
    {0x0000007c, 0x00070066},  // power
    {0x0000007d, 0x00070067},  // numpadEqual
    {0x0000007e, 0x000700d7},  // numpadSignChange
    {0x0000007f, 0x00070048},  // pause
    {0x00000080, 0x000c029f},  // showAllWindows
    {0x00000081, 0x00070085},  // numpadComma
    {0x00000082, 0x00070090},  // lang1
    {0x00000083, 0x00070091},  // lang2
    {0x00000084, 0x00070089},  // intlYen
    {0x00000085, 0x000700e3},  // metaLeft
    {0x00000086, 0x000700e7},  // metaRight
    {0x00000087, 0x00070065},  // contextMenu
    {0x00000088, 0x000c0226},  // browserStop
    {0x00000089, 0x00070079},  // again
    {0x0000008b, 0x0007007a},  // undo
    {0x0000008c, 0x00070077},  // select
    {0x0000008d, 0x0007007c},  // copy
    {0x0000008e, 0x00070074},  // open
    {0x0000008f, 0x0007007d},  // paste
    {0x00000090, 0x0007007e},  // find
    {0x00000091, 0x0007007b},  // cut
    {0x00000092, 0x00070075},  // help
    {0x00000094, 0x000c0192},  // launchApp2
    {0x00000096, 0x00010082},  // sleep
    {0x00000097, 0x00010083},  // wakeUp
    {0x00000098, 0x000c0194},  // launchApp1
    {0x0000009e, 0x000c0196},  // launchInternetBrowser
    {0x000000a0, 0x000c019e},  // lockScreen
    {0x000000a3, 0x000c018a},  // launchMail
    {0x000000a4, 0x000c022a},  // browserFavorites
    {0x000000a6, 0x000c0224},  // browserBack
    {0x000000a7, 0x000c0225},  // browserForward
    {0x000000a9, 0x000c00b8},  // eject
    {0x000000ab, 0x000c00b5},  // mediaTrackNext
    {0x000000ac, 0x000c00cd},  // mediaPlayPause
    {0x000000ad, 0x000c00b6},  // mediaTrackPrevious
    {0x000000ae, 0x000c00b7},  // mediaStop
    {0x000000af, 0x000c00b2},  // mediaRecord
    {0x000000b0, 0x000c00b4},  // mediaRewind
    {0x000000b1, 0x000c008c},  // launchPhone
    {0x000000b3, 0x000c0183},  // mediaSelect
    {0x000000b4, 0x000c0223},  // browserHome
    {0x000000b5, 0x000c0227},  // browserRefresh
    {0x000000b6, 0x000c0094},  // exit
    {0x000000bb, 0x000700b6},  // numpadParenLeft
    {0x000000bc, 0x000700b7},  // numpadParenRight
    {0x000000bd, 0x000c0201},  // newKey
    {0x000000be, 0x000c0279},  // redo
    {0x000000bf, 0x00070068},  // f13
    {0x000000c0, 0x00070069},  // f14
    {0x000000c1, 0x0007006a},  // f15
    {0x000000c2, 0x0007006b},  // f16
    {0x000000c3, 0x0007006c},  // f17
    {0x000000c4, 0x0007006d},  // f18
    {0x000000c5, 0x0007006e},  // f19
    {0x000000c6, 0x0007006f},  // f20
    {0x000000c7, 0x00070070},  // f21
    {0x000000c8, 0x00070071},  // f22
    {0x000000c9, 0x00070072},  // f23
    {0x000000ca, 0x00070073},  // f24
    {0x000000d1, 0x000c00b1},  // mediaPause
    {0x000000d6, 0x000c0203},  // close
    {0x000000d7, 0x000c00b0},  // mediaPlay
    {0x000000d8, 0x000c00b3},  // mediaFastForward
    {0x000000d9, 0x000c00e5},  // bassBoost
    {0x000000da, 0x000c0208},  // print
    {0x000000e1, 0x000c0221},  // browserSearch
    {0x000000e8, 0x000c0070},  // brightnessDown
    {0x000000e9, 0x000c006f},  // brightnessUp
    {0x000000eb, 0x000100b5},  // displayToggleIntExt
    {0x000000ed, 0x000c007a},  // kbdIllumDown
    {0x000000ee, 0x000c0079},  // kbdIllumUp
    {0x000000ef, 0x000c028c},  // mailSend
    {0x000000f0, 0x000c0289},  // mailReply
    {0x000000f1, 0x000c028b},  // mailForward
    {0x000000f2, 0x000c0207},  // save
    {0x000000f3, 0x000c01a7},  // launchDocuments
    {0x000000fc, 0x000c0075},  // brightnessAuto
    {0x0000016e, 0x000c0060},  // info
    {0x00000172, 0x000c008d},  // programGuide
    {0x0000017a, 0x000c0061},  // closedCaptionToggle
    {0x0000017c, 0x000c0232},  // zoomToggle
    {0x0000017e, 0x000c01ae},  // launchKeyboardLayout
    {0x00000190, 0x000c01b7},  // launchAudioBrowser
    {0x00000195, 0x000c018e},  // launchCalendar
    {0x0000019d, 0x000c0083},  // mediaLast
    {0x000001a2, 0x000c009c},  // channelUp
    {0x000001a3, 0x000c009d},  // channelDown
    {0x000001aa, 0x000c022d},  // zoomIn
    {0x000001ab, 0x000c022e},  // zoomOut
    {0x000001ad, 0x000c0184},  // launchWordProcessor
    {0x000001af, 0x000c0186},  // launchSpreadsheet
    {0x000001b5, 0x000c018d},  // launchContacts
    {0x000001b7, 0x000c0072},  // brightnessToggle
    {0x000001b8, 0x000c01ab},  // spellCheck
    {0x000001b9, 0x000c019c},  // logOff
    {0x0000024b, 0x000c019f},  // launchControlPanel
    {0x0000024c, 0x000c01a2},  // selectTask
    {0x0000024d, 0x000c01b1},  // launchScreenSaver
    {0x0000024e, 0x000c00cf},  // speechInputToggle
    {0x0000024f, 0x000c01cb},  // launchAssistant
    {0x00000250, 0x000c029d},  // keyboardLayoutSelect
    {0x00000258, 0x000c0073},  // brightnessMinimum
    {0x00000259, 0x000c0074},  // brightnessMaximum
    {0x00000281, 0x00000017},  // privacyScreenToggle
};

std::map<uint64_t, uint64_t> gtk_keyval_to_logical_key_map = {
    {0x000000a5, 0x00200000022},  // yen
    {0x0000fd06, 0x00100000405},  // 3270_EraseEOF
    {0x0000fd0e, 0x00100000503},  // 3270_Attn
    {0x0000fd15, 0x00100000402},  // 3270_Copy
    {0x0000fd16, 0x00100000d2f},  // 3270_Play
    {0x0000fd1b, 0x00100000406},  // 3270_ExSelect
    {0x0000fd1d, 0x00100000608},  // 3270_PrintScreen
    {0x0000fd1e, 0x0010000000d},  // 3270_Enter
    {0x0000fe03, 0x00200000105},  // ISO_Level3_Shift
    {0x0000fe08, 0x00100000709},  // ISO_Next_Group
    {0x0000fe0a, 0x0010000070a},  // ISO_Prev_Group
    {0x0000fe0c, 0x00100000707},  // ISO_First_Group
    {0x0000fe0e, 0x00100000708},  // ISO_Last_Group
    {0x0000fe20, 0x00100000009},  // ISO_Left_Tab
    {0x0000fe34, 0x0010000000d},  // ISO_Enter
    {0x0000ff08, 0x00100000008},  // BackSpace
    {0x0000ff09, 0x00100000009},  // Tab
    {0x0000ff0b, 0x00100000401},  // Clear
    {0x0000ff0d, 0x0010000000d},  // Return
    {0x0000ff13, 0x00100000509},  // Pause
    {0x0000ff14, 0x0010000010c},  // Scroll_Lock
    {0x0000ff1b, 0x0010000001b},  // Escape
    {0x0000ff21, 0x00100000719},  // Kanji
    {0x0000ff24, 0x0010000071b},  // Romaji
    {0x0000ff25, 0x00100000716},  // Hiragana
    {0x0000ff26, 0x0010000071a},  // Katakana
    {0x0000ff27, 0x00100000717},  // Hiragana_Katakana
    {0x0000ff28, 0x0010000071c},  // Zenkaku
    {0x0000ff29, 0x00100000715},  // Hankaku
    {0x0000ff2a, 0x0010000071d},  // Zenkaku_Hankaku
    {0x0000ff2f, 0x00100000714},  // Eisu_Shift
    {0x0000ff31, 0x00100000711},  // Hangul
    {0x0000ff34, 0x00100000712},  // Hangul_Hanja
    {0x0000ff37, 0x00100000703},  // Codeinput
    {0x0000ff3c, 0x00100000710},  // SingleCandidate
    {0x0000ff3e, 0x0010000070e},  // PreviousCandidate
    {0x0000ff50, 0x00100000306},  // Home
    {0x0000ff51, 0x00100000302},  // Left
    {0x0000ff52, 0x00100000304},  // Up
    {0x0000ff53, 0x00100000303},  // Right
    {0x0000ff54, 0x00100000301},  // Down
    {0x0000ff55, 0x00100000308},  // Page_Up
    {0x0000ff56, 0x00100000307},  // Page_Down
    {0x0000ff57, 0x00100000305},  // End
    {0x0000ff60, 0x0010000050c},  // Select
    {0x0000ff61, 0x00100000a0c},  // Print
    {0x0000ff62, 0x00100000506},  // Execute
    {0x0000ff63, 0x00100000407},  // Insert
    {0x0000ff65, 0x0010000040a},  // Undo
    {0x0000ff66, 0x00100000409},  // Redo
    {0x0000ff67, 0x00100000505},  // Menu
    {0x0000ff68, 0x00100000507},  // Find
    {0x0000ff69, 0x00100000504},  // Cancel
    {0x0000ff6a, 0x00100000508},  // Help
    {0x0000ff7e, 0x0010000070b},  // Mode_switch
    {0x0000ff7f, 0x0010000010a},  // Num_Lock
    {0x0000ff80, 0x00000000020},  // KP_Space
    {0x0000ff89, 0x00100000009},  // KP_Tab
    {0x0000ff8d, 0x0020000020d},  // KP_Enter
    {0x0000ff91, 0x00100000801},  // KP_F1
    {0x0000ff92, 0x00100000802},  // KP_F2
    {0x0000ff93, 0x00100000803},  // KP_F3
    {0x0000ff94, 0x00100000804},  // KP_F4
    {0x0000ff95, 0x00200000237},  // KP_Home
    {0x0000ff96, 0x00200000234},  // KP_Left
    {0x0000ff97, 0x00200000238},  // KP_Up
    {0x0000ff98, 0x00200000236},  // KP_Right
    {0x0000ff99, 0x00200000232},  // KP_Down
    {0x0000ff9a, 0x00200000239},  // KP_Page_Up
    {0x0000ff9b, 0x00200000233},  // KP_Page_Down
    {0x0000ff9c, 0x00200000231},  // KP_End
    {0x0000ff9e, 0x00200000230},  // KP_Insert
    {0x0000ff9f, 0x0020000022e},  // KP_Delete
    {0x0000ffaa, 0x0020000022a},  // KP_Multiply
    {0x0000ffab, 0x0020000022b},  // KP_Add
    {0x0000ffad, 0x0020000022d},  // KP_Subtract
    {0x0000ffae, 0x0000000002e},  // KP_Decimal
    {0x0000ffaf, 0x0020000022f},  // KP_Divide
    {0x0000ffb0, 0x00200000230},  // KP_0
    {0x0000ffb1, 0x00200000231},  // KP_1
    {0x0000ffb2, 0x00200000232},  // KP_2
    {0x0000ffb3, 0x00200000233},  // KP_3
    {0x0000ffb4, 0x00200000234},  // KP_4
    {0x0000ffb5, 0x00200000235},  // KP_5
    {0x0000ffb6, 0x00200000236},  // KP_6
    {0x0000ffb7, 0x00200000237},  // KP_7
    {0x0000ffb8, 0x00200000238},  // KP_8
    {0x0000ffb9, 0x00200000239},  // KP_9
    {0x0000ffbd, 0x0020000023d},  // KP_Equal
    {0x0000ffbe, 0x00100000801},  // F1
    {0x0000ffbf, 0x00100000802},  // F2
    {0x0000ffc0, 0x00100000803},  // F3
    {0x0000ffc1, 0x00100000804},  // F4
    {0x0000ffc2, 0x00100000805},  // F5
    {0x0000ffc3, 0x00100000806},  // F6
    {0x0000ffc4, 0x00100000807},  // F7
    {0x0000ffc5, 0x00100000808},  // F8
    {0x0000ffc6, 0x00100000809},  // F9
    {0x0000ffc7, 0x0010000080a},  // F10
    {0x0000ffc8, 0x0010000080b},  // F11
    {0x0000ffc9, 0x0010000080c},  // F12
    {0x0000ffca, 0x0010000080d},  // F13
    {0x0000ffcb, 0x0010000080e},  // F14
    {0x0000ffcc, 0x0010000080f},  // F15
    {0x0000ffcd, 0x00100000810},  // F16
    {0x0000ffce, 0x00100000811},  // F17
    {0x0000ffcf, 0x00100000812},  // F18
    {0x0000ffd0, 0x00100000813},  // F19
    {0x0000ffd1, 0x00100000814},  // F20
    {0x0000ffd2, 0x00100000815},  // F21
    {0x0000ffd3, 0x00100000816},  // F22
    {0x0000ffd4, 0x00100000817},  // F23
    {0x0000ffd5, 0x00100000818},  // F24
    {0x0000ffe1, 0x00200000102},  // Shift_L
    {0x0000ffe2, 0x00200000103},  // Shift_R
    {0x0000ffe3, 0x00200000100},  // Control_L
    {0x0000ffe4, 0x00200000101},  // Control_R
    {0x0000ffe5, 0x00100000104},  // Caps_Lock
    {0x0000ffe7, 0x00200000106},  // Meta_L
    {0x0000ffe8, 0x00200000107},  // Meta_R
    {0x0000ffe9, 0x00200000104},  // Alt_L
    {0x0000ffea, 0x00200000105},  // Alt_R
    {0x0000ffeb, 0x0010000010e},  // Super_L
    {0x0000ffec, 0x0010000010e},  // Super_R
    {0x0000ffed, 0x00100000108},  // Hyper_L
    {0x0000ffee, 0x00100000108},  // Hyper_R
    {0x0000ffff, 0x0010000007f},  // Delete
    {0x1008ff02, 0x00100000602},  // MonBrightnessUp
    {0x1008ff03, 0x00100000601},  // MonBrightnessDown
    {0x1008ff10, 0x0010000060a},  // Standby
    {0x1008ff11, 0x00100000a0f},  // AudioLowerVolume
    {0x1008ff12, 0x00100000a11},  // AudioMute
    {0x1008ff13, 0x00100000a10},  // AudioRaiseVolume
    {0x1008ff14, 0x00100000d2f},  // AudioPlay
    {0x1008ff15, 0x00100000a07},  // AudioStop
    {0x1008ff16, 0x00100000a09},  // AudioPrev
    {0x1008ff17, 0x00100000a08},  // AudioNext
    {0x1008ff18, 0x00100000c04},  // HomePage
    {0x1008ff19, 0x00100000b03},  // Mail
    {0x1008ff1b, 0x00100000c06},  // Search
    {0x1008ff1c, 0x00100000d30},  // AudioRecord
    {0x1008ff20, 0x00100000b02},  // Calendar
    {0x1008ff26, 0x00100000c01},  // Back
    {0x1008ff27, 0x00100000c03},  // Forward
    {0x1008ff28, 0x00100000c07},  // Stop
    {0x1008ff29, 0x00100000c05},  // Refresh
    {0x1008ff2a, 0x00100000607},  // PowerOff
    {0x1008ff2b, 0x0010000060b},  // WakeUp
    {0x1008ff2c, 0x00100000604},  // Eject
    {0x1008ff2d, 0x00100000b07},  // ScreenSaver
    {0x1008ff2f, 0x00200000002},  // Sleep
    {0x1008ff30, 0x00100000c02},  // Favorites
    {0x1008ff31, 0x00100000d2e},  // AudioPause
    {0x1008ff3e, 0x00100000d31},  // AudioRewind
    {0x1008ff56, 0x00100000a01},  // Close
    {0x1008ff57, 0x00100000402},  // Copy
    {0x1008ff58, 0x00100000404},  // Cut
    {0x1008ff61, 0x00100000605},  // LogOff
    {0x1008ff68, 0x00100000a0a},  // New
    {0x1008ff6b, 0x00100000a0b},  // Open
    {0x1008ff6d, 0x00100000408},  // Paste
    {0x1008ff6e, 0x00100000b0d},  // Phone
    {0x1008ff72, 0x00100000a03},  // Reply
    {0x1008ff77, 0x00100000a0d},  // Save
    {0x1008ff7b, 0x00100000a04},  // Send
    {0x1008ff7c, 0x00100000a0e},  // Spell
    {0x1008ff8b, 0x0010000050d},  // ZoomIn
    {0x1008ff8c, 0x0010000050e},  // ZoomOut
    {0x1008ff90, 0x00100000a02},  // MailForward
    {0x1008ff97, 0x00100000d2c},  // AudioForward
    {0x1008ffa7, 0x00200000000},  // Suspend
}; */

static uint32_t uapi_mouse_button_to_flutter(uint32_t uapi_button) {
  switch (uapi_button) {
    case 0x110: return 1; // BTN_LEFT
    case 0x111: return 2; // BTN_RIGHT
    case 0x112: return 3; // BTN_MIDDLE
    case 0x116: return 4; // BTN_BACK
    case 0x115: return 5; // BTN_FORWARD
    case 0x113: return 6; // BTN_SIDE
    case 0x114: return 7; // BTN_EXTRA
    case 0x100: return 8; // BTN_0
    case 0x101: return 9; // BTN_1
    case 0x102: return 10; // BTN_2
    case 0x103: return 11; // BTN_3
    case 0x104: return 12; // BTN_4
    case 0x105: return 13; // BTN_5
    case 0x106: return 14; // BTN_6
    case 0x107: return 15; // BTN_7
    case 0x108: return 16; // BTN_8
    case 0x109: return 17; // BTN_9
    default: return 0;
  }
}

static void process_cursor_motion(struct fwr_instance *instance, uint32_t time) {
  wlr_xcursor_manager_set_cursor_image(instance->cursor_mgr, "left_ptr",
                                       instance->cursor);
  //wlr_log(WLR_INFO, "%ld %d", instance->fl_proc_table.GetCurrentTime(), time);

  FlutterPointerEvent pointer_event = {};
  pointer_event.struct_size = sizeof(FlutterPointerEvent);
  if (instance->input.mouse_down) {
    pointer_event.phase = kMove;
  } else {
    pointer_event.phase = kHover;
  }
  pointer_event.x = instance->cursor->x;
  pointer_event.y = instance->cursor->y;
  pointer_event.device = 0;
  pointer_event.signal_kind = kFlutterPointerSignalKindNone;
  pointer_event.scroll_delta_x = 0;
  pointer_event.scroll_delta_y = 0;
  pointer_event.device_kind = kFlutterPointerDeviceKindMouse;
  pointer_event.buttons = instance->input.mouse_button_mask;
  // TODO this is not 100% right as we should return the timestamp from libinput.
  // On my machine these seem to be using the same source but differnt unit, is this a guarantee?
  pointer_event.timestamp = instance->fl_proc_table.GetCurrentTime();
  instance->fl_proc_table.SendPointerEvent(instance->engine, &pointer_event, 1);
}

static void on_server_cursor_motion(struct wl_listener *listener, void *data) {
  struct fwr_instance *instance = wl_container_of(listener, instance, cursor_motion);
  struct wlr_event_pointer_motion *event = data;

  wlr_cursor_move(instance->cursor, event->device, event->delta_x,
                  event->delta_y);
  process_cursor_motion(instance, event->time_msec);
}

static void on_server_cursor_motion_absolute(struct wl_listener *listener, void *data) {
  struct fwr_instance *instance = wl_container_of(listener, instance, cursor_motion_absolute);
  struct wlr_event_pointer_motion_absolute *event = data;

  wlr_cursor_warp_absolute(instance->cursor, event->device, event->x, event->y);
  process_cursor_motion(instance, event->time_msec);
}

static void on_server_cursor_button(struct wl_listener *listener, void *data) {
  struct fwr_instance *instance = wl_container_of(listener, instance, cursor_button);
  struct wlr_event_pointer_button *event = data;

  uint32_t last_mask = instance->input.mouse_button_mask;

  uint32_t fl_button = uapi_mouse_button_to_flutter(event->button);
  if (fl_button != 0) {
    uint32_t mask = 1 >> (fl_button - 1);
    if (event->state == WLR_BUTTON_PRESSED) {
      instance->input.mouse_button_mask |= mask;
    } else if (event->state == WLR_BUTTON_RELEASED) {
      instance->input.mouse_button_mask &= ~mask;
    }
  }
  uint32_t curr_mask = instance->input.mouse_button_mask;

  FlutterPointerEvent pointer_event = {};
  pointer_event.struct_size = sizeof(FlutterPointerEvent);
  if (last_mask == 0 && curr_mask != 0) {
    pointer_event.phase = kDown;
  } else if (last_mask != 0 && curr_mask == 0) {
    pointer_event.phase = kUp;
  } else {
    pointer_event.phase = kMove;
  }
  pointer_event.x = instance->cursor->x;
  pointer_event.y = instance->cursor->y;
  pointer_event.device = 0;
  pointer_event.signal_kind = kFlutterPointerSignalKindNone;
  pointer_event.scroll_delta_x = 0;
  pointer_event.scroll_delta_y = 0;
  pointer_event.device_kind = kFlutterPointerDeviceKindMouse;
  pointer_event.buttons = curr_mask;
  // TODO this is not 100% right as we should return the timestamp from libinput.
  // On my machine these seem to be using the same source but differnt unit, is this a guarantee?
  pointer_event.timestamp = instance->fl_proc_table.GetCurrentTime();
  instance->fl_proc_table.SendPointerEvent(instance->engine, &pointer_event, 1);
}

static void on_server_cursor_axis(struct wl_listener *listener, void *data) {
  struct fwr_instance *instance = wl_container_of(listener, instance, cursor_axis);
}

static void on_server_cursor_frame(struct wl_listener *listener, void *data) {
  struct fwr_instance *instance = wl_container_of(listener, instance, cursor_frame);
}

static void on_server_cursor_touch_down(struct wl_listener *listener, void *data) {
  struct fwr_instance *instance = wl_container_of(listener, instance, cursor_touch_down);
}

static void on_server_cursor_touch_up(struct wl_listener *listener, void *data) {
  struct fwr_instance *instance = wl_container_of(listener, instance, cursor_touch_up);
}

static void on_server_cursor_touch_motion(struct wl_listener *listener, void *data) {
  struct fwr_instance *instance = wl_container_of(listener, instance, cursor_touch_motion);
}

static void on_server_cursor_touch_frame(struct wl_listener *listener, void *data) {
  struct fwr_instance *instance = wl_container_of(listener, instance, cursor_touch_frame);
}

static void cb(const uint8_t *data, size_t size, void *user_data) {
  wlr_log(WLR_INFO, "callback");
}

const uint64_t kValueMask = 0x000ffffffff;
const uint64_t kUnicodePlane = 0x00000000000;
const uint64_t kGtkPlane = 0x01500000000;

static uint64_t apply_id_plane(uint64_t logical_id, uint64_t plane) {
  return (logical_id & kValueMask) | plane;
}

static void send_key_to_flutter(struct fwr_keyboard *keyboard, struct wlr_event_keyboard_key *event) {
  struct fwr_instance *instance = keyboard->instance;

  FlutterPlatformMessageResponseHandle *response_handle;
  instance->fl_proc_table.PlatformMessageCreateResponseHandle(instance->engine, cb, NULL, &response_handle);

	uint32_t keycode = event->keycode + 8;
  xkb_keysym_t codepoint = xkb_state_key_get_one_sym(
      keyboard->device->keyboard->xkb_state, keycode);

  char *buffer;
  int size;

  // First find the needed size; return value is the same as snprintf(3).
  size = xkb_state_key_get_utf8(keyboard->device->keyboard->xkb_state, keycode, NULL, 0) + 1;
  if (size > 1) {
    buffer = malloc(size);

    xkb_state_key_get_utf8(keyboard->device->keyboard->xkb_state, keycode, buffer, size);
  }

  wlr_log(WLR_INFO, "%d", keycode);

  char *type;
  FlutterKeyEventType flType;

  switch(event->state) {
    case WL_KEYBOARD_KEY_STATE_PRESSED:
      type = "keydown";
      flType = kFlutterKeyEventTypeDown;
      break;
    case WL_KEYBOARD_KEY_STATE_RELEASED:
    default:
      type = "keyup";
      flType = kFlutterKeyEventTypeUp;
      break;
  }

  FlutterKeyEvent key_event = {};
  key_event.struct_size = sizeof(FlutterKeyEvent);
  key_event.type = flType;
  key_event.physical = 0x00070004;//codepoint;
  key_event.logical = apply_id_plane(0x041, kUnicodePlane);//65;
  key_event.character = flType == kFlutterKeyEventTypeDown ? buffer : NULL;
  key_event.timestamp = instance->fl_proc_table.GetCurrentTime();
  key_event.synthesized = false;
  instance->fl_proc_table.SendKeyEvent(instance->engine, &key_event, cb, response_handle);

  platch_send(
    instance,
    "flutter/keyevent",
    &(struct platch_obj) {
      .codec = kJSONMessageCodec,
      .json_value = {
        .type = kJsonObject,
        .size = 7,
        .keys = (char*[7]) {
          "keymap",
          "toolkit",
          "unicodeScalarValues",
          "keyCode",
          "scanCode",
          "modifiers",
          "type"
        },
        .values = (struct json_value[7]) {
          /* keymap */                {.type = kJsonString, .string_value = "linux"},
          /* toolkit */               {.type = kJsonString, .string_value = "gtk"},
          /* unicodeScalarValues */   {.type = kJsonNumber, .number_value = (flType == kFlutterKeyEventTypeDown ? 0x0410 : 0x0)},
          /* keyCode */               {.type = kJsonNumber, .number_value = apply_id_plane(0x041, kUnicodePlane)},
          /* scanCode */              {.type = kJsonNumber, .number_value = 0x00070004},
          /* modifiers */             {.type = kJsonNumber, .number_value = 0x0},
          /* type */                  {.type = kJsonString, .string_value = type}
        }
      }
    },
    kJSONMessageCodec,
    NULL,
    NULL
  );
  //instance->fl_proc_table.SendPlatformMessage(instance->engine, &platform_message);
}

static void keyboard_handle_modifiers(
		struct wl_listener *listener, void *data) {
    
  /* This event is raised when a modifier key, such as shift or alt, is
	 * pressed. We simply communicate this to the client. */
	struct fwr_keyboard *keyboard =
		wl_container_of(listener, keyboard, modifiers);
	/*
	 * A seat can only have one keyboard, but this is a limitation of the
	 * Wayland protocol - not wlroots. We assign all connected keyboards to the
	 * same seat. You can swap out the underlying wlr_keyboard like this and
	 * wlr_seat handles this transparently.
	 */
	wlr_seat_set_keyboard(keyboard->instance->seat, keyboard->device);
	/* Send modifiers to the client. */
	wlr_seat_keyboard_notify_modifiers(keyboard->instance->seat,
		&keyboard->device->keyboard->modifiers);


}

static void keyboard_handle_key(
		struct wl_listener *listener, void *data) {

	struct fwr_keyboard *keyboard =	wl_container_of(listener, keyboard, key);
  struct fwr_instance *instance = keyboard->instance;
  struct wlr_event_keyboard_key *event = data;
  //struct wlr_seat *seat = instance->seat;

  /* // translate libinput keycode to xkbcommon
  uint32_t keycode = event->keycode + 8;

  // get list of keysyms basd on the keymap for this keyboard
  const xkb_keysym_t *syms;
  int nsyms = xkb_state_key_get_syms(keyboard->device->keyboard->xkb_state, keycode, &syms); */

  //bool handled = false;

  send_key_to_flutter(keyboard, event);
  //send_key_to_flutter(keyboard, event, kFlutterKeyEventTypeUp);

  // uint32_t modifiers = wlr_keyboard_get_modifiers(keyboard->device->keyboard);
	// if ((modifiers & WLR_MODIFIER_ALT) &&
	// 		event->state == WL_KEYBOARD_KEY_STATE_PRESSED) {
	// 	/* If alt is held down and this button was _pressed_, we attempt to
	// 	 * process it as a compositor keybinding. */

  //   // TODO: pass keys to flutter ?
	// 	for (int i = 0; i < nsyms; i++) {
	// 		handled = handle_keybinding(instance, syms[i]);
	// 	}
	// }

	// if (!handled) {



	// 	/* Otherwise, we pass it along to the client. */
	// 	wlr_seat_set_keyboard(seat, keyboard->device);
	// 	wlr_seat_keyboard_notify_key(seat, event->time_msec,
	// 		event->keycode, event->state);
	// }

}

static void server_new_keyboard(struct fwr_instance *instance,
		struct wlr_input_device *device) {

  struct fwr_keyboard *keyboard = calloc(1, sizeof(struct fwr_keyboard));
  keyboard->instance = instance;
  keyboard->device = device;

  // Prepare XKB keymap and asing to keyboard, default layout is "us"
  struct xkb_context *context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
	if (!context) {
	  wlr_log(WLR_ERROR, "Failed to create XKB context");
		exit(1);
	}

  struct xkb_rule_names rules = { 0 };
	rules.rules = getenv("XKB_DEFAULT_RULES");
	rules.model = getenv("XKB_DEFAULT_MODEL");
	rules.layout = getenv("XKB_DEFAULT_LAYOUT");
	rules.variant = getenv("XKB_DEFAULT_VARIANT");
	rules.options = getenv("XKB_DEFAULT_OPTIONS");
  struct xkb_keymap *keymap = xkb_keymap_new_from_names(context, &rules, XKB_KEYMAP_COMPILE_NO_FLAGS);

  wlr_keyboard_set_keymap(device->keyboard, keymap);
  xkb_keymap_unref(keymap);
  xkb_context_unref(context);
  wlr_keyboard_set_repeat_info(device->keyboard, 25, 600);

  // here we set up listeners for keyboard events
  keyboard->modifiers.notify = keyboard_handle_modifiers;
  wl_signal_add(&device->keyboard->events.modifiers, &keyboard->modifiers);
  keyboard->key.notify = keyboard_handle_key;
  wl_signal_add(&device->keyboard->events.key, &keyboard->key);

  wlr_seat_set_keyboard(instance->seat, device);

  // add keyboard to list of keyboards
  wl_list_insert(&instance->keyboards, &keyboard->link);


}

static void server_new_pointer(struct fwr_instance *instance,
		struct wlr_input_device *device) {
  wlr_cursor_attach_input_device(instance->cursor, device);
}

static void server_new_touch(struct fwr_instance *instance,
		struct wlr_input_device *device) {

}

static void on_server_new_input(struct wl_listener *listener, void *data) {
  struct fwr_instance *instance = wl_container_of(listener, instance, new_input);
  struct wlr_input_device *device = data;

  switch (device->type) {
	case WLR_INPUT_DEVICE_KEYBOARD:
		server_new_keyboard(instance, device);
		break;
	case WLR_INPUT_DEVICE_POINTER:
		server_new_pointer(instance, device);
		break;
  case WLR_INPUT_DEVICE_TOUCH:
    server_new_touch(instance, device);
    break;
	default:
		break;
	}

  // TODO seat caps
  uint32_t caps = WL_SEAT_CAPABILITY_POINTER;
  if (!wl_list_empty(&instance->keyboards)) {
   caps |= WL_SEAT_CAPABILITY_KEYBOARD;
  }
  wlr_seat_set_capabilities(instance->seat, caps);
}

void fwr_input_init(struct fwr_instance *instance) {
  instance->input.mouse_down = false;
  instance->input.mouse_button_mask = 0;

  instance->cursor = wlr_cursor_create();
	wlr_cursor_attach_output_layout(instance->cursor, instance->output_layout);

  instance->cursor_mgr = wlr_xcursor_manager_create(NULL, 24);
	wlr_xcursor_manager_load(instance->cursor_mgr, 1);

	instance->seat = wlr_seat_create(instance->wl_display, "seat0");

  instance->cursor_motion.notify = on_server_cursor_motion;
  wl_signal_add(&instance->cursor->events.motion, &instance->cursor_motion);
  instance->cursor_motion_absolute.notify = on_server_cursor_motion_absolute;
  wl_signal_add(&instance->cursor->events.motion_absolute, &instance->cursor_motion_absolute);
  instance->cursor_button.notify = on_server_cursor_button;
  wl_signal_add(&instance->cursor->events.button, &instance->cursor_button);
  instance->cursor_axis.notify = on_server_cursor_axis;
  wl_signal_add(&instance->cursor->events.axis, &instance->cursor_axis);
  instance->cursor_frame.notify = on_server_cursor_frame;
  wl_signal_add(&instance->cursor->events.frame, &instance->cursor_frame);
  instance->cursor_touch_down.notify = on_server_cursor_touch_down;
  wl_signal_add(&instance->cursor->events.touch_down, &instance->cursor_touch_down);
  instance->cursor_touch_up.notify = on_server_cursor_touch_up;
  wl_signal_add(&instance->cursor->events.touch_up, &instance->cursor_touch_up);
  instance->cursor_touch_motion.notify = on_server_cursor_touch_motion;
  wl_signal_add(&instance->cursor->events.touch_motion, &instance->cursor_touch_motion);
  instance->cursor_touch_frame.notify = on_server_cursor_touch_frame;
  wl_signal_add(&instance->cursor->events.touch_frame, &instance->cursor_touch_frame);

  wl_list_init(&instance->keyboards);

  instance->new_input.notify = on_server_new_input;
	wl_signal_add(&instance->backend->events.new_input, &instance->new_input);
}

void fwr_handle_surface_pointer_event_message(struct fwr_instance *instance, const FlutterPlatformMessageResponseHandle *handle, struct dart_value *args) {
  struct surface_pointer_event_message message;
  if (!decode_surface_pointer_event_message(args, &message)) {
    goto error;
  }

  struct fwr_view *view;
  if (!handle_map_get(instance->views, message.surface_handle, (void**) &view)) {
    // This implies a race condition of surface removal.
    // We return success here.
    goto success;
  }

  //wlr_log(WLR_INFO, "yay pointer event %d %ld", message.event_type, message.buttons);

  struct wlr_surface_state *surface_state = &view->surface->surface->current;

  double transformed_local_pos_x = message.local_pos_x / message.widget_size_x * surface_state->width;
  double transformed_local_pos_y = message.local_pos_y / message.widget_size_y * surface_state->height;

  switch (message.device_kind) {
    case pointerKindMouse: {
      switch (message.event_type) {
        case pointerDownEvent:
          wlr_seat_pointer_notify_button(instance->seat, message.timestamp / NS_PER_MS, 0x110, WLR_BUTTON_PRESSED);
          break;
        case pointerUpEvent:
          wlr_seat_pointer_notify_button(instance->seat, message.timestamp / NS_PER_MS, 0x110, WLR_BUTTON_RELEASED);
          break;
        case pointerHoverEvent:
        case pointerEnterEvent:
        case pointerMoveEvent: {
          wlr_seat_pointer_notify_enter(instance->seat, view->surface->surface, transformed_local_pos_x, transformed_local_pos_y);
          wlr_seat_pointer_notify_motion(instance->seat, message.timestamp / NS_PER_MS, transformed_local_pos_x, transformed_local_pos_y);
          break;
        }
        case pointerExitEvent: {
          wlr_seat_pointer_clear_focus(instance->seat);
          break;
        }
      }

      wlr_seat_pointer_notify_frame(instance->seat);

      break;
    }
  }


success:
  instance->fl_proc_table.SendPlatformMessageResponse(instance->engine, handle, method_call_null_success, sizeof(method_call_null_success));
  return;

error:
  wlr_log(WLR_ERROR, "Invalid surface pointer event message");
  // Send failure
  instance->fl_proc_table.SendPlatformMessageResponse(instance->engine, handle, NULL, 0);
}
