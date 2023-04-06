import("utils.archive")
import("download")

if (os.host() =="windows") then 
    local windows_llvm_version = "15.0.4"
    -- windows llvm
    local release_url = "SakuraEngine/llvm-package-windows/releases/download/llvm-"..windows_llvm_version.."/llvm-"..windows_llvm_version.."-windows-amd64-msvc17-msvcrt.7z"
    local zipname = "llvm-"..windows_llvm_version.."-windows-amd64-msvc17-msvcrt.7z"
    download.file_from_github(release_url, zipname)
    archive.extract("SDKs/"..zipname, os.projectdir())
    
    -- windows clang
    local release_url = "SakuraEngine/llvm-package-windows/releases/download/llvm-"..windows_llvm_version.."/clang-"..windows_llvm_version.."-windows-amd64-msvc17-msvcrt.7z"
    local zipname = "clang-"..windows_llvm_version.."-windows-amd64-msvc17-msvcrt.7z"
    download.file_from_github(release_url, zipname)
    archive.extract("SDKs/"..zipname, os.projectdir())
end

-- macos llvm & clang
if (os.host() =="macosx") then 
    local release_url = "llvm/llvm-project/releases/download/llvmorg-15.0.1/clang+llvm-15.0.1-x86_64-apple-darwin.tar.xz"
    local zipname = "clang+llvm-15.0.1-x86_64-apple-darwin.tar.xz"
    download.file_from_github(release_url, zipname)
    archive.extract("SDKs/"..zipname, os.projectdir())
end
