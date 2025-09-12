import os, sys
import buildutils

# region Commandline arguments ================================================================================================

CMD_ARG_REBUILD_INTRINSICS  = '-rebuild-intrinsics' in sys.argv # Rebuild the intrinsics dependency
CMD_ARG_RUN_TESTS           = '-tests'              in sys.argv # Run the tests after building
CMD_ARG_REGENERATE_BINDINGS = '-rebind'             in sys.argv # Regenerate the bindings after building

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
            'iphlpapi.lib',
            'Ws2_32.lib',
            f'/OUT:{outputFile}',
        ]
    elif tgt == 'osx':
        return [
            '-static',
            '-o',
            outputFile,
            intrinsicsObjFile,
            libraryObjFile,
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
            '/IBindings/',
            f'/Fe{outputFile}',
            f'/FoTemp/TestRunner-{tgt}-{arch}.obj',
            f'/FdBinaries/TestRunner-{tgt}-{arch}.pdb',
        ] + buildutils.MSVC_DEBUG_ARGS
    else:
        return [
            inputFile,
            getLibraryPath(tgt, arch),
            '-o',
            outputFile,
            '-ISource/',
            '-IBindings/',
            # '-v',
            # '-###',
        ] + buildutils.CLANG_DEBUG_ARGS + \
        (['-lpthread'] if tgt != 'android' else [])

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
        ] + buildutils.MSVC_DEBUG_ARGS
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
        ] + buildutils.CLANG_DEBUG_ARGS

# endregion

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

    buildTools = False
    if sys.platform == 'win32':
        buildTools = (tgt == 'windows')
    elif sys.platform == 'darwin':
        buildTools = (tgt == 'osx')
    elif sys.platform == 'linux':
        buildTools = (tgt == 'linux')
    else:
        buildTools = False

    intrinsicsCompiled    = True
    intrinsicsCompileArgs = commonCompilerArgs + getIntrinsicsCompileArgs(tgt, arch)
    if CMD_ARG_REBUILD_INTRINSICS:
        intrinsicsCompiled = (buildutils.runCommand([cCompiler] + intrinsicsCompileArgs + cStdArgs, f'{prettyTgt}-{prettyArch} Intrinsics Compile'))

    libraryCompiled    = True
    libraryCompileArgs = commonCompilerArgs + getLibraryCompileArgs(tgt, arch) + envArgs
    libraryCompiled    = (intrinsicsCompiled and buildutils.runCommand([cCompiler] + libraryCompileArgs + cStdArgs, f'{prettyTgt}-{prettyArch} Library Compile'))

    libraryLinked   = True
    libraryLinkArgs = getLibraryLinkArgs(tgt, arch)
    libraryLinked   = (libraryCompiled and buildutils.runCommand([linker] + libraryLinkArgs, f'{prettyTgt}-{prettyArch} Library Link'))

    testsSuccessful     = True
    testRunnerBuildArgs = commonCompilerArgs + getTestRunnerBuildArgs(tgt, arch) + envArgs
    testsSuccessful     = (not CMD_ARG_RUN_TESTS) or \
                          (libraryLinked and buildutils.runCommand([cCompiler] + testRunnerBuildArgs + cStdArgs, f'{prettyTgt}-{prettyArch} Test Runner Build'))

    bindGenSuccessful          = True
    bindingsGeneratorBuildArgs = commonCompilerArgs + getBindGenBuildArgs(tgt, arch) + envArgs
    bindGenSuccessful          = (not CMD_ARG_REGENERATE_BINDINGS) or (not buildTools) or \
                                 (libraryLinked and buildutils.runCommand([cCompiler] + bindingsGeneratorBuildArgs + cStdArgs, f'{prettyTgt}-{prettyArch} Bindings Generator Build'))

    return libraryLinked and testsSuccessful and bindGenSuccessful

# endregion

def main():

    # region Setup Tests ======================================================================================================

    if CMD_ARG_RUN_TESTS:
        sourcePath = getTestRunnerSourcePath()
        tests = [f.rstrip('.c') for f in os.listdir(sourcePath) if f.endswith('.c') and not f.startswith('zzzz_')]
        tests = sorted(tests)
        outputFilePath = os.path.join(sourcePath, 'zzzz_GeneratedCombinedTests.c')
        with open(outputFilePath, 'w', encoding='utf-8') as outputFile:
            outputFile.write('// This file is generated by the build script. Do not edit it manually.\n\n')

            for test in tests:
                outputFile.write('#undef MAIN_TEST_FN\n')
                outputFile.write(f'#define MAIN_TEST_FN(ctxArgName) void ZZZZ_Test_{test}(const TestContext* ctxArgName)\n')
                outputFile.write(f'#include "{test}.c"\n')
                outputFile.write('#undef MAIN_TEST_FN\n\n')

            outputFile.write(f'PNSLR_U64 ZZZZ_GetTestsCount(void) {{ return {len(tests)}ULL; }}\n\n')
            outputFile.write('void ZZZZ_GetAllTests(PNSLR_ArraySlice(TestFunctionInfo) fns)\n')
            outputFile.write('{\n')

            i: int = 0
            for test in tests:
                outputFile.write(f'    fns.data[{i}].name = PNSLR_StringLiteral("{test}");\n')
                outputFile.write(f'    fns.data[{i}].fn   = ZZZZ_Test_{test};\n\n')
                i += 1

            outputFile.write('    // done\n')
            outputFile.write('}\n')

    # endregion

    # region Project Files Definitions ========================================================================================


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
                '${workspaceFolder}/Source',
                '${workspaceFolder}/Bindings'
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
                '${workspaceFolder}/Bindings',
                f'{linuxX64Toolchain}\\usr\\include'.replace('\\', '/')
            ],
            defines          = ['PNSLR_LINUX=1', 'PNSLR_X64=1'],
            compilerArgs     = commonArgs,
        ))

    if osxTools and osxToolchain:
        commonArgs = CLANG_COMMON_ARGS + ['--sysroot', osxToolchain, '-target', 'x86_64-apple-macos11.0']

        buildPlatform(
            'MacOS',
            'Intel',
            'osx',
            'x64',
            os.path.join(osxTools, 'usr', 'bin', 'clang'),
            os.path.join(osxTools, 'bin', 'clang++'),
            CLANG_C_STD_ARGS,
            CLANG_CXX_STD_ARGS,
            os.path.join(osxTools, 'usr', 'bin', 'libtool'),
            commonArgs,
            ['-DPNSLR_OSX=1', '-DPNSLR_X64=1'],
        )

        properties.configurations.append(CCppPropertiesConfiguration(
            name             = 'MacOS-X64',
            compilerPath     = os.path.join(osxTools, 'usr', 'bin', 'clang'),
            cStandard        = 'c11',
            cppStandard      = 'c++14',
            includePath      = [
                '${workspaceFolder}/Source',
                '${workspaceFolder}/Bindings'
            ],
            defines          = ['PNSLR_OSX=1', 'PNSLR_X64=1'],
            compilerArgs     = commonArgs,
        ))

    if osxTools and osxToolchain:
        commonArgs = CLANG_COMMON_ARGS + ['--sysroot', osxToolchain, '-target', 'arm64-apple-macos11.0']

        buildPlatform(
            'MacOS',
            'Apple Silicon',
            'osx',
            'arm64',
            os.path.join(osxTools, 'usr', 'bin', 'clang'),
            os.path.join(osxTools, 'bin', 'clang++'),
            CLANG_C_STD_ARGS,
            CLANG_CXX_STD_ARGS,
            os.path.join(osxTools, 'usr', 'bin', 'libtool'),
            commonArgs,
            ['-DPNSLR_OSX=1', '-DPNSLR_ARM64=1'],
        )

        properties.configurations.append(CCppPropertiesConfiguration(
            name             = 'MacOS-ARM64',
            compilerPath     = os.path.join(osxTools, 'usr', 'bin', 'clang'),
            cStandard        = 'c11',
            cppStandard      = 'c++14',
            includePath      = [
                '${workspaceFolder}/Source',
                '${workspaceFolder}/Bindings'
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
                '${workspaceFolder}/Bindings',
                f'{linuxArm64Toolchain}\\usr\\include'.replace('\\', '/')
            ],
            defines          = ['PNSLR_LINUX=1', 'PNSLR_ARM64=1'],
            compilerArgs     = commonArgs,
        ))

    if androidToolchain:
        commonArgs = CLANG_COMMON_ARGS + [f'--sysroot={androidToolchain}\\sysroot\\', '--target=x86_64-linux-android28']

        buildPlatform(
            'Android',
            'X64',
            'android',
            'x64',
            os.path.join(androidToolchain, 'bin', 'clang.exe'),
            os.path.join(androidToolchain, 'bin', 'clang++.exe'),
            CLANG_C_STD_ARGS,
            CLANG_CXX_STD_ARGS,
            os.path.join(androidToolchain, 'bin', 'llvm-ar.exe'),
            commonArgs,
            ['-DPNSLR_ANDROID=1', '-DPNSLR_X64=1'],
        )

        properties.configurations.append(CCppPropertiesConfiguration(
            name             = 'Android-x64',
            compilerPath     = os.path.join(androidToolchain, 'bin', 'clang.exe').replace('\\', '/'),
            cStandard        = 'c11',
            cppStandard      = 'c++14',
            includePath      = [
                '${workspaceFolder}/Source',
                '${workspaceFolder}/Bindings',
                f'{androidToolchain}\\sysroot\\usr\\include'.replace('\\', '/'),
            ],
            defines          = ['PNSLR_ANDROID=1', 'PNSLR_X64=1'],
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
                '${workspaceFolder}/Bindings',
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
                '${workspaceFolder}/Source',
                '${workspaceFolder}/Bindings'
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
                '${workspaceFolder}/Source',
                '${workspaceFolder}/Bindings'
            ],
            defines          = ['PNSLR_IOS=1', 'PNSLR_ARM64=1'],
            compilerArgs     = commonArgs,
        ))

    # endregion

    buildutils.printSummary()

    # endregion

if __name__ == '__main__':
    main()
