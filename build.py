import os, sys, subprocess
from pathlib import Path

# region Commandline arguments ================================================================================================

CMD_ARG_REBUILD_INTRINSICS  = '-rebuild-intrinsics' in sys.argv # Rebuild the intrinsics dependency
CMD_ARG_RUN_TESTS           = '-tests'              in sys.argv # Run the tests after building
CMD_ARG_REGENERATE_BINDINGS = '-rebind'             in sys.argv # Regenerate the bindings after building
CMD_ARG_SILENT              = '-silent'             in sys.argv # Suppress output from the build script (but not from the compiler/linker)
CMD_ARG_VERY_SILENT         = '-very-silent'        in sys.argv # Suppress all output from the build script (including compiler/linker output)
CMD_ARG_BUILD_WINDOWS_X64   = '+windows-x64'        in sys.argv # Build the Windows x64 target
CMD_ARG_BUILD_LINUX_X64     = '+linux-x64'          in sys.argv # Build the Linux x64 target
CMD_ARG_BUILD_LINUX_ARM64   = '+linux-arm64'        in sys.argv # Build the Linux ARM64 target
CMD_ARG_BUILD_OSX_ARM64     = '+osx-arm64'          in sys.argv # Build the macOS ARM64 target
CMD_ARG_BUILD_ANDROID_ARM64 = '+android-arm64'      in sys.argv # Build the Android ARM64 target
CMD_ARG_BUILD_IOS_ARM64     = '+ios-arm64'          in sys.argv # Build the iOS ARM64 target
CMD_ARG_BUILD_IOS_SIM_ARM64 = '+iossimulator-arm64' in sys.argv # Build the iOS Simulator ARM64 target
CMD_ARG_BUILD_ALL           = True and \
                              not CMD_ARG_BUILD_WINDOWS_X64   and \
                              not CMD_ARG_BUILD_LINUX_X64     and \
                              not CMD_ARG_BUILD_LINUX_ARM64   and \
                              not CMD_ARG_BUILD_OSX_ARM64     and \
                              not CMD_ARG_BUILD_ANDROID_ARM64 and \
                              not CMD_ARG_BUILD_IOS_ARM64     and \
                              not CMD_ARG_BUILD_IOS_SIM_ARM64 and \
                              True                               # Build all targets by default if no specific target is specified

# endregion

# region Internal Constants ===================================================================================================

MSVC_DEBUG_ARGS   = ['/Zi', '/Od', '/D_DEBUG']
MSVC_COMMON_ARGS  = ['/Brepro', '/nologo', '/std:c11', '/Wall', '/WX', '/wd4100', '/wd5045', '/wd4324', '/wd4820']
CLANG_DEBUG_ARGS  = ['-g', '-O0', '-DDEBUG']
CLANG_COMMON_ARGS = [
    '-std=c11',
    '-Wall', '-Wextra', '-Wshadow', '-Wconversion', '-Wsign-conversion',
    '-Wdouble-promotion', '-Wfloat-equal', '-Wundef', '-Wswitch-enum',
    '-Wstrict-prototypes', '-Werror', '-Wno-unused-parameter'
]

# endregion

# region File paths ===========================================================================================================

def getIntrinsicsSourcePath() -> str:
    return f'Source/Dependencies/PNSLR_Intrinsics/'

def getLibrarySourcePath() -> str:
    return f'Source/'

def getTestRunnerSourcePath() -> str:
    return f'Tools/TestRunner/'

def getBindingsGeneratorSourcePath() -> str:
    return f'Tools/BindGen/'

def getIntrinsicsObjectPath(tgt: str, arch: str) -> str:
    return f'Source/Dependencies/PNSLR_Intrinsics/Prebuilt/intrinsics-{tgt}-{arch}.{'obj' if tgt == 'windows' else 'o'}'

def getLibraryObjectPath(tgt: str, arch: str) -> str:
    return f'Temp/unity-{tgt}-{arch}.{'obj' if tgt == 'windows' else 'o'}'

def getLibraryPath(tgt: str, arch: str) -> str:
    return f'Libraries/panshilar-{tgt}-{arch}.{'lib' if tgt == 'windows' else 'a'}'

def getTestRunnerExecutablePath(tgt: str, arch: str) -> str:
    return f'Binaries/TestRunner-{tgt}-{arch}{'.exe' if tgt == 'windows' else ''}'

def getBindingsGeneratorExecutablePath(tgt: str, arch: str) -> str:
    return f'Binaries/BindingsGenerator-{tgt}-{arch}{'.exe' if tgt == 'windows' else ''}'

def getIntrinsicsCompileArgs(tgt: str, arch: str) -> list[str]:
    inputFile  = getIntrinsicsSourcePath() + 'Intrinsics.c'
    outputFile = getIntrinsicsObjectPath(tgt, arch)

    if tgt == 'windows':
        return [
            '/c',
            inputFile,
            f'/Fo{outputFile}',
        ]
    else:
        return [
            '-c',
            inputFile,
            f'-o{outputFile}',
        ]

def getLibraryCompileArgs(tgt: str, arch: str) -> list[str]:
    inputFile  = getLibrarySourcePath() + 'zzzz_Unity.c'
    outputFile = getLibraryObjectPath(tgt, arch)

    if tgt == 'windows':
        return [
            '/c',
            inputFile,
            f'/Fo{outputFile}',
        ]
    else:
        return [
            '-c',
            inputFile,
            f'-o{outputFile}',
        ]

def getLibraryLinkArgs(tgt: str, arch: str) -> list[str]:
    intrinsicsObjFile = getIntrinsicsObjectPath(tgt, arch)
    libraryObjFile    = getLibraryObjectPath(tgt, arch)
    outputFile        = getLibraryPath(tgt, arch)

    if tgt == 'windows':
        return [
            '/Brepro',
            '/NOLOGO',
            intrinsicsObjFile,
            libraryObjFile,
            f'/OUT:{outputFile}',
        ]
    else:
        return [
            'rcs',
            outputFile,
            intrinsicsObjFile,
            libraryObjFile,
        ]

def getTestRunnerBuildArgs(tgt: str, arch: str) -> list[str]:
    inputFile  = getTestRunnerSourcePath() + 'TestRunner.c'
    outputFile = getTestRunnerExecutablePath(tgt, arch)

    if tgt == 'windows':
        return [
            inputFile,
            getLibraryPath(tgt, arch),
            '/ISource/',
            f'/Fe{outputFile}',
            f'/FoTemp/TestRunner-{tgt}-{arch}.obj',
            f'/FdBinaries/TestRunner-{tgt}-{arch}.pdb',
        ] + MSVC_DEBUG_ARGS
    else:
        return [
            inputFile,
            getLibraryPath(tgt, arch),
            '-o',
            outputFile,
            f'-ISource/',
            # '-v',
            # '-###',
        ] + CLANG_DEBUG_ARGS

def getBindGenBuildArgs(tgt: str, arch: str) -> list[str]:
    inputFile  = getBindingsGeneratorSourcePath() + 'BindingsGenerator.c'
    outputFile = getBindingsGeneratorExecutablePath(tgt, arch)

    if tgt == 'windows':
        return [
            inputFile,
            getLibraryPath(tgt, arch),
            '/ISource',
            f'/Fe{outputFile}',
            f'/FoTemp/BindingsGenerator-{tgt}-{arch}.obj',
            f'/FdBinaries/BindingsGenerator-{tgt}-{arch}.pdb',
        ] + MSVC_DEBUG_ARGS
    else:
        return [
            inputFile,
            getLibraryPath(tgt, arch),
            '-o',
            outputFile,
            f'-ISource/',
            # '-v',
            # '-###',
        ] + CLANG_DEBUG_ARGS

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
    # printDebug(f'Command: {' '.join(command)}')
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

# endregion

# region Main Logic  ==========================================================================================================

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

ALL_INTRINSICS_FILES:  list[str] = [str(f) for f in Path(getIntrinsicsSourcePath()       ).glob('*.c') if f.is_file()]
ALL_LIBRARY_FILES:     list[str] = [str(f) for f in Path(getLibrarySourcePath()          ).glob('*.c') if f.is_file()]
ALL_TEST_RUNNER_FILES: list[str] = [str(f) for f in Path(getTestRunnerSourcePath()       ).glob('*.c') if f.is_file()]
ALL_BIND_GEN_FILES:    list[str] = [str(f) for f in Path(getBindingsGeneratorSourcePath()).glob('*.c') if f.is_file()]

def buildPlatform(
        actuallyBuild:      bool,
        prettyTgt:          str,
        prettyArch:         str,
        tgt:                str,
        arch:               str,
        compiler:           str,
        linker:             str,
        commonCompilerArgs: list[str],
        envArgs:            list[str],
    ) -> bool:

    runTools       = (tgt == 'linux' or tgt == 'windows' or tgt == 'osx') and (arch == ('x64' if tgt == 'windows' else 'arm64' if tgt == 'osx' else ''))
    actuallyBuild2 = actuallyBuild or CMD_ARG_BUILD_ALL

    intrinsicsCompiled    = True
    intrinsicsCompileArgs = commonCompilerArgs + getIntrinsicsCompileArgs(tgt, arch)
    if CMD_ARG_REBUILD_INTRINSICS:
        intrinsicsCompiled = (not actuallyBuild2) or \
                             (True and runCommand([compiler] + intrinsicsCompileArgs, f'{prettyTgt}-{prettyArch} Intrinsics Compile'))

    libraryCompiled    = True
    libraryCompileArgs = commonCompilerArgs + getLibraryCompileArgs(tgt, arch) + envArgs
    libraryCompiled    = (not actuallyBuild2) or \
                         (intrinsicsCompiled and runCommand([compiler] + libraryCompileArgs, f'{prettyTgt}-{prettyArch} Library Compile'))

    libraryLinked   = True
    libraryLinkArgs = getLibraryLinkArgs(tgt, arch)
    libraryLinked   = (not actuallyBuild2) or \
                      (libraryCompiled and runCommand([linker] + libraryLinkArgs, f'{prettyTgt}-{prettyArch} Library Link'))

    testsSuccessful     = True
    testRunnerBuildArgs = commonCompilerArgs + getTestRunnerBuildArgs(tgt, arch) + envArgs
    testsSuccessful     = (not actuallyBuild2) or (not CMD_ARG_RUN_TESTS) or (not runTools) or \
                          (libraryLinked and runCommand([compiler] + testRunnerBuildArgs, f'{prettyTgt}-{prettyArch} Test Runner Build'))

    bindGenSuccessful          = True
    bindingsGeneratorBuildArgs = commonCompilerArgs + getBindGenBuildArgs(tgt, arch) + envArgs
    bindGenSuccessful          = (not actuallyBuild2) or (not CMD_ARG_REGENERATE_BINDINGS) or (not runTools) or \
                                 (libraryLinked and runCommand([compiler] + bindingsGeneratorBuildArgs, f'{prettyTgt}-{prettyArch} Bindings Generator Build'))

    compileCommands: list[dict[str, str]] = []

    for file in ALL_INTRINSICS_FILES:
        compileCommands.append({
            'directory': '..',
            'command':   '"' + '" "'.join([compiler] + getIntrinsicsCompileArgs(tgt, arch)) + '"',
            'file':      file,
        })

    for file in ALL_LIBRARY_FILES:
        compileCommands.append({
            'directory': '..',
            'command':   '"' + '" "'.join([compiler] + getLibraryCompileArgs(tgt, arch)) + '"',
            'file':      file,
        })

    for file in ALL_TEST_RUNNER_FILES:
        compileCommands.append({
            'directory': '..',
            'command':   '"' + '" "'.join([compiler] + getTestRunnerBuildArgs(tgt, arch)) + '"',
            'file':      file,
        })

    for file in ALL_BIND_GEN_FILES:
        compileCommands.append({
            'directory': '..',
            'command':   '"' + '" "'.join([compiler] + getBindGenBuildArgs(tgt, arch)) + '"',
            'file':      file,
        })

    compileCommandsFile = f'Build/CompileCommands-{prettyTgt}-{prettyArch}.json'
    os.makedirs(os.path.dirname(compileCommandsFile), exist_ok=True)
    with open(compileCommandsFile, 'w') as f:
        import json
        json.dump(compileCommands, f, indent = 2)

    return libraryLinked and testsSuccessful and bindGenSuccessful

# endregion

def main():

    # region Check for toolchains =============================================================================================

    windowsToolchain      = ''
    linuxX64Toolchain     = ''
    linuxArm64Toolchain   = ''
    androidToolchain      = ''
    osxTools              = ''
    iosToolchain          = ''
    iosSimulatorToolchain = ''
    osxToolchain          = ''

    if sys.platform == 'win32':
        printSectionStart()
        windowsToolchain    = getToolchainPath('Windows', 'x64',   'WINDOWS_TOOLCHAIN_FOUND',     'WINDOWS_TOOLCHAIN')
        linuxX64Toolchain   = getToolchainPath('Linux',   'x64',   'LINUX_X64_TOOLCHAIN_FOUND',   'LINUX_X64_TOOLCHAIN')
        linuxArm64Toolchain = getToolchainPath('Linux',   'ARM64', 'LINUX_ARM64_TOOLCHAIN_FOUND', 'LINUX_ARM64_TOOLCHAIN')
        androidToolchain    = getToolchainPath('Android', 'ARM64', 'ANDROID_TOOLCHAIN_FOUND',     'ANDROID_TOOLCHAIN')
        printSectionEnd()

    elif sys.platform == 'darwin':
        printSectionStart()
        osxTools              = os.getenv('BUILD_TOOLS_PATH', '')
        iosToolchain          = getToolchainPath('iOS',           'ARM64', 'IOS_SDK_FOUND',     'IOS_SDK_PATH')
        iosSimulatorToolchain = getToolchainPath('iOS-Simulator', 'ARM64', 'IOS_SIM_SDK_FOUND', 'IOS_SIM_SDK_PATH')
        osxToolchain          = getToolchainPath('macOS',         'ARM64', 'OSX_SDK_FOUND',     'OSX_SDK_PATH')
        printSectionEnd()

    else:
        print2(f'Unsupported platform: {sys.platform}')
        exit(1)

    # endregion

    # region Builds ===========================================================================================================

    if windowsToolchain:
        buildPlatform(
            CMD_ARG_BUILD_WINDOWS_X64,
            'Windows',
            'x64',
            'windows',
            'x64',
            os.path.join(windowsToolchain, 'bin', 'HostX64', 'x64', 'cl.exe'),
            os.path.join(windowsToolchain, 'bin', 'HostX64', 'x64', 'lib.exe'),
            MSVC_COMMON_ARGS + [],
            ['/DPNSLR_WINDOWS=1', '/DPNSLR_X64=1'],
        )

    if linuxX64Toolchain:
        buildPlatform(
            CMD_ARG_BUILD_LINUX_X64,
            'Linux',
            'x64',
            'linux',
            'x64',
            os.path.join(linuxX64Toolchain, 'bin', 'clang.exe'),
            os.path.join(linuxX64Toolchain, 'bin', 'llvm-ar.exe'),
            CLANG_COMMON_ARGS + [f'--sysroot={linuxX64Toolchain}\\', '--target=x86_64-unknown-linux-gnu'],
            ['-DPNSLR_LINUX=1', '-DPNSLR_X64=1'],
        )

    if osxTools and osxToolchain:
        buildPlatform(
            CMD_ARG_BUILD_OSX_ARM64,
            'MacOS',
            'Apple Silicon',
            'osx',
            'arm64',
            os.path.join(osxTools, 'usr', 'bin', 'clang'),
            os.path.join(osxTools, 'usr', 'bin', 'ar'),
            CLANG_COMMON_ARGS + [
                '--sysroot',
                osxToolchain,
                '-target',
                'arm64-apple-macos11.0',
            ],
            ['-DPNSLR_OSX=1', '-DPNSLR_ARM64=1'],
        )

    if linuxArm64Toolchain:
        buildPlatform(
            CMD_ARG_BUILD_LINUX_ARM64,
            'Linux',
            'ARM64',
            'linux',
            'arm64',
            os.path.join(linuxArm64Toolchain, 'bin', 'clang.exe'),
            os.path.join(linuxArm64Toolchain, 'bin', 'llvm-ar.exe'),
            CLANG_COMMON_ARGS + [f'--sysroot={linuxArm64Toolchain}\\', '--target=aarch64-unknown-linux-gnueabi'],
            ['-DPNSLR_LINUX=1', '-DPNSLR_ARM64=1'],
        )

    if androidToolchain:
        buildPlatform(
            CMD_ARG_BUILD_ANDROID_ARM64,
            'Android',
            'ARM64',
            'android',
            'arm64',
            os.path.join(androidToolchain, 'bin', 'clang.exe'),
            os.path.join(androidToolchain, 'bin', 'llvm-ar.exe'),
            CLANG_COMMON_ARGS + [f'--sysroot={androidToolchain}\\sysroot\\', '--target=aarch64-linux-android28'],
            ['-DPNSLR_ANDROID=1', '-DPNSLR_ARM64=1'],
        )

    if osxTools and iosToolchain:
        buildPlatform(
            CMD_ARG_BUILD_IOS_ARM64,
            'iOS',
            'ARM64',
            'ios',
            'arm64',
            os.path.join(osxTools, 'usr', 'bin', 'clang'),
            os.path.join(osxTools, 'usr', 'bin', 'ar'),
            CLANG_COMMON_ARGS + [
                '--sysroot',
                iosToolchain,
                '-miphoneos-version-min=16.0',
                '-target',
                'aarch64-apple-ios16.0',
                '-arch',
                'arm64',
            ],
            ['-DPNSLR_IOS=1', '-DPNSLR_ARM64=1'],
        )

    if osxTools and iosSimulatorToolchain:
        buildPlatform(
            CMD_ARG_BUILD_IOS_SIM_ARM64,
            'iOS-Simulator',
            'ARM64',
            'iossimulator',
            'arm64',
            os.path.join(osxTools, 'usr', 'bin', 'clang'),
            os.path.join(osxTools, 'usr', 'bin', 'ar'),
            CLANG_COMMON_ARGS + [
                '--sysroot',
                iosSimulatorToolchain,
                '-miphoneos-version-min=16.0',
                '-target',
                'aarch64-apple-ios16.0-simulator',
                '-arch',
                'arm64',
            ],
            ['-DPNSLR_IOS=1', '-DPNSLR_ARM64=1'],
        )

    # endregion

    # region Summary ==========================================================================================================

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

    sys.exit(0 if not failedProcesses else 1)

    # endregion

if __name__ == '__main__':
    main()
