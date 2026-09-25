#include "fileops.h"
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstring>
#include <memory>

namespace fs = std::filesystem;

const std::string SERVER_DIR = "server_files";

class FileServer {
private:
    static std::string ensureDir() {
        if (!fs::exists(SERVER_DIR)) {
            fs::create_directory(SERVER_DIR);
        }
        return SERVER_DIR;
    }

    static std::string getFullPath(const std::string& fname) {
        ensureDir();
        return SERVER_DIR + "/" + fname;
    }

public:
    static op_result* createFile(const std::string& fname) {
        static op_result result;
        std::memset(&result, 0, sizeof(result));

        try {
            std::string path = getFullPath(fname);
            std::ofstream file(path);
            
            if (!file.is_open()) {
                result.status = -1;
                result.message = strdup("Error: could not create file");
            } else {
                file.close();
                result.status = 0;
                result.message = strdup("File created successfully");
            }
        } catch (const std::exception& e) {
            result.status = -1;
            result.message = strdup("Error: exception during file creation");
        }

        std::cout << "[SERVER] CREATE_FILE '" << fname << "' -> " 
                  << result.message << std::endl;
        std::cout.flush();
        return &result;
    }

    static op_result* writeFile(const std::string& fname, const std::string& content) {
        static op_result result;
        std::memset(&result, 0, sizeof(result));

        try {
            std::string path = getFullPath(fname);
            std::ofstream file(path);

            if (!file.is_open()) {
                result.status = -1;
                result.message = strdup("Error: could not write file");
            } else {
                file << content;
                file.close();
                result.status = 0;
                result.message = strdup("File written successfully");
            }
        } catch (const std::exception& e) {
            result.status = -1;
            result.message = strdup("Error: exception during file write");
        }

        std::cout << "[SERVER] WRITE_FILE '" << fname << "' -> " 
                  << result.message << std::endl;
        std::cout.flush();
        return &result;
    }

    static op_result* appendFile(const std::string& fname, const std::string& content) {
        static op_result result;
        std::memset(&result, 0, sizeof(result));

        try {
            std::string path = getFullPath(fname);
            std::ofstream file(path, std::ios::app);

            if (!file.is_open()) {
                result.status = -1;
                result.message = strdup("Error: could not append to file");
            } else {
                file << content;
                file.close();
                result.status = 0;
                result.message = strdup("Content appended successfully");
            }
        } catch (const std::exception& e) {
            result.status = -1;
            result.message = strdup("Error: exception during file append");
        }

        std::cout << "[SERVER] APPEND_FILE '" << fname << "' -> " 
                  << result.message << std::endl;
        std::cout.flush();
        return &result;
    }

    static read_result* readFile(const std::string& fname) {
        static read_result result;
        static char buf[MAXDATALEN];
        std::memset(&result, 0, sizeof(result));

        try {
            std::string path = getFullPath(fname);
            std::ifstream file(path);

            if (!file.is_open()) {
                result.status = -1;
                snprintf(buf, sizeof(buf), "Error: file '%s' not found", fname.c_str());
                result.content = buf;
            } else {
                std::string content((std::istreambuf_iterator<char>(file)),
                                   std::istreambuf_iterator<char>());
                file.close();
                
                strncpy(buf, content.c_str(), sizeof(buf) - 1);
                buf[sizeof(buf) - 1] = '\0';
                
                result.status = 0;
                result.content = buf;
            }
        } catch (const std::exception& e) {
            result.status = -1;
            snprintf(buf, sizeof(buf), "Error: exception during file read");
            result.content = buf;
        }

        std::cout << "[SERVER] READ_FILE '" << fname << "' (status=" 
                  << result.status << ")" << std::endl;
        std::cout.flush();
        return &result;
    }

    static op_result* deleteFile(const std::string& fname) {
        static op_result result;
        std::memset(&result, 0, sizeof(result));

        try {
            std::string path = getFullPath(fname);
            
            if (fs::remove(path)) {
                result.status = 0;
                result.message = strdup("File deleted successfully");
            } else {
                result.status = -1;
                result.message = strdup("Error: could not delete file (does it exist?)");
            }
        } catch (const std::exception& e) {
            result.status = -1;
            result.message = strdup("Error: exception during file deletion");
        }

        std::cout << "[SERVER] DELETE_FILE '" << fname << "' -> " 
                  << result.message << std::endl;
        std::cout.flush();
        return &result;
    }

    static op_result* uploadFile(const std::string& fname, const std::string& content) {
        static op_result result;
        static char msg_buf[256];
        std::memset(&result, 0, sizeof(result));

        try {
            std::string path = getFullPath(fname);
            std::ofstream file(path, std::ios::binary);

            if (!file.is_open()) {
                result.status = -1;
                snprintf(msg_buf, sizeof(msg_buf), "Error: could not create/write file '%s' on server", fname.c_str());
                result.message = msg_buf;
            } else {
                file.write(content.data(), content.size());
                file.close();
                result.status = 0;
                snprintf(msg_buf, sizeof(msg_buf), "File '%s' uploaded successfully (%zu bytes)", fname.c_str(), content.size());
                result.message = msg_buf;
            }
        } catch (const std::exception& e) {
            result.status = -1;
            snprintf(msg_buf, sizeof(msg_buf), "Error: exception during file upload: %s", e.what());
            result.message = msg_buf;
        }

        std::cout << "[SERVER] UPLOAD_FILE '" << fname << "' -> " 
                  << result.message << std::endl;
        std::cout.flush();
        return &result;
    }

    static read_result* downloadFile(const std::string& fname) {
        static read_result result;
        static char buf[MAXDATALEN];
        std::memset(&result, 0, sizeof(result));

        try {
            std::string path = getFullPath(fname);
            std::ifstream file(path, std::ios::binary);

            if (!file.is_open()) {
                result.status = -1;
                snprintf(buf, sizeof(buf), "Error: file '%s' not found on server", fname.c_str());
                result.content = buf;
            } else {
                file.read(buf, sizeof(buf) - 1);
                std::streamsize bytes_read = file.gcount();
                buf[bytes_read] = '\0';
                file.close();
                
                result.status = 0;
                result.content = buf;
            }
        } catch (const std::exception& e) {
            result.status = -1;
            snprintf(buf, sizeof(buf), "Error: exception during file download: %s", e.what());
            result.content = buf;
        }

        std::cout << "[SERVER] DOWNLOAD_FILE '" << fname << "' (status=" 
                  << result.status << ")" << std::endl;
        std::cout.flush();
        return &result;
    }

    static list_result* listFiles() {
        static list_result result;
        static char buf[MAXDATALEN];
        std::memset(&result, 0, sizeof(result));

        try {
            ensureDir();
            std::string listing;
            int count = 0;

            for (const auto& entry : fs::directory_iterator(SERVER_DIR)) {
                if (entry.is_regular_file()) {
                    count++;
                    auto size = entry.file_size();
                    std::string item = std::to_string(count) + ". " + 
                                       entry.path().filename().string() + 
                                       " (" + std::to_string(size) + " bytes)\n";
                    if (listing.size() + item.size() < sizeof(buf) - 50) {
                        listing += item;
                    }
                }
            }

            if (count == 0) {
                listing = "No files found on server.\n";
            } else {
                listing += "Total: " + std::to_string(count) + " file(s)\n";
            }

            snprintf(buf, sizeof(buf), "%s", listing.c_str());
            result.status = 0;
            result.files = buf;
        } catch (const std::exception& e) {
            result.status = -1;
            snprintf(buf, sizeof(buf), "Error: exception listing files: %s", e.what());
            result.files = buf;
        }

        std::cout << "[SERVER] LIST_FILES -> status=" << result.status << std::endl;
        std::cout.flush();
        return &result;
    }
};

/* RPC handler functions - C linkage for compatibility with rpcgen stubs */
extern "C" {

op_result *
create_file_1_svc(char **fname, struct svc_req *rqstp)
{
    return FileServer::createFile(*fname);
}

op_result *
write_file_1_svc(file_data *fd, struct svc_req *rqstp)
{
    return FileServer::writeFile(fd->fname, fd->content);
}

op_result *
append_file_1_svc(file_data *fd, struct svc_req *rqstp)
{
    return FileServer::appendFile(fd->fname, fd->content);
}

read_result *
read_file_1_svc(char **fname, struct svc_req *rqstp)
{
    return FileServer::readFile(*fname);
}

op_result *
delete_file_1_svc(char **fname, struct svc_req *rqstp)
{
    return FileServer::deleteFile(*fname);
}

op_result *
upload_file_1_svc(file_data *fd, struct svc_req *rqstp)
{
    return FileServer::uploadFile(fd->fname, fd->content);
}

read_result *
download_file_1_svc(char **fname, struct svc_req *rqstp)
{
    return FileServer::downloadFile(*fname);
}

list_result *
list_files_1_svc(void *argp, struct svc_req *rqstp)
{
    return FileServer::listFiles();
}

} /* extern "C" */
