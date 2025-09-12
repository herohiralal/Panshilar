import os, sys
import buildutils

# region Commandline arguments ================================================================================================

CMD_ARG_REBUILD_INTRINSICS  = '-rebuild-intrinsics' in sys.argv # Rebuild the intrinsics dependency
CMD_ARG_RUN_TESTS           = '-tests'              in sys.argv # Run the tests after building
CMD_ARG_REGENERATE_BINDINGS = '-rebind'             in sys.argv # Regenerate the bindings after building

# endregion

# region File paths ===========================================================================================================

PNSLR_FOLDER_STRUCTURE = buildutils.getFolderStructure(os.path.dirname(os.path.abspath(__file__)))

PNSLR_INTRINSICS_ROOT_DIR         = PNSLR_FOLDER_STRUCTURE.depDir + 'PNSLR_Intrinsics/'
PNSLR_TEST_RUNNER_ROOT_DIR        = PNSLR_FOLDER_STRUCTURE.root   + 'Tools/TestRunner/'
PNSLR_BINDINGS_GENERATOR_ROOT_DIR = PNSLR_FOLDER_STRUCTURE.root   + 'Tools/BindGen/'

def getIntrinsicsObjectPath(plt: buildutils.Platform) -> str:
    return PNSLR_INTRINSICS_ROOT_DIR + 'Prebuilt/' + buildutils.getObjectOutputFileName('intrinsics', plt)

def getLibraryObjectPath(plt: buildutils.Platform, dbg: bool) -> str:
    return PNSLR_FOLDER_STRUCTURE.tmpDir + buildutils.getObjectOutputFileName('unity' + ('-dbg' if dbg else ''), plt)

def getLibraryPath(plt: buildutils.Platform, dbg: bool) -> str:
    return PNSLR_FOLDER_STRUCTURE.libDir + buildutils.getStaticLibOutputFileName('panshilar' + ('-dbg' if dbg else ''), plt)

def getTestRunnerExecutablePath(plt: buildutils.Platform) -> str:
    return PNSLR_FOLDER_STRUCTURE.binDir + buildutils.getExecOutputFileName('TestRunner', plt)

def getBindingsGeneratorExecutablePath(plt: buildutils.Platform) -> str:
    return PNSLR_FOLDER_STRUCTURE.binDir + buildutils.getExecOutputFileName('BindingsGenerator', plt)

def getIntrinsicsCompileCommand(plt: buildutils.Platform) -> list[str]:
    return buildutils.getCompilationCommand(
        plt,
        False,
        PNSLR_INTRINSICS_ROOT_DIR + 'Intrinsics.c',
        getIntrinsicsObjectPath(plt),
    )

def getLibraryCompileCommand(plt: buildutils.Platform, dbg: bool) -> list[str]:
    return buildutils.getCompilationCommand(
        plt,
        dbg,
        PNSLR_FOLDER_STRUCTURE.srcDir + 'zzzz_Unity.c',
        getLibraryObjectPath(plt, dbg),
    )

def getLibraryLinkCommand(plt: buildutils.Platform, dbg: bool) -> list[str]:
    return buildutils.getStaticLibLinkCommand(
        plt,
        [getIntrinsicsObjectPath(plt), getLibraryObjectPath(plt, dbg)],
        getLibraryPath(plt, dbg),
    )

def getTestRunnerCompileCommand(plt: buildutils.Platform) -> list[str]:
    return buildutils.getCompilationCommand(
        plt,
        True,
        PNSLR_TEST_RUNNER_ROOT_DIR + 'Source/zzzz_TestRunner.c',
        getTestRunnerExecutablePath(plt),
        [PNSLR_FOLDER_STRUCTURE.srcDir, PNSLR_FOLDER_STRUCTURE.bndDir],
    )

def getTestRunnerLinkCommand(plt: buildutils.Platform) -> list[str]:
    return buildutils.getExecLinkCommand(
        plt,
        True,
        [PNSLR_TEST_RUNNER_ROOT_DIR + 'Source/zzzz_TestRunner.c', getLibraryPath(plt, True)],
        ['iphlpapi.lib', 'Ws2_32.lib'] if plt.tgt == 'windows' else ['pthread'] if plt.tgt != 'android' else [],
        getTestRunnerExecutablePath(plt),
    )

def getBindingsGeneratorCompileCommand(plt: buildutils.Platform) -> list[str]:
    return buildutils.getCompilationCommand(
        plt,
        True,
        PNSLR_BINDINGS_GENERATOR_ROOT_DIR + 'Source/BindingsGenerator.c',
        getBindingsGeneratorExecutablePath(plt),
        [PNSLR_FOLDER_STRUCTURE.srcDir],
    )

def getBindingsGeneratorLinkCommand(plt: buildutils.Platform) -> list[str]:
    return buildutils.getExecLinkCommand(
        plt,
        True,
        [PNSLR_BINDINGS_GENERATOR_ROOT_DIR + 'Source/BindingsGenerator.c', getLibraryPath(plt, True)],
        ['iphlpapi.lib', 'Ws2_32.lib'] if plt.tgt == 'windows' else ['pthread'] if plt.tgt != 'android' else [],
        getBindingsGeneratorExecutablePath(plt),
    )

# endregion

# region Main Logic ===========================================================================================================

def buildIntrinsics(plt: buildutils.Platform) -> bool:
    cmd = getIntrinsicsCompileCommand(plt)
    return buildutils.runCommand(cmd, f'{plt.tgt}-{plt.arch} Intrinsics Compile')

def buildLibrary(plt: buildutils.Platform, dbg: bool) -> bool:
    cmd = getLibraryCompileCommand(plt, dbg)
    if not buildutils.runCommand(cmd, f'{plt.tgt}-{plt.arch} Library Compile'):
        return False
    cmd = getLibraryLinkCommand(plt, dbg)
    return buildutils.runCommand(cmd, f'{plt.tgt}-{plt.arch} Library Link')

def buildTestRunner(plt: buildutils.Platform) -> bool:
    cmd = getTestRunnerCompileCommand(plt)
    if not buildutils.runCommand(cmd, f'{plt.tgt}-{plt.arch} Test Runner Compile'):
        return False
    cmd = getTestRunnerLinkCommand(plt)
    return buildutils.runCommand(cmd, f'{plt.tgt}-{plt.arch} Test Runner Link')

def buildBindingsGenerator(plt: buildutils.Platform) -> bool:
    cmd = getBindingsGeneratorCompileCommand(plt)
    if not buildutils.runCommand(cmd, f'{plt.tgt}-{plt.arch} Bindings Generator Compile'):
        return False
    cmd = getBindingsGeneratorLinkCommand(plt)
    return buildutils.runCommand(cmd, f'{plt.tgt}-{plt.arch} Bindings Generator Link')

# endregion

if __name__ == '__main__':
    if CMD_ARG_RUN_TESTS:
        sourcePath = PNSLR_TEST_RUNNER_ROOT_DIR + 'Source/'
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

    for plt in buildutils.PLATFORMS:
        if CMD_ARG_REBUILD_INTRINSICS:
            buildIntrinsics(plt)

        buildLibrary(plt, False)

        if CMD_ARG_RUN_TESTS or CMD_ARG_REGENERATE_BINDINGS:
            buildLibrary(plt, True)

        if CMD_ARG_REGENERATE_BINDINGS and (plt.tgt == 'windows' or plt.tgt == 'linux' or plt.tgt == 'osx'): # desktop platforms only
            buildBindingsGenerator(plt)

        if CMD_ARG_RUN_TESTS and (plt.tgt == 'windows' or plt.tgt == 'linux' or plt.tgt == 'osx'): # desktop platforms only
            buildTestRunner(plt)

    buildutils.printSummary()
