$NDKPath = Get-Content $PSScriptRoot/ndkpath.txt

$buildScript = "$NDKPath/build/ndk-build"
if (-not ($PSVersionTable.PSEdition -eq "Core")) {
    $buildScript += ".cmd"
}

$port = 5039

$symbolsListing = ""

& $buildScript NDK_PROJECT_PATH=$PSScriptRoot APP_BUILD_SCRIPT=$PSScriptRoot/Android.mk NDK_APPLICATION_MK=$PSScriptRoot/Application.mk NDK_DEBUG=1 APP_OPTIM=debug
# Copy over all .so files that are NOT libmodloader or ID
Get-ChildItem "$PSScriptRoot/obj/local/arm64-v8a/" -Filter *.so |
ForEach-Object {
    $name = $_.Name
    $path = "/sdcard/Android/data/com.beatgames.beatsaber/files/libs/$name"
    $force = $false
    if ($name.Contains("custom-types")) {
        $path = "/sdcard/Android/data/com.beatgames.beatsaber/files/mods/$name"
        $force = $true
    }
    $res = & adb shell "ls $path"
    # If the name is not the modloader, and it doesn't exist, push it (unless forced)
    if ((!$name.Equals("libmodloader.so") -and !$path.Equals($res)) -or $force) {
        & adb push $_.FullName $path
    }
    # Trailing comma is ok
    $symbolsListing += "'target symbols add ./obj/local/arm64-v8a/$name',"
}

# Setup ports for debugging
& adb forward tcp:$port tcp:$port
# Copy over lldb-server
$lldbDest = "/sdcard/Android/data/com.beatgames.beatsaber/files/lldb/bin/lldb-server"
& adb push $PSScriptRoot/lldb-server $lldbDest
# Set permissions
& adb shell chmod 755 $lldbDest

# Copy lldb-server to local
& adb shell run-as com.beatgames.beatsaber cp $lldbDest ./lldb-server
& adb shell run-as com.beatgames.beatsaber chmod 755 ./lldb-server
# Start lldb-server
Start-Job -ScriptBlock {
    & adb shell run-as com.beatgames.beatsaber ./lldb-server platform --listen *:$port --server
}

# Stop Beat Saber and start it again
& adb shell am force-stop com.beatgames.beatsaber
& adb shell am start -n com.beatgames.beatsaber/com.unity3d.player.UnityPlayerActivity
# Get Beat Saber PID
$bsPID = & adb shell pidof com.beatgames.beatsaber
Write-Output "Waiting for debugger on PID: $bsPID..."


$deviceConnection = (adb devices | ForEach-Object{(-split $_)})[4] + ":$port"
Write-Output "Connection string: $deviceConnection"


$debugString = "vscode://vadimcn.vscode-lldb/launch/config?{request: 'attach',pid: $bsPID,program: '',initCommands: ['platform select remote-android','settings set target.inherit-env false','platform connect connect://$deviceConnection'],postRunCommands: ['pro hand -p true -s false SIGPWR','pro hand -p true -s false SIGXCPU','pro hand -p true -s false SIG33',$symbolsListing]}"

& code --open-url $debugString

& adb logcat -c
& adb logcat > log.log