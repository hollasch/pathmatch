
# --------------------------------------------------------------------- Macros

srhlib   = ..\srhlib
includes = -I$(srhlib)

!if !defined(DEBUG)
DEBUG=1
!elseif ($(DEBUG)==0)
DEBUG=0
!else
DEBUG=1
!endif

!if ($(DEBUG))
cflags = -nologo -EHsc -Zi -W4 -WX -DDEBUG=1 $(includes)
!else
cflags = -nologo -EHsc -Ox -GF -GS -W4 -WX $(includes)
!endif

# ---------------------------------------------------------------------- Rules

all: pathmatch

pathmatch: pathmatch.exe

pathmatcher.obj: $(srhlib)\pathmatcher.h $(srhlib)\pathmatcher.cpp \
                 $(srhlib)\wildcomp.h
    cl $(cflags) -c $(srhlib)\pathmatcher.cpp

wildcomp.obj: $(srhlib)\wildcomp.h $(srhlib)\wildcomp.cpp
    cl $(cflags) -c $(srhlib)\wildcomp.cpp

pathmatch.exe: pathmatch.cpp pathmatcher.obj wildcomp.obj
    cl $(cflags) -Fepathmatch.exe pathmatch.cpp pathmatcher.obj wildcomp.obj

# --------------------------------------

clean: 
    -del 2>nul *.obj

clobber: clean
    -del 2>nul *.exe

fresh: clobber all

# --------------------------------------

$(BINDIR)\pathmatch.exe: pathmatch.exe
    if defined BINDIR copy /y $? %%BINDIR%%

install: $(BINDIR)\pathmatch.exe
