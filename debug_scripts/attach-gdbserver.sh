GDB_BIN="libs/arm64-v8a/gdbserver"
APP_ID="com.beatgames.beatsaber"
APP_ACTIVITY="${APP_ID}/com.unity3d.player.UnityPlayerActivity"
APP_PWD_DIR="/data/data/${APP_ID}"
APP_PID="$(adb shell pidof "${APP_ID}")"

echo "[*] Starting gdbserver in the background..."
adb shell run-as com.beatgames.beatsaber killall gdbserver
adb shell run-as com.beatgames.beatsaber "${APP_PWD_DIR}/gdbserver" --attach 0.0.0.0:2345 "$APP_PID"