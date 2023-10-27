# PictureEditor
Take bmp files and duplicate with a requested filer

gcc -o myprogram BreenFilter.c PixelProcessor.c BmpProcessor.c -pthread
myprogram.exe -o out.bmp -i in.bmp -f b

where "c" is for swiss cheese filter or "b" for box blur filter.
order of inputs is irrelevent.
