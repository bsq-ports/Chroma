# Builds a .qmod file
& $PSScriptRoot/build.ps1

if ($?) {
    Compress-Archive -Path "./mod.json", "./libs/arm64-v8a/libchroma.so", "./libs/arm64-v8a/libbeatsaber-hook_1_2_4.so", "./libs/arm64-v8a/libcustom-json-data.so", "./libs/arm64-v8a/libquestui.so", "./libs/arm64-v8a/libbs-utils.so" -DestinationPath "./chroma_v0.1.0.zip" -Update
    Rename-Item "./chroma_v0.1.0.zip" "./chroma_v0.1.0.qmod"
}
