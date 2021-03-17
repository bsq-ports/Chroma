$NDKPath = Get-Content $PSScriptRoot/ndkpath.txt

python $NDKPath/simpleperf/app_profiler.py -p com.beatgames.beatsaber -lib .\obj\local\arm64-v8a\ --record_options=--exit-with-parent
python $NDKPath/simpleperf/report_html.py