download = download or {}

function file_from_github(zip, fname)
    import("net.http")
    import("lib.detect.find_file")
    local sdkdir = sdkdir or os.projectdir().."/SDKs"
    local zip_dir = find_file(fname, {sdkdir})
    if(zip_dir == nil) then
        local url = vformat("https://github.com/")
        print("download: "..url..zip.." to: "..os.projectdir().."/SDKs/"..zip)
        http.download(url..zip, os.projectdir().."/SDKs/"..fname, {continue = false})
    end
end