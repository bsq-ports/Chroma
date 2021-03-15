# Builds a .zip file for loading with BMBF
& $PSScriptRoot/build.ps1

if ($?) {
    Compress-Archive -Path "./libs/arm64-v8a/libbeatsaber-hook_1_1_5.so", "./libs/arm64-v8a/libcustom-json-data.so", "./libs/arm64-v8a/libchroma.so", "./bmbfmod.json" -DestinationPath "./chroma_v0.1.0.zip" -Update
}
