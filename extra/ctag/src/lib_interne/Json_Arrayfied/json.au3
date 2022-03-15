#include-once
#include <..\Json.au3>

Func JsonArrayfied($sJsonString, $iEcho = 0)
    Local $sConsoleWriteJson = ConsoleWriteJson($sJsonString, "", $iEcho)
    Local $n, $aLines = StringSplit($sConsoleWriteJson, @LF, 1)
    Local $aTemp, $iRow = 0, $iCol = 0, $m, $aJsonArrayfied[UBound($aLines) + 1][100] ; a lazy but efficient way to go about it
    For $n = 1 To $aLines[0]
        If StringInStr($aLines[$n], ":") + 2 > StringLen($aLines[$n]) Then ContinueLoop
        $aLines[$n] = StringReplace($aLines[$n], "][", "|")
        $aLines[$n] = StringReplace($aLines[$n], "]", "|")
        $aLines[$n] = StringReplace($aLines[$n], "[", "|")
        $aTemp = StringSplit($aLines[$n], "|")
        $iRow += 1
        For $m = 1 To $aTemp[0] - 1
            If $iCol < $m Then $iCol = $m
            $aJsonArrayfied[$iRow][$m - 1] = StringReplace($aTemp[$m], '"', '')
        Next
        $aJsonArrayfied[$iRow][$aTemp[0] - 1] = StringTrimLeft($aTemp[$aTemp[0]], StringInStr($aTemp[$aTemp[0]], ":") + 1)
        $aJsonArrayfied[$iRow][0] = StringMid($aTemp[$aTemp[0]], 5, StringInStr($aTemp[$aTemp[0]], ":") - 5)
    Next
    $aJsonArrayfied[0][0] = $iRow
    $aJsonArrayfied[0][1] = $iCol
    ReDim $aJsonArrayfied[$iRow + 1][$iCol + 1]
    Return $aJsonArrayfied
EndFunc   ;==>JsonArrayfied


Func ConsoleWriteJson($sJsonString, $sDesc = "", $iEcho = 1)
    Local $sOutGlobal
    If $sDesc = "" Then $sDesc = 'ConsoleWriteJson'
    Local $obj = Json_Decode($sJsonString)
    Json_Iterate($sOutGlobal, $obj, '', $sDesc, $iEcho)
    Return $sOutGlobal
EndFunc   ;==>ConsoleWriteJson

Func Json_Iterate(ByRef $sOutGlobal, $obj, $string, $pre = "", $iEcho = 1)
;~     Local $sOut = ""
    Local $temp, $i, $b
    If ($pre <> "") Then
;~         $sOut &= $pre & ": "
        If $iEcho Then ConsoleWrite($pre & ": ")
    EndIf
    Local $a = Json_Get_ShowResult($obj, $string, $sOutGlobal, $iEcho)
    If IsArray($a) Then
        For $i = 0 To UBound($a) - 1
            Json_Iterate($sOutGlobal, $obj, $string & '[' & $i & ']', $pre, $iEcho)
        Next
    ElseIf IsObj($a) Then
        $b = Json_ObjGetKeys($a)
        For $temp In $b
            Json_Iterate($sOutGlobal, $obj, $string & '["' & $temp & '"]', $pre, $iEcho)
        Next
    EndIf
    Return $sOutGlobal
EndFunc   ;==>Json_Iterate

Func Json_Get_ShowResult($Var, $Key, ByRef $sOutGlobal, $iEcho)
    Local $sOut = ""
    Local $Ret = Json_Getr($Var, $Key)
    If @error Then
        Switch @error
            Case 1
                $sOut &= "Error 1: key not exists" & @LF
                If $iEcho Then ConsoleWrite($sOut)
            Case 2
                $sOut &= "Error 2: syntax error" & @LF
                If $iEcho Then ConsoleWrite($sOut)
        EndSwitch
    Else
        $sOut &= $Key & " => " & VarGetType($Ret) & ": " & $Ret & @LF
        If $iEcho Then ConsoleWrite($sOut)
    EndIf
    $sOutGlobal &= $sOut ;& $Ret
    Return $Ret
EndFunc   ;==>Json_Get_ShowResult

Func Json_Getr($Var, $Key)
    If Not $Key Then Return $Var
    Local $Match = StringRegExp($Key, "(^\[([^\]]+)\])", 3)
	Local $Ret
    If IsArray($Match) Then
        Local $Index = Json_Decode($Match[1])
        $Key = StringTrimLeft($Key, StringLen($Match[0]))
        If IsString($Index) And Json_IsObject($Var) And Json_ObjExists($Var, $Index) Then
            $Ret = Json_Getr(Json_ObjGet($Var, $Index), $Key)
            Return SetError(@error, 0, $Ret)
        ElseIf IsNumber($Index) And IsArray($Var) And $Index >= 0 And $Index < UBound($Var) Then
            $Ret = Json_Getr($Var[$Index], $Key)
            Return SetError(@error, 0, $Ret)
        Else
            Return SetError(1, 0, "")
        EndIf
    EndIf
    Return SetError(2, 0, "")
EndFunc   ;==>Json_Getr