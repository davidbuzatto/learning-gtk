@ECHO OFF

REM Custom build script (batch file).
REM
REM Usage:
REM    .\build.bat: clean, compile and run
REM    .\build.bat -clean: clean compiled file
REM    .\build.bat -cleanAndCompile: clean compiled file and compile the project
REM    .\build.bat -compile: compile the project
REM    .\build.bat -compileAndRun: compile the project and run the compiled file
REM    .\build.bat -run: run the compiled file
REM
REM Author: Prof. Dr. David Buzatto

SET switch=%1
SHIFT

SET currentStep=-1

FOR %%I in (.) DO SET CurrentFolderName=%%~nxI
SET CompiledFile=%CurrentFolderName%.exe

IF "%switch%"=="" GOTO allSteps
IF "%switch%"=="-clean" GOTO cleanSteps
IF "%switch%"=="-cleanAndCompile" GOTO cleanAndCompileSteps
IF "%switch%"=="-compile" GOTO compileSteps
IF "%switch%"=="-compileAndRun" GOTO compileAndRunSteps
IF "%switch%"=="-run" GOTO runSteps

:allSteps
SET steps[0]=clean
SET steps[1]=compile
SET steps[2]=run
SET steps[3]=end
GOTO nextStep

:cleanSteps
SET steps[0]=clean
SET steps[1]=end
GOTO nextStep

:cleanAndCompileSteps
SET steps[0]=clean
SET steps[1]=compile
SET steps[2]=end
GOTO nextStep

:compileSteps
SET steps[0]=compile
SET steps[1]=end
GOTO nextStep

:compileAndRunSteps
SET steps[0]=compile
SET steps[1]=run
SET steps[2]=end
GOTO nextStep

:runSteps
SET steps[0]=run
SET steps[1]=end
GOTO nextStep

:nextStep
SET /A currentStep=%currentStep%+1
CALL GOTO %%steps[%currentStep%]%%

:clean
ECHO Cleaning...
IF EXIST %CompiledFile% DEL %CompiledFile%
GOTO nextStep

:compile
ECHO Compiling...
gcc src/*.c -o %CompiledFile% -IC:/msys64/ucrt64/include/gtk-4.0 -IC:/msys64/ucrt64/include/pango-1.0 -IC:/msys64/ucrt64/include/fribidi -IC:/msys64/ucrt64/include/harfbuzz -IC:/msys64/ucrt64/include/gdk-pixbuf-2.0 -IC:/msys64/ucrt64/include/webp -DLIBDEFLATE_DLL -IC:/msys64/ucrt64/include/cairo -IC:/msys64/ucrt64/include/freetype2 -IC:/msys64/ucrt64/include/libpng16 -IC:/msys64/ucrt64/include/pixman-1 -IC:/msys64/ucrt64/include/graphene-1.0 -IC:/msys64/ucrt64/lib/graphene-1.0/include -mfpmath=sse -msse -msse2 -IC:/msys64/ucrt64/include/glib-2.0 -IC:/msys64/ucrt64/lib/glib-2.0/include -O1 -Wall -Wextra -Wno-unused-parameter -pedantic-errors -std=c99 -Wno-missing-braces -I src/include/ -L lib/ -lgtk-4 -lpangocairo-1.0 -lpangowin32-1.0 -lpango-1.0 -lgdk_pixbuf-2.0 -lcairo-gobject -lcairo -lharfbuzz -lvulkan-1 -lgraphene-1.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 -lintl
GOTO nextStep

:run
ECHO Running...
IF EXIST %CompiledFile% ( %CompiledFile% ) ELSE ( ECHO %CompiledFile%% does not exists! )
GOTO nextStep

:end