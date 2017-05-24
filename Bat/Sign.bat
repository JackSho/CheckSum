set myTargetFilePath=%~1
set myTargetFileName=%~2

REM set SignCert=%~3
REM set SignCert="E:\MY\MakePackage\ChangeHttps\SingleSignTool\VTech Electronics North America.pfx"
set SignCert="%SolutionDir%Cert\SymAuthCode2017.pfx"
set Password=y28JjgC9TM

signtool.exe sign /f %SignCert% /fd SHA256 /p %Password% /d "%myTargetFileName%" /t http://timestamp.verisign.com/scripts/timestamp.dll "%myTargetFilePath%"
