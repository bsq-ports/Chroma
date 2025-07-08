Param(
    [Parameter(Mandatory=$false)]
    [Switch]$clean,
    [Parameter(Mandatory=$false)]
    [Switch]$release
)

# if user specified clean, remove all build files
if ($clean.IsPresent)
{
    if (Test-Path -Path "build")
    {
        remove-item build -R
    }
}

$NDKPath = Get-Content $PSScriptRoot/ndkpath.txt

if (($clean.IsPresent) -or (-not (Test-Path -Path "build")))
{
    $out = new-item -Path build -ItemType Directory
}


# Set build type based on release flag
$buildType = if ($release.IsPresent) { "RelWithDebInfo" } else { "Debug" }

& cmake -B build -G "Ninja" -DCMAKE_BUILD_TYPE="$buildType" .
& cmake --build ./build 