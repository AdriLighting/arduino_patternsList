

func pDev_onTime($sTime = $pDev_onTimeTimer)
	Return _ADFunc_Divers_ConverTime(TimerDiff($sTime))
EndFunc
func pDev_onTimeInit()
	$pDev_onTimeTimer = TimerInit()
EndFunc
func _lbl($str, $len = 15, $sep = " : ")
	$str = String($str)
	While (StringLen($str) < $len)
		$str &= ";"
	WEnd
	return $str & $sep
endfunc
func _upd($sData = "", $mod = 3, $hf= 3, $display = true, $sSize = '', $sColor = "0,255,0", $sLine = @ScriptLineNumber)
	if not $display then return


	Local $uD1 = UBound($sData)
	Local $uD2 = UBound($sData, 2)

	Local $sFch = ''
	Local $sLch = ''
	Local $sHeader = ''
	Local $sFooter = ''
	Switch $mod
		Case 0
			$sFch = ""
			$sLch = ""
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

	Switch $hf
		Case 0
			$sHeader = ""
			$sFooter = ""
		Case 1
			$sHeader = $sFch & "=========================================================== " & "Line(" & StringFormat("%04d", $sLine) & ") " & pDev_onTime($pDev_onTimeTimer) & @LF
			$sFooter = ""
		Case 2
			$sHeader = ""
			$sFooter = $sLch & "===========================================================" & @LF
		Case 3
			$sHeader = $sFch & "=========================================================== " & "Line(" & StringFormat("%04d", $sLine) & ") " &  pDev_onTime($pDev_onTimeTimer) & @LF
			$sFooter = $sLch & "===========================================================" & @LF
	EndSwitch
	local $sSpace 	= ";"
	Local $sRow		= ""
	Local $sPrint 	= ""
	Local $sCrLf 	= ""
	Local $sLf 		= @CRLF
	if (($uD1 = 0) And ($uD2 = 0)) Then
		if $sData = "" then $sLf = ""
		if $sHeader <> "" then Out2("***************************", "255,255,255", 7)
		$sData = StringReplace($sData,$sSpace,"  ")
		Out2($sData, $sColor, $sSize)
		if $sFooter <> "" then Out2("***************************", "255,255,255", 7)
		ConsoleWrite( _
						$sHeader & _
						$sData & $sLf & _
						$sFooter)
	Elseif (($uD1 > 0) And ($uD2 = 0)) Then
		$sCrLf = @CRLF
		if $sHeader <> "" then Out2("***************************", "255,255,255", 7)
		For $i = 0 To $uD1 - 1
			if $i >= $uD1 - 1 Then  $sCrLf = ""
			$sData[$i] 	= StringReplace($sData[$i],$sSpace,"  ")
			$sRow 		=  String($i)
			$sRow 		= _lbl($sRow, 6)
			$sRow 		= StringReplace($sRow, $sSpace,"  ")
			$sPrint 	&= $sRow  & $sData[$i] & $sCrLf
			Out2( $sRow  & $sData[$i], $sColor, $sSize)
		next
		if $sFooter <> "" then Out2("***************************", "255,255,255", 7)
		ConsoleWrite( _
						$sHeader & _
						$sPrint & @CRLF & _
						$sFooter)

	Elseif (($uD1 > 0) And ($uD2 > 0)) Then
		if $sHeader <> "" then Out2("***************************", "255,255,255", 7)
		if $sHeader <> "" then ConsoleWrite($sHeader)
		$sCrLf = @CRLF
		Local $sD1 = ""
		Local $sD2 = ""
		For $i = 0 To $uD1 - 1
			if $i >= $uD1 - 1 Then  $sCrLf = ""
			$sD2 = ""
			For $j = 1 To $uD2 - 1
				$sD2  &=  $sData[$i][$j] & " "
			next
			$sD1 =  String($sData[$i][0])
			$sD1 = _lbl($sD1, 6)
			$sD1 = StringReplace($sD1, $sSpace,"  ")
			$sD2 = StringReplace($sD2, $sSpace,"  ")
			Out2($sD1 & $sD2, $sColor, $sSize)
			ConsoleWrite($sD1 & " " & $sD2 & @CRLF)
		next
		if $sFooter <> "" then Out2("***************************", "255,255,255", 7)
		if $sFooter <> "" then ConsoleWrite($sFooter)

	EndIf
EndFunc


Func Out2($TEXT, $sColor = "0,255,0", $sSize = '') ; 65280


EndFunc   ;==>Out2
Func _GUICtrlRichEdit_AppendTextEx($RichEdit, $text, $font="Arial", $color="0,255,0", $size=12, $bold=0, $italic=0, $strike=0, $underline=0)
	Local $command = "{\rtf1\ansi"
	Local $r, $g, $b, $ul[9] = ["8", '\ul', '\uldb', '\ulth', '\ulw', '\ulwave', '\uld', '\uldash', '\uldashd']

	If $font <> "" Then $command &= "{\fonttbl {\f0 "&$font&";}}"
	If $color <> "" Then
		$b = StringSplit($color, ",")[3]
		$g = StringSplit($color, ",")[2]
		$r = StringSplit($color, ",")[1]
		If $r+$b+$g > 0 Then
			$command &= "{\colortbl;\red"&$r&"\green"&$g&"\blue"&$b&";}\cf1"
		EndIf
	EndIf

  If $size Then $command &= "\fs"&round($size*2)&" "
  If $strike Then $command &= "\strike "
  If $italic Then $command &= "\i "
  If $bold Then $command &= "\b "
  If $underline > 0 and $underline < 9 Then $command &= $ul[$underline]&" "
;~   ConsoleWrite($command&$text&"}"&@CRLF) ; Debugging line
	local	$fText = StringReplace($text,"\","/")
	 		$fText = StringReplace($fText,@LF,"\line")
	 		$fText = StringReplace($fText,@CRLF,"\line")

  Return _GUICtrlRichEdit_AppendText($RichEdit, $command&$fText&"}" )
EndFunc

func _pDev_copyFileFolder($sPath)
	Local $ChooseSaveFile, $ChooseFileOrFolder[6], $ChooseSaveFolder, $SaveFileOrFolder
	Local $sDrive = "", $sDir = "", $sFileName = "", $sExtension = ""
	_PathSplit($sPath, $sDrive, $sDir, $sFileName, $sExtension)

	Switch $sExtension
		Case ''
			$ChooseFileOrFolder[0] = $sPath
			$ChooseFileOrFolder[1] = 1
			$ChooseFileOrFolder[2] = $sDrive & $sDir
		Case Else
			$ChooseFileOrFolder[0] = $sPath
			$ChooseFileOrFolder[1] = 2
			$ChooseFileOrFolder[2] = $sDrive & $sDir
			$ChooseFileOrFolder[3] = $sFileName
			$ChooseFileOrFolder[4] = $sExtension
	EndSwitch


		If $ChooseFileOrFolder[1] = 2 Then
			$ChooseSaveFile = _CFF_ChooseSaveFile('UDF _CFF Exemple', "Selection de l'emplacment ou sera copier le fichier : " & @CRLF & $ChooseFileOrFolder[0], @ScriptDir, '', $ChooseFileOrFolder[3] & $ChooseFileOrFolder[4], False, '', '', True, @DesktopWidth / 1.5)
			If Not @error Then
				$SaveFileOrFolder = _CFF_SaveFileOrFolder($ChooseSaveFile, $ChooseFileOrFolder[0], $ChooseSaveFile[0])
				If @error Then
					_ADFunc_Divers_Upd(2, '_CFF_SaveFileOrFolder @error = ' & ' ' & @error & @LF & $SaveFileOrFolder)
				Else
					_ADFunc_Divers_Upd(3, $SaveFileOrFolder, '--> Fichier copier : ')
				EndIf
			Else
				_ADFunc_Divers_Upd(2, '_CFF_ChooseSaveFile @error = ' & ' ' & @error & @LF & $ChooseSaveFile)
			EndIf
		Else
			If $ChooseFileOrFolder[1] = 1 Then $ChooseSaveFolder = _CFF_ChooseSaveFolder('UDF _CFF Exemple', "Selection de l'emplacment ou sera copier le dossier : " & @CRLF & $ChooseFileOrFolder[0], @ScriptDir, '', $ChooseFileOrFolder[0], False, False, '', '', False, @DesktopWidth / 1.5)
			If $ChooseFileOrFolder[1] = 3 Then $ChooseSaveFolder = _CFF_ChooseSaveFolder('UDF _CFF Exemple', "Selection de l'emplacment ou seront copier les Fichier/Dossier : " & @CRLF & $ChooseFileOrFolder[0], @ScriptDir, '', '', False, True, '', '', False, @DesktopWidth / 1.5)
			If Not @error Then
				$SaveFileOrFolder = _CFF_SaveFileOrFolder($ChooseSaveFolder, $ChooseFileOrFolder[0], $ChooseSaveFolder[0])
				If @error Then
					_ADFunc_Divers_Upd(2, '_CFF_SaveFileOrFolder @error = ' & ' ' & @error & @LF & $SaveFileOrFolder)
				Else
					If $ChooseFileOrFolder[1] = 1 Then _ADFunc_Divers_Upd(3, $SaveFileOrFolder, '--> Dossier copier : ')
					If $ChooseFileOrFolder[1] = 3 Then _ADFunc_Divers_Upd(3, $SaveFileOrFolder, '--> Fichier copier : ')
				EndIf
			Else
				_ADFunc_Divers_Upd(2, '_CFF_ChooseSaveFolder @error = ' & ' ' & @error & @LF & $ChooseSaveFolder)
			EndIf
		EndIf
endfunc


Func _pDev_Inet_InetGet($sUrl, $sOutPath, $sTitle = '', $sProgress = False, $pDevGuiDebug = false, $sProgressCtrlId = Null)
	Local $pourcentage, $totalsize, $kbrecu, $hDownload = InetGet($sUrl, $sOutPath, 1, 1)
	If $sTitle == '' Then $sTitle = _ADFunc_Files_FileGetFullNameByUrl($sUrl)
	If $sProgress Then
		if $pDevGuiDebug then
			GUICtrlSetData($sProgressCtrlId, 0)
		else
			ProgressOn("Telechargment en cour", "", "0%")
		endif
	endif
	Do
		$totalsize = 0
		$kbrecu = InetGetInfo($hDownload, 0)
		If $kbrecu > 0 Then
			If $totalsize = 0 Then
				$totalsize = InetGetInfo($hDownload, 1)
			EndIf
			$pourcentage = Round($kbrecu * 100 / $totalsize)
			If $sProgress Then
				if $pDevGuiDebug then
					GUICtrlSetData($sProgressCtrlId, $pourcentage)
				else
					ProgressSet($pourcentage, $pourcentage & '%', $sTitle)
				EndIf
			EndIf
		EndIf
		Sleep(50)
	Until InetGetInfo($hDownload, 2)
	If $sProgress Then
		if $pDevGuiDebug then
			GUICtrlSetData($sProgressCtrlId, 0)
		else
			ProgressOff()
		endif
	endif
	InetClose($hDownload)
EndFunc   ;==>_ADFunc_Inet_InetGet


