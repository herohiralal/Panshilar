import os, sys, subprocess

# Commandline arguments
CMD_ARG_REBUILD_INTRINSICS  = '-rebuild-intrinsics' # Rebuild the intrinsics dependency
CMD_ARG_RUN_TESTS           = '-tests'              # Run the tests after building
CMD_ARG_REGENERATE_BINDINGS = '-regen-bindings'     # Regenerate the bindings after building

# Internal Constants
MSVC_COMMON_ARGS = ['/Brepro', '/nologo', '/std:c11', '/Wall', '/WX', '/wd4100', '/wd5045']
CLANG_COMMON_ARGS = [
    '-std=c11',
    '-Wall', '-Wextra', '-Wshadow', '-Wconversion', '-Wsign-conversion',
    '-Wdouble-promotion', '-Wfloat-equal', '-Wundef', '-Wswitch-enum',
    '-Wstrict-prototypes', '-Werror', '-Wno-unused-parameter'
]

# region File paths ===========================================================================================================

INTRINSICS_INPUT_FILE  = 'Source/Dependencies/PNSLR_Intrinsics/Intrinsics.c'
LIBRARY_INPUT_FILE     = 'Source/zzzz_Unity.c'
TEST_RUNNER_INPUT_FILE = 'Tools/TestRunner/TestRunner.c'
BIND_GEN_INPUT_FILE    = 'Tools/BindGen/BindingsGenerator.c'

def getIntrinsicsOutputFile(tgt, arch):
    return f'Source/Dependencies/PNSLR_Intrinsics/Prebuilt/intrinsics-{tgt}-{arch}.{'obj' if tgt == 'windows' else 'o'}'

def getLibraryCompileOutputFile(tgt, arch):
    return f'Temp/unity-{tgt}-{arch}.{'obj' if tgt == 'windows' else 'o'}'

def getLibraryOutputFile(tgt, arch):
    return f'Libraries/panshilar-{tgt}-{arch}.{'lib' if tgt == 'windows' else 'a'}'

def getTestRunnerOutputFile(tgt, arch):
    return f'Binaries/TestRunner-{tgt}-{arch}{'.exe' if tgt == 'windows' else ''}'

def getBindGenOutputFile(tgt, arch):
    return f'Binaries/BindingsGenerator-{tgt}-{arch}{'.exe' if tgt == 'windows' else ''}'

# endregion

#region Helper functions ======================================================================================================

def printSectionBreak():
    print('=' * 80)
    print('')
    print('')

def printInfo(message):
    print(f'\033[1;36m[INFO]:    \033[0m{message}')

def printWarn(message):
    print(f'\033[1;33m[WARNING]: \033[0m{message}')

def printErr(message):
    print(f'\033[1;31m[ERROR]:   \033[0m{message}')

def printSuccess(message):
    print(f'\033[1;32m[SUCCESS]: \033[0m {message}')
    printSectionBreak()

def printFailure(message):
    print(f'\033[1;31m[FAILURE]: \033[0m{message}')
    printSectionBreak()

def runCommand(command):
    result = subprocess.run(command, stdout = sys.stdout)
    return result.returncode == 0

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

    # region Set up commons ===================================================================================================

    objectFileExt        = 'o'
    windowsObjectFileExt = 'obj'
    staticLibExt         = 'a'
    windowsStaticLibExt  = 'lib'

    failedBuilds = []

    printSectionBreak()

    # endregion

    # region Windows-x64 Builds ===============================================================================================

    if windowsToolchainFound:
        windowsToolchain = os.getenv('WINDOWS_TOOLCHAIN')
        if not windowsToolchain:
            printErr('Windows toolchain path not set!')
            exit(1)

        printInfo(f'Using Windows toolchain at: {windowsToolchain}.')
        tgt           = 'windows'
        arch          = 'x64'
        clExecutable  = os.path.join(windowsToolchain, 'bin', 'HostX64', 'x64', 'cl.exe')
        libExecutable = os.path.join(windowsToolchain, 'bin', 'HostX64', 'x64', 'lib.exe')
        commonArgs    = MSVC_COMMON_ARGS + []

        if rebuildIntrinsics:
            printInfo('Rebuilding intrinsics...')

            args = [
                '/c',
                INTRINSICS_INPUT_FILE,
                f'/Fo{getIntrinsicsOutputFile(tgt, arch)}',
            ] + commonArgs

            if runCommand([clExecutable] + args):
                printSuccess('Intrinsics rebuilt successfully.')
            else:
                printFailure('Failed to rebuild intrinsics.')
                failedBuilds.append('Windows-x64 Intrinsics')

    # endregion

    # region Linux-x64 Builds =================================================================================================

    if linuxX64ToolchainFound:
        linuxX64Toolchain = os.getenv('LINUX_X64_TOOLCHAIN')
        if not linuxX64Toolchain:
            printErr('Linux-x64 toolchain path not set!')
            exit(1)

        printInfo(f'Using Linux-x64 toolchain at: {linuxX64Toolchain}.')
        tgt             = 'linux'
        arch            = 'x64'
        clangExecutable = os.path.join(linuxX64Toolchain, 'bin', 'clang.exe')
        arExecutable    = os.path.join(linuxX64Toolchain, 'bin', 'llvm-ar.exe')
        commonArgs      = CLANG_COMMON_ARGS + [
            f'--sysroot={linuxX64Toolchain}\\',
            '--target=x86_64-pc-linux-gnu',
        ]

        if rebuildIntrinsics:
            printInfo('Rebuilding intrinsics...')

            args = [
                '-c',
                INTRINSICS_INPUT_FILE,
                f'-o{getIntrinsicsOutputFile(tgt, arch)}',
            ] + commonArgs

            if runCommand([clangExecutable] + args):
                printSuccess('Intrinsics rebuilt successfully.')
            else:
                printFailure('Failed to rebuild intrinsics.')
                failedBuilds.append('Linux-x64 Intrinsics')

    # endregion

    # region Linux-ARM64 Builds ===============================================================================================

    if linuxArm64ToolchainFound:
        linuxArm64Toolchain = os.getenv('LINUX_ARM64_TOOLCHAIN')
        if not linuxArm64Toolchain:
            printErr('Linux-ARM64 toolchain path not set!')
            exit(1)

        printInfo(f'Using Linux-ARM64 toolchain at: {linuxArm64Toolchain}.')
        tgt             = 'linux'
        arch            = 'arm64'
        clangExecutable = os.path.join(linuxArm64Toolchain, 'bin', 'clang.exe')
        arExecutable    = os.path.join(linuxArm64Toolchain, 'bin', 'llvm-ar.exe')
        commonArgs      = CLANG_COMMON_ARGS + [
            f'--sysroot={linuxArm64Toolchain}\\',
            '--target=aarch64-unknown-linux-gnu',
        ]

        if rebuildIntrinsics:
            printInfo('Rebuilding intrinsics...')

            args = [
                '-c',
                INTRINSICS_INPUT_FILE,
                f'-o{getIntrinsicsOutputFile(tgt, arch)}',
            ] + commonArgs

            if runCommand([clangExecutable] + args):
                printSuccess('Intrinsics rebuilt successfully.')
            else:
                printFailure('Failed to rebuild intrinsics.')
                failedBuilds.append('Linux-ARM64 Intrinsics')

    # endregion

    # region Android-ARM64 Builds =============================================================================================

    if androidToolchainFound:
        androidToolchain = os.getenv('ANDROID_TOOLCHAIN')
        if not androidToolchain:
            printErr('Android toolchain path not set!')
            exit(1)

        printInfo(f'Using Android toolchain at: {androidToolchain}.')
        tgt             = 'android'
        arch            = 'arm64'
        clangExecutable = os.path.join(androidToolchain, 'bin', 'clang.exe')
        arExecutable    = os.path.join(androidToolchain, 'bin', 'llvm-ar.exe')
        commonArgs      = CLANG_COMMON_ARGS + [
            f'--sysroot={androidToolchain}\\sysroot\\',
            '--target=aarch64-linux-android28',
        ]

        if rebuildIntrinsics:
            printInfo('Rebuilding intrinsics...')

            args = [
                '-c',
                INTRINSICS_INPUT_FILE,
                f'-o{getIntrinsicsOutputFile(tgt, arch)}',
            ] + commonArgs

            if runCommand([clangExecutable] + args):
                printSuccess('Intrinsics rebuilt successfully.')
            else:
                printFailure('Failed to rebuild intrinsics.')
                failedBuilds.append('Android-ARM64 Intrinsics')

    # endregion

    exit(0)

if __name__ == '__main__':
    main()
