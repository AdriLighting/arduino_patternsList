#include-once
#include <GuiTreeView.au3>
#include <GuiListView.au3>
#include <File.au3>
#include <String.au3>
#include <GuiConstants.au3>
#include <StringSize.au3>

;~ #AutoIt3Wrapper_AU3Check_Parameters=-d -w 1 -w 2 -w 3 -w 4 -w 5 -w 6
;~ Opt('MustDeclareVars', 1)

Func _ADFunc_Files_FileGetFullNameByUrl($sFileUrl)
	Local $aFilename = StringSplit($sFileUrl, '/')
	If Not @error Then Return $aFilename[$aFilename[0]]
EndFunc   ;==>_ADFunc_Files_FileGetFullNameByUrl

Func _ADFunc_Files_GetNewFileName($sFileName, $sDir, $iExt_Mod = -1, $sDelim1 = -1, $sDelim2 = -2)
	If Not FileExists($sDir & "\" & $sFileName) Then Return $sFileName

	$sDir = StringRegExpReplace($sDir, "\\ *$", "")

	Local $sName = StringRegExpReplace($sFileName, "\.[^.]*$", "")
	If $iExt_Mod <> -1 Then $sName = StringRegExpReplace($sFileName, "\.[^.]*(\.[^.]*$|$)", "")

	Local $sExtn = StringMid($sFileName, StringLen($sName) + 1)
	Local $iCount = 1, $sRet_FileName = $sFileName
	While FileExists($sDir & "\" & $sRet_FileName)
		If $sDelim1 = -1 And $sDelim2 = -2 Then
			$sRet_FileName = $sName & " (" & $iCount & ")" & $sExtn
		ElseIf $sDelim1 <> -1 And $sDelim2 <> -2 Then
			$sRet_FileName = $sName & $sDelim1 & $iCount & $sDelim2 & $sExtn
		ElseIf $sDelim1 <> -1 And $sDelim2 = -2 Then
			$sRet_FileName = $sName & $sDelim1 & $iCount & $sExtn
		EndIf
		$iCount += 1
	WEnd

	Return $sRet_FileName
EndFunc   ;==>_ADFunc_Files_GetNewFileName

; #FUNCTION# =================================================
; Name...........: _StringAddThousandsSep
; Description ...: Returns the original numbered string with the Thousands delimiter inserted.
; Syntax.........: _StringAddThousandsSep($sString[, $sThousands = -1[, $sDecimal = -1]])
; Parameters ....: $sString    - The string to be converted.
;                  $sThousands - Optional: The Thousands delimiter
;                  $sDecimal   - Optional: The decimal delimiter
; Return values .: Success - The string with Thousands delimiter added.
; Author ........: SmOke_N (orignal _StringAddComma
; Modified.......: Valik (complete re-write, new function name)
; ===============================================================================================================================
Func _ADFunc_Files_StringAddThousandsSep($sString, $sThousands = -1, $sDecimal = -1)
	If $sString = '-' Then Return $sString ; This Line is only for FileCommander
	Local $sResult = "" ; Force string
	Local $rKey = "HKCU\Control Panel\International"
	If $sDecimal = -1 Then $sDecimal = RegRead($rKey, "sDecimal")
	If $sThousands = -1 Then $sThousands = RegRead($rKey, "sThousand")
	Local $aNumber = StringRegExp($sString, "(\D?\d+)\D?(\d*)", 1) ; This one works for negatives.
	If UBound($aNumber) = 2 Then
		Local $sLeft = $aNumber[0]
		While StringLen($sLeft)
			$sResult = $sThousands & StringRight($sLeft, 3) & $sResult
			$sLeft = StringTrimRight($sLeft, 3)
		WEnd
		$sResult = StringTrimLeft($sResult, StringLen($sThousands)) ; Strip leading thousands separator
		If $aNumber[1] <> "" Then $sResult &= $sDecimal & $aNumber[1]
	EndIf
	Return $sResult
EndFunc   ;==>_ADFunc_Files_StringAddThousandsSep

Func _ADFunc_Files_FileGetParentFolderPath($sFullPath)
	Local $sFilePath = StringLeft($sFullPath, StringInStr($sFullPath, '\', 0, -1) - 1)
	If Not @error Then Return $sFilePath
EndFunc   ;==>_ADFunc_Files_FileGetParentFolderPath

Func _ADFunc_Files_FileGetFullNameByFullPath($sFullPath)
	Local $aFilename = StringSplit($sFullPath, '\')
	If Not @error Then Return $aFilename[$aFilename[0]]
	Return SetError(1)
EndFunc   ;==>_ADFunc_Files_FileGetFullNameByFullPath

Func _ADFunc_Files_GetFileInfo($sfile)
	If Not FileExists($sfile) Then Return SetError(1)

	Local $ret[4]
	Local $sDrive = "", $sDir = "", $sFileName = "", $sExtension = ""
	_PathSplit($sfile, $sDrive, $sDir, $sFileName, $sExtension)
	$ret[0] = $sDrive
	$ret[1] = $sDir
	$ret[2] = $sFileName
	$ret[3] = $sExtension
	Return $ret
EndFunc   ;==>_ADFunc_Files_GetFileInfo

; #FUNCTION# =================================================
; Function Name.....: _FileGetProperty
; Description.......: Returns a property, or all properties, for a file.
; Version...........: 1.0.4
; Change Date.......: 09-03-2017
; AutoIt Version....: 3.3.14.x (due to the use of Static, but it could be a Global and use 3.2.12.x)
; Parameter(s)......: $FGP_Path - String containing the file path to return the property from.
;                     $FGP_PROPERTY - [optional] String containing the name of the property to return. (default = "")
;                     $iPropertyCount - [optional] The number of properties to search through for $FGP_PROPERTY, or the number of items
;                                       returned in the array if $FGP_PROPERTY is blank. (default = 500)
; Requirements(s)...: None
; Return Value(s)...: Success: Returns a string containing the property value.
;                     If $FGP_PROPERTY is blank, a two-dimensional array is returned:
;                         $av_array[0][0] = Number of properties.
;                         $av_array[1][0] = 1st property name.
;                         $as_array[1][1] = 1st property value.
;                         $av_array[n][0] = nth property name.
;                         $as_array[n][1] = nth property value.
;                     Failure: Returns an empty string and sets @error to:
;                       1 = The folder $FGP_Path does not exist.
;                       2 = The property $FGP_PROPERTY does not exist or the array could not be created.
;                       3 = Unable to create the "Shell.Application" object $objShell.
; Author(s).........: - Simucal <Simucal@gmail.com>
;                     - Modified by: Sean Hart <autoit@hartmail.ca>
;                     - Modified by: teh_hahn <sPiTsHiT@gmx.de>
;                     - Modified by: BrewManNH
;                     - Modified by: argumentum ; added some optimization, fixed Win10 issue
; URL...............: https://www.autoitscript.com/forum/topic/148232-_filegetproperty-retrieves-the-properties-of-a-file/?do=findComment&comment=1364968
; Note(s)...........: Modified the script that teh_hahn posted at the above link to include the properties that
;                     Vista and Win 7 include that Windows XP doesn't. Also removed the ReDims for the $av_ret array and
;                     replaced it with a single ReDim after it has found all the properties, this should speed things up.
;                     I further updated the code so there's a single point of return except for any errors encountered.
;                     $iPropertyCount is now a function parameter instead of being hardcoded in the function itself.
;                     Added the use of $FGP_PROPERTY as Index + 1 ( as is shown the array ), in additon to $FGP_PROPERTY as Verb
;                     Added the array Index to the @extended, as this the optimization is for just te last index used.
;                     Fixed array chop short on ReDim ( Win10 issue )
;===============================================================================
Func _ADFunc_Files_GetProperty($FGP_Path, $FGP_PROPERTY = "", $iPropertyCount = 500)
	If $FGP_PROPERTY = Default Then $FGP_PROPERTY = ""
	$FGP_Path = StringRegExpReplace($FGP_Path, '["'']', "") ; strip the quotes, if any from the incoming string
	If Not FileExists($FGP_Path) Then Return SetError(1, 0, "") ; path not found
	Local Const $objShell = ObjCreate("Shell.Application")
	If @error Then Return SetError(3, 0, "")
	Local Const $FGP_File = StringTrimLeft($FGP_Path, StringInStr($FGP_Path, "\", 0, -1))
	Local Const $FGP_Dir = StringTrimRight($FGP_Path, StringLen($FGP_File) + 1)
	Local Const $objFolder = $objShell.NameSpace($FGP_Dir)
	Local Const $objFolderItem = $objFolder.Parsename($FGP_File)
	Local $Return = "", $iError = 0, $iExtended = 0, $iLastValue = 0
	Local Static $FGP_PROPERTY_Text = "", $FGP_PROPERTY_Index = 0
	If $FGP_PROPERTY_Text = $FGP_PROPERTY And $FGP_PROPERTY_Index Then
		If $objFolder.GetDetailsOf($objFolder.Items, $FGP_PROPERTY_Index) = $FGP_PROPERTY Then
			Return SetError(0, $FGP_PROPERTY_Index, $objFolder.GetDetailsOf($objFolderItem, $FGP_PROPERTY_Index))
		EndIf
	EndIf
	If Int($FGP_PROPERTY) Then
		$Return = $objFolder.GetDetailsOf($objFolderItem, $FGP_PROPERTY - 1)
		If $Return = "" Then
			$iError = 2
		EndIf
	ElseIf $FGP_PROPERTY Then
		For $i = 0 To $iPropertyCount
			If $objFolder.GetDetailsOf($objFolder.Items, $i) = $FGP_PROPERTY Then
				$FGP_PROPERTY_Text = $FGP_PROPERTY
				$FGP_PROPERTY_Index = $i
				$iExtended = $i
				$Return = $objFolder.GetDetailsOf($objFolderItem, $i)
			EndIf
		Next
		If $Return = "" Then
			$iError = 2
		EndIf
	Else
		Local $av_ret[$iPropertyCount + 1][2]
		For $i = 1 To $iPropertyCount
			If $objFolder.GetDetailsOf($objFolder.Items, $i) Then
				$av_ret[$i][0] = $objFolder.GetDetailsOf($objFolder.Items, $i - 1)
				$av_ret[$i][1] = $objFolder.GetDetailsOf($objFolderItem, $i - 1)
				If $av_ret[$i][0] Then $iLastValue = $i
			EndIf
		Next
		ReDim $av_ret[$iLastValue + 1][2]
		$av_ret[0][0] = $iLastValue
		If Not $av_ret[1][0] Then
			$iError = 2
			$av_ret = $Return
		Else
			$Return = $av_ret
		EndIf
	EndIf
	Return SetError($iError, $iExtended, $Return)
EndFunc   ;==>_ADFunc_Files_GetProperty

; #FUNCTION# =================================================
; Function Name:	GetExtProperty($sPath,$iProp)
; Description:      Returns an extended property of a given file.
; Parameter(s):     $sPath - The path to the file you are attempting to retrieve an extended property from.
;                   $iProp - The numerical value for the property you want returned. If $iProp is is set
;							  to -1 then all properties will be returned in a 1 dimensional array in their corresponding order.
;							The properties are as follows:
;							Name = 0
;							Size = 1
;							Type = 2
;							DateModified = 3
;							DateCreated = 4
;							DateAccessed = 5
;							Attributes = 6
;							Status = 7
;							Owner = 8
;							Author = 9
;							Title = 10
;							Subject = 11
;							Category = 12
;							Pages = 13
;							Comments = 14
;							Copyright = 15
;							Artist = 16
;							AlbumTitle = 17
;							Year = 18
;							TrackNumber = 19
;							Genre = 20
;							Duration = 21
;							BitRate = 22
;							Protected = 23
;							CameraModel = 24
;							DatePictureTaken = 25
;							Dimensions = 26
;							Width = 27
;							Height = 28
;							Company = 30
;							Description = 31
;							FileVersion = 32
;							ProductName = 33
;							ProductVersion = 34
; Requirement(s):   File specified in $spath must exist.
; Return Value(s):  On Success - The extended file property, or if $iProp = -1 then an array with all properties
;                   On Failure - 0, @Error - 1 (If file does not exist)
; Author(s):        Simucal (Simucal@gmail.com)
; Note(s):
;
;===============================================================================
Func _ADFunc_Files_GetExtProperty($sPath, $iProp)
	Local $iExist, $sfile, $sDir, $oShellApp, $oDir, $oFile, $aProperty, $sProperty
	$iExist = FileExists($sPath)
	If $iExist = 0 Then
		SetError(1)
		Return 0
	Else
		$sfile = StringTrimLeft($sPath, StringInStr($sPath, "\", 0, -1))
		$sDir = StringTrimRight($sPath, (StringLen($sPath) - StringInStr($sPath, "\", 0, -1)))
		$oShellApp = ObjCreate("shell.application")
		$oDir = $oShellApp.NameSpace($sDir)
		$oFile = $oDir.Parsename($sfile)
		If $iProp = -1 Then
			Local $aProperty[35]
			For $i = 0 To 34
				$aProperty[$i] = $oDir.GetDetailsOf($oFile, $i)
			Next
			Return $aProperty
		Else
			$sProperty = $oDir.GetDetailsOf($oFile, $iProp)
			If $sProperty = "" Then
				Return 0
			Else
				Return $sProperty
			EndIf
		EndIf
	EndIf
EndFunc   ;==>_ADFunc_Files_GetExtProperty

Func _ADFunc_Files_EditFolder($file, $hParentGui = '')
;~ 	If Not FileExists($file) Then Return

	Local $sDrive = "", $sDir = "", $sFileName = "", $sExtension = ""
	_PathSplit($file, $sDrive, $sDir, $sFileName, $sExtension)

	Local $CFF_EditFileOrFolder_Gui = GUICreate("_CFF_EditFolder", 718, 745, -1, -1, -1, -1, $hParentGui)
	GUISetBkColor(0xD3D3D3)
	GUICtrlCreateGroup("", 16, 8, 689, 73)
	Local $CFF_EditFileOrFolder_Lbl1 = GUICtrlCreateLabel('', 24, 32, 676, 33, BitOR($SS_CENTER, $bs_multiline))
	GUICtrlSetData($CFF_EditFileOrFolder_Lbl1, $file)

	Local $ADFunc_Files_EditFile_Lv = GUICtrlCreateListView("Prop|Val", 16, 88, 690, 530)
	_GUICtrlListView_SetColumnWidth($ADFunc_Files_EditFile_Lv, 0, 150)
	_GUICtrlListView_SetColumnWidth($ADFunc_Files_EditFile_Lv, 1, 500)

	GUICtrlCreateListViewItem('Relative Path' & '|' & $sDrive & $sDir, $ADFunc_Files_EditFile_Lv)
	GUICtrlCreateListViewItem('FileName' & '|' & $sFileName, $ADFunc_Files_EditFile_Lv)
	GUICtrlCreateListViewItem('Taille' & '|' & _ADFunc_File_ByteSuffix(DirGetSize($file)), $ADFunc_Files_EditFile_Lv)
	GUICtrlCreateListViewItem('' & '|' & '', $ADFunc_Files_EditFile_Lv)

	Local $FileGetProperty = _ADFunc_Files_GetProperty($file, "", 500)
	For $i = 1 To UBound($FileGetProperty) - 1
		If $FileGetProperty[$i][1] = '' Then ContinueLoop
		If $FileGetProperty[$i][0] = '' Then ContinueLoop
		GUICtrlCreateListViewItem($FileGetProperty[$i][0] & '|' & $FileGetProperty[$i][1], $ADFunc_Files_EditFile_Lv)
	Next

	Local $CFF_EditFileOrFolder_Cmb = GUICtrlCreateCombo("", 16, 640, 690, 25, BitOR($gui_ss_default_combo, $cbs_dropdownlist, $ws_hscroll))
	GUICtrlSetData(-1, "Rennomer|Supprimer", "")
	GUICtrlSetCursor(-1, 0)
	Local $CFF_EditFileOrFolder_Inp = GUICtrlCreateInput("", 16, 670, 689, 21)
	GUICtrlSetState(-1, 128)
	Local $CFF_EditFileOrFolder_Btn = GUICtrlCreateButton("Valider", 16, 710, 690, 25)
	GUICtrlSetCursor(-1, 0)

	GUISetState(@SW_SHOW, $CFF_EditFileOrFolder_Gui)
	If $hParentGui <> '' Then GUISetState(@SW_DISABLE, $hParentGui)

	Local $nMsg
	Local $SetError = ''
	While 1
		$nMsg = GUIGetMsg()
		Switch $nMsg
			Case $CFF_EditFileOrFolder_Btn
				Switch GUICtrlRead($CFF_EditFileOrFolder_Cmb)
					Case "Rennomer"
						If _ADFunc_Gui_MsgBox(True, '', '_ADFunc_Files_EditFolder', 'Rennomer', 'Rennomer : ' & @CRLF & $file & @CRLF & 'ver : ' & @CRLF & $sDrive & $sDir & GUICtrlRead($CFF_EditFileOrFolder_Inp) & $sExtension, 'Oui', 'Non') = 1 Then
							DirMove($file, $sDrive & $sDir & GUICtrlRead($CFF_EditFileOrFolder_Inp) & $sExtension, 1)
							ExitLoop
						EndIf
					Case "Supprimer"
						If _ADFunc_Gui_MsgBox(True, '', '_ADFunc_Files_EditFolder', 'Supprimer', 'Supprimer : ' & @CRLF & $file, 'Oui', 'Non') = 1 Then
							DirRemove($file, 1)
							ExitLoop
						EndIf
					Case Else
						ExitLoop
				EndSwitch
			Case $CFF_EditFileOrFolder_Cmb
				Switch GUICtrlRead($CFF_EditFileOrFolder_Cmb)
					Case "Rennomer"
						GUICtrlSetState($CFF_EditFileOrFolder_Inp, 64)
						GUICtrlSetData($CFF_EditFileOrFolder_Inp, $sFileName)
					Case "Supprimer"
						GUICtrlSetState($CFF_EditFileOrFolder_Inp, 128)
						GUICtrlSetData($CFF_EditFileOrFolder_Inp, '')
				EndSwitch
			Case $GUI_EVENT_CLOSE
				$SetError = 1
				ExitLoop
		EndSwitch
	WEnd
	GUIDelete($CFF_EditFileOrFolder_Gui)
	If $hParentGui <> '' Then
		GUISetState(@SW_ENABLE, $hParentGui)
		GUISetState(@SW_RESTORE, $hParentGui)
	EndIf
	If $SetError <> '' Then Return SetError($SetError)
EndFunc   ;==>_ADFunc_Files_EditFolder

Func _ADFunc_Files_EditFile($file, $hParentGui = '')
	If Not FileExists($file) Then Return

	Local $sDrive = "", $sDir = "", $sFileName = "", $sExtension = ""
	_PathSplit($file, $sDrive, $sDir, $sFileName, $sExtension)

	Local $ADFunc_Files_EditFile_Gui = GUICreate("_CFF_EditFile", 718, 745, -1, -1, -1, -1, $hParentGui)
	GUISetBkColor(0xD3D3D3)
	GUICtrlCreateGroup("", 16, 8, 689, 73)
	Local $ADFunc_Files_EditFile_Lbl1 = GUICtrlCreateLabel('', 24, 32, 676, 33, BitOR($SS_CENTER, $bs_multiline))
	GUICtrlSetData($ADFunc_Files_EditFile_Lbl1, $file)
	Local $ADFunc_Files_EditFile_Lv = GUICtrlCreateListView("Prop|Val", 16, 88, 690, 530)
	_GUICtrlListView_SetColumnWidth($ADFunc_Files_EditFile_Lv, 0, 150)
	_GUICtrlListView_SetColumnWidth($ADFunc_Files_EditFile_Lv, 1, 500)
	Local $ADFunc_Files_EditFile_Cmb = GUICtrlCreateCombo("", 16, 640, 690, 25, BitOR($gui_ss_default_combo, $cbs_dropdownlist, $ws_hscroll))
	GUICtrlSetData(-1, "Rennomer|Supprimer", "")
	GUICtrlSetCursor(-1, 0)
	Local $ADFunc_Files_EditFile_Inp = GUICtrlCreateInput("", 16, 670, 689, 21)
	GUICtrlSetState(-1, 128)
	Local $ADFunc_Files_EditFile_Btn = GUICtrlCreateButton("Valider", 16, 710, 690, 25)
	GUICtrlSetCursor(-1, 0)

	If $hParentGui <> '' Then GUISetState(@SW_DISABLE, $hParentGui)

	GUICtrlCreateListViewItem('Relative Path' & '|' & $sDrive & $sDir, $ADFunc_Files_EditFile_Lv)
	GUICtrlCreateListViewItem('FileName' & '|' & $sFileName, $ADFunc_Files_EditFile_Lv)
	GUICtrlCreateListViewItem('Extension' & '|' & $sExtension, $ADFunc_Files_EditFile_Lv)
	GUICtrlCreateListViewItem('Taille' & '|' & _ADFunc_File_ByteSuffix(FileGetSize($file)), $ADFunc_Files_EditFile_Lv)
	GUICtrlCreateListViewItem('' & '|' & '', $ADFunc_Files_EditFile_Lv)

	Local $FileGetProperty = _ADFunc_Files_GetProperty($file, "", 500)
	For $i = 1 To UBound($FileGetProperty) - 1
		If $FileGetProperty[$i][1] = '' Then ContinueLoop
		If $FileGetProperty[$i][0] = '' Then ContinueLoop
		GUICtrlCreateListViewItem($FileGetProperty[$i][0] & '|' & $FileGetProperty[$i][1], $ADFunc_Files_EditFile_Lv)
	Next

	GUISetState(@SW_SHOW, $ADFunc_Files_EditFile_Gui)

	Local $nMsg
	Local $SetError = ''
	While 1
		$nMsg = GUIGetMsg(1)
		Switch $nMsg[0]
			Case $ADFunc_Files_EditFile_Btn
				Switch GUICtrlRead($ADFunc_Files_EditFile_Cmb)
					Case "Rennomer"
						If _ADFunc_Gui_MsgBox(True, '', '_ADFunc_Files_EditFile', 'Rennomer', 'Rennomer : ' & @CRLF & $file & @CRLF & 'ver : ' & @CRLF & $sDrive & $sDir & GUICtrlRead($ADFunc_Files_EditFile_Inp) & $sExtension, 'Oui', 'Non') = 1 Then
							FileMove($file, $sDrive & $sDir & GUICtrlRead($ADFunc_Files_EditFile_Inp) & $sExtension, 1)
							ExitLoop
						EndIf
					Case "Supprimer"
						If _ADFunc_Gui_MsgBox(True, '', '_ADFunc_Files_EditFile', 'Supprimer', 'Supprimer : ' & @CRLF & $file, 'Oui', 'Non') = 1 Then
							FileDelete($file)
							ExitLoop
						EndIf
					Case Else
						ExitLoop
				EndSwitch
			Case $ADFunc_Files_EditFile_Cmb
				Switch GUICtrlRead($ADFunc_Files_EditFile_Cmb)
					Case "Rennomer"
						GUICtrlSetState($ADFunc_Files_EditFile_Inp, 64)
						GUICtrlSetData($ADFunc_Files_EditFile_Inp, $sFileName)
					Case "Supprimer"
						GUICtrlSetState($ADFunc_Files_EditFile_Inp, 128)
						GUICtrlSetData($ADFunc_Files_EditFile_Inp, '')
				EndSwitch
			Case $GUI_EVENT_CLOSE
				$SetError = 1
				ExitLoop
		EndSwitch
	WEnd

	GUIDelete($ADFunc_Files_EditFile_Gui)
	If $hParentGui <> '' Then
		GUISetState(@SW_ENABLE, $hParentGui)
		GUISetState(@SW_RESTORE, $hParentGui)
	EndIf
	If $SetError <> '' Then Return SetError($SetError)
EndFunc   ;==>_ADFunc_Files_EditFile

Func _ADFunc_File_ByteSuffix($iBytes)
	Local $iIndex = 0, $aArray = [' bytes', ' KB', ' MB', ' GB', ' TB', ' PB', ' EB', ' ZB', ' YB']
	While $iBytes > 1023
		$iIndex += 1
		$iBytes /= 1024
	WEnd
	Return Round($iBytes) & $aArray[$iIndex]
EndFunc   ;==>_ADFunc_File_ByteSuffix


Func _ADFunc_String_StringIsNumber($v_num)
	Return (Int(StringIsInt($v_num)) + Int(StringIsFloat($v_num)) > 0)
EndFunc   ;==>_ADFunc_String_StringIsNumber

Func _ADFunc_String_StringExtractNumber_1($string)
	Local $aValues = StringRegExp($string, "(?U)(?<=\A|\D)(\d+)[\Z\D]", 3)
	If UBound($aValues) - 1 > 0 Then
		Return $aValues[0]
	Else
		Return SetError(1)
	EndIf
EndFunc   ;==>_ADFunc_String_StringExtractNumber_1

Func _ADFunc_String_StringExtractNumber_2($string)
	Local $aValues = StringRegExp($string, '\d+', 1)
	If UBound($aValues) - 1 > 0 Then
		Return $aValues[0]
	Else
		Return SetError(1)
	EndIf
EndFunc   ;==>_ADFunc_String_StringExtractNumber_2


Func _ADFunc_Inet_InetGet($sUrl, $sOutPath, $sTitle = '', $sProgress = False)
	Local $pourcentage, $totalsize, $kbrecu, $hDownload = InetGet($sUrl, $sOutPath, 1, 1)
	If $sTitle == '' Then $sTitle = _ADFunc_Files_FileGetFullNameByUrl($sUrl)
	If $sProgress Then ProgressOn("Telechargment en cour", "", "0%")
	Do
		$totalsize = 0
		$kbrecu = InetGetInfo($hDownload, 0)
		If $kbrecu > 0 Then
			If $totalsize = 0 Then
				$totalsize = InetGetInfo($hDownload, 1)
			EndIf
			$pourcentage = Round($kbrecu * 100 / $totalsize)
			If $sProgress Then ProgressSet($pourcentage, $pourcentage & '%', $sTitle)
		EndIf
		Sleep(50)
	Until InetGetInfo($hDownload, 2)
	If $sProgress Then ProgressOff()
	InetClose($hDownload)
EndFunc   ;==>_ADFunc_Inet_InetGet

Func _ADFunc_Inet_InternetIsAvailable()
	If Ping('www.bing.com', 1) Or InetRead('http://www.google.com/humans.txt', 19) Then
		Return True
	Else
		Return False
	EndIf
EndFunc   ;==>_ADFunc_Inet_InternetIsAvailable


Func _ADFunc_Treeview_FindChild($hWnd, $sPath, $hChild = 0)
	$hChild = _GUICtrlTreeView_GetFirstChild($hWnd, $hChild)
	If _GUICtrlTreeView_GetText($hWnd, $hChild) = $sPath Then Return $hChild
	Do
		$hChild = _GUICtrlTreeView_GetNextChild($hWnd, $hChild)
;~ 		If _GUICtrlTreeView_GetText($hWnd, $hChild) = $sPath Then Return $hChild
	Until Not $hChild
	Return 0
EndFunc   ;==>_ADFunc_Treeview_FindChild

Func _ADFunc_Treeview_ReadFolder($hFolder, $hItem, $sPath)
	Local Const $sDelim = '\'
	Local $hSearch, $sfile
	$hSearch = FileFindFirstFile($sPath & $sDelim & '*')
	If $hSearch = -1 Then Return True
	Do
		$sfile = FileFindNextFile($hSearch)
		If @error Then ExitLoop
		If @extended And Not _ADFunc_Treeview_IsReparsePoint($sPath & $sDelim & $sfile) Then _GUICtrlTreeView_AddChild($hFolder, $hItem, $sfile, 0, 1)
	Until False
	FileClose($hSearch)
EndFunc   ;==>_ADFunc_Treeview_ReadFolder

Func _ADFunc_Treeview_IsReparsePoint($FLS) ; coded by progandy
	Local $DA = DllCall('kernel32.dll', 'dword', 'GetFileAttributesW', 'wstr', $FLS)
	If @error Then Return SetError(1, @error, False)
	Return BitAND($DA[0], 1024) = 1024
EndFunc   ;==>_ADFunc_Treeview_IsReparsePoint

Func _ADFunc_Treeview_GuiCtrlSetPath($nID, $sPath, $iFit = 0)
	; coded by funkey
	; 2011, Nov 24th
	Local $hCtrl = GUICtrlGetHandle($nID)
	Local $hDC = _WinAPI_GetDC($hCtrl)
	Local $tPath = DllStructCreate("char[260]")
	Local $pPath = DllStructGetPtr($tPath)
	DllStructSetData($tPath, 1, $sPath)
	Local $hFont = _SendMessage($hCtrl, 49, 0, 0, 0, "wparam", "lparam", "hwnd")
	Local $hFont_old = _WinAPI_SelectObject($hDC, $hFont)
	Local $aPos = ControlGetPos($hCtrl, "", "")
	DllCall("Shlwapi.dll", "BOOL", "PathCompactPath", "handle", $hDC, "ptr", $pPath, "int", $aPos[2] - $iFit)
	_WinAPI_SelectObject($hDC, $hFont_old)
	_WinAPI_DeleteDC($hDC)
	GUICtrlSetData($nID, DllStructGetData($tPath, 1))
	Return DllStructGetData($tPath, 1)
EndFunc   ;==>_ADFunc_Treeview_GuiCtrlSetPath

Func _ADFunc_ListView_SetAllItemsChecked($hWnd, $bCheck = True)
	If Not IsHWnd($hWnd) Then $hWnd = GUICtrlGetHandle($hWnd)
	For $i = 0 To _GUICtrlListView_GetItemCount($hWnd)
		_GUICtrlListView_SetItemChecked($hWnd, $i, $bCheck)
	Next
EndFunc   ;==>_ADFunc_ListView_SetAllItemsChecked

Func _ADFunc_ListView_SetAllItemsSelected($hWnd, $bSelected = True, $bFocused = False)
	If Not IsHWnd($hWnd) Then $hWnd = GUICtrlGetHandle($hWnd)
	For $i = 0 To _GUICtrlListView_GetItemCount($hWnd)
		_GUICtrlListView_SetItemSelected($hWnd, $i, $bSelected, $bFocused)
	Next
EndFunc   ;==>_ADFunc_ListView_SetAllItemsSelected

Func _ADFunc_ListView_CountItemChecked($hWnd)
	If Not IsHWnd($hWnd) Then $hWnd = GUICtrlGetHandle($hWnd)
	Local $CountItemChecked = 0
	For $i = 0 To _GUICtrlListView_GetItemCount($hWnd) - 1
		If _GUICtrlListView_GetItemChecked($hWnd, $i) Then
			$CountItemChecked += 1
		EndIf
	Next
	Return $CountItemChecked
EndFunc   ;==>_ADFunc_ListView_CountItemChecked

Func _ADFunc_ListView_CountItemSelected($hWnd)
	If Not IsHWnd($hWnd) Then $hWnd = GUICtrlGetHandle($hWnd)
	Local $CountItemChecked = 0
	For $i = 0 To _GUICtrlListView_GetItemCount($hWnd) - 1
		If _GUICtrlListView_GetItemSelected($hWnd, $i) Then
			$CountItemChecked += 1
		EndIf
	Next
	Return $CountItemChecked
EndFunc   ;==>_ADFunc_ListView_CountItemSelected


Func _ADFunc_Array_ArrayDeleteClones($sArray)
	If Not IsArray($sArray) Then Return SetError(1)
	If UBound($sArray, 0) > 1 Then Return SetError(2)

	Local $newarr[0], $IsFound = 0, $NewArrCnt = 0, $Extended = 0
	For $i = 0 To UBound($sArray) - 1
		$IsFound = 0
		For $j = 0 To UBound($newarr) - 1
			If $sArray[$i] = $newarr[$j] Then
				$IsFound = 1
				$Extended += 1
				ExitLoop
			EndIf
		Next
		If Not $IsFound Then
			ReDim $newarr[$NewArrCnt + 1]
			$newarr[$NewArrCnt] = $sArray[$i]
			$NewArrCnt += 1
		EndIf
	Next
;~ 	$NewArr[0] = UBound($NewArr) - 1
	Return SetError(0, $Extended, $newarr)
EndFunc   ;==>_ADFunc_Array_ArrayDeleteClones

Func _ADFunc_Array_ArrayAdd_2D(ByRef $avArray, $vValue, $x, $y)

	If $x > UBound($avArray) - 1 Then
		ReDim $avArray[$x + 1][UBound($avArray, 2)]
	EndIf

	If $y > UBound($avArray, 2) - 1 Then
		ReDim $avArray[UBound($avArray)][$y + 1]
	EndIf

	$avArray[$x][$y] = $vValue

	Return True
EndFunc   ;==>_ADFunc_Array_ArrayAdd_2D


Func _ADFunc_Divers_HiWord($x)
	Return BitShift($x, 16)
EndFunc   ;==>_ADFunc_Divers_HiWord

Func _ADFunc_Divers_LoWord($x)
	Return BitAND($x, 0xFFFF)
EndFunc   ;==>_ADFunc_Divers_LoWord

Func _ADFunc_Divers_ConverTime($time)
	Local $secunds = Int($time / 1000)
	Local $minutes = Int($secunds / 60)
	Local $hours = Int($minutes / 60)
	Local $ret = $hours & ":" & StringFormat("%02d", Mod($minutes, 60)) & ":" & StringFormat("%02d", Mod($secunds, 60))
	Return $ret
EndFunc   ;==>_ADFunc_Divers_ConverTime

Func _ADFunc_Divers_Upd($mod = 0, $string = '', $sFstring = '', $sArrStart = 0, $sArrDim = '', $sArrMod = ' ', $sPrint = '')
	Local $sData = ''
	Local $sData2 = ''
	Local $sFch = ''
	Local $sLch = ''
	If $sArrMod = '|' Then
		If StringInStr($string, '|') Then
			Local $split = StringSplit($string, '|')
			$string = ''
			$sArrStart = 0
			Dim $string[0]
			For $i = 1 To UBound($split) - 1
				_ArrayAdd($string, $split[$i])
			Next
		EndIf
	EndIf
	Switch $mod
		Case 0
			If $string = '' Then
				ConsoleWrite(@LF)
			Else
				If IsArray($string) Then
					For $i = $sArrStart To UBound($string) - 1

						Switch $sArrDim
							Case -1
								If $string[$i] = '' Then ContinueLoop
								$sData &= $sFstring & $string[$i] & @LF
								$sData2 &= $string[$i] & @LF
							Case Else
								If $string[$i][$sArrDim] = '' Then ContinueLoop
								$sData &= $sFstring & $string[$i][$sArrDim] & @LF
								$sData2 &= $string[$i][$sArrDim] & @LF
;~ 							Case 2
;~ 								If $string[$i][1] = '' Then ContinueLoop
;~ 								$sData &= $sFstring & $string[$i][0] & $sArrMod & $string[$i][1] & @LF
;~ 								$sData2 &= $string[$i][0] & $sArrMod & $string[$i][1] & @LF
;~ 							Case 3
;~ 								$sData &= $sFstring & $string[$i] & $sArrMod
;~ 								$sData2 &= $string[$i] & $sArrMod
;~ 							Case 4
;~ 								If $string[$i][$sArrDim] = '' Then ContinueLoop
;~ 								$sData &= $sFstring & $string[$i][$sArrDim] & $sArrMod
;~ 								$sData2 &= $string[$i][$sArrDim] & $sArrMod
;~ 							Case 5
;~ 								If $string[$i][1] = '' Then ContinueLoop
;~ 								$sData &= $sFstring & $string[$i][0] & ' ' & $string[$i][1] & $sArrMod
;~ 								$sData2 &= $string[$i][0] & ' ' & $string[$i][1] & $sArrMod
						EndSwitch
					Next
					$sData = StringTrimRight($sData, 1)
;~ 					If $sArrMod <> '' Then $sData = StringTrimRight($sData, StringLen($sArrMod))
					ConsoleWrite($sData & @LF)
					If $sPrint <> '' Then FileWrite($sPrint, $sData2 & @CRLF)
				Else
					ConsoleWrite($sFstring & $string & @LF)
					If $sPrint <> '' Then FileWrite($sPrint, $string & @CRLF)
				EndIf
			EndIf

		Case 1, 2, 3
			If $string = '' Then
				ConsoleWrite(@LF)
			Else
				Switch $mod
					Case 1
						$sFch = "+"
						$sLch = "-"
					Case 2
						$sFch = "!"
						$sLch = "!"
					Case 3
						$sFch = "+"
						$sLch = "+"
				EndSwitch
				If IsArray($string) Then
					For $i = $sArrStart To UBound($string) - 1
						Switch $sArrDim
							Case -1
								If $string[$i] = '' Then ContinueLoop
								$sData &= $sFstring & $string[$i] & @LF
								$sData2 &= $string[$i] & @LF
;~ 							Case Else
;~ 								If $string[$i][$sArrDim] = '' Then ContinueLoop
;~ 								$sData &= $sFstring & $string[$i][$sArrDim] & @LF
;~ 								$sData2 &= $string[$i][$sArrDim] & @LF
;~ 							Case 2
;~ 								If $string[$i][1] = '' Then ContinueLoop
;~ 								$sData &= $sFstring & $string[$i][0] & ' ' & $string[$i][1] & @LF
;~ 								$sData2 &= $string[$i][0] & ' ' & $string[$i][1] & @LF

;~ 							Case 3
;~ 								$sData &= $sFstring & $string[$i] & $sArrMod
;~ 								$sData2 &= $string[$i] & $sArrMod
;~ 							Case 4
;~ 								If $string[$i][$sArrDim] = '' Then ContinueLoop
;~ 								$sData &= $sFstring & $string[$i][$sArrDim] & $sArrMod
;~ 								$sData2 &= $string[$i][$sArrDim] & $sArrMod
;~ 							Case 5
;~ 								If $string[$i][1] = '' Then ContinueLoop
;~ 								$sData &= $sFstring & $string[$i][0] & ' ' & $string[$i][1] & $sArrMod
;~ 								$sData2 &= $string[$i][0] & ' ' & $string[$i][1] & $sArrMod
						EndSwitch
					Next
					$sData = StringTrimRight($sData, 1)
					$sData2 = StringTrimRight($sData2, 1)
;~ 					If $sArrMod <> '' Then $sData = StringTrimRight($sData, StringLen($sArrMod))
					ConsoleWrite($sFch & "===========================================================" & @LF & _
							$sData & @LF & _
							$sLch & "===========================================================" & @LF)
					If $sPrint <> '' Then FileWrite($sPrint, $sData2 & @CRLF)
				Else
					ConsoleWrite($sFch & "===========================================================" & @LF & _
							$sFstring & $string & @LF & _
							$sLch & "===========================================================" & @LF)
					If $sPrint <> '' Then FileWrite($sPrint, $string & @CRLF)
				EndIf
			EndIf
	EndSwitch
EndFunc   ;==>_ADFunc_Divers_Upd

Func _ADFunc_Divers_RGB2BGR($iColor)
	Return BitAND(BitShift(String(Binary($iColor)), 8), 0xFFFFFF)
EndFunc   ;==>_ADFunc_Divers_RGB2BGR


Func _ADFunc_Gui_ComboBox($sTiltle = 'titre', $sDataCmb = 'ComboBox', $sDefaultCmb = '', $sDataLbl = 'ComboBox', $sDataCmb2 = "", $sDefaultCmb2 = "", $sDataLbl2 = "", $sW = 500)
	Local $data
	Local $sh = 100

	If $sDataCmb2 <> '' Then
		$sh = $sh + 70
	EndIf

	Local $GuiComboBox = GUICreate($sTiltle, $sW, $sh, -1, -1, -1, BitOR($WS_EX_TOOLWINDOW, $WS_EX_TOPMOST, $WS_EX_WINDOWEDGE))
	WinSetOnTop($GuiComboBox, '', 1)

	Local $sTop = 8
	GUICtrlCreateLabel($sDataLbl, 8, $sTop, $sW - 16, 17, $SS_CENTER)
	$sTop += 24
	Local $GuiComboBox_Cmb = GUICtrlCreateCombo("", 8, $sTop, $sW - 16, 25, BitOR($gui_ss_default_combo, $cbs_dropdownlist, $ws_hscroll))
	If IsArray($sDataCmb) Then
		$data = ''
		For $i = 0 To UBound($sDataCmb) - 1
			$data &= $sDataCmb[$i] & '|'
		Next
		$data = StringTrimRight($data, 1)
		If $sDefaultCmb = '' Then $sDefaultCmb = $sDataCmb[0]
		GUICtrlSetData($GuiComboBox_Cmb, $data, $sDefaultCmb)
	Else
		If $sDefaultCmb = '' And StringInStr($sDataCmb, '|') Then
			$sDefaultCmb = StringSplit($sDataCmb, '|')[1]
		ElseIf $sDefaultCmb = '' And StringInStr($sDataCmb, '|') = 0 Then
			$sDefaultCmb = $sDataCmb
		EndIf
		GUICtrlSetData($GuiComboBox_Cmb, $sDataCmb, $sDefaultCmb)
	EndIf

	Local $GuiComboBox_Cmb2
	If $sDataCmb2 <> '' Then
		$sTop += 35
		GUICtrlCreateLabel($sDataLbl2, 8, $sTop, $sW - 16, 17, $SS_CENTER)
		$sTop += 24
		$GuiComboBox_Cmb2 = GUICtrlCreateCombo("", 8, $sTop, $sW - 16, 25, BitOR($gui_ss_default_combo, $cbs_dropdownlist, $ws_hscroll))
		If IsArray($sDataCmb2) Then
			$data = ''
			For $i = 0 To UBound($sDataCmb2) - 1
				$data &= $sDataCmb2[$i] & '|'
			Next
			$data = StringTrimRight($data, 1)
			If $sDefaultCmb2 = '' Then $sDefaultCmb2 = $sDataCmb2[0]
			GUICtrlSetData($GuiComboBox_Cmb2, $data, $sDefaultCmb2)
		Else
			If $sDefaultCmb2 = '' And StringInStr($sDataCmb2, '|') Then
				$sDefaultCmb2 = StringSplit($sDataCmb2, '|')[1]
			ElseIf $sDefaultCmb2 = '' And StringInStr($sDataCmb2, '|') = 0 Then
				$sDefaultCmb2 = $sDataCmb2
			EndIf
			GUICtrlSetData($GuiComboBox_Cmb2, $sDataCmb2, $sDefaultCmb2)
		EndIf
	EndIf

	$sTop += 40
	Local $GuiComboBox_BtnOk = GUICtrlCreateButton("Valider", 8, $sTop, ($sW / 2) - 8, 25)
	GUICtrlSetCursor(-1, 0)
	Local $GuiComboBox_BtnCancell = GUICtrlCreateButton("Anuller", ($sW / 2) + 2, $sTop, ($sW / 2) - 8, 25)
	GUICtrlSetCursor(-1, 0)

	GUISetState(@SW_SHOW, $GuiComboBox)

	Local $nMsg, $sRet[2], $sSetError = 0
	While 1
		$nMsg = GUIGetMsg()
		Switch $nMsg
			Case $GuiComboBox_BtnOk
				$sRet[0] = GUICtrlRead($GuiComboBox_Cmb)
				If $sDataCmb2 <> '' Then
					$sRet[1] = GUICtrlRead($GuiComboBox_Cmb2)
					If GUICtrlRead($GuiComboBox_Cmb2) = '' And GUICtrlRead($GuiComboBox_Cmb) = '' Then $sSetError = 1
				Else
					If GUICtrlRead($GuiComboBox_Cmb) = '' Then $sSetError = 1
				EndIf
				ExitLoop
			Case $GuiComboBox_BtnCancell
				$sSetError = 1
				ExitLoop
			Case $GUI_EVENT_CLOSE
				$sSetError = 1
				ExitLoop
		EndSwitch
	WEnd

	GUIDelete($GuiComboBox)

	Switch $sSetError
		Case 1
			Return SetError(1)
		Case 0
			Return $sRet
	EndSwitch
EndFunc   ;==>_ADFunc_Gui_ComboBox

Func _ADFunc_Gui_Combo_SetData($n = 0, $flag = 0, $fla2 = 0, $cmb = "", $data = "", $sDefault = "")

	If Not IsArray($data) Then
		GUICtrlSetData($cmb, '')
		Return False
	EndIf

	GUICtrlSetData($cmb, '')

	Local $aDefault, $sData = ""
	For $i = $n To UBound($data) - 1
		Switch $flag
			Case 0
				$sData &= $data[$i] & "|"
			Case 1
				$sData &= $data[$i][$fla2] & "|"
		EndSwitch
	Next

	If $sDefault = "" Then
		$aDefault = StringSplit($sData, "|")
		$aDefault = $aDefault[1]
	EndIf
	If $sDefault <> '' Then $aDefault = $sDefault

	GUICtrlSetData($cmb, $sData, $aDefault)
	Return True
EndFunc   ;==>_ADFunc_Gui_Combo_SetData

Func _ADFunc_Gui_IsChecked($idControlID)
	Return BitAND(GUICtrlRead($idControlID), $GUI_CHECKED) = $GUI_CHECKED
EndFunc   ;==>_ADFunc_Gui_IsChecked

Func _ADFunc_Gui_GuictrlcreateBorderGui($W, $H, $H2 = 2, $color = 0x0000FF, $cColor = 0x8A8A8A)
	_ADFunc_Gui_Guictrlcreaterect(0, 0, $W, $H2, $color, $cColor)
	_ADFunc_Gui_Guictrlcreaterect(0, $H - $H2, $W, $H2, $color, $cColor)
	_ADFunc_Gui_Guictrlcreaterect(0, 0, $H2, $H, $color, $cColor)
	_ADFunc_Gui_Guictrlcreaterect($W - $H2, 0, $H2, $H, $color, $cColor)
EndFunc   ;==>_ADFunc_Gui_GuictrlcreateBorderGui

Func _ADFunc_Gui_GuictrlcreateBorderLabel($T, $L, $W, $H, $H2, $color = 0x8A8A8A)
	local $sRetuned[4]
	$sRetuned[0] = GUICtrlCreateLabel("", $L - $H2, $T - $H2, $W + $H2, $H2)
	GUICtrlSetBkColor(-1, $color)
	GUICtrlSetState(-1, $gui_disable)

	$sRetuned[1] = GUICtrlCreateLabel("", $L - $H2, $T - $H2, $H2, $H + $H2)
	GUICtrlSetBkColor(-1, $color)
	GUICtrlSetState(-1, $gui_disable)

	$sRetuned[2] = GUICtrlCreateLabel("", $L - $H2, $T + $H, $W + $H2, $H2)
	GUICtrlSetBkColor(-1, $color)
	GUICtrlSetState(-1, $gui_disable)

	$sRetuned[3] = GUICtrlCreateLabel("", $L + $W, $T - $H2, $H2, $H + $H2 * 2)
	GUICtrlSetBkColor(-1, $color)
	GUICtrlSetState(-1, $gui_disable)
	return $sRetuned
EndFunc   ;==>_ADFunc_Gui_GuictrlcreateBorderLabel

Func _ADFunc_Gui_Guictrlcreaterect($x, $y, $width, $height, $color, $cColor)
	GUICtrlCreateLabel("", $x, $y, $width, $height)
	GUICtrlSetColor(-1, $cColor)
	GUICtrlSetBkColor(-1, $color)
	GUICtrlSetState(-1, $gui_disable)
EndFunc   ;==>_ADFunc_Gui_Guictrlcreaterect

Func _ADFunc_Gui_GUICtrlCreateGroupEx($sText, $iLeft, $iTop, $iWidth, $iHeight, $bColor = 0xC0C0C0, $OutlineColor = 0xFFFFFF)
	Local $aLabel[6] = [5], $aLabelInner[6] = [5]
	Local $aStringSize = _StringSize($sText)
	$aLabel[1] = GUICtrlCreateLabel('', $iLeft + 1, $iTop + 1, 1, $iHeight) ; Left Line.
	$aLabelInner[1] = GUICtrlCreateLabel('', $iLeft + 2, $iTop + 1, 1, $iHeight) ; Inner/Outer Left Line.
	$aLabel[2] = GUICtrlCreateLabel('', $iLeft + 1, $iTop + 1, 10, 1) ; Top Left Line.
	$aLabelInner[2] = GUICtrlCreateLabel('', $iLeft + 2, $iTop + 2, 10 - 1, 1) ; Top Inner/Outer Left Line.
	GUICtrlCreateLabel(' ' & $sText, $iLeft + 7, $iTop - 6, $aStringSize[2] - 3, 15)
	$aLabel[3] = GUICtrlCreateLabel('', $iLeft + $aStringSize[2] + 4, $iTop + 1, $iWidth - $aStringSize[2] - 3, 1) ; Top Right Line.
	$aLabelInner[3] = GUICtrlCreateLabel('', $iLeft + $aStringSize[2] + 4, $iTop + 2, $iWidth - $aStringSize[2] - 3, 1) ; Top Inner/Outer Right Line.
	$aLabel[4] = GUICtrlCreateLabel('', $iLeft + $iWidth + 1, $iTop + 1, 1, $iHeight) ; Right Line.
	$aLabelInner[4] = GUICtrlCreateLabel('', $iLeft + $iWidth + 2, $iTop + 1, 1, $iHeight + 1) ; Right Inner/Outer Line.
	$aLabel[5] = GUICtrlCreateLabel('', $iLeft + 1, $iTop + $iHeight + 1, $iWidth + 1, 1) ; Bottom Line.
	$aLabelInner[5] = GUICtrlCreateLabel('', $iLeft + 2, $iTop + $iHeight + 2, $iWidth + 2, 1) ; Bottom Inner/Outer Line.
	For $i = 1 To $aLabel[0]
		GUICtrlSetBkColor($aLabel[$i], $bColor)
		GUICtrlSetBkColor($aLabelInner[$i], $OutlineColor)
	Next
EndFunc   ;==>_ADFunc_Gui_GUICtrlCreateGroupEx

Func _ADFunc_Gui_MsgBox($sShowMsgBox = True, $Hparent = '', $title = 'CFF_MsgBox', $lbl1 = 'Label 1', $lbl2 = 'Label 2', $btn1 = 'Ok', $btn2 = 'No', $btn3 = '', $btn4 = '', $sW = '', $sh = '')
	If Not $sShowMsgBox Then Return

	Local $NbrBtn = 0
	Local $sSSize, $sSHsize, $tSSize

	If $sW = '' And $sh = '' Then

		$tSSize = _StringSize($lbl1, 9, 900, Default)
		$sSSize = _StringSize($lbl2, 8.5, Default, Default)
		$sSHsize = $sSSize[3]
		If $sSHsize > 17 Then
			$sSHsize = $sSHsize - 17
		Else
			$sSHsize = 0
		EndIf

		If $sSSize[2] > 0 And $sSSize[2] <= 500 Then
			$sh = 90 + $sSHsize
			$sW = $sSSize[2] + 16
		ElseIf $sSSize[2] >= 500 And $sSSize[2] < 1050 Then
			$sh = 105 + $sSHsize
			$sW = 500 + 16
		ElseIf $sSSize[2] >= 1050 And $sSSize[2] < 1500 Then
			$sh = 115 + $sSHsize
			$sW = 500 + 16
		ElseIf $sSSize[2] >= 1500 And $sSSize[2] < 1700 Then
			$sh = 120 + $sSHsize
			$sW = 500 + 16
		ElseIf $sSSize[2] >= 1700 And $sSSize[2] < 2000 Then
			$sh = 130 + $sSHsize
			$sW = 500 + 16
		ElseIf $sSSize[2] >= 2000 And $sSSize[2] < 2500 Then
			$sh = 140 + $sSHsize
			$sW = 500 + 16
		ElseIf $sSSize[2] >= 2500 Then
			$sh = 145 + $sSHsize
			$sW = 500 + 16
		Else
			$sW = $sW
			$sh = $sh
		EndIf
	EndIf

	If UBound($tSSize) >= 2 Then
		If $tSSize[2] > $sW Then
			$sW = $tSSize[2] + 60
		EndIf
	EndIf

	If $btn1 <> '' Then $NbrBtn += 1
	If $btn2 <> '' Then $NbrBtn += 1
	If $btn3 <> '' Then $NbrBtn += 1
	If $btn4 <> '' Then $NbrBtn += 1
	If $NbrBtn = 1 And $sW < 300 Then $sW = 300
	If $NbrBtn > 1 And $NbrBtn <= 2 And $sW < 300 Then $sW = 300
	If $NbrBtn > 2 And $NbrBtn <= 3 And $sW < 400 Then $sW = 400
	If $NbrBtn > 3 And $NbrBtn <= 4 And $sW < 550 Then $sW = 550
	If $sh < 90 Then $sh = 90

	Local $ADFunc_MsgBox = GUICreate($title, $sW, $sh, -1, -1, -1, BitOR($WS_EX_TOOLWINDOW, $WS_EX_TOPMOST, $WS_EX_WINDOWEDGE))
;~ 	Local $CFF_MsgBox = GUICreate($title, $sW, $sH)
	GUISetBkColor(0xC0C0C0)
	Local $top = 8
	GUICtrlCreateLabel($lbl1, 6, $top, $sW - 15, 17, 0x01, $WS_EX_CLIENTEDGE)
	GUICtrlSetFont(-1, 8.5, 600)
	$top += 25
	GUICtrlCreateLabel($lbl2, 6, $top, $sW - 15, $sh - 70, BitOR(0x01, $BS_MULTILINE), $WS_EX_CLIENTEDGE)
	$top += ($sh - 70) + 5
	Local $wbtn = ($sW / $NbrBtn) - 8
	Local $lbtn = 6
	Local $ADFunc_MsgBox_Btn1
	If $NbrBtn > 0 Then $ADFunc_MsgBox_Btn1 = GUICtrlCreateButton($btn1, $lbtn, $top, $wbtn, 25, -1, $WS_EX_CLIENTEDGE)
	GUICtrlSetCursor(-1, 0)
	$lbtn += $wbtn + 5
	Local $ADFunc_MsgBox_Btn2
	If $NbrBtn > 1 Then $ADFunc_MsgBox_Btn2 = GUICtrlCreateButton($btn2, $lbtn, $top, $wbtn, 25, -1, $WS_EX_CLIENTEDGE)
	GUICtrlSetCursor(-1, 0)
	$lbtn += $wbtn + 5
	Local $ADFunc_MsgBox_Btn3
	If $NbrBtn > 2 Then $ADFunc_MsgBox_Btn3 = GUICtrlCreateButton($btn3, $lbtn, $top, $wbtn, 25, -1, $WS_EX_CLIENTEDGE)
	GUICtrlSetCursor(-1, 0)
	$lbtn += $wbtn + 5
	Local $ADFunc_MsgBox_Btn4
	If $NbrBtn > 3 Then $ADFunc_MsgBox_Btn4 = GUICtrlCreateButton($btn4, $lbtn, $top, $wbtn, 25, -1, $WS_EX_CLIENTEDGE)
	GUICtrlSetCursor(-1, 0)

	If $Hparent <> '' Then GUISetState(@SW_DISABLE, $Hparent)
	GUISetState(@SW_SHOW, $ADFunc_MsgBox)

	WinSetOnTop($ADFunc_MsgBox, '', 1)

	Local $nMsg, $ret
	While 1
		$nMsg = GUIGetMsg()
		Switch $nMsg

			Case $ADFunc_MsgBox_Btn4
				If $NbrBtn > 3 Then
					$ret = 3
					ExitLoop
				EndIf

			Case $ADFunc_MsgBox_Btn3
				If $NbrBtn > 2 And $NbrBtn < 4 Then
					$ret = 3
					ExitLoop
				EndIf
				If $NbrBtn > 3 Then
					$ret = 4
					ExitLoop
				EndIf

			Case $ADFunc_MsgBox_Btn1
				If $NbrBtn > 0 Then
					$ret = 1
					ExitLoop
				EndIf

			Case $ADFunc_MsgBox_Btn2
				If $NbrBtn > 1 Then
					$ret = 2
					ExitLoop
				EndIf
			Case -3
				$ret = -1
				ExitLoop
		EndSwitch
	WEnd

	GUIDelete($ADFunc_MsgBox)

	If $Hparent <> '' Then
		GUISetState(@SW_ENABLE, $Hparent)
		GUISetState(@SW_RESTORE, $Hparent)
	EndIf

;~ 	_CFF_Upd(1, '_CFF_MsgBox $ret ' & $ret)

	Return $ret
EndFunc   ;==>_ADFunc_Gui_MsgBox


#cs
	Func _ADFunc_Exemple()
	Local $msg
	$msg = _ADFunc_Gui_MsgBox(True, '', 'Copier/Deplacer', 'Copier/deplacer :', '' & @CRLF & 'Ver :' & @CRLF & '', 'Copier et remplacer', 'Deplacer et remplacer', 'Copier et rennomer', 'Deplacer et rennomer')
	_ADFunc_Divers_Upd(3, $msg)
	$msg = _ADFunc_Gui_MsgBox(True, '', 'Copier/Deplacer', 'Copier/deplacer :', '' & @CRLF & 'Ver :' & @CRLF & '', 'non', 'oui', '', '', '')
	_ADFunc_Divers_Upd(3, $msg)
	EndFunc   ;==>_ADFunc_Exemple
#ce
#cs
	;~ #include <_ADFunc.au3>
	;~ _HexCouleurs_Show()
	;~ $cmb = _ADFunc_Gui_MsgBox(True, '', '', 'dd', 'testggggggghhhhhhhhhhhhhhhhhhhhhhhhhhhhhhggggggggggggggggggfffffffffffffffffff' & @LF & 'test', 1, 2, 3, 4)
	;~ _ADFunc_Divers_Upd(3, $cmb, "--> ")
	;~ $source = _CFF_ChooseFileOrFolder('Selection (Fichier/Dossier)', @ScriptDir, '', True)
	;~ $source2 = _CFF_ChooseFileOrFolder('Selection (Fichier/Dossier)', @ScriptDir, '', True)
	;~ If Not @error Then
	;~ 	$data = ''
	;~ 	For $i = 0 To UBound($source) -1
	;~ 		$data &= $source[$i] & '|'
	;~ 	Next
	;~ 	$data = StringTrimRight($data, 1)
	;~ 	$cmb = _ADFunc_Gui_ComboBox('HELLO', $data, '', 1, $source2, '', 2)
	;~ 	If Not @error Then _ADFunc_Divers_Upd(0, $cmb, "-->" & @TAB)
	;~ EndIf
	;~ _ArrayDisplay($cmb)
	;~ $cmb = _ADFunc_Gui_ComboBox('', 1,'',1,2,'','')
	;~ If @error Then _ADFunc_MsgBox(true, '', '', '_ADFunc_Gui_ComboBox', '@' , 1, 2)
	;~ _ArrayDisplay($cmb)
#ce
