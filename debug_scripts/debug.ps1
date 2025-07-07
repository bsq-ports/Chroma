& $PSScriptRoot/../copy.ps1

$port = 5039

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
