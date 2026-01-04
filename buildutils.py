import os, sys, subprocess
from dataclasses import dataclass, asdict

# region Commandline arguments ================================================================================================

CMD_ARG_SILENT      = '-silent'      in sys.argv # Suppress output from the build script (but not from the compiler/linker)
CMD_ARG_VERY_SILENT = '-very-silent' in sys.argv # Suppress all output from the build script (including compiler/linker output)
CMD_ARG_HOST_ONLY   = '-host-only'   in sys.argv # Only build for the host platform

# endregion

# region Global Config ========================================================================================================

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
    vulkanSdk:           str = ''

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

    failed = False
    printSectionStart()

    if sys.platform == 'win32':
        toolch.windowsToolchain    = getToolchainPath('Windows', 'x64',   'WINDOWS_TOOLCHAIN_FOUND',     'WINDOWS_TOOLCHAIN')
        toolch.linuxX64Toolchain   = getToolchainPath('Linux',   'x64',   'LINUX_X64_TOOLCHAIN_FOUND',   'LINUX_X64_TOOLCHAIN')
        toolch.linuxArm64Toolchain = getToolchainPath('Linux',   'ARM64', 'LINUX_ARM64_TOOLCHAIN_FOUND', 'LINUX_ARM64_TOOLCHAIN')
        toolch.androidToolchain    = getToolchainPath('Android', 'ARM64', 'ANDROID_TOOLCHAIN_FOUND',     'ANDROID_TOOLCHAIN')

    elif sys.platform == 'darwin':
        toolch.osxTools = os.getenv('BUILD_TOOLS_PATH', '')
        toolch.iosSdk   = getToolchainPath('iOS',           'ARM64', 'IOS_SDK_FOUND',     'IOS_SDK_PATH')
        toolch.iosSdk   = getToolchainPath('iOS-Simulator', 'ARM64', 'IOS_SIM_SDK_FOUND', 'IOS_SIM_SDK_PATH')
        toolch.osxSdk   = getToolchainPath('macOS',         'ARM64', 'OSX_SDK_FOUND',     'OSX_SDK_PATH')

    else:
        failed = True
        printErr('Unsupported host platform! Only Windows and OSX are supported as host platforms.')

    vkSdkPath = os.getenv('VULKAN_SDK', '')
    if vkSdkPath:
        toolch.vulkanSdk = vkSdkPath.replace('/', '\\')
        printInfo(f'Using Vulkan SDK at: {toolch.vulkanSdk}.')

    printSectionEnd()

    if failed:
        printErr('One or more toolchains were not found or not set up correctly. Please check the output above for details.')
        sys.exit(1)

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
        output = ['/Brepro', '/nologo', '/Wall', '/WX'] + (['/MTd'] if dbg else ['/MT'])
        if useCxx:
            output += ['/EHs-', '/GR-']
    else:
        output = ['-Werror']
        if useCxx:
            output += ['-fno-exceptions', '-fno-rtti']

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

        if plt.tgt == 'android':
            output += ['-DANDROID=1', '-D_FORTIFY_SOURCE=2']

        # removed libpath additions here becauses on most platforms, we're using volk
        # which dynamically loads the vulkan library at runtime
        if TOOLCHAINS.vulkanSdk:
            if plt.tgt == 'windows':
                output += [
                    f'/I{TOOLCHAINS.vulkanSdk}/Include',
                ]
            else:
                output += [
                    f'-I{TOOLCHAINS.vulkanSdk}/Include',
                ]

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
            f'-I{plt.toolch}\\sources\\android\\native_app_glue\\'
        ]
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

    if plt.tgt == 'osx' or plt.tgt == 'ios' or plt.tgt == 'iossimulator':
        output += ['-fobjc-arc']

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
        useCxx:   bool      = False,
        objcFwks: list[str] = []
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

    if plt.tgt == 'osx' or plt.tgt == 'ios' or plt.tgt == 'iossimulator':
        output += ['-ObjC']
        for fwk in objcFwks:
            output += ['-framework', fwk]

    return output

def getDynamicLibBuildCommand(
        plt:      Platform,
        dbg:      bool,
        inputFs:  list[str],
        sysLibs:  list[str],
        outputF:  str,
        useCxx:   bool      = False,
        objcFwks: list[str] = []
    ) -> list[str]:
    output = getExecBuildCommand(
        plt      = plt,
        dbg      = dbg,
        inputFs  = inputFs,
        sysLibs  = sysLibs,
        outputF  = outputF,
        useCxx   = useCxx,
        objcFwks = objcFwks,
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
            defines = ['_DEBUG'] if plt.tgt == 'windows' else ['DEBUG'],
            compilerArgs = getCommonCompilationArgs(
                plt          = plt,
                dbg          = False,
                compileOnly  = False,
                addEnvArgs   = False,
                addStdArgs   = False,
                useCxx       = False,
            ),
        )

        if TOOLCHAINS.vulkanSdk:
            config.includePath += [f'{TOOLCHAINS.vulkanSdk}\\Include'.replace('\\', '/')]

        if plt.tgt == 'android':
            config.defines += ['ANDROID=1', '_FORTIFY_SOURCE=2']
            config.includePath += [
                f'{plt.toolch}\\toolchains\\llvm\\prebuilt\\windows-x86_64\\sysroot\\usr\\include'.replace('\\', '/'),
                f'{plt.toolch}\\sources\\android\\native_app_glue'.replace('\\', '/')
            ]

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
