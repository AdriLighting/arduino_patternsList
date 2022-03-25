#include <File.au3>
#include <Array.au3>


Global Const $main_path = "D:\programmation_code\Arduino\Arduino_Projet\0A_projet\2_mylib_v1\lib\arduino_patternsList\"

clean($main_path)
Func clean($p1)
	Local $sDrive = "", $sDir = "", $sFileName = "", $sExtension = ""
	Local $split_1

	Local $list_f = _FileListToArrayRec($p1, "*", 2, 1, 0, 2)
	for $i = 0 to ubound($list_f) - 1
;~ 		Local $aPathSplit = _PathSplit($list_f[$i], $sDrive, $sDir, $sFileName, $sExtension)
		$split_1 = StringSplit($list_f[$i], "\")
		if not @error then
			Local $name = $split_1[UBound($split_1)-2]
			if ($name = "") then ContinueLoop
			Local $find = False
			Switch ($name)
				Case ".vs"
					$find = true
				Case "__vm"
					$find = true
				Case "Debug"
					$find = true
				Case ".pio"
					$find = true
			EndSwitch
			if ($find) Then
				ConsoleWrite($list_f[$i] & @CRLF &  $name & @CRLF)
				DirRemove($list_f[$i], 1)
			EndIf

		EndIf
	next

endfunc
