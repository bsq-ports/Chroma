#!/bin/bash

GDB_BIN="libs/arm64-v8a/gdbserver"
APP_ID="com.beatgames.beatsaber"
APP_ACTIVITY="${APP_ID}/com.unity3d.player.UnityPlayerActivity"
APP_PWD_DIR="/data/data/${APP_ID}"

echo "[*] Pushing GDB binary"

adb push "${GDB_BIN}" "/sdcard/gdbserver"

echo "[*] Starting ${APP_ACTIVITY} in debug mode..."

adb shell am start -n "${APP_ACTIVITY}"

sleep 2

APP_PID="$(adb shell pidof "${APP_ID}")"

echo "[*] ${APP_ID} launched with pid ${APP_PID}"
echo "[*] Configuring device..."

adb shell setprop debug.debuggerd.wait_for_debugger true
adb forward tcp:8700 "jdwp:$APP_PID"
adb forward tcp:2345 tcp:2345
adb shell run-as com.beatgames.beatsaber cp "/sdcard/gdbserver" "${APP_PWD_DIR}/gdbserver"
adb shell run-as com.beatgames.beatsaber chmod +x "${APP_PWD_DIR}/gdbserver"

sleep 2

#echo "[*] Running JDB...resuming process..."
#jdb -attach localhost:8700  -sourcepath . &
#JDB_PID=$!
#sleep 1
#kill $JDB_PID

echo "[*] Starting gdbserver in the background..."
adb shell run-as com.beatgames.beatsaber killall gdbserver
adb shell run-as com.beatgames.beatsaber "${APP_PWD_DIR}/gdbserver" --attach 0.0.0.0:2345 "$APP_PID"