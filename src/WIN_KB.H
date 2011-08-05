/*
                     Файл WIN_KB.H
   Соответствие имен клавиш виртуальным кодам Windows,
*/

#pragma once

//специальные кнопки WinKB
#define KeyCodLWin         VK_LWIN // парные кнопки внизу, слева и
#define KeyCodRWin         VK_RWIN // справа, только KEYDOWN (ON) !
#define KeyCodWinApps      VK_APPS // одиночная кнопка справа внизу

#define KeyCodEsc          VK_ESCAPE
#define KeyCodF1           VK_F1
#define KeyCodF2           VK_F2
#define KeyCodF3           VK_F3
#define KeyCodF4           VK_F4
#define KeyCodF5           VK_F5
#define KeyCodF6           VK_F6
#define KeyCodF7           VK_F7
#define KeyCodF8           VK_F8
#define KeyCodF9           VK_F9
#define KeyCodF10          VK_F10
#define KeyCodF11          VK_F11
#define KeyCodF12          VK_F12
#define KeyCodPrntScrn     VK_SNAPSHOT
#define KeyCodScrlLck      VK_SCROLL
#define KeyCodPause        VK_PAUSE

#define KeyCodTilda        192 // ~
#define KeyCod1            '1' // 1 и !
#define KeyCod2            '2' // 2 и "амфора" (или "собака")
#define KeyCod3            '3' // ... #
#define KeyCod4            '4' //     $
#define KeyCod5            '5' //     %
#define KeyCod6            '6' //     ^
#define KeyCod7            '7' //     &
#define KeyCod8            '8' //     *
#define KeyCod9            '9' //     (
#define KeyCod0            '0' //     )
#define KeyCodMinus        189 // - и _
#define KeyCodPlus         187 // + и =
#define KeyCodBS           VK_BACK
#define KeyCodIns          VK_INSERT
#define KeyCodHome         VK_HOME
#define KeyCodPgUp         VK_PRIOR
#define KeyCodNL           VK_NUMLOCK
#define KeyCodAddDiv       VK_DIVIDE
#define KeyCodAddMul       VK_MULTIPLY
#define KeyCodAddMinus     VK_SUBTRACT

#define KeyCodTab          VK_TAB
#define KeyCodQ            'Q'
#define KeyCodW            'W'
#define KeyCodE            'E'
#define KeyCodR            'R'
#define KeyCodT            'T'
#define KeyCodY            'Y'
#define KeyCodU            'U'
#define KeyCodI            'I'
#define KeyCodO            'O'
#define KeyCodP            'P'
#define KeyCodLBracket     219 // {
#define KeyCodRBracket     221 // }
#define KeyCodEnter        VK_RETURN
#define KeyCodDel          VK_DELETE
#define KeyCodEnd          VK_END
#define KeyCodPgDn         VK_NEXT
#define KeyCodAdd7         VK_NUMPAD7
#define KeyCodAdd8         VK_NUMPAD8
#define KeyCodAdd9         VK_NUMPAD9
#define KeyCodAddPlus      VK_ADD

#define KeyCodCapsLock     VK_CAPITAL
#define KeyCodA            'A'
#define KeyCodS            'S'
#define KeyCodD            'D'
#define KeyCodF            'F'
#define KeyCodG            'G'
#define KeyCodH            'H'
#define KeyCodJ            'J'
#define KeyCodK            'K'
#define KeyCodL            'L'
#define KeyCodSemicolon    186 // ;
#define KeyCodInvCommas    222 // '
#define KeyCodBackSlash    220 /*** \ ***/
#define KeyCodAdd4         VK_NUMPAD4
#define KeyCodAdd5         VK_NUMPAD5
#define KeyCodAdd6         VK_NUMPAD6

#define KeyCodLShift       KeyCodShift // одинаковые коды !
#define KeyCodRShift       KeyCodShift
#define KeyCodShift        VK_SHIFT

#define KeyCodZ            'Z'
#define KeyCodX            'X'
#define KeyCodC            'C'
#define KeyCodV            'V'
#define KeyCodB            'B'
#define KeyCodN            'N'
#define KeyCodM            'M'
#define KeyCodComma        188 // ,
#define KeyCodPoint        190 // .
#define KeyCodDiv          191 // /
#define KeyCodUp           VK_UP
#define KeyCodAdd1         VK_NUMPAD1
#define KeyCodAdd2         VK_NUMPAD2
#define KeyCodAdd3         VK_NUMPAD3
#define KeyCodAddEnter     KeyCodEnter // одинаковые коды!

#define KeyCodLCtrl        KeyCodCtrl // одинаковые коды!
#define KeyCodRCtrl        KeyCodCtrl
#define KeyCodCtrl         VK_CONTROL

#define KeyCodLAlt         KeyCodAlt  // одинаковые коды!
#define KeyCodRAlt         KeyCodAlt
#define KeyCodAlt          VK_MENU

#define KeyCodSpace        VK_SPACE
#define KeyCodLeft         VK_LEFT
#define KeyCodDown         VK_DOWN
#define KeyCodRight        VK_RIGHT
#define KeyCodAdd0         VK_NUMPAD0 // если не NumLock - выдается KeyCodIns!
#define KeyCodAddDel       VK_DECIMAL // если не NumLock - выдается KeyCodDel!

