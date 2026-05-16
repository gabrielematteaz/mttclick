cl /std:c++latest /utf-8 /EHsc /Fo:out\release\ /Fd:out\release\ /Fe:bin\release\mttclick /Iinclude /W4 /O2 /MD src\* user32.lib Shell32.lib /link /SUBSYSTEM:WINDOWS
