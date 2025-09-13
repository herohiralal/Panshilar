import os, sys
import buildutils

# region Commandline arguments ================================================================================================

CMD_ARG_REBUILD_INTRINSICS  = '-rebuild-intrinsics' in sys.argv # Rebuild the intrinsics dependency
CMD_ARG_RUN_TESTS           = '-tests'              in sys.argv # Run the tests after building
CMD_ARG_REGENERATE_BINDINGS = '-rebind'             in sys.argv # Regenerate the bindings after building

# endregion

# region File paths ===========================================================================================================

FOLDER_STRUCTURE = buildutils.getFolderStructure(os.path.dirname(os.path.abspath(__file__)))

INTRINSICS_ROOT_DIR         = FOLDER_STRUCTURE.depDir + 'PNSLR_Intrinsics/'
TEST_RUNNER_ROOT_DIR        = FOLDER_STRUCTURE.root   + 'Tools/TestRunner/'
BINDINGS_GENERATOR_ROOT_DIR = FOLDER_STRUCTURE.root   + 'Tools/BindGen/'

INTRINSICS_MAIN_FILE         = INTRINSICS_ROOT_DIR         + 'Intrinsics.c'
MAIN_FILE                    = FOLDER_STRUCTURE.srcDir     + 'zzzz_Unity.c'
TEST_RUNNER_MAIN_FILE        = TEST_RUNNER_ROOT_DIR        + 'zzzz_TestRunner.c'
BINDINGS_GENERATOR_MAIN_FILE = BINDINGS_GENERATOR_ROOT_DIR + 'BindingsGenerator.c'

def getIntrinsicsObjectPath(plt: buildutils.Platform) -> str:
    return INTRINSICS_ROOT_DIR + 'Prebuilt/' + buildutils.getObjectOutputFileName('intrinsics', plt)

def getLibraryObjectPath(plt: buildutils.Platform) -> str:
    return FOLDER_STRUCTURE.tmpDir + buildutils.getObjectOutputFileName('unity', plt)

def getLibraryPath(plt: buildutils.Platform) -> str:
    return FOLDER_STRUCTURE.libDir + buildutils.getStaticLibOutputFileName('panshilar', plt)

def getTestRunnerObjectPath(plt: buildutils.Platform) -> str:
    return FOLDER_STRUCTURE.tmpDir +  buildutils.getObjectOutputFileName('testrunner', plt)

def getTestRunnerExecutablePath(plt: buildutils.Platform) -> str:
    return FOLDER_STRUCTURE.binDir + buildutils.getExecOutputFileName('TestRunner', plt)

def getBindingsGeneratorObjectPath(plt: buildutils.Platform) -> str:
    return FOLDER_STRUCTURE.tmpDir +  buildutils.getObjectOutputFileName('bindgen', plt)

def getBindingsGeneratorExecutablePath(plt: buildutils.Platform) -> str:
    return FOLDER_STRUCTURE.binDir + buildutils.getExecOutputFileName('BindingsGenerator', plt)

def getIntrinsicsCompileCommand(plt: buildutils.Platform) -> list[str]:
    return buildutils.getCompilationCommand(plt, False, INTRINSICS_MAIN_FILE, getIntrinsicsObjectPath(plt))

def getLibraryCompileCommand(plt: buildutils.Platform) -> list[str]:
    return buildutils.getCompilationCommand(plt, False, MAIN_FILE, getLibraryObjectPath(plt))

def getLibraryLinkCommand(plt: buildutils.Platform) -> list[str]:
    return buildutils.getStaticLibLinkCommand(plt, [getIntrinsicsObjectPath(plt), getLibraryObjectPath(plt)], getLibraryPath(plt))

def getTestRunnerBuildCommand(plt: buildutils.Platform) -> list[str]:
    return buildutils.getExecBuildCommand(
        plt,
        True,
        [TEST_RUNNER_MAIN_FILE, MAIN_FILE, INTRINSICS_MAIN_FILE],
        [FOLDER_STRUCTURE.srcDir, FOLDER_STRUCTURE.bndDir],
        ['iphlpapi.lib', 'Ws2_32.lib'] if plt.tgt == 'windows' else ['pthread'] if plt.tgt == 'linux' else [],
        getTestRunnerExecutablePath(plt),
    )

def getBindingsGeneratorBuildCommand(plt: buildutils.Platform) -> list[str]:
    return buildutils.getExecBuildCommand(
        plt,
        True,
        [BINDINGS_GENERATOR_MAIN_FILE, MAIN_FILE, INTRINSICS_MAIN_FILE],
        [FOLDER_STRUCTURE.srcDir, FOLDER_STRUCTURE.bndDir],
        ['iphlpapi.lib', 'Ws2_32.lib'] if plt.tgt == 'windows' else [],
        getBindingsGeneratorExecutablePath(plt),
    )

# endregion

# region Main Logic ===========================================================================================================

def buildIntrinsics(plt: buildutils.Platform) -> bool:
    return buildutils.runCommand(getIntrinsicsCompileCommand(plt), f'{plt.prettyTgt}-{plt.prettyArch} Intrinsics Compile')

def buildLibrary(plt: buildutils.Platform) -> bool:
    return buildutils.runCommand(getLibraryCompileCommand(plt), f'{plt.prettyTgt}-{plt.prettyArch} Library Compile') and \
           buildutils.runCommand(getLibraryLinkCommand(plt),    f'{plt.prettyTgt}-{plt.prettyArch} Library Link')

def buildTestRunner(plt: buildutils.Platform) -> bool:
    return buildutils.runCommand(getTestRunnerBuildCommand(plt), f'{plt.prettyTgt}-{plt.prettyArch} Test Runner Build')

def buildBindingsGenerator(plt: buildutils.Platform) -> bool:
    return buildutils.runCommand(getBindingsGeneratorBuildCommand(plt), f'{plt.prettyTgt}-{plt.prettyArch} Bindings Generator Build')

# endregion

if __name__ == '__main__':
    buildutils.setupVsCodeLspStuff()

    if CMD_ARG_RUN_TESTS:
        tests = [f.rstrip('.c') for f in os.listdir(TEST_RUNNER_ROOT_DIR) if f.endswith('.c') and not f.startswith('zzzz_')]
        tests = sorted(tests)
        outputFilePath = os.path.join(TEST_RUNNER_ROOT_DIR, 'zzzz_GeneratedCombinedTests.c')
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
        cnt = True
        if CMD_ARG_REBUILD_INTRINSICS:
            cnt = cnt and buildIntrinsics(plt)

        cnt = cnt and buildLibrary(plt)

        if CMD_ARG_REGENERATE_BINDINGS and (plt.tgt == 'windows' or plt.tgt == 'osx'): # host platforms only
            cnt = cnt and buildBindingsGenerator(plt)

        if CMD_ARG_RUN_TESTS and (plt.tgt == 'windows' or plt.tgt == 'linux' or plt.tgt == 'osx'): # desktop platforms only
            cnt = cnt and buildTestRunner(plt)

    buildutils.printSummary()

    if len(buildutils.failedProcesses) > 0:
        exit(1)
