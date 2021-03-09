[Setup]
AppName=ClickOff
AppVerName=ClickOff version 1.90
AppCopyright=Copyright © Johannes Hübner
DefaultDirName={pf}\ClickOff
DefaultGroupName=ClickOff
AllowNoIcons=1
MinVersion=4,4

[Files]
Source: "Clickoff.exe"; DestDir: "{app}"
Source: "Brasileiro.lng"; DestDir: "{app}"
Source: "Deutsch.lng"; DestDir: "{app}"
Source: "English.lng"; DestDir: "{app}"
Source: "Español.lng"; DestDir: "{app}"
Source: "Français.lng"; DestDir: "{app}"
Source: "Italiano.lng"; DestDir: "{app}"
Source: "Greek.lng"; DestDir: "{app}"
Source: "Readme.txt"; DestDir: "{app}"; Flags: isreadme
Source: "getting_started_ge.html"; DestDir: "{app}";
Source: "getting_started_en.html"; DestDir: "{app}";
Source: "getting_started_br.html"; DestDir: "{app}";
Source: "getting_started_el.html"; DestDir: "{app}";

[Run]
Filename: "{app}\CLICKOFF.EXE"; Parameters: "/setup"; StatusMsg: "Intializing ClickOff for first run..."; Flags: waituntilidle

[Icons]
Name: "{group}\ClickOff starten bzw. anzeigen"; Filename: "{app}\CLICKOFF.EXE"; comment: "start/display ClickOff"; WorkingDir: "{app}"
Name: "{group}\ClickOff beenden"; Filename: "{app}\CLICKOFF.EXE"; Parameters: "/exit"; comment: "exit ClickOff"; WorkingDir: "{app}"
Name: "{group}\ClickOff ohne TrayIcon"; Filename: "{app}\CLICKOFF.EXE"; Parameters:"/notray"; comment: "start ClickOff without TrayIcon"; WorkingDir: "{app}"
Name: "{userstartup}\ClickOff"; Filename: "{app}\CLICKOFF.EXE"; comment: "ClickOff starten"; WorkingDir: "{app}"

[UninstallDelete]
Type: files; Name: "{userappdata}\ClickOff\clickoff.cfg"
Type: files; Name: "{userappdata}\ClickOff\clickoff.hwnd"
Type: files; Name: "{userappdata}\ClickOff\clickoff.cfg"

[UninstallRun]
Filename: "{app}\clickoff.exe"; Parameters: "/exit"

[Messages]
FinishedLabel=Setup has finished installing ClickOff on your computer. The application may be launched by selecting the installed icons. Setup has also created an icon in your startup folder to have [name] start automatically when windows starts.
SetupLdrStartupMessage=This will install ClickOff, exit any former version of ClickOff before proceding. Do you wish to continue?
