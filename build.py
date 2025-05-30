import os, sys, subprocess

# region Commandline arguments ================================================================================================

CMD_ARG_REBUILD_INTRINSICS  = '-rebuild-intrinsics' # Rebuild the intrinsics dependency
CMD_ARG_RUN_TESTS           = '-tests'              # Run the tests after building
CMD_ARG_REGENERATE_BINDINGS = '-regen-bindings'     # Regenerate the bindings after building

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
            '/nologo',
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
            '/ISource/'
            f'/Fe{outputFile}',
            f'/FoTemp/TestRunner-{tgt}-{arch}.obj',
            f'FdBinaries/TestRunner-{tgt}-{arch}.pdb',
        ] + MSVC_DEBUG_ARGS
    else:
        return [
            inputFile,
            f'-o{outputFile}',
        ] + CLANG_DEBUG_ARGS

def getBindingsGeneratorExecutablePath(tgt: str, arch: str) -> str:
    return f'Binaries/BindingsGenerator-{tgt}-{arch}{'.exe' if tgt == 'windows' else ''}'

def getBindGenBuildArgs(tgt: str, arch: str) -> list[str]:
    inputFile = 'Tools/BindGen/BindingsGenerator.c'
    outputFile = getBindingsGeneratorExecutablePath(tgt, arch)

    if tgt == 'windows':
        return [
            inputFile,
            f'/Fo{outputFile}',
        ]
    else:
        return [
            inputFile,
            f'-o{outputFile}',
        ]

# endregion

# region Global State =========================================================================================================

failedProcesses: list[str] = []  # List to track failed processes

# endregion

#region Helper functions ======================================================================================================

def printSectionBreak():
    print('=' * 80)
    print('')
    print('')

def printInfo(message: str):
    print(f'\033[1;36m[INFO]:    \033[0m{message}')

def printWarn(message: str):
    print(f'\033[1;33m[WARNING]: \033[0m{message}')

def printErr(message: str):
    print(f'\033[1;31m[ERROR]:   \033[0m{message}')

def printSuccess(message: str):
    print(f'\033[1;32m[SUCCESS]: \033[0m {message}')
    printSectionBreak()

def printFailure(message: str):
    print(f'\033[1;31m[FAILURE]: \033[0m{message}')
    printSectionBreak()

def runCommand(command: list[str], name: str) -> bool:
    printInfo(f'Running: {name}')
    result = subprocess.run(command, stdout = sys.stdout)
    if result.returncode == 0:
        printSuccess(f'Completed successfully: {name}')
    else:
        printFailure(f'Failed to complete: {name}')
        failedProcesses.append(name)
    return result.returncode == 0

# endregion

# region Main Build Function ==================================================================================================

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
        printInfo(f'Rebuilding intrinsics for {prettyTgt}-{prettyArch}...')
        args = commonCompilerArgs + getIntrinsicsCompileArgs(tgt, arch)
        if not runCommand([compiler] + args, f'{prettyTgt}-{prettyArch} Intrinsics Compile'):
            return False

    printInfo(f'Compiling Panshilar library for {prettyTgt}-{prettyArch}...')
    args = commonCompilerArgs + getLibraryCompileArgs(tgt, arch) + envArgs
    if not runCommand([compiler] + args, f'{prettyTgt}-{prettyArch} Library Compile'):
        return False

    printInfo(f'Linking Panshilar library for {prettyTgt}-{prettyArch}...')
    args = getLibraryLinkArgs(tgt, arch)
    if not runCommand([linker] + args, f'{prettyTgt}-{prettyArch} Library Link'):
        return False

    testsSuccessful = True
    if runTests:
        printInfo(f'Building Test Runner for {prettyTgt}-{prettyArch}...')
        args = commonCompilerArgs + getTestRunnerBuildArgs(tgt, arch)
        testsSuccessful = runCommand([compiler] + args, f'{prettyTgt}-{prettyArch} Test Runner Build') and \
                          runCommand([getTestRunnerExecutablePath(tgt, arch)], f'{prettyTgt}-{prettyArch} Test Runner Execution')

    bindGenSuccessful = True
    if regenerateBindings:
        printInfo(f'Regenerating bindings for {prettyTgt}-{prettyArch}...')
        args = commonCompilerArgs + getBindGenBuildArgs(tgt, arch)
        bindGenSuccessful = runCommand([compiler] + args, f'{prettyTgt}-{prettyArch} BindGen Build') and \
                            runCommand([getBindingsGeneratorExecutablePath(tgt, arch)], f'{prettyTgt}-{prettyArch} BindGen Execution')

    return testsSuccessful and bindGenSuccessful

# endregion

def main():

    # region Gather commandline arguments =====================================================================================

    rebuildIntrinsics  = CMD_ARG_REBUILD_INTRINSICS  in sys.argv
    runTests           = CMD_ARG_RUN_TESTS           in sys.argv
    regenerateBindings = CMD_ARG_REGENERATE_BINDINGS in sys.argv

    # endregion

    # region Check for toolchains =============================================================================================

    if sys.platform == 'win32':
        windowsToolchainFound    = '1' == os.getenv('WINDOWS_TOOLCHAIN_FOUND')
        linuxX64ToolchainFound   = '1' == os.getenv('LINUX_X64_TOOLCHAIN_FOUND')
        linuxArm64ToolchainFound = '1' == os.getenv('LINUX_ARM64_TOOLCHAIN_FOUND')
        androidToolchainFound    = '1' == os.getenv('ANDROID_TOOLCHAIN_FOUND')

        iosToolchainFound = False
        iosSimulatorToolchainFound = False
        osxToolchainFound = False

        if not windowsToolchainFound:
            printWarn('Windows toolchain not found!')

        if not linuxX64ToolchainFound:
            printWarn('Linux-x64 toolchain not found!')

        if not linuxArm64ToolchainFound:
            printWarn('Linux-ARM64 toolchain not found!')

        if not androidToolchainFound:
            printWarn('Android toolchain not found!')

        if not windowsToolchainFound and not linuxX64ToolchainFound and not linuxArm64ToolchainFound and not androidToolchainFound:
            printErr('No relevant toolchains found!')
            exit(1)

    elif sys.platform == 'darwin':
        windowsToolchainFound    = False
        linuxX64ToolchainFound   = False
        linuxArm64ToolchainFound = False
        androidToolchainFound    = False

        iosToolchainFound          = '1' == os.getenv('IOS_TOOLCHAIN_FOUND')
        iosSimulatorToolchainFound = '1' == os.getenv('IOS_SIMULATOR_TOOLCHAIN_FOUND')
        osxToolchainFound          = '1' == os.getenv('OSX_TOOLCHAIN_FOUND')

        if not iosToolchainFound:
            printWarn('iOS toolchain not found!')

        if not iosSimulatorToolchainFound:
            printWarn('iOS Simulator toolchain not found!')

        if not osxToolchainFound:
            printWarn('macOS toolchain not found!')

        if not iosToolchainFound and not iosSimulatorToolchainFound and not osxToolchainFound:
            printErr('No relevant toolchains found!')
            exit(1)

    else:
        print(f'Unsupported platform: {sys.platform}')
        exit(1)

    # endregion

    printSectionBreak()

    # region Windows-x64 Builds ===============================================================================================

    if windowsToolchainFound:
        windowsToolchain = os.getenv('WINDOWS_TOOLCHAIN')
        if not windowsToolchain:
            printErr('Windows toolchain path not set!')
            exit(1)

        printInfo(f'Using Windows toolchain at: {windowsToolchain}.')
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

    # endregion

    # region Linux-x64 Builds =================================================================================================

    if linuxX64ToolchainFound:
        linuxX64Toolchain = os.getenv('LINUX_X64_TOOLCHAIN')
        if not linuxX64Toolchain:
            printErr('Linux-x64 toolchain path not set!')
            exit(1)

        printInfo(f'Using Linux-x64 toolchain at: {linuxX64Toolchain}.')
        buildPlatform(
            'Linux',
            'x64',
            'linux',
            'x64',
            os.path.join(linuxX64Toolchain, 'bin', 'clang.exe'),
            os.path.join(linuxX64Toolchain, 'bin', 'llvm-ar.exe'),
            CLANG_COMMON_ARGS + [f'--sysroot={linuxX64Toolchain}\\', '--target=x86_64-pc-linux-gnu'],
            ['-DPNSLR_LINUX=1', '-DPNSLR_X64=1'],
            rebuildIntrinsics, runTests, regenerateBindings,
        )

    # endregion

    # region Linux-ARM64 Builds ===============================================================================================

    if linuxArm64ToolchainFound:
        linuxArm64Toolchain = os.getenv('LINUX_ARM64_TOOLCHAIN')
        if not linuxArm64Toolchain:
            printErr('Linux-ARM64 toolchain path not set!')
            exit(1)

        printInfo(f'Using Linux-ARM64 toolchain at: {linuxArm64Toolchain}.')
        buildPlatform(
            'Linux',
            'ARM64',
            'linux',
            'arm64',
            os.path.join(linuxArm64Toolchain, 'bin', 'clang.exe'),
            os.path.join(linuxArm64Toolchain, 'bin', 'llvm-ar.exe'),
            CLANG_COMMON_ARGS + [f'--sysroot={linuxArm64Toolchain}\\', '--target=aarch64-unknown-linux-gnu'],
            ['-DPNSLR_LINUX=1', '-DPNSLR_ARM64=1'],
            rebuildIntrinsics, runTests, regenerateBindings,
        )

    # endregion

    # region Android-ARM64 Builds =============================================================================================

    if androidToolchainFound:
        androidToolchain = os.getenv('ANDROID_TOOLCHAIN')
        if not androidToolchain:
            printErr('Android toolchain path not set!')
            exit(1)

        printInfo(f'Using Android toolchain at: {androidToolchain}.')
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

    # endregion

    # region Summary ==========================================================================================================

    if not failedProcesses:
        printSuccess('All builds completed successfully!')
    else:
        printErr('Failed processes:')
        for process in failedProcesses:
            print(f' - {process}')
        printFailure('One or more processes failed. Please check the output above for details.')

    # endregion

if __name__ == '__main__':
    main()
