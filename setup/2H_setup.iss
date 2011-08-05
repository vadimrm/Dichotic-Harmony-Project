[Setup]
;--- Русский язык для инсталлера Inno Setup 3.0.7
MessagesFile=compiler:Russian-18-3.0.5.isl

AppName=Dichotic Harmony Project
AppVerName=Dichotic Harmony Project. Version 3.04 from August, 2011.
AppCopyright=Copyright © 2010 Vadim Madgazin
DefaultDirName={pf}\Dichotic Harmony Project
DefaultGroupName=Dichotic Harmony Project

DisableStartupPrompt=yes
;--- Алгоритм и уровень компрессии (макс.=9)
Compression=bzip/9
;--- Мин версии win95-4 winNT-4
MinVersion=4,4
ExtraDiskSpaceRequired=9000000
;--- Показ лицензионного соглашения: ставим коммернарий и
; избавляем юзера от лишнего диалога с выбором "I Agree"
; LicenseFile=license.txt
;--- картинка сетапа (по умолчанию будет битмап 26 кбайт)
WizardImageFile=musicstore.bmp

;--- Выходной файл дистрибутива
OutputBaseFilename=2h_setup

;--- включаем ассоциацию .daccords файлов
; См. Inno Setup 3 -> Innosetup FAQ; линк IE: file:///E:/Program%20Files/Inno%20Setup%203/isfaq.htm#assoc
ChangesAssociations=yes
[Registry]
Root: HKCR; Subkey: ".daccords"; ValueType: string; ValueName: ""; ValueData: "Dichotic Harmony Accords Generator"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "Dichotic Harmony Accords Generator"; ValueType: string; ValueName: ""; ValueData: "Dichotic Harmony Accords Generator"; Flags: uninsdeletekey
Root: HKCR; Subkey: "Dichotic Harmony Accords Generator\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\bin\2hag.exe"" ""%1"""

 Root: HKCR; Subkey: "Dichotic Harmony Accords Generator\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\bin\2hag.exe,0"
;Root: HKCR; Subkey: "Dichotic Harmony Accords Generator\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\bin\icon.ico,0"

[Dirs]

Name: "{app}\addons"
Name: "{app}\arxiv"
Name: "{app}\bin"
Name: "{app}\bin\accords"
Name: "{app}\bin\accords\2H_LIB"
Name: "{app}\bin\accords\DISS_MUSIC"
Name: "{app}\bin\accords\MUSIC"
Name: "{app}\midi"
Name: "{app}\midi\2H_LIB"
Name: "{app}\midi\DISS_MUSIC"
Name: "{app}\setup"
Name: "{app}\src"
Name: "{app}\utils"

[Files]

Source: "..\*";                            DestDir: "{app}"
Source: "..\addons\*";                     DestDir: "{app}\addons"
Source: "..\arxiv\*";                      DestDir: "{app}\arxiv"
Source: "..\bin\*";                        DestDir: "{app}\bin"
Source: "..\bin\accords\*";                DestDir: "{app}\bin\accords"
Source: "..\bin\accords\2H_LIB\*";         DestDir: "{app}\bin\accords\2H_LIB"
Source: "..\bin\accords\DISS_MUSIC\*";     DestDir: "{app}\bin\accords\DISS_MUSIC"
Source: "..\bin\accords\MUSIC\*";          DestDir: "{app}\bin\accords\MUSIC"
Source: "..\midi\*";                       DestDir: "{app}\midi"
Source: "..\midi\2H_LIB\*";                DestDir: "{app}\midi\2H_LIB"
Source: "..\midi\DISS_MUSIC\*";            DestDir: "{app}\midi\DISS_MUSIC"
Source: "..\setup\*";                      DestDir: "{app}\setup"
Source: "..\src\*";                        DestDir: "{app}\src"
Source: "..\utils\*";                      DestDir: "{app}\utils"

[Icons]

; --- ярлыки в меню запуска программ
Name: "{group}\Dichotic Harmony Accords Generator"; Filename: "{app}\bin\2hag.exe"; WorkingDir: "{app}\bin";
Name: "{group}\Документация к программе DHAG";      Filename: "{app}\bin\help.txt";
Name: "{group}\Дихотические MIDI файлы";            Filename: "{app}\midi";
Name: "{group}\Uninstall";                          Filename: "{uninstallexe}";

;--- ярлыки на десктопе
Name: "{userdesktop}\Dichotic Harmony Project";           Filename: "{app}";
Name: "{userdesktop}\Dichotic Harmony Accords Generator"; Filename: "{app}\bin\2hag.exe"; WorkingDir: "{app}\bin";

[Run]

; --- по умолчанию запускаем это
Filename: "{app}\bin\2hag.exe";  WorkingDir: "{app}\bin"; Description: "Запустить Dichotic Harmony Accords Generator"; Flags: postinstall nowait skipifsilent;

