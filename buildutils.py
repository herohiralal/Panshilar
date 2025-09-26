import os, sys, subprocess, uuid, random
from pathlib import Path
from dataclasses import dataclass, asdict

# region Commandline arguments ================================================================================================

CMD_ARG_SILENT      = '-silent'      in sys.argv # Suppress output from the build script (but not from the compiler/linker)
CMD_ARG_VERY_SILENT = '-very-silent' in sys.argv # Suppress all output from the build script (including compiler/linker output)
CMD_ARG_HOST_ONLY   = '-host-only'   in sys.argv # Only build for the host platform

# endregion

# region Global Config ========================================================================================================

# Use Native App Glue for Android builds (requires NDK); if you're using Dvaarpaal, or any derivatives, it includes a
# separate version of GameActivity, which contains its own version of Native App Glue, so you wouldn't need this.
# This is only present here, because TestRunner technically needs it.
G_UseNativeAppGlueForAndroid = False

# endregion

# region Global State =========================================================================================================

succeededProcesses: list[str] = []  # List to track successful processes
failedProcesses:    list[str] = []  # List to track failed processes

# endregion

#region Helper functions ======================================================================================================

def print2(message: str):
    if CMD_ARG_SILENT or CMD_ARG_VERY_SILENT:
        return

    print(message)

def printSectionStart():
    print2('')
    print2('=' * 80)

def printSectionEnd():
    print2('=' * 80)
    print2('')

def printDebug(message: str):
    print2(  f'\033[1m[DEBUG]:    \033[0m\033[90m{message}\033[0m')

def printInfo(message: str):
    print2(f'\033[1;36m[INFO]:    \033[0m{message}')

def printWarn(message: str):
    print2(f'\033[1;33m[WARNING]: \033[0m{message}')

def printErr(message: str):
    print2(f'\033[1;31m[ERROR]:   \033[0m{message}')

def printSuccess(message: str):
    print2(f'\033[1;32m[SUCCESS]: \033[0m{message}')

def printFailure(message: str):
    print2(f'\033[1;31m[FAILURE]: \033[0m{message}')

def runCommand(command: list[str], name: str) -> bool:
    printSectionStart()
    printInfo(f'Running: {name}')
    printDebug(f'Command: {' '.join(command)}')
    if CMD_ARG_VERY_SILENT:
        result = subprocess.run(command, stdout = subprocess.DEVNULL, stderr = subprocess.DEVNULL)
    else:
        result = subprocess.run(command, stdout = sys.stdout, stderr = sys.stderr)

    if result.returncode == 0:
        printSuccess(f'Completed successfully: {name}')
        succeededProcesses.append(name)
    else:
        printFailure(f'Failed to complete: {name}')
        failedProcesses.append(name)
    printSectionEnd()
    return result.returncode == 0

def printSummary():
    printSectionStart()

    if succeededProcesses:
        printInfo('Succeeded processes:')
        for process in succeededProcesses:
            print2(f' - {process}')

    if failedProcesses:
        printErr('Failed processes:')
        for process in failedProcesses:
            print2(f' - {process}')
        printFailure('One or more processes failed. Please check the output above for details.')
    else:
        printSuccess('All processes completed successfully!')

    printSectionEnd()

# endregion

# region Toolchains ===========================================================================================================

@dataclass
class Toolchains:
    windowsToolchain:    str = ''
    linuxX64Toolchain:   str = ''
    linuxArm64Toolchain: str = ''
    androidToolchain:    str = ''
    osxTools:            str = ''
    osxSdk:              str = ''
    iosSdk:              str = ''
    iosSimSdk:           str = ''

def getToolchainPath(prettyTgt: str, prettyArch: str, statusBoolEnvVarName: str, toolchainEnvVarName: str) -> str:
    toolchainFound = '1' == os.getenv(statusBoolEnvVarName)
    if not toolchainFound:
        printWarn(f'{prettyTgt}-{prettyArch} toolchain not found!')
        return ''

    toolchainPath = os.getenv(toolchainEnvVarName)
    if not toolchainPath:
        printErr(f'{prettyTgt}-{prettyArch} toolchain path not set!')
        failedProcesses.append(f'{prettyTgt}-{prettyArch} Toolchain Path Query')
        return ''

    printInfo(f'Using {prettyTgt} {prettyArch} toolchain at: {toolchainPath}.')
    return toolchainPath

def getToolchains() -> Toolchains:
    toolch: Toolchains = Toolchains()
    if sys.platform == 'win32':
        printSectionStart()
        toolch.windowsToolchain    = getToolchainPath('Windows', 'x64',   'WINDOWS_TOOLCHAIN_FOUND',     'WINDOWS_TOOLCHAIN')
        toolch.linuxX64Toolchain   = getToolchainPath('Linux',   'x64',   'LINUX_X64_TOOLCHAIN_FOUND',   'LINUX_X64_TOOLCHAIN')
        toolch.linuxArm64Toolchain = getToolchainPath('Linux',   'ARM64', 'LINUX_ARM64_TOOLCHAIN_FOUND', 'LINUX_ARM64_TOOLCHAIN')
        toolch.androidToolchain    = getToolchainPath('Android', 'ARM64', 'ANDROID_TOOLCHAIN_FOUND',     'ANDROID_TOOLCHAIN')
        printSectionEnd()

    elif sys.platform == 'darwin':
        printSectionStart()
        toolch.osxTools = os.getenv('BUILD_TOOLS_PATH', '')
        toolch.iosSdk   = getToolchainPath('iOS',           'ARM64', 'IOS_SDK_FOUND',     'IOS_SDK_PATH')
        toolch.iosSdk   = getToolchainPath('iOS-Simulator', 'ARM64', 'IOS_SIM_SDK_FOUND', 'IOS_SIM_SDK_PATH')
        toolch.osxSdk   = getToolchainPath('macOS',         'ARM64', 'OSX_SDK_FOUND',     'OSX_SDK_PATH')
        printSectionEnd()

    else:
        raise NotImplementedError(f'Unsupported platform: {sys.platform}')

    return toolch

TOOLCHAINS = getToolchains()

# endregion

# region Platforms ============================================================================================================

@dataclass
class Platform:
    tgt:         str
    arch:        str
    prettyTgt:   str
    prettyArch:  str
    toolch:      str

def getPlatforms(all: bool) -> list[Platform]:
    platforms: list[Platform] = []

    if TOOLCHAINS.windowsToolchain:
        platforms.append(Platform('windows', 'x64', 'Windows', 'x64', TOOLCHAINS.windowsToolchain))

    if TOOLCHAINS.osxSdk:
        platforms.append(Platform('osx', 'arm64', 'macOS', 'ARM64', TOOLCHAINS.osxSdk))

    if all or not CMD_ARG_HOST_ONLY: # cross compilation platforms

        if TOOLCHAINS.linuxX64Toolchain:
            platforms.append(Platform('linux', 'x64', 'Linux', 'x64', TOOLCHAINS.linuxX64Toolchain))

        if TOOLCHAINS.linuxArm64Toolchain:
            platforms.append(Platform('linux', 'arm64', 'Linux', 'ARM64', TOOLCHAINS.linuxArm64Toolchain))

        if TOOLCHAINS.androidToolchain:
            platforms.append(Platform('android', 'x64', 'Android', 'x64', TOOLCHAINS.androidToolchain))
            platforms.append(Platform('android', 'arm64', 'Android', 'ARM64', TOOLCHAINS.androidToolchain))

        if TOOLCHAINS.iosSdk:
            platforms.append(Platform('ios', 'arm64', 'iOS', 'ARM64', TOOLCHAINS.iosSdk))

        if TOOLCHAINS.iosSimSdk:
            platforms.append(Platform('iossimulator', 'arm64', 'iOS-Simulator', 'ARM64', TOOLCHAINS.iosSimSdk))

    return platforms

VALID_PLATFORMS    = getPlatforms(True)
PLATFORMS_TO_BUILD = getPlatforms(False)

def getPlatformTgtDefine(plt: Platform) -> str:
    if plt.tgt == 'windows':
        return 'PNSLR_WINDOWS'
    elif plt.tgt == 'linux':
        return 'PNSLR_LINUX'
    elif plt.tgt == 'osx':
        return 'PNSLR_OSX'
    elif plt.tgt == 'android':
        return 'PNSLR_ANDROID'
    elif plt.tgt == 'ios' or plt.tgt == 'iossimulator':
        return 'PNSLR_IOS'
    else:
        raise NotImplementedError(f'Unsupported platform target: {plt.tgt}')

def getPlatformArchDefine(plt: Platform) -> str:
    if plt.arch == 'x64':
        return 'PNSLR_X64'
    elif plt.arch == 'arm64':
        return 'PNSLR_ARM64'
    else:
        raise NotImplementedError(f'Unsupported platform architecture: {plt.arch}')

def getCCompiler(plt: Platform) -> str:
    if plt.tgt == 'windows':
        return os.path.join(plt.toolch, 'bin', 'HostX64', 'x64', 'cl.exe')
    elif plt.tgt == 'linux':
        return os.path.join(plt.toolch, 'bin', 'clang.exe')
    elif plt.tgt == 'android':
        return os.path.join(plt.toolch, 'toolchains', 'llvm', 'prebuilt', 'windows-x86_64', 'bin', 'clang.exe')
    elif plt.tgt == 'osx' or plt.tgt == 'ios' or plt.tgt == 'iossimulator':
        return os.path.join(TOOLCHAINS.osxTools, 'usr', 'bin', 'clang')
    else:
        raise NotImplementedError(f'Unsupported platform target: {plt.tgt}')

def getCxxCompiler(plt: Platform) -> str:
    if plt.tgt == 'windows':
        return os.path.join(plt.toolch, 'bin', 'HostX64', 'x64', 'cl.exe')
    elif plt.tgt == 'linux':
        return os.path.join(plt.toolch, 'bin', 'clang++.exe')
    elif plt.tgt == 'android':
        return os.path.join(plt.toolch, 'toolchains', 'llvm', 'prebuilt', 'windows-x86_64', 'bin', 'clang++.exe')
    elif plt.tgt == 'osx' or plt.tgt == 'ios' or plt.tgt == 'iossimulator':
        return os.path.join(TOOLCHAINS.osxTools, 'usr', 'bin', 'clang++')
    else:
        raise NotImplementedError(f'Unsupported platform target: {plt.tgt}')

def getStaticLibLinker(plt: Platform) -> str:
    if plt.tgt == 'windows':
        return os.path.join(plt.toolch, 'bin', 'HostX64', 'x64', 'lib.exe')
    elif plt.tgt == 'linux':
        return os.path.join(plt.toolch, 'bin', 'llvm-ar.exe')
    elif plt.tgt == 'android':
        return os.path.join(plt.toolch, 'toolchains', 'llvm', 'prebuilt', 'windows-x86_64', 'bin', 'llvm-ar.exe')
    elif plt.tgt == 'osx':
        return os.path.join(TOOLCHAINS.osxTools, 'usr', 'bin', 'libtool')
    elif plt.tgt == 'ios' or plt.tgt == 'iossimulator':
        return os.path.join(TOOLCHAINS.osxTools, 'usr', 'bin', 'ar')
    else:
        raise NotImplementedError(f'Unsupported platform target: {plt.tgt}')

def getObjectOutputFileName(
        name: str,
        plt:  Platform,
    ) -> str:
    return f'{name}-{plt.tgt}-{plt.arch}.{'obj' if plt.tgt == 'windows' else 'o'}'

def getStaticLibOutputFileName(
        name: str,
        plt:  Platform,
    ) -> str:
    return f'{'' if plt.tgt == 'windows' else 'lib'}{name}-{plt.tgt}-{plt.arch}.{'lib' if plt.tgt == 'windows' else 'a'}'

def getDynamicLibOutputFileName(
        name: str,
        plt:  Platform,
    ) -> str:
    if plt.tgt == 'windows':
        return f'{name}-{plt.tgt}-{plt.arch}.dll'
    elif plt.tgt == 'linux' or plt.tgt == 'android':
        return f'lib{name}-{plt.tgt}-{plt.arch}.so'
    elif plt.tgt == 'osx' or plt.tgt == 'ios' or plt.tgt == 'iossimulator':
        return f'lib{name}-{plt.tgt}-{plt.arch}.dylib'
    else:
        raise NotImplementedError(f'Unsupported platform target: {plt.tgt}')

def getExecOutputFileName(
        name: str,
        plt:  Platform,
    ) -> str:
    return f'{name}-{plt.tgt}-{plt.arch}{'.exe' if plt.tgt == 'windows' else ''}'

def getCommonCompilationArgs(
        plt:         Platform,
        dbg:         bool,
        compileOnly: bool,
        addEnvArgs:  bool = False,
        addStdArgs:  bool = False,
        useCxx:      bool = False,
    ) -> list[str]:

    output: list[str]
    if plt.tgt == 'windows':
        output = ['/Brepro', '/nologo', '/Wall', '/WX']
    else:
        output = ['-Werror']

    if dbg:
        if plt.tgt == 'windows':
            output += ['/Zi', '/Od', '/D_DEBUG']
        else:
            output += ['-g', '-O0', '-DDEBUG']
    else:
        if plt.tgt == 'windows':
            output += ['/O2', '/DNDEBUG', '/GL']
        else:
            output += ['-O2', '-DNDEBUG', '-flto']

    if compileOnly:
        output += ['/c'] if plt.tgt == 'windows' else ['-c']

    if addEnvArgs:
        specifier: str = '/D' if plt.tgt == 'windows' else '-D'
        output += [f'{specifier}{getPlatformTgtDefine(plt)}=1', f'{specifier}{getPlatformArchDefine(plt)}=1']
        output += [f'{specifier}{'PNSLR_DBG' if dbg else 'PNSLR_REL'}=1']
        if plt.tgt == 'android':
            output += ['-DANDROID=1', '-D_FORTIFY_SOURCE=2']

    if addStdArgs:
        specifier: str = '/std:' if plt.tgt == 'windows' else '-std='
        output += [f'{specifier}c++14'] if useCxx else [f'{specifier}c11']

    if plt.tgt == 'windows':
        pass
    elif plt.tgt == 'linux':
        output += [f'--sysroot={plt.toolch}\\']
        if plt.arch == 'x64':
            output += ['--target=x86_64-unknown-linux-gnu']
        elif plt.arch == 'arm64':
            output += ['--target=aarch64-unknown-linux-gnueabi']
        else:
            raise NotImplementedError(f'Unsupported architecture for Linux: {plt.arch}')
    elif plt.tgt == 'android':
        output += [
            f'--sysroot={plt.toolch}\\toolchains\\llvm\\prebuilt\\windows-x86_64\\sysroot\\',
            '-fPIC',
        ] + ([f'-I{plt.toolch}\\sources\\android\\native_app_glue\\'] if G_UseNativeAppGlueForAndroid else [])
        if plt.arch == 'x64':
            output += ['--target=x86_64-none-linux-android28']
        elif plt.arch == 'arm64':
            output += ['--target=aarch64-none-linux-android28']
        else:
            raise NotImplementedError(f'Unsupported architecture for Android: {plt.arch}')
    elif plt.tgt == 'osx':
        output += ['--sysroot', plt.toolch, '-target', 'arm64-apple-macos11.0']
    elif plt.tgt == 'ios':
        output += [
            '--sysroot',
            plt.toolch,
            '-miphoneos-version-min=16.0',
            '-target',
            'arm64-apple-ios16.0',
            '-arch',
            'arm64',
        ]
    elif plt.tgt == 'iossimulator':
        output += [
            '--sysroot',
            plt.toolch,
            '-miphoneos-version-min=16.0',
            '-target',
            'arm64-apple-ios16.0-simulator',
            '-arch',
            'arm64',
        ]
    else:
        raise NotImplementedError(f'Unsupported platform target: {plt.tgt}')

    return output

def getCompilationCommand(
        plt:      Platform,
        dbg:      bool,
        inputF:   str,
        outputF:  str,
        useCxx:   bool      = False,
    ) -> list[str]:
    output: list[str] = [(getCxxCompiler(plt) if useCxx else getCCompiler(plt))]

    output += getCommonCompilationArgs(
        plt          = plt,
        dbg          = dbg,
        compileOnly  = True,
        addEnvArgs   = True,
        addStdArgs   = True,
        useCxx       = useCxx,
    )

    output += [inputF]
    output += ['/Fo'+outputF] if plt.tgt == 'windows' else ['-o', outputF]

    if dbg and plt.tgt == 'windows':
        output += ['/Fd'+outputF.rstrip('.obj') + '.pdb']

    return output

def getStaticLibLinkCommand(
        plt:     Platform,
        inputFs: list[str],
        outputF: str,
    ) -> list[str]:
    output: list[str] = [getStaticLibLinker(plt)]
    if plt.tgt == 'windows':
        output += [
            '/Brepro',
            '/NOLOGO',
            '/OUT:' + outputF,
        ] + inputFs
    elif plt.tgt == 'osx':
        output += [
            '-static',
            '-o',
            outputF,
        ] + inputFs
    else:
        output += [
            'rcs',
            outputF,
        ] + inputFs

    return output

def getExecBuildCommand(
        plt:      Platform,
        dbg:      bool,
        inputFs:  list[str],
        sysLibs:  list[str],
        outputF:  str,
        useCxx:   bool = False,
    ) -> list[str]:
    output: list[str] = [(getCxxCompiler(plt) if useCxx else getCCompiler(plt))]

    output += getCommonCompilationArgs(
        plt          = plt,
        dbg          = dbg,
        compileOnly  = False,
        addEnvArgs   = True,
        addStdArgs   = True,
        useCxx       = useCxx,
    )

    output += inputFs
    output += ['/Fe'+outputF] if plt.tgt == 'windows' else ['-o', outputF]

    if plt.tgt == 'windows':
        output += sysLibs
        output += ['/DEBUG', '/Fd'+outputF.rstrip('.exe') + '.pdb'] if dbg else []
        output += ['/Fo'+os.path.dirname(outputF)+'/']
    else:
        for lib in sysLibs:
            output += ['-l' + lib]

    return output

def getDynamicLibBuildCommand(
        plt:      Platform,
        dbg:      bool,
        inputFs:  list[str],
        sysLibs:  list[str],
        outputF:  str,
        useCxx:   bool = False,
    ) -> list[str]:
    output = getExecBuildCommand(
        plt      = plt,
        dbg      = dbg,
        inputFs  = inputFs,
        sysLibs  = sysLibs,
        outputF  = outputF,
        useCxx   = useCxx,
    )

    if plt.tgt == 'windows':
        output += ['/LDd'] if dbg else ['/LD']
    else:
        output += ['-shared']
        if plt.tgt == 'linux':
            output += ['-fPIC']

    return output

# endregion

# region LSP Setup ============================================================================================================

def setupVsCodeLspStuff():
    @dataclass
    class CCppPropertiesConfiguration:
        name:             str
        compilerPath:     str
        cStandard:        str
        cppStandard:      str
        includePath:      list[str]
        defines:          list[str]
        compilerArgs:     list[str]

    @dataclass
    class CCppProperties:
        version:        int
        configurations: list[CCppPropertiesConfiguration]

    properties: CCppProperties = CCppProperties(version = 4, configurations = [])

    for plt in VALID_PLATFORMS:
        config: CCppPropertiesConfiguration = CCppPropertiesConfiguration(
            name = f'{plt.prettyTgt}-{plt.prettyArch}',
            compilerPath = getCCompiler(plt),
            cStandard = 'c11',
            cppStandard = 'c++14',
            includePath = [],
            defines = [
                f'{getPlatformTgtDefine(plt)}=1',
                f'{getPlatformArchDefine(plt)}=1',
                f'PNSLR_DBG=1',
            ],
            compilerArgs = getCommonCompilationArgs(
                plt          = plt,
                dbg          = False,
                compileOnly  = False,
                addEnvArgs   = False,
                addStdArgs   = False,
                useCxx       = False,
            ),
        )

        if plt.tgt == 'android':
            config.defines += ['ANDROID=1', '_FORTIFY_SOURCE=2']
            config.includePath += [
                f'{plt.toolch}\\toolchains\\llvm\\prebuilt\\windows-x86_64\\sysroot\\usr\\include'.replace('\\', '/'),
            ] + ([f'{plt.toolch}\\sources\\android\\native_app_glue'.replace('\\', '/')] if G_UseNativeAppGlueForAndroid else [])

        if plt.tgt == 'linux':
            config.includePath += [f'{plt.toolch}\\usr\\include'.replace('\\', '/')]

        properties.configurations.append(config)

    ccppPropsFile = '.vscode/c_cpp_properties.json'
    os.makedirs(os.path.dirname(ccppPropsFile), exist_ok = True)
    with open(ccppPropsFile, 'w', encoding = 'utf-8') as f:
        import json
        json.dump(asdict(properties), f, indent = 4)

# endregion

# region Folder Structure =====================================================================================================

@dataclass
class FolderStructure:
    root:   str
    binDir: str
    bndDir: str
    libDir: str
    tmpDir: str
    srcDir: str
    depDir: str

def getFolderStructure(root: str) -> FolderStructure:
    return FolderStructure(
        root   = root                                        .replace('\\', '/') + '/',
        binDir = os.path.join(root, 'Binaries')              .replace('\\', '/') + '/',
        bndDir = os.path.join(root, 'Bindings')              .replace('\\', '/') + '/',
        libDir = os.path.join(root, 'Libraries')             .replace('\\', '/') + '/',
        tmpDir = os.path.join(root, 'Temp')                  .replace('\\', '/') + '/',
        srcDir = os.path.join(root, 'Source')                .replace('\\', '/') + '/',
        depDir = os.path.join(root, 'Source', 'Dependencies').replace('\\', '/') + '/',
    )

# endregion

# region Android Project Generator ============================================================================================

def createAndroidProject(
        appName: str = "MyApp",
        pkgName: str = "com.example.myapp",
        projDir: str = "ProjectFiles/Android",
        grdlVer: str = "8.13",
        minSdk:  int = 29,
        tgtSdk:  int = 35,
        verCode: int = 1,
        verName: str = "1.0",
        cxxMain: str = "",
        cMain:   str = "",
    ):

    # Create directory structure
    dirs = [
        f"./{projDir}",
        f"./{projDir}/.idea",
        f"./{projDir}/app/src/main/cpp",
        f"./{projDir}/gradle/wrapper"
    ]

    # using game activity instead of native activity
    if not G_UseNativeAppGlueForAndroid:
        dirs += [f"./{projDir}/app/src/main/java/{pkgName.replace('.', '/')}"]

    for dirPath in dirs:
        Path(dirPath).mkdir(parents=True, exist_ok=True)

    # Root build.gradle.kts
    rootBuild = f"""\
plugins {{
    id("com.android.application") version "8.13.0" apply false
}}
"""

    # App build.gradle.kts
    appBuild = f"""\
plugins {{
    id("com.android.application")
}}

android {{
    namespace = "{pkgName}"
    compileSdk = {tgtSdk}

    defaultConfig {{
        applicationId = "{pkgName}"
        minSdk = {minSdk}
        targetSdk = {tgtSdk}
        versionCode = {verCode}
        versionName = "{verName}"

        ndk {{
            abiFilters += listOf("arm64-v8a", "x86_64")
        }}

        externalNativeBuild {{
            cmake {{
                cFlags += "-std=c11"
                cppFlags += "-std=c++14"
            }}
        }}
    }}

    buildTypes {{
        release {{
            isMinifyEnabled = false
        }}
    }}

    externalNativeBuild {{
        cmake {{
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }}
    }}
}}

dependencies {{
    implementation("androidx.appcompat:appcompat:1.6.1")
    implementation("androidx.games:games-activity:3.0.5")
}}
"""

    # settings.gradle.kts
    settings = f"""\
pluginManagement {{
    repositories {{
        google {{
            content {{
                includeGroupByRegex("com\\\\.android.*")
                includeGroupByRegex("com\\\\.google.*")
                includeGroupByRegex("androidx.*")
            }}
        }}
        mavenCentral()
        gradlePluginPortal()
    }}
}}
dependencyResolutionManagement {{
    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)
    repositories {{
        google()
        mavenCentral()
    }}
}}

rootProject.name = "{appName}"
include(":app")
"""

    # gradle.properties
    gradleProps = """\
org.gradle.jvmargs=-Xmx2048m -Dfile.encoding=UTF-8
org.gradle.parallel=true
android.useAndroidX=true
android.nonTransitiveRClass=true
"""

    # gradle-wrapper.properties
    wrapperProps = f"""\
distributionBase=GRADLE_USER_HOME
distributionPath=wrapper/dists
distributionUrl=https\\://services.gradle.org/distributions/gradle-{grdlVer}-bin.zip
zipStoreBase=GRADLE_USER_HOME
zipStorePath=wrapper/dists
"""

    mainActivity = ''
    if not G_UseNativeAppGlueForAndroid:
        mainActivity = f"""\
package {pkgName};

import android.view.View;
import com.google.androidgamesdk.GameActivity;

public class MainActivity extends GameActivity {{
    static {{
        System.loadLibrary("nativelib");
    }}

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {{
        super.onWindowFocusChanged(hasFocus);

        if (hasFocus) {{
            hideSystemUi();
        }}
    }}

    private void hideSystemUi() {{
        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                        | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_FULLSCREEN
        );
    }}
}}
"""

    # AndroidManifest.xml
    manifest = f"""\
<?xml version="1.0" encoding="utf-8"?>
<manifest
    xmlns:android="http://schemas.android.com/apk/res/android"
    android:installLocation="auto"
    xmlns:tools="http://schemas.android.com/tools">

    <application
        android:label="{appName}"
        android:theme="{\
            '@android:style/Theme.NoTitleBar.Fullscreen' \
                if G_UseNativeAppGlueForAndroid else \
            '@style/Theme.AppCompat.NoActionBar'}">
        <activity
            android:name="{'.MainActivity' if mainActivity else 'android.app.NativeActivity'}"
            android:exported="true">
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.LAUNCHER" />
            </intent-filter>
            <meta-data
                android:name="android.app.lib_name"
                android:value="nativelib" />
        </activity>
    </application>
</manifest>
"""

    # workspace.xml
    workspace = f"""\
<?xml version="1.0" encoding="UTF-8"?>
<project version="4">
  <component name="RunManager">
    <configuration name="app" type="AndroidRunConfigurationType" factoryName="Android App">
      <module name="{appName}.app" />
      <option name="CLEAR_LOGCAT" value="true" />
      <option name="SHOW_LOGCAT_AUTOMATICALLY" value="true" />
      <option name="DEBUGGER_TYPE" value="Native" />
    </configuration>
  </component>
</project>
"""

    nativeSymbolToPreserve = 'ANativeActivity_onCreate' if G_UseNativeAppGlueForAndroid else 'Java_com_google_androidgamesdk_GameActivity_initializeNativeCode'
    # CMakeLists.txt
    cmake = f"""\
cmake_minimum_required(VERSION 3.22.1)

project("nativelib")

add_library(nativelib SHARED
{'    nativelib.cpp\n' if cxxMain else ''}{'    nativelib.c\n'   if cMain   else ''})

target_compile_options(nativelib PRIVATE -Wall)
target_compile_definitions(nativelib PRIVATE PNSLR_ANDROID=1)

if (CMAKE_ANDROID_ARCH_ABI STREQUAL "x86_64")
    target_compile_definitions(nativelib PRIVATE PNSLR_X64=1)
elseif (CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a")
    target_compile_definitions(nativelib PRIVATE PNSLR_ARM64=1)
endif()

target_compile_definitions(nativelib PRIVATE
    $<$<CONFIG:Debug>:PNSLR_DBG=1>
    $<$<CONFIG:Release>:PNSLR_REL=1>
    $<$<CONFIG:RelWithDebInfo>:PNSLR_REL=1>
    $<$<CONFIG:MinSizeRel>:PNSLR_REL=1>
)\

{"""
target_include_directories(nativelib PRIVATE
    ${{ANDROID_NDK}}/sources/android/native_app_glue
)
""" if G_UseNativeAppGlueForAndroid else ''}\

set(CMAKE_SHARED_LINKER_FLAGS
    "${{CMAKE_SHARED_LINKER_FLAGS}} -u {nativeSymbolToPreserve}"
)

target_link_libraries(nativelib
    jnigraphics
    android
    log
)

set_source_files_properties(nativelib.c PROPERTIES
    LANGUAGE C
    C_STANDARD 11
)

set_source_files_properties(nativelib.cpp PROPERTIES
    LANGUAGE CXX
    CXX_STANDARD 14
)
"""


    gitignore = """\
*.iml
.gradle
/local.properties
/.idea
.DS_Store
/build
/captures
.externalNativeBuild
.cxx
local.properties

/app/build
/app/src/main/assets
"""

    # Write all files
    files = [
        (f"./{projDir}/.idea/workspace.xml",                      workspace),
        (f"./{projDir}/.gitignore",                               gitignore),
        (f"./{projDir}/build.gradle.kts",                         rootBuild),
        (f"./{projDir}/settings.gradle.kts",                      settings),
        (f"./{projDir}/gradle.properties",                        gradleProps),
        (f"./{projDir}/gradle/wrapper/gradle-wrapper.properties", wrapperProps),
        (f"./{projDir}/app/build.gradle.kts",                     appBuild),
        (f"./{projDir}/app/src/main/AndroidManifest.xml",         manifest),
        (f"./{projDir}/app/src/main/cpp/CMakeLists.txt",          cmake),
    ]

    if mainActivity:
        files += [(
            f"./{projDir}/app/src/main/java/{pkgName.replace('.', '/')}/MainActivity.java",
            mainActivity
        )]

    if cxxMain:
        cxxRelPath = os.path.relpath('./' + cxxMain, f"./{projDir}/app/src/main/cpp/").replace('\\', '/')
        nativeCpp = f'#include "{cxxRelPath}"\n'
        files += [(f"./{projDir}/app/src/main/cpp/nativelib.cpp", nativeCpp)]

    if cMain:
        cRelPath = os.path.relpath('./' + cMain, f"./{projDir}/app/src/main/cpp/").replace('\\', '/')
        nativeC = f'#include "{cRelPath}"\n'
        files += [(f"./{projDir}/app/src/main/cpp/nativelib.c", nativeC)]

    for filepath, content in files:
        with open(filepath, 'w') as f:
            f.write(content)

    printSectionStart()
    printSuccess('Generated Android Studio Project')
    succeededProcesses.append('Android Studio Project Generation')
    printSectionEnd()

# endregion

# region Visual Studio Generator ==============================================================================================

def createVisualStudioProject(
        projName: str = "MyProject",
        projDir:  str = "ProjectFiles/VisualStudio",
        tempDir:  str = "Temp",
        outDir:   str = "Binaries",
        cxxMain:  str = "",
        cMain:    str = "",
    ):

    tempDirRelPath = os.path.relpath('./' + tempDir, f"./{projDir}").replace('/', '\\')
    outDirRelPath  = os.path.relpath('./' + outDir,  f"./{projDir}").replace('/', '\\')

    # Create directory structure
    dirs = [
        f"./{projDir}",
        f"./{projDir}/{tempDirRelPath}",
        f"./{projDir}/{outDirRelPath}",
    ]

    for dirPath in dirs:
        Path(dirPath).mkdir(parents=True, exist_ok=True)

    # Generate GUIDs for project
    projGuid = str(uuid.uuid4()).upper()
    slnGuid = str(uuid.uuid4()).upper()

    # Solution file (.sln)
    solutionContent = f"""\
Microsoft Visual Studio Solution File, Format Version 12.00
# Visual Studio Version 17
VisualStudioVersion = 17.0.31903.59
MinimumVisualStudioVersion = 10.0.40219.1
Project("{{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}}") = "{projName}", "{projName}.vcxproj", "{{{projGuid}}}"
EndProject
Global
	GlobalSection(SolutionConfigurationPlatforms) = preSolution
		Debug|ARM64 = Debug|ARM64
		Debug|x64 = Debug|x64
		Release|ARM64 = Release|ARM64
		Release|x64 = Release|x64
	EndGlobalSection
	GlobalSection(ProjectConfigurationPlatforms) = postSolution
		{{{projGuid}}}.Debug|ARM64.ActiveCfg = Debug|ARM64
		{{{projGuid}}}.Debug|ARM64.Build.0 = Debug|ARM64
		{{{projGuid}}}.Debug|x64.ActiveCfg = Debug|x64
		{{{projGuid}}}.Debug|x64.Build.0 = Debug|x64
		{{{projGuid}}}.Release|ARM64.ActiveCfg = Release|ARM64
		{{{projGuid}}}.Release|ARM64.Build.0 = Release|ARM64
		{{{projGuid}}}.Release|x64.ActiveCfg = Release|x64
		{{{projGuid}}}.Release|x64.Build.0 = Release|x64
	EndGlobalSection
	GlobalSection(SolutionProperties) = preSolution
		HideSolutionNode = FALSE
	EndGlobalSection
	GlobalSection(ExtensibilityGlobals) = postSolution
		SolutionGuid = {{{slnGuid}}}
	EndGlobalSection
EndGlobal
"""

    # Project file (.vcxproj)
    sourceFiles = ""
    if cxxMain:
        cxxRelPath = os.path.relpath('./' + cxxMain, f"./{projDir}").replace('/', '\\')
        sourceFiles += f"""<ClCompile Include="{cxxRelPath}">
        <LanguageStandard>stdcpp14</LanguageStandard>
        <CompileAs>CompileAsCpp</CompileAs>
    </ClCompile>
"""

    if cMain:
        cRelPath = os.path.relpath('./' + cMain, f"./{projDir}").replace('/', '\\')
        sourceFiles += f"""\
    <ClCompile Include="{cRelPath}">
        <LanguageStandard_C>stdc11</LanguageStandard_C>
        <CompileAs>CompileAsC</CompileAs>
    </ClCompile>
"""

    projectContent = f"""\
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|x64">
      <Configuration>Debug</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64">
      <Configuration>Release</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Debug|ARM64">
      <Configuration>Debug</Configuration>
      <Platform>ARM64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|ARM64">
      <Configuration>Release</Configuration>
      <Platform>ARM64</Platform>
    </ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <VCProjectVersion>17.0</VCProjectVersion>
    <ProjectGuid>{{{projGuid}}}</ProjectGuid>
    <RootNamespace>{projName}</RootNamespace>
    <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\\Microsoft.Cpp.Default.props" />
  <PropertyGroup>
    <ConfigurationType>Application</ConfigurationType>
    <PlatformToolset>v143</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)'=='Debug'">
    <UseDebugLibraries>true</UseDebugLibraries>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)'=='Release'">
    <UseDebugLibraries>false</UseDebugLibraries>
    <WholeProgramOptimization>true</WholeProgramOptimization>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\\Microsoft.Cpp.props" />
  <ImportGroup Label="PropertySheets">
    <Import Project="$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <PropertyGroup>
    <IntDir>{tempDirRelPath}\\VisualStudio\\$(Configuration)\\$(Platform)\\</IntDir>
    <OutDir>{outDirRelPath}\\</OutDir>
    <TargetName>{projName}-windows-$(Configuration)-$(Platform)</TargetName>
  </PropertyGroup>
  <ItemDefinitionGroup>
    <ClCompile>
      <WarningLevel>Level4</WarningLevel>
      <SDLCheck>true</SDLCheck>
      <ConformanceMode>true</ConformanceMode>
      <PreprocessorDefinitions>PNSLR_WINDOWS=1;%(PreprocessorDefinitions)</PreprocessorDefinitions>
    </ClCompile>
    <Link>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)'=='Debug'">
    <ClCompile>
      <PreprocessorDefinitions>PNSLR_DBG=1;_DEBUG;%(PreprocessorDefinitions)</PreprocessorDefinitions>
    </ClCompile>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)'=='Release'">
    <ClCompile>
      <FunctionLevelLinking>true</FunctionLevelLinking>
      <IntrinsicFunctions>true</IntrinsicFunctions>
      <PreprocessorDefinitions>PNSLR_REL=1;NDEBUG;%(PreprocessorDefinitions)</PreprocessorDefinitions>
    </ClCompile>
    <Link>
      <EnableCOMDATFolding>true</EnableCOMDATFolding>
      <OptimizeReferences>true</OptimizeReferences>
    </Link>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Platform)'=='x64'">
    <ClCompile>
      <PreprocessorDefinitions>PNSLR_X64=1;%(PreprocessorDefinitions)</PreprocessorDefinitions>
    </ClCompile>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Platform)'=='ARM64'">
    <ClCompile>
      <PreprocessorDefinitions>PNSLR_ARM64=1;%(PreprocessorDefinitions)</PreprocessorDefinitions>
    </ClCompile>
  </ItemDefinitionGroup>
  <ItemGroup>
{sourceFiles}  </ItemGroup>
  <Import Project="$(VCTargetsPath)\\Microsoft.Cpp.targets" />
</Project>
"""

    # .gitignore
    gitignore = f"""\
.idea/
.vs/
*.vcxproj.user
*.vcxproj.filters
{tempDirRelPath}/
{outDirRelPath}/
*.pdb
*.idb
*.ilk
*.log
*.tlog
*.lastbuildstate
"""

    # Write all files
    files = [
        (f"./{projDir}/{projName}.sln", solutionContent),
        (f"./{projDir}/{projName}.vcxproj", projectContent),
        (f"./{projDir}/.gitignore", gitignore),
    ]

    for filepath, content in files:
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(content)

    printSectionStart()
    printSuccess('Generated Visual Studio Project')
    succeededProcesses.append('Visual Studio Project Generation')
    printSectionEnd()

# endregion

#region XCode Generator =======================================================================================================

def createXCodeProject(
        projName: str = "MyProject",
        pkgName:  str = "com.example.myproject",
        projDir:  str = "ProjectFiles/Xcode",
        tempDir:  str = "Temp",
        outDir:   str = "Binaries",
        cxxMain:  str = "",
        cMain:    str = "",
        objcMain: str = "",
        objcxxMain: str = "",
    ):

    # Create directory structure
    dirs = [
        f"./{projDir}",
        f"./{projDir}/{projName}.xcodeproj",
        f"./{projDir}/{projName}.xcodeproj/project.xcworkspace",
        f"./{projDir}/{projName}.xcodeproj/project.xcworkspace/xcshareddata",
        f"./{projDir}/{projName}.xcodeproj/xcshareddata/xcschemes",
    ]

    for dirPath in dirs:
        Path(dirPath).mkdir(parents=True, exist_ok=True)

    outDirRelPath = os.path.relpath('./' + outDir, f"./{projDir}").replace('\\', '/')
    tempDirRelPath = os.path.relpath('./' + tempDir, f"./{projDir}").replace('\\', '/')
    outputProductName = f'{projName}-$(CONFIGURATION)-$(CURRENT_ARCH)'

    # Generate UUIDs for various objects
    def genUUID():
        return ''.join(random.choices('0123456789ABCDEF', k=24))

    # Project and target IDs
    projID = genUUID()
    iosTargetID = genUUID()
    macTargetID = genUUID()
    iosBuildConfigDebugID = genUUID()
    iosBuildConfigReleaseID = genUUID()
    macBuildConfigDebugID = genUUID()
    macBuildConfigReleaseID = genUUID()
    projBuildConfigDebugID = genUUID()
    projBuildConfigReleaseID = genUUID()
    iosConfigListID = genUUID()
    macConfigListID = genUUID()
    projConfigListID = genUUID()
    mainGroupID = genUUID()
    productsGroupID = genUUID()
    sourcesGroupID = genUUID()
    iosProductRefID = genUUID()
    macProductRefID = genUUID()
    iosBuildPhaseSourcesID = genUUID()
    macBuildPhaseSourcesID = genUUID()
    iosBuildPhaseFrameworksID = genUUID()
    macBuildPhaseFrameworksID = genUUID()
    iosBuildPhaseResourcesID = genUUID()
    macBuildPhaseResourcesID = genUUID()

    # File reference IDs
    fileRefs = {}
    buildFiles = {}

    sourceFiles: list[tuple[str,str,str]] = []
    if cMain:
        fileRefs['c'] = genUUID()
        buildFiles['c_ios'] = genUUID()
        buildFiles['c_mac'] = genUUID()
        sourceFiles.append(('c', cMain, 'sourcecode.c.c'))
    if cxxMain:
        fileRefs['cpp'] = genUUID()
        buildFiles['cpp_ios'] = genUUID()
        buildFiles['cpp_mac'] = genUUID()
        sourceFiles.append(('cpp', cxxMain, 'sourcecode.cpp.cpp'))
    if objcMain:
        fileRefs['objc'] = genUUID()
        buildFiles['objc_ios'] = genUUID()
        buildFiles['objc_mac'] = genUUID()
        sourceFiles.append(('objc', objcMain, 'sourcecode.c.objc'))
    if objcxxMain:
        fileRefs['objcxx'] = genUUID()
        buildFiles['objcxx_ios'] = genUUID()
        buildFiles['objcxx_mac'] = genUUID()
        sourceFiles.append(('objcxx', objcxxMain, 'sourcecode.cpp.objcpp'))

    # Info.plist IDs
    iosInfoPlistID = genUUID()
    macInfoPlistID = genUUID()

    # Build file references section
    buildFilesSection = ""
    for key, path, _ in sourceFiles:
        relPath = os.path.relpath('./' + path, f"./{projDir}").replace('\\', '/')
        buildFilesSection += f"""\
		{buildFiles[key + '_ios']} /* {os.path.basename(path)} in Sources */ = {{isa = PBXBuildFile; fileRef = {fileRefs[key]} /* {os.path.basename(path)} */; }};
		{buildFiles[key + '_mac']} /* {os.path.basename(path)} in Sources */ = {{isa = PBXBuildFile; fileRef = {fileRefs[key]} /* {os.path.basename(path)} */; }};
"""

    # File references section
    fileRefsSection = ""
    for key, path, fileType in sourceFiles:
        relPath = os.path.relpath('./' + path, f"./{projDir}").replace('\\', '/')
        fileRefsSection += f"""\
		{fileRefs[key]} /* {os.path.basename(path)} */ = {{isa = PBXFileReference; lastKnownFileType = {fileType}; path = "{relPath}"; sourceTree = "<group>"; }};
"""

    # Build phase sources
    iosBuildSources = ""
    macBuildSources = ""
    for key, path, _ in sourceFiles:
        iosBuildSources += f"""\
				{buildFiles[key + '_ios']} /* {os.path.basename(path)} in Sources */,
"""
        macBuildSources += f"""\
				{buildFiles[key + '_mac']} /* {os.path.basename(path)} in Sources */,
"""

    # Source group children
    sourceGroupChildren = ""
    for key, path, _ in sourceFiles:
        sourceGroupChildren += f"""\
				{fileRefs[key]} /* {os.path.basename(path)} */,
"""

    # project.pbxproj
    pbxproj = f"""\
// !$*UTF8*$!
{{
	archiveVersion = 1;
	classes = {{
	}};
	objectVersion = 56;
	objects = {{

/* Begin PBXBuildFile section */
{buildFilesSection}/* End PBXBuildFile section */

/* Begin PBXFileReference section */
		{iosProductRefID} /* {projName}.app */ = {{isa = PBXFileReference; explicitFileType = wrapper.application; includeInIndex = 0; path = {projName}.app; sourceTree = BUILT_PRODUCTS_DIR; }};
		{macProductRefID} /* {projName}.app */ = {{isa = PBXFileReference; explicitFileType = wrapper.application; includeInIndex = 0; path = {projName}.app; sourceTree = BUILT_PRODUCTS_DIR; }};
		{iosInfoPlistID} /* iOS-Info.plist */ = {{isa = PBXFileReference; lastKnownFileType = text.plist.xml; path = "iOS-Info.plist"; sourceTree = "<group>"; }};
		{macInfoPlistID} /* macOS-Info.plist */ = {{isa = PBXFileReference; lastKnownFileType = text.plist.xml; path = "macOS-Info.plist"; sourceTree = "<group>"; }};
{fileRefsSection}/* End PBXFileReference section */

/* Begin PBXFrameworksBuildPhase section */
		{iosBuildPhaseFrameworksID} /* Frameworks */ = {{
			isa = PBXFrameworksBuildPhase;
			buildActionMask = 2147483647;
			files = (
			);
			runOnlyForDeploymentPostprocessing = 0;
		}};
		{macBuildPhaseFrameworksID} /* Frameworks */ = {{
			isa = PBXFrameworksBuildPhase;
			buildActionMask = 2147483647;
			files = (
			);
			runOnlyForDeploymentPostprocessing = 0;
		}};
/* End PBXFrameworksBuildPhase section */

/* Begin PBXGroup section */
		{mainGroupID} = {{
			isa = PBXGroup;
			children = (
				{sourcesGroupID} /* Sources */,
				{productsGroupID} /* Products */,
				{iosInfoPlistID} /* iOS-Info.plist */,
				{macInfoPlistID} /* macOS-Info.plist */,
			);
			sourceTree = "<group>";
		}};
		{productsGroupID} /* Products */ = {{
			isa = PBXGroup;
			children = (
				{iosProductRefID} /* {projName}.app */,
				{macProductRefID} /* {projName}.app */,
			);
			name = Products;
			sourceTree = "<group>";
		}};
		{sourcesGroupID} /* Sources */ = {{
			isa = PBXGroup;
			children = (
{sourceGroupChildren}			);
			name = Sources;
			sourceTree = "<group>";
		}};
/* End PBXGroup section */

/* Begin PBXNativeTarget section */
		{iosTargetID} /* {projName} (iOS) */ = {{
			isa = PBXNativeTarget;
			buildConfigurationList = {iosConfigListID} /* Build configuration list for PBXNativeTarget "{projName} (iOS)" */;
			buildPhases = (
				{iosBuildPhaseSourcesID} /* Sources */,
				{iosBuildPhaseFrameworksID} /* Frameworks */,
				{iosBuildPhaseResourcesID} /* Resources */,
			);
			buildRules = (
			);
			dependencies = (
			);
			name = "{projName} (iOS)";
			productName = {projName};
			productReference = {iosProductRefID} /* {projName}.app */;
			productType = "com.apple.product-type.application";
		}};
		{macTargetID} /* {projName} (macOS) */ = {{
			isa = PBXNativeTarget;
			buildConfigurationList = {macConfigListID} /* Build configuration list for PBXNativeTarget "{projName} (macOS)" */;
			buildPhases = (
				{macBuildPhaseSourcesID} /* Sources */,
				{macBuildPhaseFrameworksID} /* Frameworks */,
				{macBuildPhaseResourcesID} /* Resources */,
			);
			buildRules = (
			);
			dependencies = (
			);
			name = "{projName} (macOS)";
			productName = {projName};
			productReference = {macProductRefID} /* {projName}.app */;
			productType = "com.apple.product-type.application";
		}};
/* End PBXNativeTarget section */

/* Begin PBXProject section */
		{projID} /* Project object */ = {{
			isa = PBXProject;
			attributes = {{
				BuildIndependentTargetsInParallel = 1;
				LastUpgradeCheck = 1540;
				TargetAttributes = {{
					{iosTargetID} = {{
						CreatedOnToolsVersion = 15.4;
					}};
					{macTargetID} = {{
						CreatedOnToolsVersion = 15.4;
					}};
				}};
			}};
			buildConfigurationList = {projConfigListID} /* Build configuration list for PBXProject "{projName}" */;
			compatibilityVersion = "Xcode 14.0";
			developmentRegion = en;
			hasScannedForEncodings = 0;
			knownRegions = (
				en,
				Base,
			);
			mainGroup = {mainGroupID};
			productRefGroup = {productsGroupID} /* Products */;
			projectDirPath = "";
			projectRoot = "";
			targets = (
				{iosTargetID} /* {projName} (iOS) */,
				{macTargetID} /* {projName} (macOS) */,
			);
		}};
/* End PBXProject section */

/* Begin PBXResourcesBuildPhase section */
		{iosBuildPhaseResourcesID} /* Resources */ = {{
			isa = PBXResourcesBuildPhase;
			buildActionMask = 2147483647;
			files = (
			);
			runOnlyForDeploymentPostprocessing = 0;
		}};
		{macBuildPhaseResourcesID} /* Resources */ = {{
			isa = PBXResourcesBuildPhase;
			buildActionMask = 2147483647;
			files = (
			);
			runOnlyForDeploymentPostprocessing = 0;
		}};
/* End PBXResourcesBuildPhase section */

/* Begin PBXSourcesBuildPhase section */
		{iosBuildPhaseSourcesID} /* Sources */ = {{
			isa = PBXSourcesBuildPhase;
			buildActionMask = 2147483647;
			files = (
{iosBuildSources}			);
			runOnlyForDeploymentPostprocessing = 0;
		}};
		{macBuildPhaseSourcesID} /* Sources */ = {{
			isa = PBXSourcesBuildPhase;
			buildActionMask = 2147483647;
			files = (
{macBuildSources}			);
			runOnlyForDeploymentPostprocessing = 0;
		}};
/* End PBXSourcesBuildPhase section */

/* Begin XCBuildConfiguration section */
		{iosBuildConfigDebugID} /* Debug */ = {{
			isa = XCBuildConfiguration;
			buildSettings = {{
				ASSETCATALOG_COMPILER_APPICON_NAME = AppIcon;
				CLANG_CXX_LANGUAGE_STANDARD = "c++14";
				GCC_C_LANGUAGE_STANDARD = c11;
				CODE_SIGN_STYLE = Automatic;
				CURRENT_PROJECT_VERSION = 1;
				GENERATE_INFOPLIST_FILE = NO;
				INFOPLIST_FILE = "iOS-Info.plist";
				INFOPLIST_KEY_UIApplicationSupportsIndirectInputEvents = YES;
				INFOPLIST_KEY_UILaunchStoryboardName = "";
				INFOPLIST_KEY_UIMainStoryboardFile = "";
				INFOPLIST_KEY_UISupportedInterfaceOrientations = "UIInterfaceOrientationPortrait UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight";
				INFOPLIST_KEY_UISupportedInterfaceOrientations_iPad = "UIInterfaceOrientationPortrait UIInterfaceOrientationPortraitUpsideDown UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight";
				IPHONEOS_DEPLOYMENT_TARGET = 15.0;
				LD_RUNPATH_SEARCH_PATHS = (
					"$(inherited)",
					"@executable_path/Frameworks",
				);
				MARKETING_VERSION = 1.0;
				PRODUCT_BUNDLE_IDENTIFIER = {pkgName};
				PRODUCT_NAME = "{outputProductName}";
				SDKROOT = iphoneos;
				SWIFT_EMIT_LOC_STRINGS = YES;
				TARGETED_DEVICE_FAMILY = "1,2";
				CONFIGURATION_BUILD_DIR = "{outDirRelPath}/ios/";
				SYMROOT = "{outDirRelPath}/ios/";
				OBJROOT = "{tempDirRelPath}/ios/";
				GCC_PREPROCESSOR_DEFINITIONS = (
					"DEBUG=1",
					"PNSLR_DBG=1",
					"PNSLR_IOS=1",
					"$(inherited)",
				);
				"GCC_PREPROCESSOR_DEFINITIONS[arch=arm64]" = (
					"$(GCC_PREPROCESSOR_DEFINITIONS)",
					"PNSLR_ARM64=1",
				);
				"GCC_PREPROCESSOR_DEFINITIONS[arch=x86_64]" = (
					"$(GCC_PREPROCESSOR_DEFINITIONS)",
					"PNSLR_X64=1",
				);
			}};
			name = Debug;
		}};
		{iosBuildConfigReleaseID} /* Release */ = {{
			isa = XCBuildConfiguration;
			buildSettings = {{
				ASSETCATALOG_COMPILER_APPICON_NAME = AppIcon;
				CLANG_CXX_LANGUAGE_STANDARD = "c++14";
				GCC_C_LANGUAGE_STANDARD = c11;
				CODE_SIGN_STYLE = Automatic;
				CURRENT_PROJECT_VERSION = 1;
				GENERATE_INFOPLIST_FILE = NO;
				INFOPLIST_FILE = "iOS-Info.plist";
				INFOPLIST_KEY_UIApplicationSupportsIndirectInputEvents = YES;
				INFOPLIST_KEY_UILaunchStoryboardName = "";
				INFOPLIST_KEY_UIMainStoryboardFile = "";
				INFOPLIST_KEY_UISupportedInterfaceOrientations = "UIInterfaceOrientationPortrait UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight";
				INFOPLIST_KEY_UISupportedInterfaceOrientations_iPad = "UIInterfaceOrientationPortrait UIInterfaceOrientationPortraitUpsideDown UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight";
				IPHONEOS_DEPLOYMENT_TARGET = 15.0;
				LD_RUNPATH_SEARCH_PATHS = (
					"$(inherited)",
					"@executable_path/Frameworks",
				);
				MARKETING_VERSION = 1.0;
				PRODUCT_BUNDLE_IDENTIFIER = {pkgName};
				PRODUCT_NAME = "{outputProductName}";
				SDKROOT = iphoneos;
				SWIFT_EMIT_LOC_STRINGS = YES;
				TARGETED_DEVICE_FAMILY = "1,2";
				VALIDATE_PRODUCT = YES;
				CONFIGURATION_BUILD_DIR = "{outDirRelPath}/ios/";
				SYMROOT = "{outDirRelPath}/ios/";
				OBJROOT = "{tempDirRelPath}/ios/";
				GCC_PREPROCESSOR_DEFINITIONS = (
					"PNSLR_REL=1",
					"PNSLR_IOS=1",
					"$(inherited)",
				);
				"GCC_PREPROCESSOR_DEFINITIONS[arch=arm64]" = (
					"$(GCC_PREPROCESSOR_DEFINITIONS)",
					"PNSLR_ARM64=1",
				);
				"GCC_PREPROCESSOR_DEFINITIONS[arch=x86_64]" = (
					"$(GCC_PREPROCESSOR_DEFINITIONS)",
					"PNSLR_X64=1",
				);
			}};
			name = Release;
		}};
		{macBuildConfigDebugID} /* Debug */ = {{
			isa = XCBuildConfiguration;
			buildSettings = {{
				ASSETCATALOG_COMPILER_APPICON_NAME = AppIcon;
				CLANG_CXX_LANGUAGE_STANDARD = "c++14";
				GCC_C_LANGUAGE_STANDARD = c11;
				CODE_SIGN_STYLE = Automatic;
				COMBINE_HIDPI_IMAGES = YES;
				CURRENT_PROJECT_VERSION = 1;
				GENERATE_INFOPLIST_FILE = NO;
				INFOPLIST_FILE = "macOS-Info.plist";
				INFOPLIST_KEY_NSHumanReadableCopyright = "";
				INFOPLIST_KEY_NSMainStoryboardFile = "";
				INFOPLIST_KEY_NSPrincipalClass = NSApplication;
				LD_RUNPATH_SEARCH_PATHS = (
					"$(inherited)",
					"@executable_path/../Frameworks",
				);
				MACOSX_DEPLOYMENT_TARGET = 12.0;
				MARKETING_VERSION = 1.0;
				PRODUCT_BUNDLE_IDENTIFIER = {pkgName};
				PRODUCT_NAME = "{outputProductName}";
				SDKROOT = macosx;
				SWIFT_EMIT_LOC_STRINGS = YES;
				CONFIGURATION_BUILD_DIR = "{outDirRelPath}/osx/";
				SYMROOT = "{outDirRelPath}/osx/";
				OBJROOT = "{tempDirRelPath}/osx/";
				GCC_PREPROCESSOR_DEFINITIONS = (
					"DEBUG=1",
					"PNSLR_DBG=1",
					"PNSLR_OSX=1",
					"$(inherited)",
				);
				"GCC_PREPROCESSOR_DEFINITIONS[arch=arm64]" = (
					"$(GCC_PREPROCESSOR_DEFINITIONS)",
					"PNSLR_ARM64=1",
				);
				"GCC_PREPROCESSOR_DEFINITIONS[arch=x86_64]" = (
					"$(GCC_PREPROCESSOR_DEFINITIONS)",
					"PNSLR_X64=1",
				);
			}};
			name = Debug;
		}};
		{macBuildConfigReleaseID} /* Release */ = {{
			isa = XCBuildConfiguration;
			buildSettings = {{
				ASSETCATALOG_COMPILER_APPICON_NAME = AppIcon;
				CLANG_CXX_LANGUAGE_STANDARD = "c++14";
				GCC_C_LANGUAGE_STANDARD = c11;
				CODE_SIGN_STYLE = Automatic;
				COMBINE_HIDPI_IMAGES = YES;
				CURRENT_PROJECT_VERSION = 1;
				GENERATE_INFOPLIST_FILE = NO;
				INFOPLIST_FILE = "macOS-Info.plist";
				INFOPLIST_KEY_NSHumanReadableCopyright = "";
				INFOPLIST_KEY_NSMainStoryboardFile = "";
				INFOPLIST_KEY_NSPrincipalClass = NSApplication;
				LD_RUNPATH_SEARCH_PATHS = (
					"$(inherited)",
					"@executable_path/../Frameworks",
				);
				MACOSX_DEPLOYMENT_TARGET = 12.0;
				MARKETING_VERSION = 1.0;
				PRODUCT_BUNDLE_IDENTIFIER = {pkgName};
				PRODUCT_NAME = "{outputProductName}";
				SDKROOT = macosx;
				SWIFT_EMIT_LOC_STRINGS = YES;
				CONFIGURATION_BUILD_DIR = "{outDirRelPath}/osx/";
				SYMROOT = "{outDirRelPath}/osx/";
				OBJROOT = "{tempDirRelPath}/osx/";
				GCC_PREPROCESSOR_DEFINITIONS = (
					"PNSLR_REL=1",
					"PNSLR_OSX=1",
					"$(inherited)",
				);
				"GCC_PREPROCESSOR_DEFINITIONS[arch=arm64]" = (
					"$(GCC_PREPROCESSOR_DEFINITIONS)",
					"PNSLR_ARM64=1",
				);
				"GCC_PREPROCESSOR_DEFINITIONS[arch=x86_64]" = (
					"$(GCC_PREPROCESSOR_DEFINITIONS)",
					"PNSLR_X64=1",
				);
			}};
			name = Release;
		}};
		{projBuildConfigDebugID} /* Debug */ = {{
			isa = XCBuildConfiguration;
			buildSettings = {{
				ALWAYS_SEARCH_USER_PATHS = NO;
				CLANG_ANALYZER_NONNULL = YES;
				CLANG_ANALYZER_NUMBER_OBJECT_CONVERSION = YES_AGGRESSIVE;
				CLANG_CXX_LANGUAGE_STANDARD = "c++14";
				CLANG_ENABLE_MODULES = YES;
				CLANG_ENABLE_OBJC_ARC = YES;
				CLANG_ENABLE_OBJC_WEAK = YES;
				CLANG_WARN_BLOCK_CAPTURE_AUTORELEASING = YES;
				CLANG_WARN_BOOL_CONVERSION = YES;
				CLANG_WARN_COMMA = YES;
				CLANG_WARN_CONSTANT_CONVERSION = YES;
				CLANG_WARN_DEPRECATED_OBJC_IMPLEMENTATIONS = YES;
				CLANG_WARN_DIRECT_OBJC_ISA_USAGE = YES_ERROR;
				CLANG_WARN_DOCUMENTATION_COMMENTS = YES;
				CLANG_WARN_EMPTY_BODY = YES;
				CLANG_WARN_ENUM_CONVERSION = YES;
				CLANG_WARN_INFINITE_RECURSION = YES;
				CLANG_WARN_INT_CONVERSION = YES;
				CLANG_WARN_NON_LITERAL_NULL_CONVERSION = YES;
				CLANG_WARN_OBJC_IMPLICIT_RETAIN_SELF = YES;
				CLANG_WARN_OBJC_LITERAL_CONVERSION = YES;
				CLANG_WARN_OBJC_ROOT_CLASS = YES_ERROR;
				CLANG_WARN_QUOTED_INCLUDE_IN_FRAMEWORK_HEADER = YES;
				CLANG_WARN_RANGE_LOOP_ANALYSIS = YES;
				CLANG_WARN_STRICT_PROTOTYPES = YES;
				CLANG_WARN_SUSPICIOUS_MOVE = YES;
				CLANG_WARN_UNGUARDED_AVAILABILITY = YES_AGGRESSIVE;
				CLANG_WARN_UNREACHABLE_CODE = YES;
				CLANG_WARN__DUPLICATE_METHOD_MATCH = YES;
				COPY_PHASE_STRIP = NO;
				DEBUG_INFORMATION_FORMAT = dwarf;
				ENABLE_STRICT_OBJC_MSGSEND = YES;
				ENABLE_TESTABILITY = YES;
				GCC_C_LANGUAGE_STANDARD = c11;
				GCC_DYNAMIC_NO_PIC = NO;
				GCC_NO_COMMON_BLOCKS = YES;
				GCC_OPTIMIZATION_LEVEL = 0;
				GCC_WARN_64_TO_32_BIT_CONVERSION = YES;
				GCC_WARN_ABOUT_RETURN_TYPE = YES_ERROR;
				GCC_WARN_UNDECLARED_SELECTOR = YES;
				GCC_WARN_UNINITIALIZED_AUTOS = YES_AGGRESSIVE;
				GCC_WARN_UNUSED_FUNCTION = YES;
				GCC_WARN_UNUSED_VARIABLE = YES;
				MTL_ENABLE_DEBUG_INFO = INCLUDE_SOURCE;
				MTL_FAST_MATH = YES;
				ONLY_ACTIVE_ARCH = YES;
				ENABLE_USER_SCRIPT_SANDBOXING = NO;
			}};
			name = Debug;
		}};
		{projBuildConfigReleaseID} /* Release */ = {{
			isa = XCBuildConfiguration;
			buildSettings = {{
				ALWAYS_SEARCH_USER_PATHS = NO;
				CLANG_ANALYZER_NONNULL = YES;
				CLANG_ANALYZER_NUMBER_OBJECT_CONVERSION = YES_AGGRESSIVE;
				CLANG_CXX_LANGUAGE_STANDARD = "c++14";
				CLANG_ENABLE_MODULES = YES;
				CLANG_ENABLE_OBJC_ARC = YES;
				CLANG_ENABLE_OBJC_WEAK = YES;
				CLANG_WARN_BLOCK_CAPTURE_AUTORELEASING = YES;
				CLANG_WARN_BOOL_CONVERSION = YES;
				CLANG_WARN_COMMA = YES;
				CLANG_WARN_CONSTANT_CONVERSION = YES;
				CLANG_WARN_DEPRECATED_OBJC_IMPLEMENTATIONS = YES;
				CLANG_WARN_DIRECT_OBJC_ISA_USAGE = YES_ERROR;
				CLANG_WARN_DOCUMENTATION_COMMENTS = YES;
				CLANG_WARN_EMPTY_BODY = YES;
				CLANG_WARN_ENUM_CONVERSION = YES;
				CLANG_WARN_INFINITE_RECURSION = YES;
				CLANG_WARN_INT_CONVERSION = YES;
				CLANG_WARN_NON_LITERAL_NULL_CONVERSION = YES;
				CLANG_WARN_OBJC_IMPLICIT_RETAIN_SELF = YES;
				CLANG_WARN_OBJC_LITERAL_CONVERSION = YES;
				CLANG_WARN_OBJC_ROOT_CLASS = YES_ERROR;
				CLANG_WARN_QUOTED_INCLUDE_IN_FRAMEWORK_HEADER = YES;
				CLANG_WARN_RANGE_LOOP_ANALYSIS = YES;
				CLANG_WARN_STRICT_PROTOTYPES = YES;
				CLANG_WARN_SUSPICIOUS_MOVE = YES;
				CLANG_WARN_UNGUARDED_AVAILABILITY = YES_AGGRESSIVE;
				CLANG_WARN_UNREACHABLE_CODE = YES;
				CLANG_WARN__DUPLICATE_METHOD_MATCH = YES;
				COPY_PHASE_STRIP = NO;
				DEBUG_INFORMATION_FORMAT = "dwarf-with-dsym";
				ENABLE_NS_ASSERTIONS = NO;
				ENABLE_STRICT_OBJC_MSGSEND = YES;
				GCC_C_LANGUAGE_STANDARD = c11;
				GCC_NO_COMMON_BLOCKS = YES;
				GCC_WARN_64_TO_32_BIT_CONVERSION = YES;
				GCC_WARN_ABOUT_RETURN_TYPE = YES_ERROR;
				GCC_WARN_UNDECLARED_SELECTOR = YES;
				GCC_WARN_UNINITIALIZED_AUTOS = YES_AGGRESSIVE;
				GCC_WARN_UNUSED_FUNCTION = YES;
				GCC_WARN_UNUSED_VARIABLE = YES;
				MTL_ENABLE_DEBUG_INFO = NO;
				MTL_FAST_MATH = YES;
				ENABLE_USER_SCRIPT_SANDBOXING = NO;
			}};
			name = Release;
		}};
/* End XCBuildConfiguration section */

/* Begin XCConfigurationList section */
		{iosConfigListID} /* Build configuration list for PBXNativeTarget "{projName} (iOS)" */ = {{
			isa = XCConfigurationList;
			buildConfigurations = (
				{iosBuildConfigDebugID} /* Debug */,
				{iosBuildConfigReleaseID} /* Release */,
			);
			defaultConfigurationIsVisible = 0;
			defaultConfigurationName = Release;
		}};
		{macConfigListID} /* Build configuration list for PBXNativeTarget "{projName} (macOS)" */ = {{
			isa = XCConfigurationList;
			buildConfigurations = (
				{macBuildConfigDebugID} /* Debug */,
				{macBuildConfigReleaseID} /* Release */,
			);
			defaultConfigurationIsVisible = 0;
			defaultConfigurationName = Release;
		}};
		{projConfigListID} /* Build configuration list for PBXProject "{projName}" */ = {{
			isa = XCConfigurationList;
			buildConfigurations = (
				{projBuildConfigDebugID} /* Debug */,
				{projBuildConfigReleaseID} /* Release */,
			);
			defaultConfigurationIsVisible = 0;
			defaultConfigurationName = Release;
		}};
/* End XCConfigurationList section */
	}};
	rootObject = {projID} /* Project object */;
}}
"""

    # iOS Info.plist
    iosInfoPlist = f"""\
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>CFBundleDevelopmentRegion</key>
	<string>$(DEVELOPMENT_LANGUAGE)</string>
	<key>CFBundleExecutable</key>
	<string>$(EXECUTABLE_NAME)</string>
	<key>CFBundleIdentifier</key>
	<string>$(PRODUCT_BUNDLE_IDENTIFIER)</string>
	<key>CFBundleInfoDictionaryVersion</key>
	<string>6.0</string>
	<key>CFBundleName</key>
	<string>$(PRODUCT_NAME)</string>
	<key>CFBundlePackageType</key>
	<string>$(PRODUCT_BUNDLE_PACKAGE_TYPE)</string>
	<key>CFBundleShortVersionString</key>
	<string>$(MARKETING_VERSION)</string>
	<key>CFBundleVersion</key>
	<string>$(CURRENT_PROJECT_VERSION)</string>
	<key>LSRequiresIPhoneOS</key>
	<true/>
	<key>UIApplicationSupportsIndirectInputEvents</key>
	<true/>
	<key>UILaunchStoryboardName</key>
	<string></string>
	<key>UIMainStoryboardFile</key>
	<string></string>
	<key>UIRequiredDeviceCapabilities</key>
	<array>
		<string>armv7</string>
	</array>
	<key>UIStatusBarHidden</key>
	<true/>
	<key>UISupportedInterfaceOrientations</key>
	<array>
		<string>UIInterfaceOrientationPortrait</string>
		<string>UIInterfaceOrientationLandscapeLeft</string>
		<string>UIInterfaceOrientationLandscapeRight</string>
	</array>
	<key>UISupportedInterfaceOrientations~ipad</key>
	<array>
		<string>UIInterfaceOrientationPortrait</string>
		<string>UIInterfaceOrientationPortraitUpsideDown</string>
		<string>UIInterfaceOrientationLandscapeLeft</string>
		<string>UIInterfaceOrientationLandscapeRight</string>
	</array><key>UIApplicationSceneManifest</key>
    <dict>
        <key>UIApplicationSupportsMultipleScenes</key>
        <false/>
    </dict>
</dict>
</plist>
"""

    # macOS Info.plist
    macInfoPlist = f"""\
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>CFBundleDevelopmentRegion</key>
	<string>$(DEVELOPMENT_LANGUAGE)</string>
	<key>CFBundleExecutable</key>
	<string>$(EXECUTABLE_NAME)</string>
	<key>CFBundleIconFile</key>
	<string></string>
	<key>CFBundleIdentifier</key>
	<string>$(PRODUCT_BUNDLE_IDENTIFIER)</string>
	<key>CFBundleInfoDictionaryVersion</key>
	<string>6.0</string>
	<key>CFBundleName</key>
	<string>$(PRODUCT_NAME)</string>
	<key>CFBundlePackageType</key>
	<string>$(PRODUCT_BUNDLE_PACKAGE_TYPE)</string>
	<key>CFBundleShortVersionString</key>
	<string>$(MARKETING_VERSION)</string>
	<key>CFBundleVersion</key>
	<string>$(CURRENT_PROJECT_VERSION)</string>
	<key>LSMinimumSystemVersion</key>
	<string>$(MACOSX_DEPLOYMENT_TARGET)</string>
	<key>NSHumanReadableCopyright</key>
	<string></string>
	<key>NSMainStoryboardFile</key>
	<string></string>
	<key>NSPrincipalClass</key>
	<string>NSApplication</string>
</dict>
</plist>
"""

    # xcscheme files for iOS and macOS
    iosScheme = f"""\
<?xml version="1.0" encoding="UTF-8"?>
<Scheme
   LastUpgradeVersion = "1540"
   version = "1.7">
   <BuildAction
      parallelizeBuildables = "YES"
      buildImplicitDependencies = "YES">
      <BuildActionEntries>
         <BuildActionEntry
            buildForTesting = "YES"
            buildForRunning = "YES"
            buildForProfiling = "YES"
            buildForArchiving = "YES"
            buildForAnalyzing = "YES">
            <BuildableReference
               BuildableIdentifier = "primary"
               BlueprintIdentifier = "{iosTargetID}"
               BuildableName = "{projName}.app"
               BlueprintName = "{projName} (iOS)"
               ReferencedContainer = "container:{projName}.xcodeproj">
            </BuildableReference>
         </BuildActionEntry>
      </BuildActionEntries>
   </BuildAction>
   <TestAction
      buildConfiguration = "Debug"
      selectedDebuggerIdentifier = "Xcode.DebuggerFoundation.Debugger.LLDB"
      selectedLauncherIdentifier = "Xcode.DebuggerFoundation.Launcher.LLDB"
      shouldUseLaunchSchemeArgsEnv = "YES">
   </TestAction>
   <LaunchAction
      buildConfiguration = "Debug"
      selectedDebuggerIdentifier = "Xcode.DebuggerFoundation.Debugger.LLDB"
      selectedLauncherIdentifier = "Xcode.DebuggerFoundation.Launcher.LLDB"
      launchStyle = "0"
      useCustomWorkingDirectory = "NO"
      ignoresPersistentStateOnLaunch = "NO"
      debugDocumentVersioning = "YES"
      debugServiceExtension = "internal"
      allowLocationSimulation = "YES">
      <BuildableProductRunnable
         runnableDebuggingMode = "0">
         <BuildableReference
            BuildableIdentifier = "primary"
            BlueprintIdentifier = "{iosTargetID}"
            BuildableName = "{projName}.app"
            BlueprintName = "{projName} (iOS)"
            ReferencedContainer = "container:{projName}.xcodeproj">
         </BuildableReference>
      </BuildableProductRunnable>
   </LaunchAction>
   <ProfileAction
      buildConfiguration = "Release"
      shouldUseLaunchSchemeArgsEnv = "YES"
      savedToolIdentifier = ""
      useCustomWorkingDirectory = "NO"
      debugDocumentVersioning = "YES">
      <BuildableProductRunnable
         runnableDebuggingMode = "0">
         <BuildableReference
            BuildableIdentifier = "primary"
            BlueprintIdentifier = "{iosTargetID}"
            BuildableName = "{projName}.app"
            BlueprintName = "{projName} (iOS)"
            ReferencedContainer = "container:{projName}.xcodeproj">
         </BuildableReference>
      </BuildableProductRunnable>
   </ProfileAction>
   <AnalyzeAction
      buildConfiguration = "Debug">
   </AnalyzeAction>
   <ArchiveAction
      buildConfiguration = "Release"
      revealArchiveInOrganizer = "YES">
   </ArchiveAction>
</Scheme>
"""

    macScheme = f"""\
<?xml version="1.0" encoding="UTF-8"?>
<Scheme
   LastUpgradeVersion = "1540"
   version = "1.7">
   <BuildAction
      parallelizeBuildables = "YES"
      buildImplicitDependencies = "YES">
      <BuildActionEntries>
         <BuildActionEntry
            buildForTesting = "YES"
            buildForRunning = "YES"
            buildForProfiling = "YES"
            buildForArchiving = "YES"
            buildForAnalyzing = "YES">
            <BuildableReference
               BuildableIdentifier = "primary"
               BlueprintIdentifier = "{macTargetID}"
               BuildableName = "{projName}.app"
               BlueprintName = "{projName} (macOS)"
               ReferencedContainer = "container:{projName}.xcodeproj">
            </BuildableReference>
         </BuildActionEntry>
      </BuildActionEntries>
   </BuildAction>
   <TestAction
      buildConfiguration = "Debug"
      selectedDebuggerIdentifier = "Xcode.DebuggerFoundation.Debugger.LLDB"
      selectedLauncherIdentifier = "Xcode.DebuggerFoundation.Launcher.LLDB"
      shouldUseLaunchSchemeArgsEnv = "YES">
   </TestAction>
   <LaunchAction
      buildConfiguration = "Debug"
      selectedDebuggerIdentifier = "Xcode.DebuggerFoundation.Debugger.LLDB"
      selectedLauncherIdentifier = "Xcode.DebuggerFoundation.Launcher.LLDB"
      launchStyle = "0"
      useCustomWorkingDirectory = "NO"
      ignoresPersistentStateOnLaunch = "NO"
      debugDocumentVersioning = "YES"
      debugServiceExtension = "internal"
      allowLocationSimulation = "YES">
      <BuildableProductRunnable
         runnableDebuggingMode = "0">
         <BuildableReference
            BuildableIdentifier = "primary"
            BlueprintIdentifier = "{macTargetID}"
            BuildableName = "{projName}.app"
            BlueprintName = "{projName} (macOS)"
            ReferencedContainer = "container:{projName}.xcodeproj">
         </BuildableReference>
      </BuildableProductRunnable>
   </LaunchAction>
   <ProfileAction
      buildConfiguration = "Release"
      shouldUseLaunchSchemeArgsEnv = "YES"
      savedToolIdentifier = ""
      useCustomWorkingDirectory = "NO"
      debugDocumentVersioning = "YES">
      <BuildableProductRunnable
         runnableDebuggingMode = "0">
         <BuildableReference
            BuildableIdentifier = "primary"
            BlueprintIdentifier = "{macTargetID}"
            BuildableName = "{projName}.app"
            BlueprintName = "{projName} (macOS)"
            ReferencedContainer = "container:{projName}.xcodeproj">
         </BuildableReference>
      </BuildableProductRunnable>
   </ProfileAction>
   <AnalyzeAction
      buildConfiguration = "Debug">
   </AnalyzeAction>
   <ArchiveAction
      buildConfiguration = "Release"
      revealArchiveInOrganizer = "YES">
   </ArchiveAction>
</Scheme>
"""

    # WorkspaceSettings.xcsettings
    workspaceSettings = """\
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>BuildLocationStyle</key>
	<string>UseAppPreferences</string>
	<key>CustomBuildLocationType</key>
	<string>RelativeToDerivedData</string>
	<key>DerivedDataLocationStyle</key>
	<string>Default</string>
	<key>ShowSharedSchemesAutomaticallyEnabled</key>
	<true/>
</dict>
</plist>
"""

    # contents.xcworkspacedata
    workspaceData = f"""\
<?xml version="1.0" encoding="UTF-8"?>
<Workspace
   version = "1.0">
   <FileRef
      location = "self:">
   </FileRef>
</Workspace>
"""

    # .gitignore
    gitignore = """\
# Xcode
#
# gitignore contributors: remember to update Global/Xcode.gitignore, Objective-C.gitignore & Swift.gitignore

## User settings
xcuserdata/

## compatibility with Xcode 8 and earlier (ignoring not required starting Xcode 9)
*.xcscmblueprint
*.xccheckout

## compatibility with Xcode 3 and earlier (ignoring not required starting Xcode 4)
build/
DerivedData/
*.moved-aside
*.pbxuser
!default.pbxuser
*.mode1v3
!default.mode1v3
*.mode2v3
!default.mode2v3
*.perspectivev3
!default.perspectivev3

## Obj-C/Swift specific
*.hmap

## App packaging
*.ipa
*.dSYM.zip
*.dSYM

# CocoaPods
Pods/

# Carthage
Carthage/Build/

# fastlane
fastlane/report.xml
fastlane/Preview.html
fastlane/screenshots/**/*.png
fastlane/test_output

# Code Injection
*.xcworkspace
!default.xcworkspace

iOSInjectionProject/
.DS_Store
"""

    # Write all files
    files = [
        (f"./{projDir}/{projName}.xcodeproj/project.pbxproj", pbxproj),
        (f"./{projDir}/{projName}.xcodeproj/project.xcworkspace/contents.xcworkspacedata", workspaceData),
        (f"./{projDir}/{projName}.xcodeproj/project.xcworkspace/xcshareddata/WorkspaceSettings.xcsettings", workspaceSettings),
        (f"./{projDir}/{projName}.xcodeproj/xcshareddata/xcschemes/{projName} (iOS).xcscheme", iosScheme),
        (f"./{projDir}/{projName}.xcodeproj/xcshareddata/xcschemes/{projName} (macOS).xcscheme", macScheme),
        (f"./{projDir}/iOS-Info.plist", iosInfoPlist),
        (f"./{projDir}/macOS-Info.plist", macInfoPlist),
        (f"./{projDir}/.gitignore", gitignore),
    ]

    for filepath, content in files:
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(content)

    printSectionStart()
    printSuccess('Generated XCode Project')
    succeededProcesses.append('XCode Project Generation')
    printSectionEnd()

# endregion


# endregion
