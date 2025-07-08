param(
    [Parameter(ValueFromRemainingArguments=$true)]
    [string[]]$args,
    [switch]$log = $false,
    [switch]$release = $false
)

if ($release) {
    & $PSScriptRoot/build.ps1 -release
} else {
    & $PSScriptRoot/build.ps1
}
if ($?) {
    adb push build/libchroma.so /sdcard/ModData/com.beatgames.beatsaber/Modloader/mods/libchroma.so
    if ($?) {
        adb shell am force-stop com.beatgames.beatsaber
        adb shell am start com.beatgames.beatsaber/com.unity3d.player.UnityPlayerActivity
        if ($log.IsPresent) {
            $timestamp = Get-Date -Format "MM-dd HH:mm:ss.fff"
            adb logcat -c
            adb logcat > log.txt
        }
    }
}
