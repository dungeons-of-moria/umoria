cl -c -AL -Os -Gs -Zp -Gt32 -DPC_CURSES *.c
link <mlink.lnk
exemod moria.exe /stack 3fff
rename moria.exe moria.xxx
exepack moria.xxx moria.exe
delete moria.xxx

