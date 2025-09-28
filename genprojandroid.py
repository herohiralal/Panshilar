import os
from pathlib import Path

# region Main =================================================================================================================

def run(
        appName:         str  = "MyApp",
        pkgName:         str  = "com.example.myapp",
        projDir:         str  = "ProjectFiles/Android",
        grdlVer:         str  = "8.13",
        minSdk:          int  = 29,
        tgtSdk:          int  = 35,
        verCode:         int  = 1,
        verName:         str  = "1.0",
        cxxMain:         str  = "",
        cMain:           str  = "",
        useGameActivity: bool = True,
    ):

    # Create directory structure
    dirs = [
        f"./{projDir}",
        f"./{projDir}/.idea",
        f"./{projDir}/app/src/main/cpp",
        f"./{projDir}/gradle/wrapper"
    ]

    # using game activity instead of native activity
    if useGameActivity:
        dirs += [f"./{projDir}/app/src/main/java/{pkgName.replace('.', '/')}"]

    for dirPath in dirs:
        Path(dirPath).mkdir(parents=True, exist_ok=True)

    # Root build.gradle.kts
    rootBuild = f"""\
plugins {{
    id("com.android.application") version "8.13.0" apply false
}}
"""

    # App build.gradle.kts
    appBuild = f"""\
plugins {{
    id("com.android.application")
}}

android {{
    namespace = "{pkgName}"
    compileSdk = {tgtSdk}

    defaultConfig {{
        applicationId = "{pkgName}"
        minSdk = {minSdk}
        targetSdk = {tgtSdk}
        versionCode = {verCode}
        versionName = "{verName}"

        ndk {{
            abiFilters += listOf("arm64-v8a", "x86_64")
        }}

        externalNativeBuild {{
            cmake {{
                cFlags += "-std=c11"
                cppFlags += "-std=c++14"
            }}
        }}
    }}

    buildTypes {{
        release {{
            isMinifyEnabled = false
        }}
    }}

    externalNativeBuild {{
        cmake {{
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }}
    }}
}}

dependencies {{
    implementation("androidx.appcompat:appcompat:1.6.1")
    implementation("androidx.games:games-activity:3.0.5")
}}
"""

    # settings.gradle.kts
    settings = f"""\
pluginManagement {{
    repositories {{
        google {{
            content {{
                includeGroupByRegex("com\\\\.android.*")
                includeGroupByRegex("com\\\\.google.*")
                includeGroupByRegex("androidx.*")
            }}
        }}
        mavenCentral()
        gradlePluginPortal()
    }}
}}
dependencyResolutionManagement {{
    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)
    repositories {{
        google()
        mavenCentral()
    }}
}}

rootProject.name = "{appName}"
include(":app")
"""

    # gradle.properties
    gradleProps = """\
org.gradle.jvmargs=-Xmx2048m -Dfile.encoding=UTF-8
org.gradle.parallel=true
android.useAndroidX=true
android.nonTransitiveRClass=true
"""

    # gradle-wrapper.properties
    wrapperProps = f"""\
distributionBase=GRADLE_USER_HOME
distributionPath=wrapper/dists
distributionUrl=https\\://services.gradle.org/distributions/gradle-{grdlVer}-bin.zip
zipStoreBase=GRADLE_USER_HOME
zipStorePath=wrapper/dists
"""

    mainActivity = ''
    if useGameActivity:
        mainActivity = f"""\
package {pkgName};

import android.view.View;
import com.google.androidgamesdk.GameActivity;

public class MainActivity extends GameActivity {{
    static {{
        System.loadLibrary("nativelib");
    }}

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {{
        super.onWindowFocusChanged(hasFocus);

        if (hasFocus) {{
            hideSystemUi();
        }}
    }}

    private void hideSystemUi() {{
        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                        | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_FULLSCREEN
        );
    }}
}}
"""

    # AndroidManifest.xml
    manifest = f"""\
<?xml version="1.0" encoding="utf-8"?>
<manifest
    xmlns:android="http://schemas.android.com/apk/res/android"
    android:installLocation="auto"
    xmlns:tools="http://schemas.android.com/tools">

    <application
        android:label="{appName}"
        android:theme="{\
            '@style/Theme.AppCompat.NoActionBar' \
                if useGameActivity else \
            '@android:style/Theme.NoTitleBar.Fullscreen'}">
        <activity
            android:name="{'.MainActivity' if mainActivity else 'android.app.NativeActivity'}"
            android:exported="true">
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.LAUNCHER" />
            </intent-filter>
            <meta-data
                android:name="android.app.lib_name"
                android:value="nativelib" />
        </activity>
    </application>
</manifest>
"""

    # workspace.xml
    workspace = f"""\
<?xml version="1.0" encoding="UTF-8"?>
<project version="4">
  <component name="RunManager">
    <configuration name="app" type="AndroidRunConfigurationType" factoryName="Android App">
      <module name="{appName}.app" />
      <option name="CLEAR_LOGCAT" value="true" />
      <option name="SHOW_LOGCAT_AUTOMATICALLY" value="true" />
      <option name="DEBUGGER_TYPE" value="Native" />
    </configuration>
  </component>
</project>
"""

    if useGameActivity:
        nativeSymbolToPreserve = 'Java_com_google_androidgamesdk_GameActivity_initializeNativeCode'
    else:
        nativeSymbolToPreserve = 'ANativeActivity_onCreate'

    # CMakeLists.txt
    cmake = f"""\
cmake_minimum_required(VERSION 3.22.1)

project("nativelib")

add_library(nativelib SHARED
{'    nativelib.cpp\n' if cxxMain else ''}{'    nativelib.c\n'   if cMain   else ''})

target_compile_options(nativelib PRIVATE -Wall)
target_compile_definitions(nativelib PRIVATE PNSLR_ANDROID=1)

if (CMAKE_ANDROID_ARCH_ABI STREQUAL "x86_64")
    target_compile_definitions(nativelib PRIVATE PNSLR_X64=1)
elseif (CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a")
    target_compile_definitions(nativelib PRIVATE PNSLR_ARM64=1)
endif()

target_compile_definitions(nativelib PRIVATE
    $<$<CONFIG:Debug>:PNSLR_DBG=1>
    $<$<CONFIG:Release>:PNSLR_REL=1>
    $<$<CONFIG:RelWithDebInfo>:PNSLR_REL=1>
    $<$<CONFIG:MinSizeRel>:PNSLR_REL=1>
)\

{'' if useGameActivity else """
target_include_directories(nativelib PRIVATE
    ${{ANDROID_NDK}}/sources/android/native_app_glue
)
"""}\

set(CMAKE_SHARED_LINKER_FLAGS
    "${{CMAKE_SHARED_LINKER_FLAGS}} -u {nativeSymbolToPreserve}"
)

target_link_libraries(nativelib
    jnigraphics
    android
    log
)

set_source_files_properties(nativelib.c PROPERTIES
    LANGUAGE C
    C_STANDARD 11
)

set_source_files_properties(nativelib.cpp PROPERTIES
    LANGUAGE CXX
    CXX_STANDARD 14
)
"""


    gitignore = """\
*.iml
.gradle
/local.properties
/.idea
.DS_Store
/build
/captures
.externalNativeBuild
.cxx
local.properties

/app/build
/app/src/main/assets
"""

    # Write all files
    files = [
        (f"./{projDir}/.idea/workspace.xml",                      workspace),
        (f"./{projDir}/.gitignore",                               gitignore),
        (f"./{projDir}/build.gradle.kts",                         rootBuild),
        (f"./{projDir}/settings.gradle.kts",                      settings),
        (f"./{projDir}/gradle.properties",                        gradleProps),
        (f"./{projDir}/gradle/wrapper/gradle-wrapper.properties", wrapperProps),
        (f"./{projDir}/app/build.gradle.kts",                     appBuild),
        (f"./{projDir}/app/src/main/AndroidManifest.xml",         manifest),
        (f"./{projDir}/app/src/main/cpp/CMakeLists.txt",          cmake),
    ]

    if mainActivity:
        files += [(
            f"./{projDir}/app/src/main/java/{pkgName.replace('.', '/')}/MainActivity.java",
            mainActivity
        )]

    if cxxMain:
        cxxRelPath = os.path.relpath('./' + cxxMain, f"./{projDir}/app/src/main/cpp/").replace('\\', '/')
        nativeCpp = f'#include "{cxxRelPath}"\n'
        files += [(f"./{projDir}/app/src/main/cpp/nativelib.cpp", nativeCpp)]

    if cMain:
        cRelPath = os.path.relpath('./' + cMain, f"./{projDir}/app/src/main/cpp/").replace('\\', '/')
        nativeC = f'#include "{cRelPath}"\n'
        files += [(f"./{projDir}/app/src/main/cpp/nativelib.c", nativeC)]

    for filepath, content in files:
        with open(filepath, 'w') as f:
            f.write(content)

    print(f"Android project '{appName}' created successfully in '{projDir}'.")

# endregion
