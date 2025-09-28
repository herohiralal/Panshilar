import os, random
from pathlib import Path

#region XCode Generator =======================================================================================================

def run(
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

    print(f"XCode project '{projName}' created successfully in '{projDir}'.")

# endregion
