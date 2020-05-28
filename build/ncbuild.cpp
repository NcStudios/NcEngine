#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <typeinfo>
#include <chrono>

const std::vector<std::string> NC_BUILD_INCLUDE_PATHS =
{
    "-I..\\nc",
    "-I..\\nc\\component",
    "-I..\\nc\\debug",
    "-I..\\nc\\engine",
    "-I..\\nc\\graphics",
    "-I..\\nc\\graphics\\d3dresource",
    "-I..\\nc\\graphics\\DirectXMath\\Inc",
    "-I..\\nc\\input",
    "-I..\\nc\\math",
    "-I..\\nc\\scene",
    "-I..\\nc\\time",
    "-I..\\nc\\utils\\editor",
    "-I..\\nc\\utils\\editor\\imgui",
    "-I..\\nc\\utils\\objloader",
    "-I..\\nc\\views",
    "-I..\\nc\\win32",
    "-I..\\project\\Components",
    "-I..\\project\\Models",
    "-I..\\project\\Scenes"
};

const std::vector<std::string> NC_BUILD_LINK_FLAGS =
{
    "-lgdi32",
    "-ld3d11",
    "-ld3dcompiler_47"
};

const std::vector<std::string> NC_BUILD_TARGETS_COMMON = 
{
    "..\\nc\\*.cpp",
    "..\\nc\\component\\*.cpp",
    "..\\nc\\input\\*.cpp",
    "..\\nc\\math\\*.cpp",
    "..\\nc\\scene\\*.cpp",
    "..\\nc\\time\\*.cpp",
    "..\\nc\\views\\*.cpp"
};

const std::vector<std::string> NC_BUILD_TARGETS_ENGINE = 
{
    "..\\nc\\engine\\*.cpp",
    "..\\nc\\win32\\*.cpp"
};

const std::vector<std::string> NC_BUILD_TARGETS_GRAPHICS =
{
    "..\\nc\\graphics\\*.cpp",
    "..\\nc\\graphics\\d3dresource\\*.cpp"
};

const std::vector<std::string> NC_BUILD_TARGETS_UTILS =
{
    "..\\nc\\utils\\editor\\*.cpp",
    "..\\nc\\utils\\objloader\\*.cpp"
};

const std::vector<std::string> NC_BUILD_TARGETS_PROJECT =
{
    "..\\project\\Components\\*.cpp",
    "..\\project\\Scenes\\*.cpp"
};

const std::vector<std::string> NC_BUILD_TARGETS_EXTERNAL =
{
    "..\\nc\\utils\\editor\\imgui\\*.cpp"
};

struct BuildOptions
{
    std::string compiler      = "g++";
    std::string mode          = "-g -D NC_DEBUG";
    std::string standard      = "-std=c++17";
    std::string optimizations = "";
    std::string warnings      = "-Wall";
    std::string defines       = "";
    std::string includes      = "";
    std::string appName       = "-o app";
    std::string objCompile    = "-c";
    std::string targets       = "";    
    std::string linkFlags     = "";
    std::string objDirectory  = "obj";
    bool        link          = true;  
    bool        compile       = true;
};

std::string GetBuildDirectory();

void AddIncludePaths(BuildOptions& buildOptions, const std::vector<std::string>& includePaths);

void AddLinkFlags(BuildOptions& buildOptions, const std::vector<std::string>& linkFlags);

void AddTargets(BuildOptions& buildOptions, const std::vector<std::string>& targets);

void CommandToOptionValue(const std::string& commandLineArg, std::string& optionOut, std::string& valueOut);

void ProcessOptionValue(BuildOptions& buildOptions, const std::string& option, const std::string& value);

std::string AssembleCompileCommand(const BuildOptions& buildOptions);

std::string AssembleLinkCommand(const BuildOptions& buildOptions);

bool Compile(const BuildOptions& buildOptions, LPSTR builDir);

bool Link(BuildOptions& buildOptions, LPSTR buildDir);

int main(int argc, char* argv[]){
    std::cout << "\nStarting NCBuild\n\n";

    auto startTime = std::chrono::high_resolution_clock::now();

    BuildOptions buildOptions;    
    AddIncludePaths(buildOptions, NC_BUILD_INCLUDE_PATHS);

    for(int i = 1; i < argc; ++i)
    {
        std::string arg(argv[i]);
        std::string option;
        std::string value;
        CommandToOptionValue(arg, option, value);
        ProcessOptionValue(buildOptions, option, value);
    }

    std::string buildDirectoryString = GetBuildDirectory();
    LPSTR       buildDirectory       = const_cast<char *>(buildDirectoryString.c_str());

    if(buildOptions.compile)
    {
        if(!Compile(buildOptions, buildDirectory)) return 1;
    }

    if(buildOptions.link)
    {
        if(!Link(buildOptions, buildDirectory)) return 1;
    }

    auto endTime   = std::chrono::high_resolution_clock::now();
    auto timeDelta = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
    
    std::cout << "\nBuild Finished in " << timeDelta << " seconds\n\n";
    return 0;
}

std::string GetBuildDirectory()
{
    uint32_t DIRECTORY_BUFFER_SIZE = 100;
    char currentDirectory[DIRECTORY_BUFFER_SIZE];
    GetCurrentDirectory(DIRECTORY_BUFFER_SIZE, (LPSTR)currentDirectory);
    return std::string(currentDirectory) + "\\obj";
}

void AddIncludePaths(BuildOptions& buildOptions, const std::vector<std::string>& includeDirectories)
{
    for(auto& inc : includeDirectories)
    {
        buildOptions.includes += inc + " ";
    }
}

void AddLinkFlags(BuildOptions& buildOptions, const std::vector<std::string>& linkFlags)
{
    for(auto& link : linkFlags)
    {
        buildOptions.linkFlags += link + " ";
    }
}

void AddTargets(BuildOptions& buildOptions, const std::vector<std::string>& targets)
{
    for(auto& targ : targets)
    {
        buildOptions.targets += targ + " ";
    }
}

void CommandToOptionValue(const std::string& command, std::string& optionOut, std::string& argOut)
{
    int splitIndex = command.find('=');
    int optionLen  = splitIndex;
    int argLen     = command.size() - splitIndex;
    optionOut      = command.substr(0, optionLen);
    argOut         = command.substr(splitIndex + 1, argLen);
}

void ProcessOptionValue(BuildOptions& buildOptions, const std::string& option, const std::string& arg)
{
    if(option == "name")
    {
        buildOptions.appName = "-o " + arg;
    }
    else if (option == "mode")
    {
        if ((arg == "release") || (arg == "RELEASE")) 
        {
            buildOptions.mode = "";
        }
    }
    else if(option == "def")
    {
        buildOptions.defines = " -D " + arg;
    }
    else if(option == "link")
    {
        if( (arg == "true") || (arg == "t") || (arg == "TRUE"))
        {
            buildOptions.link = true;
        }
        else if( (arg == "false") || (arg == "f") || (arg == "FALSE"))
        {
            buildOptions.link = false;
        }
    }
    else if(option == "compile")
    {
        if( (arg == "true") || (arg == "t") || (arg == "TRUE"))
        {
            buildOptions.compile = true;
        }
        else if( (arg == "false") || (arg == "f") || (arg == "FALSE"))
        {
            buildOptions.compile = false;
        }
    }
    else if(option == "targ")
    {
        if(arg == "all")
        {
            AddTargets(buildOptions, NC_BUILD_TARGETS_COMMON);
            AddTargets(buildOptions, NC_BUILD_TARGETS_ENGINE);
            AddTargets(buildOptions, NC_BUILD_TARGETS_GRAPHICS);
            AddTargets(buildOptions, NC_BUILD_TARGETS_UTILS);
            AddTargets(buildOptions, NC_BUILD_TARGETS_PROJECT);
            AddTargets(buildOptions, NC_BUILD_TARGETS_EXTERNAL);
        }
        else if(arg == "common")
        {
            AddTargets(buildOptions, NC_BUILD_TARGETS_COMMON);
        }
        else if(arg == "engine")
        {
            AddTargets(buildOptions, NC_BUILD_TARGETS_ENGINE);
        }
        else if(arg == "graphics")
        {
            AddTargets(buildOptions, NC_BUILD_TARGETS_GRAPHICS);
        }
        else if(arg == "utils")
        {
            AddTargets(buildOptions, NC_BUILD_TARGETS_UTILS);
        }
        else if(arg == "project")
        {
            AddTargets(buildOptions, NC_BUILD_TARGETS_PROJECT);
        }   
        else if(arg == "external")
        {
            AddTargets(buildOptions, NC_BUILD_TARGETS_EXTERNAL);
        }
        else
        {
            std::cout << "Unkown target: " << arg << '\n';
        }
    }
}

std::string AssembleCompileCommand(const BuildOptions& buildOptions)
{
    return buildOptions.compiler      + " " +
           buildOptions.mode      + " " +
           buildOptions.standard      + " " +
           buildOptions.optimizations + " " +
           buildOptions.warnings      + " " +
           buildOptions.defines       + " " +
           buildOptions.includes      + " " +
           buildOptions.objCompile    + " " +
           buildOptions.targets;   
}

std::string AssembleLinkCommand(const BuildOptions& buildOptions)
{
    return buildOptions.compiler  + " " +
           buildOptions.standard  + " " +
           buildOptions.optimizations + " " +
           buildOptions.warnings      + " " +
           buildOptions.appName       + " " +
           buildOptions.objDirectory  + "\\*.o " +
           buildOptions.linkFlags;
}

bool Compile(const BuildOptions& buildOptions, LPSTR buildDir)
{
    std::string assembledCommand = AssembleCompileCommand(buildOptions);
    LPSTR       convertedCommand = const_cast<char *>(assembledCommand.c_str());

    PROCESS_INFORMATION processInfo;
    STARTUPINFO         startupInfo;
    ZeroMemory(&processInfo, sizeof(processInfo));
    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    
    if(!CreateProcess(NULL, convertedCommand, NULL, NULL, FALSE, 0, NULL, buildDir, &startupInfo, &processInfo))
    {
        std::cout << "CreateProcess (compiling) failed: " << GetLastError() << '\n';
        return false;
    }
    
    std::cout << "Compiling:\n\n" << assembledCommand << '\n';
    WaitForSingleObject(processInfo.hProcess, INFINITE);
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
    std::cout << "\nCompilation finished\n";
    return true;
}

bool Link(BuildOptions& buildOptions, LPSTR buildDir)
{
    AddLinkFlags(buildOptions, NC_BUILD_LINK_FLAGS);
    std::string linkCommand = AssembleLinkCommand(buildOptions);
    LPSTR       convertedLinkCommand = const_cast<char *>(linkCommand.c_str());

    PROCESS_INFORMATION processInfo;
    STARTUPINFO         startupInfo;
    ZeroMemory(&processInfo, sizeof(processInfo));
    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);

    if(!CreateProcess(NULL, convertedLinkCommand, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo))
    {
        std::cout << "CreateProcess (linking) failed: " << GetLastError() << '\n';
        return false;
    }

    std::cout << "\nLinking:\n\n" << linkCommand << '\n';
    WaitForSingleObject(processInfo.hProcess, INFINITE);
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
    std::cout << "\nLinking finished\n";
    return true;
}


