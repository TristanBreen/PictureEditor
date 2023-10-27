# PictureEditor
Take bmp files and duplicate with a requested filer.

To execute:
gcc -o myprogram BreenFilter.c PixelProcessor.c BmpProcessor.c -pthread <br>
myprogram.exe -o out.bmp -i in.bmp -f b <br>

where "c" is for swiss cheese filter or "b" for box blur filter.<br>
order of inputs is irrelevent.<br>
