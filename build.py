import os, sys, subprocess

# region Commandline arguments ================================================================================================

CMD_ARG_REBUILD_INTRINSICS  = '-rebuild-intrinsics' # Rebuild the intrinsics dependency
CMD_ARG_RUN_TESTS           = '-tests'              # Run the tests after building
CMD_ARG_REGENERATE_BINDINGS = '-rebind'             # Regenerate the bindings after building

# endregion

# region Internal Constants ===================================================================================================

MSVC_DEBUG_ARGS   = ['/Zi', '/Od', '/D_DEBUG']
MSVC_COMMON_ARGS  = ['/Brepro', '/nologo', '/std:c11', '/Wall', '/WX', '/wd4100', '/wd5045']
CLANG_DEBUG_ARGS  = ['-g', '-O0', '-DDEBUG']
CLANG_COMMON_ARGS = [
    '-std=c11',
    '-Wall', '-Wextra', '-Wshadow', '-Wconversion', '-Wsign-conversion',
    '-Wdouble-promotion', '-Wfloat-equal', '-Wundef', '-Wswitch-enum',
    '-Wstrict-prototypes', '-Werror', '-Wno-unused-parameter'
]

# endregion

# region File paths ===========================================================================================================

def getIntrinsicsObjectPath(tgt: str, arch: str) -> str:
    return f'Source/Dependencies/PNSLR_Intrinsics/Prebuilt/intrinsics-{tgt}-{arch}.{'obj' if tgt == 'windows' else 'o'}'

def getIntrinsicsCompileArgs(tgt: str, arch: str) -> list[str]:
    inputFile = 'Source/Dependencies/PNSLR_Intrinsics/Intrinsics.c'
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

def getLibraryObjectPath(tgt: str, arch: str) -> str:
    return f'Temp/unity-{tgt}-{arch}.{'obj' if tgt == 'windows' else 'o'}'

def getLibraryCompileArgs(tgt: str, arch: str) -> list[str]:
    inputFile = 'Source/zzzz_Unity.c'
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

def getLibraryPath(tgt: str, arch: str) -> str:
    return f'Libraries/panshilar-{tgt}-{arch}.{'lib' if tgt == 'windows' else 'a'}'

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

def getTestRunnerExecutablePath(tgt: str, arch: str) -> str:
    return f'Binaries/TestRunner-{tgt}-{arch}{'.exe' if tgt == 'windows' else ''}'

def getTestRunnerBuildArgs(tgt: str, arch: str) -> list[str]:
    inputFile  = 'Tools/TestRunner/TestRunner.c'
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

def getBindingsGeneratorExecutablePath(tgt: str, arch: str) -> str:
    return f'Binaries/BindingsGenerator-{tgt}-{arch}{'.exe' if tgt == 'windows' else ''}'

def getBindGenBuildArgs(tgt: str, arch: str) -> list[str]:
    inputFile = 'Tools/BindGen/BindingsGenerator.c'
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

failedProcesses: list[str] = []  # List to track failed processes

# endregion

#region Helper functions ======================================================================================================

def printSectionStart():
    print('')
    print('=' * 80)

def printSectionEnd():
    print('=' * 80)
    print('')

def printDebug(message: str):
   print(f'\033[1m[DEBUG]:\033[0m\033[90m   {message}\033[0m')

def printInfo(message: str):
    print(f'\033[1;36m[INFO]:    \033[0m{message}')

def printWarn(message: str):
    print(f'\033[1;33m[WARNING]: \033[0m{message}')

def printErr(message: str):
    print(f'\033[1;31m[ERROR]:   \033[0m{message}')

def printSuccess(message: str):
    print(f'\033[1;32m[SUCCESS]: \033[0m {message}')

def printFailure(message: str):
    print(f'\033[1;31m[FAILURE]: \033[0m{message}')

def runCommand(command: list[str], name: str) -> bool:
    printSectionStart()
    printInfo(f'Running: {name}')
    printDebug(f'Command: {" ".join(command)}')
    result = subprocess.run(command, stdout = sys.stdout)
    if result.returncode == 0:
        printSuccess(f'Completed successfully: {name}')
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

def buildPlatform(
        prettyTgt:          str,
        prettyArch:         str,
        tgt:                str,
        arch:               str,
        compiler:           str,
        linker:             str,
        commonCompilerArgs: list[str],
        envArgs:            list[str],
        rebuildIntrinsics:  bool,
        runTests:           bool,
        regenerateBindings: bool
    ) -> bool:
    if rebuildIntrinsics:
        args = commonCompilerArgs + getIntrinsicsCompileArgs(tgt, arch)
        if not runCommand([compiler] + args, f'{prettyTgt}-{prettyArch} Intrinsics Compile'):
            return False

    args = commonCompilerArgs + getLibraryCompileArgs(tgt, arch) + envArgs
    if not runCommand([compiler] + args, f'{prettyTgt}-{prettyArch} Library Compile'):
        return False

    args = getLibraryLinkArgs(tgt, arch)
    if not runCommand([linker] + args, f'{prettyTgt}-{prettyArch} Library Link'):
        return False

    testsSuccessful = True
    if runTests and (tgt == 'linux' or tgt == 'windows' or tgt == 'osx') and (arch == ('x64' if tgt == 'windows' else 'arm64' if tgt == 'osx' else '')):
        args = commonCompilerArgs + getTestRunnerBuildArgs(tgt, arch) + envArgs
        testsSuccessful = runCommand([compiler] + args, f'{prettyTgt}-{prettyArch} Test Runner Build') # and \ TODO: re-enable
        #                 runCommand([getTestRunnerExecutablePath(tgt, arch)], f'{prettyTgt}-{prettyArch} Test Runner Execution')

    bindGenSuccessful = True
    if regenerateBindings and (tgt == 'linux' or tgt == 'windows' or tgt == 'osx') and (arch == ('x64' if tgt == 'windows' else 'arm64' if tgt == 'osx' else '')):
        args = commonCompilerArgs + getBindGenBuildArgs(tgt, arch) + envArgs
        bindGenSuccessful = runCommand([compiler] + args, f'{prettyTgt}-{prettyArch} BindGen Build') # and \ TODO: re-enable
        #                   runCommand([getBindingsGeneratorExecutablePath(tgt, arch)], f'{prettyTgt}-{prettyArch} BindGen Execution')

    return testsSuccessful and bindGenSuccessful

# endregion

def main():

    # region Gather commandline arguments =====================================================================================

    rebuildIntrinsics  = CMD_ARG_REBUILD_INTRINSICS  in sys.argv
    runTests           = CMD_ARG_RUN_TESTS           in sys.argv
    regenerateBindings = CMD_ARG_REGENERATE_BINDINGS in sys.argv

    # endregion

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
        print(f'Unsupported platform: {sys.platform}')
        exit(1)

    # endregion

    # region Builds ===========================================================================================================

    if windowsToolchain:
        buildPlatform(
            'Windows',
            'x64',
            'windows',
            'x64',
            os.path.join(windowsToolchain, 'bin', 'HostX64', 'x64', 'cl.exe'),
            os.path.join(windowsToolchain, 'bin', 'HostX64', 'x64', 'lib.exe'),
            MSVC_COMMON_ARGS + [],
            ['/DPNSLR_WINDOWS=1', '/DPNSLR_X64=1'],
            rebuildIntrinsics, runTests, regenerateBindings,
        )

    if linuxX64Toolchain:
        buildPlatform(
            'Linux',
            'x64',
            'linux',
            'x64',
            os.path.join(linuxX64Toolchain, 'bin', 'clang.exe'),
            os.path.join(linuxX64Toolchain, 'bin', 'llvm-ar.exe'),
            CLANG_COMMON_ARGS + [f'--sysroot={linuxX64Toolchain}\\', '--target=x86_64-unknown-linux-gnu'],
            ['-DPNSLR_LINUX=1', '-DPNSLR_X64=1'],
            rebuildIntrinsics, runTests, regenerateBindings,
        )

    if osxTools and osxToolchain:
        buildPlatform(
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
            rebuildIntrinsics, runTests, regenerateBindings,
        )

    if linuxArm64Toolchain:
        buildPlatform(
            'Linux',
            'ARM64',
            'linux',
            'arm64',
            os.path.join(linuxArm64Toolchain, 'bin', 'clang.exe'),
            os.path.join(linuxArm64Toolchain, 'bin', 'llvm-ar.exe'),
            CLANG_COMMON_ARGS + [f'--sysroot={linuxArm64Toolchain}\\', '--target=aarch64-unknown-linux-gnueabi'],
            ['-DPNSLR_LINUX=1', '-DPNSLR_ARM64=1'],
            rebuildIntrinsics, runTests, regenerateBindings,
        )

    if androidToolchain:
        buildPlatform(
            'Android',
            'ARM64',
            'android',
            'arm64',
            os.path.join(androidToolchain, 'bin', 'clang.exe'),
            os.path.join(androidToolchain, 'bin', 'llvm-ar.exe'),
            CLANG_COMMON_ARGS + [f'--sysroot={androidToolchain}\\sysroot\\', '--target=aarch64-linux-android28'],
            ['-DPNSLR_ANDROID=1', '-DPNSLR_ARM64=1'],
            rebuildIntrinsics, runTests, regenerateBindings,
        )

    if osxTools and iosToolchain:
        buildPlatform(
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
            rebuildIntrinsics, runTests, regenerateBindings,
        )

    if osxTools and iosSimulatorToolchain:
        buildPlatform(
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
            ['-DPNSLR_IOS_SIMULATOR=1', '-DPNSLR_ARM64=1'],
            rebuildIntrinsics, runTests, regenerateBindings,
        )

    # endregion

    # region Summary ==========================================================================================================

    printSectionStart()
    if not failedProcesses:
        printSuccess('All builds completed successfully!')
    else:
        printErr('Failed processes:')
        for process in failedProcesses:
            print(f' - {process}')
        printFailure('One or more processes failed. Please check the output above for details.')
    printSectionEnd()

    sys.exit(0 if not failedProcesses else 1)

    # endregion

if __name__ == '__main__':
    main()
