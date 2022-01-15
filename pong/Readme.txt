To Enable Music, you need a mp3 file, rename it to "ov" (without .mp3) and let it in the folder of the output
You need Visual Studio -> Pong works currenty only on 64 bit OS!

-- You should build the Release 64x Version

After Building pong, you need to copy the following DLL files from folder \ffmpeg\bin : 
- avcodec-58.dll
- avdevice-58.dll
- avfilter-7.dll
- avformat-58.dll
- avutil-56.dll
- swresample-3.dll
- swscale-5.dll

and the following DLL files from folder \SFML-2.5.1\bin :
- sfml-graphics-2.dll
- sfml-system-2.dll
- sfml-window-2.dll

into the output folder of the Project (Propaply \x64\Release)

