import os, sys, subprocess
from dataclasses import dataclass, asdict

# region Commandline arguments ================================================================================================

CMD_ARG_REBUILD_INTRINSICS  = '-rebuild-intrinsics' in sys.argv # Rebuild the intrinsics dependency
CMD_ARG_RUN_TESTS           = '-tests'              in sys.argv # Run the tests after building
CMD_ARG_REGENERATE_BINDINGS = '-rebind'             in sys.argv # Regenerate the bindings after building
CMD_ARG_SILENT              = '-silent'             in sys.argv # Suppress output from the build script (but not from the compiler/linker)
CMD_ARG_VERY_SILENT         = '-very-silent'        in sys.argv # Suppress all output from the build script (including compiler/linker output)

# endregion

# region Internal Constants ===================================================================================================

MSVC_DEBUG_ARGS    = ['/Zi', '/Od', '/D_DEBUG']
MSVC_C_STD_ARGS    = ['/std:c11']
MSVC_CXX_STD_ARGS  = ['/std:c++14']
MSVC_COMMON_ARGS   = ['/Brepro', '/nologo', '/Wall', '/WX', '/wd4100', '/wd5045', '/wd4324', '/wd4820', '/wd4127']
CLANG_DEBUG_ARGS   = ['-g', '-O0', '-DDEBUG']
CLANG_C_STD_ARGS   = ['-std=c11']
CLANG_CXX_STD_ARGS = ['-std=c++14']
CLANG_COMMON_ARGS  = [
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
        ] # + MSVC_DEBUG_ARGS
    else:
        return [
            '-c',
            inputFile,
            f'-o{outputFile}',
        ] # + CLANG_DEBUG_ARGS

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
    inputFile  = getTestRunnerSourcePath() + 'zzzz_TestRunner.c'
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
            '-lpthread',
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
            '-lpthread',
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

def buildPlatform(
        prettyTgt:          str,
        prettyArch:         str,
        tgt:                str,
        arch:               str,
        cCompiler:          str,
        cxxCompiler:        str,
        cStdArgs:           list[str],
        cxxStdArgs:         list[str],
        linker:             str,
        commonCompilerArgs: list[str],
        envArgs:            list[str],
    ) -> bool:

    runTools       = (tgt == 'linux' or tgt == 'windows' or tgt == 'osx')
    actuallyBuild2 = True

    intrinsicsCompiled    = True
    intrinsicsCompileArgs = commonCompilerArgs + getIntrinsicsCompileArgs(tgt, arch)
    if CMD_ARG_REBUILD_INTRINSICS:
        intrinsicsCompiled = (not actuallyBuild2) or \
                             (True and runCommand([cCompiler] + intrinsicsCompileArgs + cStdArgs, f'{prettyTgt}-{prettyArch} Intrinsics Compile'))

    libraryCompiled    = True
    libraryCompileArgs = commonCompilerArgs + getLibraryCompileArgs(tgt, arch) + envArgs
    libraryCompiled    = (not actuallyBuild2) or \
                         (intrinsicsCompiled and runCommand([cCompiler] + libraryCompileArgs + cStdArgs, f'{prettyTgt}-{prettyArch} Library Compile'))

    libraryLinked   = True
    libraryLinkArgs = getLibraryLinkArgs(tgt, arch)
    libraryLinked   = (not actuallyBuild2) or \
                      (libraryCompiled and runCommand([linker] + libraryLinkArgs, f'{prettyTgt}-{prettyArch} Library Link'))

    testsSuccessful     = True
    testRunnerBuildArgs = commonCompilerArgs + getTestRunnerBuildArgs(tgt, arch) + envArgs
    testsSuccessful     = (not actuallyBuild2) or (not CMD_ARG_RUN_TESTS) or (not runTools) or \
                          (libraryLinked and runCommand([cCompiler] + testRunnerBuildArgs + cStdArgs, f'{prettyTgt}-{prettyArch} Test Runner Build'))

    bindGenSuccessful          = True
    bindingsGeneratorBuildArgs = commonCompilerArgs + getBindGenBuildArgs(tgt, arch) + envArgs
    bindGenSuccessful          = (not actuallyBuild2) or (not CMD_ARG_REGENERATE_BINDINGS) or (not runTools) or \
                                 (libraryLinked and runCommand([cCompiler] + bindingsGeneratorBuildArgs + cStdArgs, f'{prettyTgt}-{prettyArch} Bindings Generator Build'))

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

    # region Setup Tests ======================================================================================================

    if CMD_ARG_RUN_TESTS:
        sourcePath = getTestRunnerSourcePath()
        tests = [f.rstrip('.c') for f in os.listdir(sourcePath) if f.endswith('.c') and not f.startswith('zzzz_')]
        outputFilePath = os.path.join(sourcePath, 'zzzz_GeneratedCombinedTests.c')
        with open(outputFilePath, 'w', encoding='utf-8') as outputFile:
            outputFile.write('// This file is generated by the build script. Do not edit it manually.\n\n')

            for test in tests:
                outputFile.write('#undef MAIN_TEST_FN\n')
                outputFile.write(f'#define MAIN_TEST_FN(ctxArgName) void ZZZZ_Test_{test}(const TestContext* ctxArgName)\n')
                outputFile.write(f'#include "{test}.c"\n')
                outputFile.write('#undef MAIN_TEST_FN\n\n')

            outputFile.write(f'u64 ZZZZ_GetTestsCount(void) {{ return {len(tests)}; }}\n\n')
            outputFile.write('void ZZZZ_GetAllTests(ArraySlice(TestFunctionInfo) fns)\n')
            outputFile.write('{\n')

            i: int = 0
            for test in tests:
                outputFile.write(f'    fns.data[{i}].name = PNSLR_STRING_LITERAL("{test}");\n')
                outputFile.write(f'    fns.data[{i}].fn   = ZZZZ_Test_{test};\n\n')
                i += 1

            outputFile.write('    // done\n')
            outputFile.write('}\n')

    # endregion

    # region Project Files Definitions ========================================================================================

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

    # endregion

    # region Builds ===========================================================================================================

    if windowsToolchain:
        commonArgs = MSVC_COMMON_ARGS + []

        buildPlatform(
            'Windows',
            'x64',
            'windows',
            'x64',
            os.path.join(windowsToolchain, 'bin', 'HostX64', 'x64', 'cl.exe'),
            os.path.join(windowsToolchain, "bin", 'HostX64', 'x64', 'cl.exe'),
            MSVC_C_STD_ARGS,
            MSVC_CXX_STD_ARGS,
            os.path.join(windowsToolchain, 'bin', 'HostX64', 'x64', 'lib.exe'),
            commonArgs,
            ['/DPNSLR_WINDOWS=1', '/DPNSLR_X64=1'],
        )

        properties.configurations.append(CCppPropertiesConfiguration(
            name             = 'Windows-x64',
            compilerPath     = os.path.join(windowsToolchain, 'bin', 'HostX64', 'x64', 'cl.exe').replace('\\', '/'),
            cStandard        = 'c11',
            cppStandard      = 'c++14',
            includePath      = [
                '${workspaceFolder}/Source'
            ],
            defines          = ['PNSLR_WINDOWS=1', 'PNSLR_X64=1'],
            compilerArgs     = commonArgs
        ))

    if linuxX64Toolchain:
        commonArgs = CLANG_COMMON_ARGS + [f'--sysroot={linuxX64Toolchain}\\', '--target=x86_64-unknown-linux-gnu']

        buildPlatform(
            'Linux',
            'x64',
            'linux',
            'x64',
            os.path.join(linuxX64Toolchain, 'bin', 'clang.exe'),
            os.path.join(linuxX64Toolchain, 'bin', 'clang++.exe'),
            CLANG_C_STD_ARGS,
            CLANG_CXX_STD_ARGS,
            os.path.join(linuxX64Toolchain, 'bin', 'llvm-ar.exe'),
            commonArgs,
            ['-DPNSLR_LINUX=1', '-DPNSLR_X64=1'],
        )

        properties.configurations.append(CCppPropertiesConfiguration(
            name             = 'Linux-x64',
            compilerPath     = os.path.join(linuxX64Toolchain, 'bin', 'clang.exe').replace('\\', '/'),
            cStandard        = 'c11',
            cppStandard      = 'c++14',
            includePath      = [
                '${workspaceFolder}/Source',
                f'{linuxX64Toolchain}\\usr\\include'.replace('\\', '/')
            ],
            defines          = ['PNSLR_LINUX=1', 'PNSLR_X64=1'],
            compilerArgs     = commonArgs,
        ))

    if osxTools and osxToolchain:
        commonArgs = CLANG_COMMON_ARGS + ['--sysroot', osxToolchain, '-target', 'x86_64-apple-macos11.0']

        buildPlatform(
            'MacOS',
            'Apple Silicon',
            'osx',
            'arm64',
            os.path.join(osxTools, 'usr', 'bin', 'clang'),
            os.path.join(osxTools, 'bin', 'clang++'),
            CLANG_C_STD_ARGS,
            CLANG_CXX_STD_ARGS,
            os.path.join(osxTools, 'usr', 'bin', 'ar'),
            commonArgs,
            ['-DPNSLR_OSX=1', '-DPNSLR_ARM64=1'],
        )

        properties.configurations.append(CCppPropertiesConfiguration(
            name             = 'MacOS-ARM64',
            compilerPath     = os.path.join(osxTools, 'usr', 'bin', 'clang'),
            cStandard        = 'c11',
            cppStandard      = 'c++14',
            includePath      = [
                '${workspaceFolder}/Source'
            ],
            defines          = ['PNSLR_OSX=1', 'PNSLR_ARM64=1'],
            compilerArgs     = commonArgs,
        ))

    if linuxArm64Toolchain:
        commonArgs = CLANG_COMMON_ARGS + [f'--sysroot={linuxArm64Toolchain}\\', '--target=aarch64-unknown-linux-gnueabi']

        buildPlatform(
            'Linux',
            'ARM64',
            'linux',
            'arm64',
            os.path.join(linuxArm64Toolchain, 'bin', 'clang.exe'),
            os.path.join(linuxArm64Toolchain, 'bin', 'clang++.exe'),
            CLANG_C_STD_ARGS,
            CLANG_CXX_STD_ARGS,
            os.path.join(linuxArm64Toolchain, 'bin', 'llvm-ar.exe'),
            commonArgs,
            ['-DPNSLR_LINUX=1', '-DPNSLR_ARM64=1'],
        )

        properties.configurations.append(CCppPropertiesConfiguration(
            name             = 'Linux-ARM64',
            compilerPath     = os.path.join(linuxArm64Toolchain, 'bin', 'clang.exe').replace('\\', '/'),
            cStandard        = 'c11',
            cppStandard      = 'c++14',
            includePath      = [
                '${workspaceFolder}/Source',
                f'{linuxArm64Toolchain}\\usr\\include'.replace('\\', '/')
            ],
            defines          = ['PNSLR_LINUX=1', 'PNSLR_ARM64=1'],
            compilerArgs     = commonArgs,
        ))

    if androidToolchain:
        commonArgs = CLANG_COMMON_ARGS + [f'--sysroot={androidToolchain}\\sysroot\\', '--target=aarch64-linux-android28']

        buildPlatform(
            'Android',
            'ARM64',
            'android',
            'arm64',
            os.path.join(androidToolchain, 'bin', 'clang.exe'),
            os.path.join(androidToolchain, 'bin', 'clang++.exe'),
            CLANG_C_STD_ARGS,
            CLANG_CXX_STD_ARGS,
            os.path.join(androidToolchain, 'bin', 'llvm-ar.exe'),
            commonArgs,
            ['-DPNSLR_ANDROID=1', '-DPNSLR_ARM64=1'],
        )

        properties.configurations.append(CCppPropertiesConfiguration(
            name             = 'Android-ARM64',
            compilerPath     = os.path.join(androidToolchain, 'bin', 'clang.exe').replace('\\', '/'),
            cStandard        = 'c11',
            cppStandard      = 'c++14',
            includePath      = [
                '${workspaceFolder}/Source',
                f'{androidToolchain}\\sysroot\\usr\\include'.replace('\\', '/'),
            ],
            defines          = ['PNSLR_ANDROID=1', 'PNSLR_ARM64=1'],
            compilerArgs     = commonArgs,
        ))

    if osxTools and iosToolchain:
        commonArgs = CLANG_COMMON_ARGS + ['--sysroot', iosToolchain, '-miphoneos-version-min=16.0', '-target', 'aarch64-apple-ios16.0', '-arch', 'arm64']

        buildPlatform(
            'iOS',
            'ARM64',
            'ios',
            'arm64',
            os.path.join(osxTools, 'usr', 'bin', 'clang'),
            os.path.join(osxTools, 'usr', 'bin', 'clang++'),
            CLANG_C_STD_ARGS,
            CLANG_CXX_STD_ARGS,
            os.path.join(osxTools, 'usr', 'bin', 'ar'),
            commonArgs,
            ['-DPNSLR_IOS=1', '-DPNSLR_ARM64=1'],
        )

        properties.configurations.append(CCppPropertiesConfiguration(
            name             = 'iOS-ARM64',
            compilerPath     = os.path.join(osxTools, 'usr', 'bin', 'clang'),
            cStandard        = 'c11',
            cppStandard      = 'c++14',
            includePath      = [
                '${workspaceFolder}/Source'
            ],
            defines          = ['PNSLR_IOS=1', 'PNSLR_ARM64=1'],
            compilerArgs     = commonArgs,
        ))

    if osxTools and iosSimulatorToolchain:
        commonArgs = CLANG_COMMON_ARGS + ['--sysroot', iosSimulatorToolchain, '-miphoneos-version-min=16.0', '-target', 'aarch64-apple-ios16.0-simulator', '-arch', 'arm64']
        buildPlatform(
            'iOS-Simulator',
            'ARM64',
            'iossimulator',
            'arm64',
            os.path.join(osxTools, 'usr', 'bin', 'clang'),
            os.path.join(osxTools, 'usr', 'bin', 'clang++'),
            CLANG_C_STD_ARGS,
            CLANG_CXX_STD_ARGS,
            os.path.join(osxTools, 'usr', 'bin', 'ar'),
            commonArgs,
            ['-DPNSLR_IOS=1', '-DPNSLR_ARM64=1'],
        )

        properties.configurations.append(CCppPropertiesConfiguration(
            name             = 'iOS-Simulator-ARM64',
            compilerPath     = os.path.join(osxTools, 'usr', 'bin', 'clang'),
            cStandard        = 'c11',
            cppStandard      = 'c++14',
            includePath      = [
                '${workspaceFolder}/Source'
            ],
            defines          = ['PNSLR_IOS=1', 'PNSLR_ARM64=1'],
            compilerArgs     = commonArgs,
        ))


    # Write the C/C++ properties file
    ccppPropertiesFile = '.vscode/c_cpp_properties.json'
    os.makedirs(os.path.dirname(ccppPropertiesFile), exist_ok=True)
    with open(ccppPropertiesFile, 'w') as f:
        import json
        json.dump(asdict(properties), f, indent = 2)

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
