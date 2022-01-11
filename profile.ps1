$NDKPath = Get-Content $PSScriptRoot/ndkpath.txt

python $NDKPath/simpleperf/app_profiler.py -p com.beatgames.beatsaber -a com.unity3d.player.UnityPlayerActivity -lib .\build\debug\ --ndk_path $NDKPath --record_options=--exit-with-parent -r "--call-graph fp"

if ($?) {
    python $NDKPath/simpleperf/report_html.py
}