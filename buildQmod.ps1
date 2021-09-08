# Builds a .qmod file
& $PSScriptRoot/build.ps1

if ($?) {
    Compress-Archive -Path "./mod.json", "./libs/arm64-v8a/libchroma.so", "./libs/arm64-v8a/libbeatsaber-hook_2_3_0.so", "./libs/arm64-v8a/libcustom-json-data.so", "./libs/arm64-v8a/libtracks.so", "./libs/arm64-v8a/libquestui.so", "./libs/arm64-v8a/libcustom-types.so" -DestinationPath "./chroma_v0.1.0.zip" -Update
    Remove-Item "./chroma_v2.4.10q4.qmod"
    Rename-Item "./chroma_v0.1.0.zip" "./chroma_v2.4.10q4.qmod"
}
