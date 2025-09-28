import os, uuid
from pathlib import Path

# region Visual Studio Generator ==============================================================================================

def run(
        projName: str = "MyProject",
        projDir:  str = "ProjectFiles/VisualStudio",
        tempDir:  str = "Temp",
        outDir:   str = "Binaries",
        cxxMain:  str = "",
        cMain:    str = "",
    ):

    tempDirRelPath = os.path.relpath('./' + tempDir, f"./{projDir}").replace('/', '\\')
    outDirRelPath  = os.path.relpath('./' + outDir,  f"./{projDir}").replace('/', '\\')

    # Create directory structure
    dirs = [
        f"./{projDir}",
        f"./{projDir}/{tempDirRelPath}",
        f"./{projDir}/{outDirRelPath}",
    ]

    for dirPath in dirs:
        Path(dirPath).mkdir(parents=True, exist_ok=True)

    # Generate GUIDs for project
    projGuid = str(uuid.uuid4()).upper()
    slnGuid = str(uuid.uuid4()).upper()

    # Solution file (.sln)
    solutionContent = f"""\
Microsoft Visual Studio Solution File, Format Version 12.00
# Visual Studio Version 17
VisualStudioVersion = 17.0.31903.59
MinimumVisualStudioVersion = 10.0.40219.1
Project("{{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}}") = "{projName}", "{projName}.vcxproj", "{{{projGuid}}}"
EndProject
Global
	GlobalSection(SolutionConfigurationPlatforms) = preSolution
		Debug|ARM64 = Debug|ARM64
		Debug|x64 = Debug|x64
		Release|ARM64 = Release|ARM64
		Release|x64 = Release|x64
	EndGlobalSection
	GlobalSection(ProjectConfigurationPlatforms) = postSolution
		{{{projGuid}}}.Debug|ARM64.ActiveCfg = Debug|ARM64
		{{{projGuid}}}.Debug|ARM64.Build.0 = Debug|ARM64
		{{{projGuid}}}.Debug|x64.ActiveCfg = Debug|x64
		{{{projGuid}}}.Debug|x64.Build.0 = Debug|x64
		{{{projGuid}}}.Release|ARM64.ActiveCfg = Release|ARM64
		{{{projGuid}}}.Release|ARM64.Build.0 = Release|ARM64
		{{{projGuid}}}.Release|x64.ActiveCfg = Release|x64
		{{{projGuid}}}.Release|x64.Build.0 = Release|x64
	EndGlobalSection
	GlobalSection(SolutionProperties) = preSolution
		HideSolutionNode = FALSE
	EndGlobalSection
	GlobalSection(ExtensibilityGlobals) = postSolution
		SolutionGuid = {{{slnGuid}}}
	EndGlobalSection
EndGlobal
"""

    # Project file (.vcxproj)
    sourceFiles = ""
    if cxxMain:
        cxxRelPath = os.path.relpath('./' + cxxMain, f"./{projDir}").replace('/', '\\')
        sourceFiles += f"""<ClCompile Include="{cxxRelPath}">
        <LanguageStandard>stdcpp14</LanguageStandard>
        <CompileAs>CompileAsCpp</CompileAs>
    </ClCompile>
"""

    if cMain:
        cRelPath = os.path.relpath('./' + cMain, f"./{projDir}").replace('/', '\\')
        sourceFiles += f"""\
    <ClCompile Include="{cRelPath}">
        <LanguageStandard_C>stdc11</LanguageStandard_C>
        <CompileAs>CompileAsC</CompileAs>
    </ClCompile>
"""

    projectContent = f"""\
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|x64">
      <Configuration>Debug</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64">
      <Configuration>Release</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Debug|ARM64">
      <Configuration>Debug</Configuration>
      <Platform>ARM64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|ARM64">
      <Configuration>Release</Configuration>
      <Platform>ARM64</Platform>
    </ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <VCProjectVersion>17.0</VCProjectVersion>
    <ProjectGuid>{{{projGuid}}}</ProjectGuid>
    <RootNamespace>{projName}</RootNamespace>
    <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\\Microsoft.Cpp.Default.props" />
  <PropertyGroup>
    <ConfigurationType>Application</ConfigurationType>
    <PlatformToolset>v143</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)'=='Debug'">
    <UseDebugLibraries>true</UseDebugLibraries>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)'=='Release'">
    <UseDebugLibraries>false</UseDebugLibraries>
    <WholeProgramOptimization>true</WholeProgramOptimization>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\\Microsoft.Cpp.props" />
  <ImportGroup Label="PropertySheets">
    <Import Project="$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <PropertyGroup>
    <IntDir>{tempDirRelPath}\\VisualStudio\\$(Configuration)\\$(Platform)\\</IntDir>
    <OutDir>{outDirRelPath}\\</OutDir>
    <TargetName>{projName}-windows-$(Configuration)-$(Platform)</TargetName>
  </PropertyGroup>
  <ItemDefinitionGroup>
    <ClCompile>
      <WarningLevel>Level4</WarningLevel>
      <SDLCheck>true</SDLCheck>
      <ConformanceMode>true</ConformanceMode>
      <PreprocessorDefinitions>PNSLR_WINDOWS=1;%(PreprocessorDefinitions)</PreprocessorDefinitions>
    </ClCompile>
    <Link>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)'=='Debug'">
    <ClCompile>
      <PreprocessorDefinitions>PNSLR_DBG=1;_DEBUG;%(PreprocessorDefinitions)</PreprocessorDefinitions>
    </ClCompile>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)'=='Release'">
    <ClCompile>
      <FunctionLevelLinking>true</FunctionLevelLinking>
      <IntrinsicFunctions>true</IntrinsicFunctions>
      <PreprocessorDefinitions>PNSLR_REL=1;NDEBUG;%(PreprocessorDefinitions)</PreprocessorDefinitions>
    </ClCompile>
    <Link>
      <EnableCOMDATFolding>true</EnableCOMDATFolding>
      <OptimizeReferences>true</OptimizeReferences>
    </Link>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Platform)'=='x64'">
    <ClCompile>
      <PreprocessorDefinitions>PNSLR_X64=1;%(PreprocessorDefinitions)</PreprocessorDefinitions>
    </ClCompile>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Platform)'=='ARM64'">
    <ClCompile>
      <PreprocessorDefinitions>PNSLR_ARM64=1;%(PreprocessorDefinitions)</PreprocessorDefinitions>
    </ClCompile>
  </ItemDefinitionGroup>
  <ItemGroup>
{sourceFiles}  </ItemGroup>
  <Import Project="$(VCTargetsPath)\\Microsoft.Cpp.targets" />
</Project>
"""

    # .gitignore
    gitignore = f"""\
.idea/
.vs/
*.vcxproj.user
*.vcxproj.filters
{tempDirRelPath}/
{outDirRelPath}/
*.pdb
*.idb
*.ilk
*.log
*.tlog
*.lastbuildstate
"""

    # Write all files
    files = [
        (f"./{projDir}/{projName}.sln", solutionContent),
        (f"./{projDir}/{projName}.vcxproj", projectContent),
        (f"./{projDir}/.gitignore", gitignore),
    ]

    for filepath, content in files:
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(content)

    print(f"Visual Studio 2022 project '{projName}' created successfully in '{projDir}'.")

# endregion
