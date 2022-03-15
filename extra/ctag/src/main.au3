#Region ;**** Directives created by AutoIt3Wrapper_GUI ****
#AutoIt3Wrapper_Icon=res\arduino-icon.ico
#AutoIt3Wrapper_Outfile=..\arduinolib.exe
#AutoIt3Wrapper_Res_Comment=Arduino library bank
#AutoIt3Wrapper_Res_Description=Arduino library bank
#AutoIt3Wrapper_Res_Fileversion=0.0.1
#AutoIt3Wrapper_Res_ProductName=Arduino library bank
#AutoIt3Wrapper_Res_CompanyName=AdriLIghting
#AutoIt3Wrapper_Res_LegalCopyright=AdriLIghting
#AutoIt3Wrapper_Res_Language=1036
#AutoIt3Wrapper_AU3Check_Parameters=-q -d -w 1 -w 2 -w 3 -w- 4 -w 5 -w 6 -w- 7
#EndRegion ;**** Directives created by AutoIt3Wrapper_GUI ****

#include <File.au3>
#include <GuiMenu.au3>
#include <GuiRichEdit.au3>

#include "lib_interne/Json.au3"
#include "lib_interne/_ADFunc.au3"
#include "lib_interne/ArrayMultiColSort.au3"
#include "lib_interne/Json_Arrayfied/json.au3"

 Opt("MustDeclareVars", 1)

Global $cpp_getTags_array[0][12]
Global $fp_ctag = "C:\Users\adri\Downloads\ctags58\ctags58\ctags.exe"
Global $fo_ctag = @TempDir & "\"
Global $fp_file = "D:\programmation_code\Arduino\Arduino_Projet\0A_projet\2_mylib_v1\lib\arduino_patternsList\src\apapi.cpp"



_cpp_getTags(@TempDir & "\tags", $fp_file)
;~ if FileExists(@TempDir & "\ctag_doxy") Then FileDelete(@TempDir & "\ctg_doxy")
_printFile($fp_file)
func _format($macro, $value, byref $in, $size = 16, $size2 = 0)
	local $v1 = ' * '&$macro&' '
	While (StringLen($v1) < $size)
		$v1 &= " "
	WEnd
	$v1 &= $value
	if ($size2<>0) Then
		While (StringLen($v1) < 26)
			$v1 &= " "
		WEnd
	EndIf
	$in &= $v1 & @crlf
EndFunc
func _printFile($sInFile)
	Local $line_doc, $doncata, $final, $split
	if FileExists(@DesktopDir & "\test.cpp") Then FileDelete(@DesktopDir & "\test.cpp")
	For $A = 0 To _FileCountLines($sInFile)
		$line_doc = FileReadLine($sInFile,  $A)
		$doncata = ""
		for $i = 0 to UBound($cpp_getTags_array)-1
			if StringInStr($line_doc, $cpp_getTags_array[$i][0], 1, 1) and not StringInStr($line_doc, ";", 0, 1) and StringInStr($line_doc, "{", 0, 1) then
				if ($cpp_getTags_array[$i][3] = "f" and $cpp_getTags_array[$i][2] <> "" ) then
					consoleWrite("-----search--------" & @lf & @tab & $line_doc & @lf & "-----search--------" & @lf)
					$final = "/**" & @crlf
					_format('@fn', StringSplit($cpp_getTags_array[$i][1], ";")[1], $final, 16)
					$final &= ' *' & @crlf
					_format('@brief', '', $final, 16)
					_format('@details', '', $final, 16)
					$final &= ' *' & @crlf
					_format('@author', 'Adrien Grellard', $final, 16)
					_format('@date', @MDAY & "/" & @MON & "/" & @YEAR, $final, 16)
					if ($cpp_getTags_array[$i][11] <> "()") Then
						$final &= ' *' & @crlf
						$cpp_getTags_array[$i][11] = StringReplace($cpp_getTags_array[$i][11], "(", "")
						$cpp_getTags_array[$i][11] = StringReplace($cpp_getTags_array[$i][11], ")", "")
						$split = StringSplit($cpp_getTags_array[$i][11], ',')
						if Not @error Then
							for $j = 1 to UBound($split)-1
								if ( StringInStr($split[$j], "&") or  StringInStr($split[$j], "*")  ) Then
									if  StringInStr($split[$j], "const")  Then
									_format('@param[in]', StringSplit($split[$j]," ")[UBound(StringSplit($split[$j]," "))-1], $final, 16, 26)
									else
									_format('@param[out]', StringSplit($split[$j]," ")[UBound(StringSplit($split[$j]," "))-1], $final, 16, 26)
									endif
								Else
									_format('@param[in]', StringSplit($split[$j]," ")[UBound(StringSplit($split[$j]," "))-1], $final, 16, 26)
								EndIf
							Next
						Else
							if ( StringInStr($cpp_getTags_array[$i][11], "&") or  StringInStr($cpp_getTags_array[$i][11], $final, "*")  ) Then
								if  StringInStr($cpp_getTags_array[$i][11], "const")  Then
								_format('@param[in]', StringSplit($cpp_getTags_array[$i][11]," ")[UBound(StringSplit($cpp_getTags_array[$i][11]," "))-1], $final, 16, 26)
								else
								_format('@param[out]', StringSplit($cpp_getTags_array[$i][11]," ")[UBound(StringSplit($cpp_getTags_array[$i][11]," "))-1], $final, 16, 26)
								endif
							Else
								_format('@param[in]', StringSplit($cpp_getTags_array[$i][11]," ")[UBound(StringSplit($cpp_getTags_array[$i][11]," "))-1], $final, 16, 26)
							EndIf
						EndIf
					EndIf
;~ 					if ($cpp_getTags_array[$i][2] == "void") Then
;~ 						_format('@return', 'void()', $final, 16)
;~ 					Else
;~ 						if ($cpp_getTags_array[$i][2] <> "") Then _format('@return', '', $final, 16)
;~ 					EndIf
					$doncata = $final & " */" & @crlf
					ExitLoop
				EndIf
			EndIf
		Next
		FileWrite(@DesktopDir & "\test.cpp", $doncata & $line_doc & @crlf )
	Next
EndFunc
Func _cpp_getTags($sFo_source, $include)

;~ 	$cpp_getTags_fp = $include

;~ 	if FileExists($sFo_source) 	then FileDelete($sFo_source)
;~ 	if FileExists($outIni) 		then FileDelete($outIni)

	local $cmd = ' ctags -R --languages=C,C++ --c++-kinds=+p --fields=+iaS --extra=+q '& '"' & $include & '"'
;~ 	local $cmd = ' ctags -F -R '& '"' & $include & '"'

	RunWait ($fp_ctag & String($cmd), $fo_ctag, @SW_hide)

	local $line, $str, $split_1, $split_2, $split_3, $res, $out, $type_1, $class, $enum, $struct, $namespace, $iAsc, $fline, $sign, $access, $type_2, $data, $find

	For $j = 0 To _FileCountLines($sFo_source)

		$line 	= FileReadLine($sFo_source, $j)
		$str 	= StringReplace($line, $include, "")

		$split_1 = _StringBetween($str, "", "/^")
		if @error then ContinueLoop


		$split_2 = _StringBetween($str, '/^', '$/;"')
		if @error then ContinueLoop

		$str = StringReplace($str, $split_2[0], "")
		$str = StringReplace($str, $split_1[0], "")
		$str = StringReplace($str, '/^', "")
		$str = StringReplace($str, '$/;"', "")
;~ 		ConsoleWrite( "STR = " & $str & @lf)

		$res = StringRegExp($str, '.', 3)
		$out = ""
		For $char in $res
			$iAsc = Asc($char)
			if $iAsc = 9 then
				$out &= @CRLF
			else
				$out &= $char
			EndIf
		Next
;~ 		ConsoleWrite( "OUT = " & $out & @lf)

		$split_3 = StringSplit($out, @CRLF)
		Local $ret[0]
		For $i = 1 to UBound($split_3) - 1
			if $split_3[$i] <> "" then _ArrayAdd($ret, $split_3[$i])
		next
		$type_1  = ""
		$fline 	= ""
		$class 	= ""
		$sign 	= ""
		$enum 	= ""
		$struct	= ""
		$namespace	= ""
		$access	= ""
		if UBound($ret)-1 > 0 then $type_1 	= $ret[0]
;~ 		if ($type_1 <> "function") then ContinueLoop
		if UBound($ret)-1 > 0 then $fline	= $ret[1]
		For $i = 0 to UBound($ret) - 1
			$split_3 = StringSplit($ret[$i], ":")
			if UBound($split_3)-1 > 1 then
;~ 				ConsoleWrite( @tab & $split_3[1] & @lf)
				if  $split_3[1] = "line" 		then $fline 	= $split_3[2]
				if  $split_3[1] = "signature" 	then $sign 		= $split_3[2]
				if  $split_3[1] = "class" 		then $class 	= $split_3[2]
				if  $split_3[1] = "enum" 		then $enum 		= $split_3[2]
				if  $split_3[1] = "struct" 		then $struct	= $split_3[2]
				if  $split_3[1] = "namespace"	then $namespace	= $split_3[2]
				if  $split_3[1] = "access"		then $access	= $split_3[2]
			endif
		next

		$split_1[0] = StringStripWS($split_1[0], 1+2+4)
		$split_1[0] = StringReplace($split_1[0], "	", " ")

		$split_2[0] = StringStripWS($split_2[0], 1+2+4)
		$split_2[0] = StringReplace($split_2[0], "	", " ")
		$split_2[0] = StringReplace($split_2[0], "{", ";")

		$type_2 = StringSplit($split_2[0], " ")
		$data = ""
		$find = 0
;~ 		ConsoleWrite( @tab & $split_2[0] & @lf)
;~ 		_ArrayDisplay($type_2)

		for $i = 1 to UBound($type_2)-1
			$data &= $type_2[$i] & " "
			if $type_1 = "function" and $class <> "" and StringInStr($type_2[$i], ":") then
;~ 				ConsoleWrite("ExitLoop CLASS" & @lf)
				ExitLoop
			EndIf
			if $type_1 = "function" and $class = "" and StringInStr($type_2[$i], "(") then
;~ 				ConsoleWrite("ExitLoop FUNC" & @lf)
				ExitLoop
			EndIf
			if $type_1 = "member" and $class = "" and StringInStr($type_2[$i], "(") then
;~ 				ConsoleWrite("ExitLoop FUNC" & @lf)
				ExitLoop
			EndIf
			if StringInStr($type_2[$i], "=") then
					$find = 1
			EndIf
			if $type_2[$i] = "*" then $find = 1
			if $type_2[$i] = "&" then $find = 1


			if $find <> 0 then ExitLoop
		next
;~ 		ConsoleWrite("$data " & $data & @lf)
		$data = StringReplace($data, "=", "")
		$data = StringStripWS($data, 1+2+4)
		if $find = 1 Then
;~ 			ConsoleWrite("StringRight " & StringRight($data, 2) & @lf)
;~ 			ConsoleWrite("StringInStr " & StringInStr($data, "*") & @lf)
			if StringInStr($data, "*") = 0 then $find = 0
;~ 			elseif StringInStr($data, "&") = 0 then $find = 0
;~ 			if StringRight($data, 2) = " &" then $find = 1
		EndIf
;~ 		ConsoleWrite("$find$find " & $find & @lf)
;~ _ArrayDisplay($type_2)
		if $find = 1 Then

			$type_2 = $data
		Elseif $find = 0 Then

			if StringInStr($type_2[1], "::") then
				$type_2[1] = ""
;~ 				ConsoleWrite("WTF" & @lf)
			EndIf
			$type_2 = $type_2[1]
		Else
			$type_2 = ""
		EndIf
		$type_2 = StringStripWS($type_2, 1+2+4)

		ConsoleWrite( $line & @lf)
		ConsoleWrite( "name 		= " & $split_1[0] & @lf)
		ConsoleWrite( "def_line 	= " & $split_2[0] & @lf)
		ConsoleWrite( "type_2 		= " & $type_2 & @lf)
		ConsoleWrite( "type_1 		= " & $type_1 & @lf)
		ConsoleWrite( "line_nbr	= " 	& $fline & @lf)
		ConsoleWrite( "class 		= " & $class & @lf)
		ConsoleWrite( "access 		= " & $access & @lf)
		ConsoleWrite( "enum 		= " & $enum & @lf)
		ConsoleWrite( "struct 		= " & $struct & @lf)
		ConsoleWrite( "namespace	= "	& $namespace & @lf)
		ConsoleWrite( "sign		= " 	& $sign & @lf)

			_ADFunc_Array_ArrayAdd_2D($cpp_getTags_array, $split_1[0],	UBound($cpp_getTags_array), 0)
			_ADFunc_Array_ArrayAdd_2D($cpp_getTags_array, $split_2[0],	UBound($cpp_getTags_array)-1, 1)
			_ADFunc_Array_ArrayAdd_2D($cpp_getTags_array, $type_2,		UBound($cpp_getTags_array)-1, 2)
			_ADFunc_Array_ArrayAdd_2D($cpp_getTags_array, $type_1,		UBound($cpp_getTags_array)-1, 3)
			_ADFunc_Array_ArrayAdd_2D($cpp_getTags_array, $type_1,		UBound($cpp_getTags_array)-1, 4)
			_ADFunc_Array_ArrayAdd_2D($cpp_getTags_array, $fline,		UBound($cpp_getTags_array)-1, 5)
			_ADFunc_Array_ArrayAdd_2D($cpp_getTags_array, $class,		UBound($cpp_getTags_array)-1, 6)
			_ADFunc_Array_ArrayAdd_2D($cpp_getTags_array, $access,		UBound($cpp_getTags_array)-1, 7)
			_ADFunc_Array_ArrayAdd_2D($cpp_getTags_array, $enum,		UBound($cpp_getTags_array)-1, 8)
			_ADFunc_Array_ArrayAdd_2D($cpp_getTags_array, $struct,		UBound($cpp_getTags_array)-1, 9)
			_ADFunc_Array_ArrayAdd_2D($cpp_getTags_array, $namespace,	UBound($cpp_getTags_array)-1, 10)
			_ADFunc_Array_ArrayAdd_2D($cpp_getTags_array, $sign,		UBound($cpp_getTags_array)-1, 11)

		Switch $type_1
			Case "f"

			Case "member"
				if StringRight($split_2[0], 1) = ";" then
					if $class 		<> "" then ConsoleWrite( "member		= " & $split_2[0] & " OF class = " & $class & @lf)
					if $struct 		<> "" then ConsoleWrite( "member		= " & $split_2[0] & " OF struct = " & $struct & @lf)
					if $namespace 	<> "" then ConsoleWrite( "member		= " & $split_2[0] & " OF namespace = " & $namespace & @lf)
				Else
					if $class <> 	"" then
						ConsoleWrite( "member		= " & $type_2 & " " & $split_1[0] & ";" & " OF class = " & $class & @lf)
					elseif $struct <> 	"" then
						ConsoleWrite( "member		= " & $type_2 & " " & $split_1[0] & ";" & " OF struct = " & $struct & @lf)
					elseif $namespace <> 	"" then
						ConsoleWrite( "member		= " & $type_2 & " " & $split_1[0] & ";" & " OF namespace = " & $namespace & @lf)
					Else
						ConsoleWrite("MEMBER NOT FOOUND" & @lf)
					EndIf
				EndIf

			case "function"
				if StringRight($split_2[0], 1) = ";" then
					ConsoleWrite( "function	= " & $split_2[0] & @lf)
				Else
					if $class <> 	"" then ConsoleWrite("function	= " & $type_2 & " " & $class & "::" & $split_1[0] & $sign & ";" & @lf)
					if $class = 	"" then ConsoleWrite("function	= " & $type_2 & " " & $split_1[0] & $sign & ";" & @lf)
				endif
			case "variable"
;~ 				if StringRight($split_2[0],1) = ";" then
;~ 					ConsoleWrite( "variable	= " & $split_2[0] & @lf)
;~ 				Else
					ConsoleWrite( "variable	= " & $type_2 & " " & $split_1[0] & ";" & @lf)
;~ 				endif
			case "enumerator"
;~ 				ConsoleWrite( "UNREGISTER TYPE = " & $type_1 & @lf)
				ConsoleWrite( "enumerator	= " & $split_1[0] & " OF enum = " & $enum & @lf)
			case "struct"
				ConsoleWrite( "struct		= " & $split_1[0] & @lf)
			case Else
				ConsoleWrite( "UNREGISTER TYPE = " & $type_1 & @lf)
		EndSwitch

		ConsoleWrite(@lf)
;~ 		if StringLeft($line, 5) <> "line:" then ConsoleWrite( $line & @lf)

;~ 		IniWrite($outIni, $line, "name", 		$split_1[0])
;~ 		IniWrite($outIni, $line, "def_line", 	$split_2[0])
;~ 		IniWrite($outIni, $line, "type_1", 		$type_1)
;~ 		IniWrite($outIni, $line, "line_nbr",	$fline)
;~ 		IniWrite($outIni, $line, "class", 		$class)
	Next
EndFunc   ;==>_searchFunc_au3
