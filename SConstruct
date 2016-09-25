# vim: syntax=python

import sys, os

win64CrossCompile = ARGUMENTS.get('win64', 0)
mingwCrossCompile = win64CrossCompile or ARGUMENTS.get('win32', 0)
linux32CrossCompile = 0
disableSDL = 0          # set this to 1 on Linux with SDL version 1.2.10
disableLua = 0
buildUtilities = 1
enableGLShaders = 1
enableDebug = 0
buildRelease = 1
useLuaJIT = 0           # for mingwCrossCompile, use LuaJIT instead of Lua 5.3

#env = Environment(ENV = os.environ)
#CacheDir("./.build_cache/win32" if mingwCrossCompile else "./.build_cache/native")

compilerFlags = ''
if buildRelease:
    if linux32CrossCompile or (mingwCrossCompile and not win64CrossCompile):
        compilerFlags = ' -march=pentium2 -mtune=generic '
if enableDebug and not buildRelease:
    compilerFlags = ' -Wno-long-long -Wshadow -Winline -g -O2 ' + compilerFlags
    compilerFlags = ' -Wall -W -ansi -pedantic ' + compilerFlags
else:
    compilerFlags = ' -Wall -O3 ' + compilerFlags
    compilerFlags = compilerFlags + ' -fno-inline-functions '
    compilerFlags = compilerFlags + ' -fomit-frame-pointer -ffast-math '

fltkConfig = 'fltk-config'

# -----------------------------------------------------------------------------

programNamePrefix = ""
buildingLinuxPackage = 0
if not mingwCrossCompile:
    if sys.platform[:5] == 'linux':
        try:
            instPrefix = os.environ["UB_INSTALLDIR"]
            if instPrefix:
                instPrefix += "/usr"
                buildingLinuxPackage = 1
        except:
            pass
    if not buildingLinuxPackage:
        instPrefix = os.environ["HOME"]
        instShareDir = instPrefix + "/.local/share"
    else:
        instShareDir = instPrefix + "/share"
    instBinDir = instPrefix + "/bin"
    instDataDir = instShareDir + "/ep128emu"
    instPixmapDir = instShareDir + "/pixmaps"
    instDesktopDir = instShareDir + "/applications"
    instROMDir = instDataDir + "/roms"
    instConfDir = instDataDir + "/config"
    instDiskDir = instDataDir + "/disk"
    programNamePrefix = "ep"

oldSConsVersion = 0
try:
    EnsureSConsVersion(0, 97)
except:
    print 'WARNING: using old SCons version'
    oldSConsVersion = 1

ep128emuLibEnvironment = Environment(ENV = { 'PATH' : os.environ['PATH'],
                                             'HOME' : os.environ['HOME'] })
if linux32CrossCompile:
    compilerFlags = ' -m32 ' + compilerFlags
ep128emuLibEnvironment.Append(CCFLAGS = Split(compilerFlags))
ep128emuLibEnvironment.Append(CPPPATH = ['.', './src'])
if not mingwCrossCompile:
    ep128emuLibEnvironment.Append(CPPPATH = ['/usr/local/include'])
if sys.platform[:6] == 'darwin':
    ep128emuLibEnvironment.Append(CPPPATH = ['/usr/X11R6/include'])
if not linux32CrossCompile:
    linkFlags = ' -L. '
else:
    linkFlags = ' -m32 -L. -L/usr/X11R6/lib '
ep128emuLibEnvironment.Append(LINKFLAGS = Split(linkFlags))
if mingwCrossCompile:
    if not win64CrossCompile:
        mingwPrefix = 'C:/mingw32'
        ep128emuLibEnvironment.Prepend(CCFLAGS = ['-m32'])
    else:
        mingwPrefix = 'C:/mingw64'
        ep128emuLibEnvironment.Prepend(CCFLAGS = ['-m64'])
    ep128emuLibEnvironment.Append(CPPPATH = [mingwPrefix + '/include'])
    if sys.platform[:3] == 'win':
        toolNamePrefix = ''
    elif win64CrossCompile:
        toolNamePrefix = 'x86_64-w64-mingw32-'
    else:
        toolNamePrefix = 'i686-w64-mingw32-'
    ep128emuLibEnvironment['AR'] = toolNamePrefix + 'ar'
    ep128emuLibEnvironment['CC'] = toolNamePrefix + 'gcc'
    ep128emuLibEnvironment['CPP'] = toolNamePrefix + 'cpp'
    ep128emuLibEnvironment['CXX'] = toolNamePrefix + 'g++'
    ep128emuLibEnvironment['LINK'] = toolNamePrefix + 'g++'
    ep128emuLibEnvironment['RANLIB'] = toolNamePrefix + 'ranlib'
    if not disableLua:
        if useLuaJIT:
            ep128emuLibEnvironment.Append(
                CPPPATH = [mingwPrefix + '/include/lua5.1'])
        else:
            ep128emuLibEnvironment.Append(
                CPPPATH = [mingwPrefix + '/include/lua5.3'])
    ep128emuLibEnvironment.Append(LIBS = ['comdlg32', 'comctl32', 'ole32',
                                          'uuid', 'ws2_32', 'winmm', 'gdi32',
                                          'user32', 'kernel32'])
    ep128emuLibEnvironment.Prepend(CCFLAGS = ['-mthreads'])
    ep128emuLibEnvironment.Prepend(LINKFLAGS = ['-mthreads'])

if not oldSConsVersion:
    ep128emuGUIEnvironment = ep128emuLibEnvironment.Clone()
else:
    ep128emuGUIEnvironment = ep128emuLibEnvironment.Copy()
if mingwCrossCompile:
    ep128emuGUIEnvironment.Prepend(LIBS = ['fltk'])
else:
    try:
        if not ep128emuGUIEnvironment.ParseConfig(
            '%s --use-images --cxxflags --ldflags' % fltkConfig):
            raise Exception()
    except:
        print 'WARNING: could not run fltk-config'
        ep128emuGUIEnvironment.Append(LIBS = ['fltk_images', 'fltk'])
        if not oldSConsVersion:
            ep128emuGUIEnvironment.Append(LIBS = ['fltk_jpeg', 'fltk_png',
                                                  'fltk_z'])
        ep128emuGUIEnvironment.Append(LIBS = ['X11'])

if not oldSConsVersion:
    ep128emuGLGUIEnvironment = ep128emuLibEnvironment.Clone()
else:
    ep128emuGLGUIEnvironment = ep128emuLibEnvironment.Copy()
if mingwCrossCompile:
    ep128emuGLGUIEnvironment.Prepend(LIBS = ['fltk_gl', 'fltk',
                                             'glu32', 'opengl32'])
else:
    try:
        if not ep128emuGLGUIEnvironment.ParseConfig(
            '%s --use-gl --use-images --cxxflags --ldflags' % fltkConfig):
            raise Exception()
    except:
        print 'WARNING: could not run fltk-config'
        ep128emuGLGUIEnvironment.Append(LIBS = ['fltk_images', 'fltk_gl'])
        ep128emuGLGUIEnvironment.Append(LIBS = ['fltk'])
        if not oldSConsVersion:
            ep128emuGLGUIEnvironment.Append(LIBS = ['fltk_jpeg', 'fltk_png',
                                                    'fltk_z'])
    if sys.platform[:5] == 'linux':
        ep128emuGLGUIEnvironment.Append(LIBS = ['GL', 'X11'])

ep128emuLibEnvironment['CPPPATH'] = ep128emuGLGUIEnvironment['CPPPATH']

imageLibTestProgram = '''
    #include <FL/Fl.H>
    #include <FL/Fl_Shared_Image.H>
    int main()
    {
      Fl_Shared_Image *tmp = Fl_Shared_Image::get("foo");
      tmp->release();
      return 0;
    }
'''

portAudioLibTestProgram = '''
    #include <stdio.h>
    #include <portaudio.h>
    int main()
    {
      (void) Pa_Initialize();
      (void) Pa_GetDeviceInfo(0);
      (void) Pa_Terminate();
      return 0;
    }
'''

def imageLibTest(env):
    usingJPEGLib = 'jpeg' in env['LIBS']
    usingPNGLib = 'png' in env['LIBS']
    usingZLib = 'z' in env['LIBS']
    if usingJPEGLib or usingPNGLib or usingZLib:
        if not oldSConsVersion:
            tmpEnv = env.Clone()
        else:
            tmpEnv = env.Copy()
        if usingJPEGLib:
            tmpEnv['LIBS'].remove('jpeg')
        if usingPNGLib:
            tmpEnv['LIBS'].remove('png')
        if usingZLib:
            tmpEnv['LIBS'].remove('z')
        tmpConfig = tmpEnv.Configure()
        if tmpConfig.TryLink(imageLibTestProgram, '.cpp'):
            tmpConfig.Finish()
            if usingJPEGLib:
                env['LIBS'].remove('jpeg')
            if usingPNGLib:
                env['LIBS'].remove('png')
            if usingZLib:
                env['LIBS'].remove('z')
        else:
            if (usingJPEGLib
                and not tmpConfig.CheckLib('jpeg', None, None, 'C++', 0)):
                env['LIBS'].remove('jpeg')
            if (usingPNGLib
                and not tmpConfig.CheckLib('png', None, None, 'C++', 0)):
                env['LIBS'].remove('png')
            if (usingZLib
                and not tmpConfig.CheckLib('z', None, None, 'C++', 0)):
                env['LIBS'].remove('z')
            tmpConfig.Finish()
            tmpConfig2 = env.Configure()
            if not tmpConfig2.TryLink(imageLibTestProgram, '.cpp'):
                print ' *** error: libjpeg, libpng, or zlib is not found'
                Exit(-1)
            tmpConfig2.Finish()

def portAudioLibTest(env, libNames):
    if not oldSConsVersion:
        tmpEnv = env.Clone()
    else:
        tmpEnv = env.Copy()
    if libNames.__len__() > 0:
        tmpEnv.Append(LIBS = libNames)
    tmpEnv.Append(LIBS = ['pthread'])
    if sys.platform[:5] == 'linux':
        tmpEnv.Append(LIBS = ['rt'])
    tmpConfig = tmpEnv.Configure()
    retval = tmpConfig.TryLink(portAudioLibTestProgram, '.c')
    tmpConfig.Finish()
    return retval

def checkPortAudioLib(env):
    alsaLibNeeded = 0
    jackLibNeeded = 0
    if not portAudioLibTest(env, []):
        if portAudioLibTest(env, ['asound']):
            alsaLibNeeded = 1
        elif portAudioLibTest(env, ['jack']):
            jackLibNeeded = 1
        elif portAudioLibTest(env, ['jack', 'asound']):
            alsaLibNeeded = 1
            jackLibNeeded = 1
        else:
            print ' *** error: PortAudio library is not found'
            Exit(-1)
    if jackLibNeeded:
        env.Append(LIBS = ['jack'])
    if alsaLibNeeded:
        env.Append(LIBS = ['asound'])

if not oldSConsVersion:
    imageLibTest(ep128emuGUIEnvironment)
    imageLibTest(ep128emuGLGUIEnvironment)

configure = ep128emuLibEnvironment.Configure()
if not configure.CheckCHeader('sndfile.h'):
    print ' *** error: libsndfile 1.0 is not found'
    Exit(-1)
if not configure.CheckCHeader('portaudio.h'):
    print ' *** error: PortAudio is not found'
    Exit(-1)
elif configure.CheckType('PaStreamCallbackTimeInfo', '#include <portaudio.h>'):
    havePortAudioV19 = 1
else:
    havePortAudioV19 = 0
    print 'WARNING: using old v18 PortAudio interface'
if not configure.CheckCXXHeader('FL/Fl.H'):
    if configure.CheckCXXHeader('/usr/include/fltk-1.1/FL/Fl.H'):
        ep128emuLibEnvironment.Append(CPPPATH = ['/usr/include/fltk-1.1'])
    else:
        print ' *** error: FLTK 1.1 is not found'
    Exit(-1)
fltkVersion13 = 0
if configure.CheckCXXHeader('FL/Fl_Cairo.H'):
    fltkVersion13 = 1
    if sys.platform[:5] == 'linux' and not mingwCrossCompile:
        ep128emuGUIEnvironment.Append(LIBS = ['Xinerama', 'Xft'])
        ep128emuGLGUIEnvironment.Append(LIBS = ['Xinerama', 'Xft'])
    # print 'WARNING: using FLTK 1.3.x - this may not work reliably yet'
else:
    ep128emuLibEnvironment.Append(CPPPATH = ['./Fl_Native_File_Chooser'])
    ep128emuGUIEnvironment.Append(CPPPATH = ['./Fl_Native_File_Chooser'])
    ep128emuGLGUIEnvironment.Append(CPPPATH = ['./Fl_Native_File_Chooser'])
if not configure.CheckCHeader('GL/gl.h'):
    print ' *** error: OpenGL is not found'
    Exit(-1)
if enableGLShaders:
    if not configure.CheckType('PFNGLCOMPILESHADERPROC',
                               '#include <GL/gl.h>\n#include <GL/glext.h>'):
        enableGLShaders = 0
        print 'WARNING: disabling GL shader support'
if configure.CheckCHeader('stdint.h'):
    ep128emuLibEnvironment.Append(CCFLAGS = ['-DHAVE_STDINT_H'])
if sys.platform[:5] == 'linux' and not mingwCrossCompile:
    if configure.CheckCHeader('linux/fd.h'):
        ep128emuLibEnvironment.Append(CCFLAGS = ['-DHAVE_LINUX_FD_H'])
if not disableSDL:
    haveSDL = configure.CheckCHeader('SDL/SDL.h')
else:
    haveSDL = 0
oldLuaVersion = 0
luaPkgName = ''
if not disableLua:
    haveLua = configure.CheckCHeader('lua.h')
    haveLua = haveLua and configure.CheckCHeader('lauxlib.h')
    haveLua = haveLua and configure.CheckCHeader('lualib.h')
    if not haveLua and sys.platform[:5] == 'linux' and not mingwCrossCompile:
        for pkgName in ['lua-5.1', 'lua51', 'lua-5.3', 'lua53',
                        'lua-5.2', 'lua52', 'lua']:
            print 'Checking for Lua package ' + pkgName + '...'
            try:
                if not ep128emuLibEnvironment.ParseConfig(
                           'pkg-config --silence-errors --cflags ' + pkgName):
                    raise Exception()
            except:
                continue
            luaPkgName = pkgName
            haveLua = 1
            break
    elif haveLua:
        if not configure.CheckType('lua_Integer',
                                   '#include <lua.h>\n#include <lauxlib.h>'):
            oldLuaVersion = 1
            print 'WARNING: using old Lua 5.0.x API'
else:
    haveLua = 0
configure.Finish()

if not havePortAudioV19:
    ep128emuLibEnvironment.Append(CCFLAGS = ['-DUSING_OLD_PORTAUDIO_API'])
if haveSDL:
    ep128emuLibEnvironment.Append(CCFLAGS = ['-DHAVE_SDL_H'])
if haveLua:
    ep128emuLibEnvironment.Append(CCFLAGS = ['-DHAVE_LUA_H'])
    if oldLuaVersion:
        ep128emuLibEnvironment.Append(CCFLAGS = ['-DUSING_OLD_LUA_API'])
if enableGLShaders:
    ep128emuLibEnvironment.Append(CCFLAGS = ['-DENABLE_GL_SHADERS'])
if not fltkVersion13:
    ep128emuLibEnvironment.Append(CCFLAGS = ['-DFLTK1'])

ep128emuGUIEnvironment['CCFLAGS'] = ep128emuLibEnvironment['CCFLAGS']
ep128emuGUIEnvironment['CPPPATH'] = ep128emuLibEnvironment['CPPPATH']
ep128emuGUIEnvironment['CXXFLAGS'] = ep128emuLibEnvironment['CXXFLAGS']
ep128emuGLGUIEnvironment['CCFLAGS'] = ep128emuLibEnvironment['CCFLAGS']
ep128emuGLGUIEnvironment['CPPPATH'] = ep128emuLibEnvironment['CPPPATH']
ep128emuGLGUIEnvironment['CXXFLAGS'] = ep128emuLibEnvironment['CXXFLAGS']

if buildRelease:
    ep128emuGUIEnvironment.Append(LINKFLAGS = ['-s'])
    ep128emuGLGUIEnvironment.Append(LINKFLAGS = ['-s'])

def fluidCompile(flNames):
    cppNames = []
    for flName in flNames:
        if flName.endswith('.fl'):
            cppName = flName[:-3] + '_fl.cpp'
            hppName = flName[:-3] + '_fl.hpp'
            Command([cppName, hppName], flName,
                    'fluid -c -o %s -h %s $SOURCES' % (cppName, hppName))
            cppNames += [cppName]
    return cppNames

ep128emuLibSources = Split('''
    src/bplist.cpp
    src/cfg_db.cpp
    src/debuglib.cpp
    src/display.cpp
    src/dotconf.c
    src/emucfg.cpp
    src/fileio.cpp
    src/fldisp.cpp
    src/gldisp.cpp
    src/guicolor.cpp
    src/joystick.cpp
    src/script.cpp
    src/snd_conv.cpp
    src/soundio.cpp
    src/system.cpp
    src/tape.cpp
    src/videorec.cpp
    src/vm.cpp
    src/vmthread.cpp
    src/wd177x.cpp
''')
if not fltkVersion13:
    ep128emuLibSources += ['Fl_Native_File_Chooser/Fl_Native_File_Chooser.cxx']
ep128emuLib = ep128emuLibEnvironment.StaticLibrary('ep128emu',
                                                   ep128emuLibSources)

# -----------------------------------------------------------------------------

if not oldSConsVersion:
    ep128LibEnvironment = ep128emuLibEnvironment.Clone()
else:
    ep128LibEnvironment = ep128emuLibEnvironment.Copy()
ep128LibEnvironment.Append(CPPPATH = ['./z80'])

ep128Lib = ep128LibEnvironment.StaticLibrary('ep128', Split('''
    src/dave.cpp
    src/ep128vm.cpp
    src/ioports.cpp
    src/memory.cpp
    src/nick.cpp
    z80/z80.cpp
    z80/z80funcs2.cpp
    src/epmemcfg.cpp
    src/ide.cpp
    src/snapshot.cpp
'''))

# -----------------------------------------------------------------------------

if not oldSConsVersion:
    zx128LibEnvironment = ep128emuLibEnvironment.Clone()
else:
    zx128LibEnvironment = ep128emuLibEnvironment.Copy()
zx128LibEnvironment.Append(CPPPATH = ['./z80'])

zx128Lib = zx128LibEnvironment.StaticLibrary('zx128', Split('''
    src/ay3_8912.cpp
    src/zx128vm.cpp
    src/zxioport.cpp
    src/zxmemory.cpp
    src/ula.cpp
    src/zx_snap.cpp
'''))

# -----------------------------------------------------------------------------

if not oldSConsVersion:
    cpc464LibEnvironment = ep128emuLibEnvironment.Clone()
else:
    cpc464LibEnvironment = ep128emuLibEnvironment.Copy()
cpc464LibEnvironment.Append(CPPPATH = ['./z80'])

cpc464Lib = cpc464LibEnvironment.StaticLibrary('cpc464', Split('''
    src/cpc464vm.cpp
    src/cpcio.cpp
    src/cpcmem.cpp
    src/crtc6845.cpp
    src/cpcvideo.cpp
    src/fdc765.cpp
    src/cpcdisk.cpp
    src/cpc_snap.cpp
'''))

# -----------------------------------------------------------------------------

if not oldSConsVersion:
    ep128emuEnvironment = ep128emuGLGUIEnvironment.Clone()
else:
    ep128emuEnvironment = ep128emuGLGUIEnvironment.Copy()
ep128emuEnvironment.Append(CPPPATH = ['./z80', './gui'])
if luaPkgName:
    # using pkg-config
    if not ep128emuEnvironment.ParseConfig('pkg-config --libs ' + luaPkgName):
        print ' *** error: Lua library is not found'
        Exit(-1)
elif haveLua:
    if not mingwCrossCompile:
        ep128emuEnvironment.Append(LIBS = ['lua'])
    elif not useLuaJIT:
        ep128emuEnvironment.Append(LIBS = ['lua53'])
    else:
        ep128emuEnvironment.Append(LIBS = ['lua51'])
    if oldLuaVersion:
        ep128emuEnvironment.Append(LIBS = ['lualib'])
if haveSDL:
    ep128emuEnvironment.Append(LIBS = ['SDL'])
ep128emuEnvironment.Append(LIBS = ['portaudio', 'sndfile'])
if not mingwCrossCompile:
    if not oldSConsVersion:
        checkPortAudioLib(ep128emuEnvironment)
    ep128emuEnvironment.Append(LIBS = ['pthread'])
    if sys.platform[:5] == 'linux':
        ep128emuEnvironment.Append(LIBS = ['rt'])
else:
    ep128emuEnvironment.Prepend(LINKFLAGS = ['-mwindows'])
ep128emuEnvironment.Prepend(LIBS = ['ep128', 'zx128', 'cpc464', 'ep128emu'])

ep128emuSources = ['gui/gui.cpp']
ep128emuSources += fluidCompile(['gui/gui.fl', 'gui/disk_cfg.fl',
                                 'gui/disp_cfg.fl', 'gui/kbd_cfg.fl',
                                 'gui/snd_cfg.fl', 'gui/vm_cfg.fl',
                                 'gui/debug.fl', 'gui/about.fl'])
ep128emuSources += ['gui/debugger.cpp', 'gui/monitor.cpp', 'gui/main.cpp']
if mingwCrossCompile:
    ep128emuResourceObject = ep128emuEnvironment.Command(
        'resource/resource.o',
        ['resource/ep128emu.rc', 'resource/cpc464emu.ico',
         'resource/ep128emu.ico', 'resource/zx128emu.ico'],
        toolNamePrefix + 'windres -v --use-temp-file '
        + '--preprocessor="gcc.exe -E -xc -DRC_INVOKED" '
        + '-o $TARGET resource/ep128emu.rc')
    ep128emuSources += [ep128emuResourceObject]
ep128emu = ep128emuEnvironment.Program('ep128emu', ep128emuSources)
Depends(ep128emu, ep128Lib)
Depends(ep128emu, zx128Lib)
Depends(ep128emu, cpc464Lib)
Depends(ep128emu, ep128emuLib)

if sys.platform[:6] == 'darwin':
    Command('ep128emu.app/Contents/MacOS/ep128emu', 'ep128emu',
            'mkdir -p ep128emu.app/Contents/MacOS ; cp -pf $SOURCES $TARGET')

# -----------------------------------------------------------------------------

if not oldSConsVersion:
    tapeeditEnvironment = ep128emuGUIEnvironment.Clone()
else:
    tapeeditEnvironment = ep128emuGUIEnvironment.Copy()
tapeeditEnvironment.Append(CPPPATH = ['./tapeutil'])
tapeeditEnvironment.Prepend(LIBS = ['ep128emu'])
if haveSDL:
    tapeeditEnvironment.Append(LIBS = ['SDL'])
tapeeditEnvironment.Append(LIBS = ['sndfile'])
tapeeditSources = fluidCompile(['tapeutil/tapeedit.fl'])
tapeeditSources += ['tapeutil/tapeio.cpp']
if not mingwCrossCompile:
    tapeeditEnvironment.Append(LIBS = ['pthread'])
else:
    tapeeditEnvironment.Prepend(LINKFLAGS = ['-mwindows'])
    tapeeditResourceObject = tapeeditEnvironment.Command(
        'resource/te_resrc.o',
        ['resource/tapeedit.rc', 'resource/tapeedit.ico'],
        toolNamePrefix + 'windres -v --use-temp-file '
        + '--preprocessor="gcc.exe -E -xc -DRC_INVOKED" '
        + '-o $TARGET resource/tapeedit.rc')
    tapeeditSources += [tapeeditResourceObject]
tapeedit = tapeeditEnvironment.Program('tapeedit', tapeeditSources)
Depends(tapeedit, ep128emuLib)

if sys.platform[:6] == 'darwin':
    Command('ep128emu.app/Contents/MacOS/tapeedit', 'tapeedit',
            'mkdir -p ep128emu.app/Contents/MacOS ; cp -pf $SOURCES $TARGET')

# -----------------------------------------------------------------------------

if not oldSConsVersion:
    makecfgEnvironment = ep128emuGUIEnvironment.Clone()
else:
    makecfgEnvironment = ep128emuGUIEnvironment.Copy()
makecfgEnvironment.Append(CPPPATH = ['./installer'])
makecfgEnvironment.Prepend(LIBS = ['ep128emu'])
if haveSDL:
    makecfgEnvironment.Append(LIBS = ['SDL'])
makecfgEnvironment.Append(LIBS = ['sndfile'])
if not mingwCrossCompile:
    makecfgEnvironment.Append(LIBS = ['pthread'])
else:
    makecfgEnvironment.Prepend(LINKFLAGS = ['-mwindows'])

makecfg = makecfgEnvironment.Program(programNamePrefix + 'makecfg',
    ['installer/makecfg.cpp'] + fluidCompile(['installer/mkcfg.fl']))
Depends(makecfg, ep128emuLib)

if sys.platform[:6] == 'darwin':
    Command('ep128emu.app/Contents/MacOS/makecfg', 'makecfg',
            'mkdir -p ep128emu.app/Contents/MacOS ; cp -pf $SOURCES $TARGET')

# -----------------------------------------------------------------------------

if buildUtilities:
    if not oldSConsVersion:
        compressLibEnvironment = ep128emuLibEnvironment.Clone()
    else:
        compressLibEnvironment = ep128emuLibEnvironment.Copy()
    compressLibEnvironment.Append(CPPPATH = ['./util/epcompress/src'])
    compressLib = compressLibEnvironment.StaticLibrary(
                      'epcompress', Split('''
                          util/epcompress/src/archive.cpp
                          util/epcompress/src/compress0.cpp
                          util/epcompress/src/compress2.cpp
                          util/epcompress/src/compress3.cpp
                          util/epcompress/src/compress.cpp
                          util/epcompress/src/decompress0.cpp
                          util/epcompress/src/decompress2.cpp
                          util/epcompress/src/decompress3.cpp
                          util/epcompress/src/sfxcode.cpp
                          util/epcompress/src/sfxdecomp.cpp
                      '''))
    if not oldSConsVersion:
        epcompressEnvironment = compressLibEnvironment.Clone()
    else:
        epcompressEnvironment = compressLibEnvironment.Copy()
    epcompressEnvironment.Prepend(LIBS = [compressLib])
    if buildRelease:
        epcompressEnvironment.Append(LINKFLAGS = ['-s'])
    epcompress = epcompressEnvironment.Program(
                     'epcompress', ['util/epcompress/src/main.cpp'])
    Depends(epcompress, compressLib)
    if not oldSConsVersion:
        dtfEnvironment = epcompressEnvironment.Clone()
    else:
        dtfEnvironment = epcompressEnvironment.Copy()
    dtf = dtfEnvironment.Program('dtf', ['util/dtf/dtf.cpp'])
    Depends(dtf, compressLib)
    if not oldSConsVersion:
        epimgconvEnvironment = epcompressEnvironment.Clone()
    else:
        epimgconvEnvironment = epcompressEnvironment.Copy()
    epimgconvEnvironment['CCFLAGS'] = ep128emuGUIEnvironment['CCFLAGS']
    epimgconvEnvironment['CPPPATH'] = ep128emuGUIEnvironment['CPPPATH']
    epimgconvEnvironment['CXXFLAGS'] = ep128emuGUIEnvironment['CXXFLAGS']
    epimgconvEnvironment['LIBS'] = ep128emuGUIEnvironment['LIBS']
    epimgconvEnvironment.Append(CPPPATH = ['./util/epcompress/src'])
    if not mingwCrossCompile:
        epimgconvEnvironment.Prepend(LIBS = [compressLib])
    else:
        epimgconvEnvironment.Prepend(LIBS = [compressLib, 'fltk_images',
                                             'fltk_png', 'fltk_jpeg', 'z'])
    epimgconv = epimgconvEnvironment.Program(
                    'epimgconv', Split('''
                        util/epimgconv/src/attr16.cpp
                        util/epimgconv/src/epimgconv.cpp
                        util/epimgconv/src/imageconv.cpp
                        util/epimgconv/src/imgwrite.cpp
                        util/epimgconv/src/main.cpp
                        util/epimgconv/src/pixel16_1.cpp
                        util/epimgconv/src/pixel16_2.cpp
                        util/epimgconv/src/pixel256.cpp
                        util/epimgconv/src/pixel2.cpp
                        util/epimgconv/src/pixel4.cpp
                    '''))
    Depends(epimgconv, compressLib)

# -----------------------------------------------------------------------------

if not mingwCrossCompile:
    if buildingLinuxPackage:
        makecfgEnvironment.InstallAs([instBinDir + "/ep128emu.bin",
                                      instBinDir + "/ep128emu"],
                                     [ep128emu, "installer/ep128emu"])
    else:
        makecfgEnvironment.Install(instBinDir, ep128emu)
    makecfgEnvironment.Install(instBinDir,
                               [tapeedit, makecfg,
                                "resource/cpc464emu", "resource/zx128emu"])
    if buildUtilities:
        makecfgEnvironment.Install(instBinDir, [dtf, epcompress, epimgconv])
    makecfgEnvironment.Install(instPixmapDir,
                               ["resource/cpc464emu.png",
                                "resource/ep128emu.png",
                                "resource/tapeedit.png",
                                "resource/zx128emu.png"])
    makecfgEnvironment.Install(instDesktopDir,
                               ["resource/cpc464emu.desktop",
                                "resource/ep128.desktop",
                                "resource/eptapeedit.desktop",
                                "resource/zx128.desktop"])
    if not buildingLinuxPackage:
        makecfgEnvironment.Command(
            instROMDir + "/ep128emu_roms.bin", None,
            ['curl -o "' + instROMDir + '/ep128emu_roms.bin" '
             + 'http://ep128emu.enterpriseforever.com/roms/ep128emu_roms.bin'])
        makecfgEnvironment.Command(
            instROMDir + "/exos232uk.rom",
            [makecfg, instROMDir + "/ep128emu_roms.bin"],
            ['./' + programNamePrefix + 'makecfg -f "' + instDataDir + '"'])
    else:
        makecfgEnvironment.Command(instROMDir, None, [])
    makecfgEnvironment.Install(instConfDir,
                               ["config/clearkbd.cfg", "config/ep_keys.cfg"])
    makecfgEnvironment.Install(instDiskDir,
                               ["disk/disk.zip", "disk/ide126m.vhd.bz2",
                                "disk/ide189m.vhd.bz2"])
    makecfgEnvironment.Alias("install",
                             [instBinDir, instPixmapDir, instDesktopDir,
                              instDataDir, instROMDir, instConfDir,
                              instDiskDir])

