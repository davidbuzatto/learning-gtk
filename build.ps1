# Custom build script (windows powershell).
#
# Usage:
#    .\build.ps1: clean, compile and run
#    .\build.ps1 -clean: clean compiled file
#    .\build.ps1 -cleanAndCompile: clean compiled file and compile the project
#    .\build.ps1 -compile: compile the project
#    .\build.ps1 -compileAndRun: compile the project and run the compiled file
#    .\build.ps1 -run: run the compiled file
#
# Author: Prof. Dr. David Buzatto

param(
    [switch]$clean,
    [switch]$cleanAndCompile,
    [switch]$compile,
    [switch]$compileAndRun,
    [switch]$run
);

$CurrentFolderName = Split-Path -Path (Get-Location) -Leaf
$CompiledFile = "$CurrentFolderName.exe"

$all = $false
if ( -not( $clean -or $cleanAndCompile -or $compile -or $compileAndRun -or $run ) ) {
    $all = $true
}

# clean
if ( $clean -or $cleanAndCompile -or $all ) {
    Write-Host "Cleaning..."
    if ( Test-Path $CompiledFile ) {
        Remove-Item $CompiledFile
    }
}

# compile
if ( $compile -or $cleanAndCompile -or $compileAndRun -or $all ) {
    Write-Host "Compiling..."
    gcc src/*.c -o $CompiledFile `
        -IC:/msys64/ucrt64/include/gtk-4.0 `
        -IC:/msys64/ucrt64/include/pango-1.0 `
        -IC:/msys64/ucrt64/include/fribidi `
        -IC:/msys64/ucrt64/include/harfbuzz `
        -IC:/msys64/ucrt64/include/gdk-pixbuf-2.0 `
        -IC:/msys64/ucrt64/include/webp `
        -DLIBDEFLATE_DLL `
        -IC:/msys64/ucrt64/include/cairo `
        -IC:/msys64/ucrt64/include/freetype2 `
        -IC:/msys64/ucrt64/include/libpng16 `
        -IC:/msys64/ucrt64/include/pixman-1 `
        -IC:/msys64/ucrt64/include/graphene-1.0 `
        -IC:/msys64/ucrt64/lib/graphene-1.0/include `
        -mfpmath=sse `
        -msse `
        -msse2 `
        -IC:/msys64/ucrt64/include/glib-2.0 `
        -IC:/msys64/ucrt64/lib/glib-2.0/include `
        -O1 `
        -Wall `
        -Wextra `
        -Wno-unused-parameter `
        -pedantic-errors `
        -std=c99 `
        -Wno-missing-braces `
        -I src/include/ `
        -L lib/ `
        -lgtk-4 `
        "-lpangocairo-1.0" `
        "-lpangowin32-1.0" `
        "-lpango-1.0" `
        "-lgdk_pixbuf-2.0" `
        -lcairo-gobject `
        -lcairo `
        -lharfbuzz `
        -lvulkan-1 `
        "-lgraphene-1.0" `
        "-lgio-2.0" `
        "-lgobject-2.0" `
        "-lglib-2.0" `
        -lintl
}

# run
if ( $run -or $compileAndRun -or $all ) {
    Write-Host "Running..."
    if ( Test-Path $CompiledFile ) {
        & .\$CompiledFile
    } else {
        Write-Host "$CompiledFile does not exists!"
    }
}