
#Region ;**** Directives created by AutoIt3Wrapper_GUI ****
#AutoIt3Wrapper_Outfile=ctags_list.exe
#AutoIt3Wrapper_AU3Check_Parameters=-w 3 -w 4 -w 5 -w 6
#EndRegion ;**** Directives created by AutoIt3Wrapper_GUI ****

Opt("MustDeclareVars", 1)

#RequireAdmin
#include <File.au3>
#include <String.au3>
#include <GuiListView.au3>
#include <GUIConstantsEx.au3>
#include <WindowsConstants.au3>
#include <Misc.au3>

#include <E:\programmation_code\Autoit\Autoit_Scripts\Autoit_Scripts_Source\UDF\_ADFunc.au3>
#include <E:\programmation_code\Autoit\Autoit_Scripts\Autoit_Scripts_Source\UDF\ArrayMultiColSort.au3>


Global Const $sc_dragmove = 61458
#cs
−−fields=[+|−]flags



Specifies the available extension fields which are to be included in the entries of the tag file (see TAG FILE FORMAT, below, for more information). The parameter flags is a set of one-letter flags, each representing one type of extension field to include, with the following meanings (disabled by default unless indicated):



a - Access (or export) of class members

f - File-restricted scoping [enabled]

i - Inheritance information

k - Kind of tag as a single letter [enabled]

K - Kind of tag as full name

l - Language of source file containing tag

m - Implementation information

n - Line number of tag definition

s - Scope of tag definition [enabled]

S - Signature of routine (e.g. prototype or parameter list)

z - Include the "kind:" key in kind field

t - Type and name of a variable or typedef as "typeref:" field [enabled]


C++
    c  classes
    d  macro definitions
    e  enumerators (values inside an enumeration)
    f  function definitions
    g  enumeration names
    l  local variables [off]
    m  class, struct, and union members
    n  namespaces
    p  function prototypes [off]
    s  structure names
    t  typedefs
    u  union names
    v  variable definitions
    x  external and forward variable declarations [off]
#ce
;~ #RequireAdmin

	Global $fp_ctag  	= @UserProfileDir & "\.adriProject_autoit\arduinoProjectManager\ctags58\ctags.exe"
	Global $fo_ctag  	= @UserProfileDir & "\.adriProject_autoit\arduinoProjectManager\ctags58\"
	Global $fp_ctagOut 	= @UserProfileDir & "\.adriProject_autoit\arduinoProjectManager\ctags58\tags"
	Global $fp_fileRead

Global $cpp_getTags_array[0][9]
Global $cpp_getTags_fp = ""
Global $aSortData[][] = [ _
    [4, "class,struct,namespace,member,function,enum,enumerator"], _
    [6, 1]]

_SingletonFunc()

Global Const $gui_1 = GUICreate("ctags_list", 480, 355, -1,-1, bitor($ws_popup,  $WS_MINIMIZEBOX  ))
Global Const $pDev_lv_ctags = GUICtrlCreateListView("", 0, 10, 480, 	320, BitOR($LVS_SHOWSELALWAYS, $LVS_SINGLESEL, $LVS_NOSORTHEADER, $LVS_REPORT))
_GUICtrlListView_AddColumn($pDev_lv_ctags, "line", 30)
_GUICtrlListView_AddColumn($pDev_lv_ctags, "name", 150)
_GUICtrlListView_AddColumn($pDev_lv_ctags, "type", 125)
_GUICtrlListView_AddColumn($pDev_lv_ctags, "type", 60)
_GUICtrlListView_AddColumn($pDev_lv_ctags, "from", 250)
Global Const $inp_serach = GUICtrlCreateinput("", 5, 335, 120,15)
Global Const $bt_minimize = GUICtrlCreateButton("-", 130, 335, 30,15)
GUISetCursor(0)
GUISetState(@SW_SHOW)
WinSetTrans($gui_1, "", 200)
WinSetOnTop($gui_1, "", 1)
GUIRegisterMsg($WM_NOTIFY, "_WM_NOTIFY")
GUIRegisterMsg($wm_lbuttondown, "_Wm_lbuttondown")
; iniwrite(@ScriptDir & "\ctags_list.ini", "run", "single", "")
Global $currInputGit = GUICtrlRead($inp_serach)
;~ 		_cpp_getTags($fp_ctagOut,  "C:\Users\adri\Documents\Arduino\libraries\ADRIIOT\src\adriiot_main.h",  @ScriptDir & "\tags.ini")
;~ 		_ArrayMultiColSort($cpp_getTags_array, $aSortData)
;~ 		_ArrayDisplay($cpp_getTags_array)

$fp_fileRead = "E:\programmation_code\Arduino\Arduino_Projet\_projet\ALS_effectSystem\lib\ALS\effect\effectTools.cpp"
;~ 			Redim $cpp_getTags_array[0][9]
;~ 			_cpp_getTags($fp_ctagOut,  IniRead(@ScriptDir & "\ctags_list.ini", "run", "path", ""),  @ScriptDir & "\tags.ini")
;~ 			Global $aSortData[][] = [ _
;~ 				[4, "class,struct,namespace,member,function,enum,enumerator"], _
;~ 				[6, 1]]

;~ 			_ArrayMultiColSort($cpp_getTags_array, $aSortData)
;~ 			_pDev_lv_ctags_updata()

_runcTag($fp_fileRead)

; _cmdLine()
_mainLoop()

func _runcTag($_fpSrc)
	Local $sDrive = "", $sDir = "", $sFileName = "", $sExtension = ""
	Local $aPathSplit = _PathSplit($_fpSrc, $sDrive, $sDir, $sFileName, $sExtension)
;~ 	_ArrayDisplay($aPathSplit, "_PathSplit of " & @ScriptFullPath)
	DirRemove(@ScriptDir & '\ctagout\')
	DirCreate(@ScriptDir & '\ctagout\')
	if $sExtension = ".h" Then
		if FileExists($sDrive& $sDir& $sFileName & ".cpp") Then
				Redim $cpp_getTags_array[0][9]

				_cpp_getTags($fp_ctagOut,  $sDrive& $sDir& $sFileName & ".cpp",  @ScriptDir & "\tags.ini")
				Global $aSortData[][] = [ _
					[4, "class,struct,namespace,member,function,enum,enumerator"], _
					[6, 1]]

				_ArrayMultiColSort($cpp_getTags_array, $aSortData)
				_pDev_iw($sFileName & ".cpp")
		EndIf
	EndIf
	if $sExtension = ".cpp" Then
		if FileExists($sDrive& $sDir& $sFileName & ".h") Then
				Redim $cpp_getTags_array[0][9]
				_cpp_getTags($fp_ctagOut,  $sDrive& $sDir& $sFileName & ".h",  @ScriptDir & "\tags.ini")
				Global $aSortData[][] = [ _
					[4, "class,struct,namespace,member,function,enum,enumerator"], _
					[6, 1]]
				_ArrayMultiColSort($cpp_getTags_array, $aSortData)
				_pDev_iw($sFileName & ".h")
		EndIf
	EndIf

				Redim $cpp_getTags_array[0][9]
				_cpp_getTags($fp_ctagOut,  $_fpSrc,  @ScriptDir & "\tags.ini")
				Global $aSortData[][] = [ _
					[4, "class,struct,namespace,member,function,enum,enumerator"], _
					[6, 1]]
				_ArrayMultiColSort($cpp_getTags_array, $aSortData)
				_pDev_iw($sFileName & $sExtension)
				_pDev_lv_ctags_updata()


EndFunc
Func _pDev_iw($fName)
	; _GUICtrlListView_DeleteAllItems($pDev_lv_ctags)
	local $_fw = @ScriptDir & '\ctagout\'

	$_fw = $_fw & $fName

	For $i = 0 To UBound($cpp_getTags_array) - 1
		; local $index = _GUICtrlListView_AddItem($pDev_lv_ctags, $cpp_getTags_array[$i][5])
		; _GUICtrlListView_AddSubItem($pDev_lv_ctags, $index, $cpp_getTags_array[$i][0], 1)
		; _GUICtrlListView_AddSubItem($pDev_lv_ctags, $index, $cpp_getTags_array[$i][2], 2)
		; _GUICtrlListView_AddSubItem($pDev_lv_ctags, $index, $cpp_getTags_array[$i][3], 3)
		IniWrite($_fw, $cpp_getTags_array[$i][5], "line", 	$cpp_getTags_array[$i][5])
		IniWrite($_fw, $cpp_getTags_array[$i][5], "name", 	$cpp_getTags_array[$i][0])
		IniWrite($_fw, $cpp_getTags_array[$i][5], "type_1", $cpp_getTags_array[$i][2])
		IniWrite($_fw, $cpp_getTags_array[$i][5], "type_2", $cpp_getTags_array[$i][3])
		Switch $cpp_getTags_array[$i][3]
			Case "member"
;~ 				if $cpp_getTags_array[$i][6] 	<> "" then _GUICtrlListView_AddSubItem($pDev_lv_ctags, $index, $cpp_getTags_array[$i][6], 4)
;~ 				if $cpp_getTags_array[$i][9] 	<> "" then _GUICtrlListView_AddSubItem($pDev_lv_ctags, $index, $cpp_getTags_array[$i][9], 4)
;~ 				if $cpp_getTags_array[$i][10] 	<> "" then _GUICtrlListView_AddSubItem($pDev_lv_ctags, $index, $cpp_getTags_array[$i][10], 4)

				if $cpp_getTags_array[$i][6] 	<> "" then IniWrite($_fw, $cpp_getTags_array[$i][5], "from", 	$cpp_getTags_array[$i][6])
				if $cpp_getTags_array[$i][9] 	<> "" then IniWrite($_fw, $cpp_getTags_array[$i][5], "from", 	$cpp_getTags_array[$i][9])
				if $cpp_getTags_array[$i][10] 	<> "" then IniWrite($_fw, $cpp_getTags_array[$i][5], "from", 	$cpp_getTags_array[$i][10])
			case "function"
				; _GUICtrlListView_AddSubItem($pDev_lv_ctags, $index, $cpp_getTags_array[$i][6], 4)
				IniWrite($_fw, $cpp_getTags_array[$i][5], "from", 	$cpp_getTags_array[$i][6])
			case "variable"
			case "enumerator"
				; _GUICtrlListView_AddSubItem($pDev_lv_ctags, $index, $cpp_getTags_array[$i][8], 4)
				IniWrite($_fw, $cpp_getTags_array[$i][5], "from", 	$cpp_getTags_array[$i][8])
			case "struct"
			case Else
		EndSwitch
	Next
endfunc
func _mainLoop()
	While 1
		local $nMsg = GUIGetMsg()
		Switch $nMsg
			Case $bt_minimize
				GUISetState(@SW_MINIMIZE)
			Case $GUI_EVENT_CLOSE
				Exit
			case else
				Local $currRead_2 = GUICtrlRead($inp_serach)
				if $currRead_2 <> $currInputGit Then
					$currInputGit = $currRead_2
					if $currRead_2 <> "" then
						_pDev_lv_pj_filter()
					else
						_pDev_lv_ctags_updata()
					endif
				EndIf
		EndSwitch

		if IniRead(@ScriptDir & "\ctags_list.ini", "run", "single", "") = 1 Then
			iniwrite(@ScriptDir & "\ctags_list.ini", "run", "single", "")

			Redim $cpp_getTags_array[0][9]
			$fp_fileRead = IniRead(@ScriptDir & "\ctags_list.ini", "run", "path", "")
			_cpp_getTags($fp_ctagOut,  IniRead(@ScriptDir & "\ctags_list.ini", "run", "path", ""),  @ScriptDir & "\tags.ini")
			Global $aSortData[][] = [ _
				[4, "class,struct,namespace,member,function,enum,enumerator"], _
				[6, 1]]

			_ArrayMultiColSort($cpp_getTags_array, $aSortData)
			_pDev_lv_ctags_updata()

		EndIf
	WEnd
EndFunc
func _SingletonFunc()
	If _Singleton("ctags_list", 1) = 0 Then
		local $str = ""
		for $i = 1 to UBound($CmdLine) - 1
			$str &= $CmdLine[$i] & " "
		Next
		iniwrite(@ScriptDir & "\ctags_list.ini", "run", "path", _StringBetween($str, "-path[", "]")[0] )
		iniwrite(@ScriptDir & "\ctags_list.ini", "run", "single", 1)
		Exit
	endif
EndFunc
func _cmdLine()
	If UBound($CmdLine) > 1 Then
		local $str = ""
		for $i = 1 to UBound($CmdLine) - 1
			$str &= $CmdLine[$i] & " "
		Next

		Redim $cpp_getTags_array[0][9]
		_cpp_getTags($fp_ctagOut,  _StringBetween($str, "-path[", "]")[0],  @ScriptDir & "\tags.ini")
		Global $aSortData[][] = [ _
			[4, "class,struct,namespace,member,function,enum,enumerator"], _
			[6, 1]]

		_ArrayMultiColSort($cpp_getTags_array, $aSortData)
		_pDev_lv_ctags_updata()

	Else
		Exit
	EndIf
EndFunc
func _pDev_lv_pj_filter()

	_GUICtrlListView_BeginUpdate($pDev_lv_ctags)
	_GUICtrlListView_DeleteAllItems($pDev_lv_ctags)
;~ 	Local $sDataCmb = IniReadSectionNames($pDev_fp_ini_libs)
	For $i = 0 To UBound($cpp_getTags_array) - 1

		if (_pDev_Checkword(GUICtrlRead($inp_serach), $cpp_getTags_array[$i][0]) >= 3) then

			local $index = _GUICtrlListView_AddItem($pDev_lv_ctags, $cpp_getTags_array[$i][5])
			_GUICtrlListView_AddSubItem($pDev_lv_ctags, $index, $cpp_getTags_array[$i][0], 1)
			_GUICtrlListView_AddSubItem($pDev_lv_ctags, $index, $cpp_getTags_array[$i][2], 2)
			_GUICtrlListView_AddSubItem($pDev_lv_ctags, $index, $cpp_getTags_array[$i][3], 3)

			Switch $cpp_getTags_array[$i][3]
				Case "member"
					if $cpp_getTags_array[$i][6] 	<> "" then _GUICtrlListView_AddSubItem($pDev_lv_ctags, $index, $cpp_getTags_array[$i][6], 4)
					if $cpp_getTags_array[$i][9] 	<> "" then _GUICtrlListView_AddSubItem($pDev_lv_ctags, $index, $cpp_getTags_array[$i][9], 4)
					if $cpp_getTags_array[$i][10] 	<> "" then _GUICtrlListView_AddSubItem($pDev_lv_ctags, $index, $cpp_getTags_array[$i][10], 4)
				case "function"
					_GUICtrlListView_AddSubItem($pDev_lv_ctags, $index, $cpp_getTags_array[$i][6], 4)
				case "variable"
				case "enumerator"
					_GUICtrlListView_AddSubItem($pDev_lv_ctags, $index, $cpp_getTags_array[$i][8], 4)
				case "struct"
				case Else
			EndSwitch
		endif

	Next
	_GUICtrlListView_EndUpdate($pDev_lv_ctags)
EndFunc
Func _pDev_Checkword($sWord, $sValidChars)
    Local $sChar
	Local $cnt
    For $i = 1 To StringLen($sWord)
        $sChar = StringMid($sWord, $i, 1)
        If StringInStr($sValidChars, $schar) Then
            $sValidChars = StringReplace($sValidChars, $sChar, "", 1)
			$cnt+=1
        Else
            Return 0
        EndIf
    Next
    Return $cnt
EndFunc

Func _pDev_lv_ctags_updata()
	_GUICtrlListView_DeleteAllItems($pDev_lv_ctags)
	For $i = 0 To UBound($cpp_getTags_array) - 1
		local $index = _GUICtrlListView_AddItem($pDev_lv_ctags, $cpp_getTags_array[$i][5])
		_GUICtrlListView_AddSubItem($pDev_lv_ctags, $index, $cpp_getTags_array[$i][0], 1)
		_GUICtrlListView_AddSubItem($pDev_lv_ctags, $index, $cpp_getTags_array[$i][2], 2)
		_GUICtrlListView_AddSubItem($pDev_lv_ctags, $index, $cpp_getTags_array[$i][3], 3)

		Switch $cpp_getTags_array[$i][3]
			Case "member"
				if $cpp_getTags_array[$i][6] 	<> "" then _GUICtrlListView_AddSubItem($pDev_lv_ctags, $index, $cpp_getTags_array[$i][6], 4)
				if $cpp_getTags_array[$i][9] 	<> "" then _GUICtrlListView_AddSubItem($pDev_lv_ctags, $index, $cpp_getTags_array[$i][9], 4)
				if $cpp_getTags_array[$i][10] 	<> "" then _GUICtrlListView_AddSubItem($pDev_lv_ctags, $index, $cpp_getTags_array[$i][10], 4)
			case "function"
				_GUICtrlListView_AddSubItem($pDev_lv_ctags, $index, $cpp_getTags_array[$i][6], 4)
			case "variable"
			case "enumerator"
				_GUICtrlListView_AddSubItem($pDev_lv_ctags, $index, $cpp_getTags_array[$i][8], 4)
			case "struct"
			case Else
		EndSwitch
	Next
endfunc

Func _cpp_getTags($sFo_source, $include, $outIni)
	redim $cpp_getTags_array[0][9]

	$cpp_getTags_fp = $include

	if FileExists($sFo_source) 	then FileDelete($sFo_source)
	if FileExists($outIni) 		then FileDelete($outIni)

	local $cmd = ' ctags -R --fields=+iakKmnsSt '& '"' & $include & '"'

	RunWait ($fp_ctag & String($cmd), $fo_ctag, @SW_show)

	local $line, $str, $split_1, $split_2, $split_3, $res, $out, $type_1, $class, $enum, $struct, $namespace, $iAsc, $fline, $sign, $access, $type_2, $data, $find

	For $j = 0 To _FileCountLines($sFo_source)

		$line 	= FileReadLine($sFo_source, $j)
		$str 	= StringReplace($line, $include, "")


		ConsoleWrite($line & @crlf)

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
		$type_1 	= ""
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


Func _WM_NOTIFY($hWnd, $iMsg, $wParam, $lParam)
	#forceref $hWnd, $iMsg, $wParam, $lParam
	Local $tNMHDR, $hWndFrom, $iCode
	$tNMHDR = DllStructCreate($tagNMHDR, $lParam)
	$hWndFrom = DllStructGetData($tNMHDR, 'hWndFrom')
	$iCode = DllStructGetData($tNMHDR, 'Code')

	Local $tCustDraw = DllStructCreate($tagNMLVCUSTOMDRAW, $lParam)
	Local $iDrawStage = DllStructGetData($tCustDraw, "dwDrawStage")
	Local $iSubItem = DllStructGetData($tCustDraw, "iSubItem")
	Local $iItem = DllStructGetData($tCustDraw, "dwItemSpec")
	#forceref $iDrawStage, $iSubItem, $iItem, $lParam

	Local $aSelIndices, $aItems, $sName
	#forceref $sName
	Local $hwndLv_ref = GUICtrlGetHandle($pDev_lv_ctags)
	Switch $hWndFrom
		Case $hwndLv_ref
			Switch $iCode

				Case $NM_DBLCLK
					$aSelIndices = _GUICtrlListView_GetSelectedIndices($hwndLv_ref, True)
					If $aSelIndices[0] > 0 Then
						$aItems = $aSelIndices[1]
						$sName = _GUICtrlListView_GetItemText($hwndLv_ref, $aItems, 0)
Run("subl " & '"' & $cpp_getTags_fp & '"' & ":" & _GUICtrlListView_GetItemText($hwndLv_ref, $aItems, 0), "", @SW_HIDE)

					EndIf
			EndSwitch

	EndSwitch
EndFunc


Func _wm_lbuttondown($Form1 = $gui_1)
    _SendMessage($Form1, $wm_syscommand, $sc_dragmove, 0)
EndFunc   ;==