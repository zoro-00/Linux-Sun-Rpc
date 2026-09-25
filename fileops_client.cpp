#include "fileops.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <limits>
#include <filesystem>

namespace fs = std::filesystem;

class FileClient {
private:
    CLIENT* client;

    static std::string readInput(const std::string& prompt) {
        std::string input;
        std::cout << prompt;
        std::getline(std::cin, input);
        return input;
    }

    static int readChoice() {
        int choice = -1;
        std::cout << "Enter choice: ";
        std::string line;
        std::getline(std::cin, line);
        
        if (!line.empty()) {
            try {
                choice = std::stoi(line);
            } catch (const std::exception&) {
                choice = -1;
            }
        }
        return choice;
    }

    void displayMenu() const {
        std::cout << "\n----- RPC File Operations Menu -----\n"
                  << "1. Create File\n"
                  << "2. Write File (overwrite)\n"
                  << "3. Append to File\n"
                  << "4. Read File\n"
                  << "5. Delete File\n"
                  << "6. List All Files\n"
                  << "7. Upload File\n"
                  << "8. Download File\n"
                  << "9. Exit\n";
    }

    void handleCreate() {
        std::string fname = readInput("Enter filename: ");
        char* fnamePtr = const_cast<char*>(fname.c_str());
        
        op_result* result = create_file_1(&fnamePtr, client);
        if (result == nullptr) {
            clnt_perror(client, "call failed");
        } else {
            std::cout << "Server response [" << result->status << "]: " 
                      << result->message << std::endl;
        }
    }

    void handleWrite() {
        std::string fname = readInput("Enter filename: ");
        std::string content = readInput("Enter content: ");
        
        file_data fd;
        fd.fname = const_cast<char*>(fname.c_str());
        fd.content = const_cast<char*>(content.c_str());
        
        op_result* result = write_file_1(&fd, client);
        if (result == nullptr) {
            clnt_perror(client, "call failed");
        } else {
            std::cout << "Server response [" << result->status << "]: " 
                      << result->message << std::endl;
        }
    }

    void handleAppend() {
        std::string fname = readInput("Enter filename: ");
        std::string content = readInput("Enter content to append: ");
        
        file_data fd;
        fd.fname = const_cast<char*>(fname.c_str());
        fd.content = const_cast<char*>(content.c_str());
        
        op_result* result = append_file_1(&fd, client);
        if (result == nullptr) {
            clnt_perror(client, "call failed");
        } else {
            std::cout << "Server response [" << result->status << "]: " 
                      << result->message << std::endl;
        }
    }

    void handleRead() {
        std::string fname = readInput("Enter filename: ");
        char* fnamePtr = const_cast<char*>(fname.c_str());
        
        read_result* result = read_file_1(&fnamePtr, client);
        if (result == nullptr) {
            clnt_perror(client, "call failed");
        } else if (result->status == 0) {
            std::cout << "----- File Content -----\n"
                      << result->content << "\n"
                      << "-------------------------\n";
        } else {
            std::cout << "Error: " << result->content << std::endl;
        }
    }

    void handleDelete() {
        std::string fname = readInput("Enter filename: ");
        char* fnamePtr = const_cast<char*>(fname.c_str());
        
        op_result* result = delete_file_1(&fnamePtr, client);
        if (result == nullptr) {
            clnt_perror(client, "call failed");
        } else {
            std::cout << "Server response [" << result->status << "]: " 
                      << result->message << std::endl;
        }
    }

    void handleList() {
        char dummy = 0;
        list_result* result = list_files_1(&dummy, client);
        if (result == nullptr) {
            clnt_perror(client, "call failed");
        } else if (result->status == 0) {
            std::cout << "\n----- Server Files -----\n"
                      << result->files
                      << "------------------------\n";
        } else {
            std::cout << "Error: " << result->files << std::endl;
        }
    }

    void handleUpload() {
        std::string localPath = readInput("Enter local file path to upload: ");
        if (localPath.empty()) {
            std::cout << "Error: Local path cannot be empty.\n";
            return;
        }

        std::ifstream infile(localPath, std::ios::binary);
        if (!infile.is_open()) {
            std::cout << "Error: Cannot open local file '" << localPath << "' for reading.\n";
            return;
        }

        std::string content((std::istreambuf_iterator<char>(infile)),
                            std::istreambuf_iterator<char>());
        infile.close();

        fs::path p(localPath);
        std::string defaultName = p.filename().string();

        std::string remoteName = readInput("Enter remote filename (press Enter for '" + defaultName + "'): ");
        if (remoteName.empty()) {
            remoteName = defaultName;
        }

        file_data fd;
        fd.fname = const_cast<char*>(remoteName.c_str());
        fd.content = const_cast<char*>(content.c_str());

        op_result* result = upload_file_1(&fd, client);
        if (result == nullptr) {
            clnt_perror(client, "call failed");
        } else {
            std::cout << "Server response [" << result->status << "]: " 
                      << result->message << std::endl;
        }
    }

    void handleDownload() {
        std::string remoteName = readInput("Enter remote filename to download: ");
        if (remoteName.empty()) {
            std::cout << "Error: Remote filename cannot be empty.\n";
            return;
        }

        char* fnamePtr = const_cast<char*>(remoteName.c_str());
        read_result* result = download_file_1(&fnamePtr, client);

        if (result == nullptr) {
            clnt_perror(client, "call failed");
        } else if (result->status == 0) {
            std::string localPath = readInput("Enter local path to save file (press Enter for '" + remoteName + "'): ");
            if (localPath.empty()) {
                localPath = remoteName;
            }

            std::ofstream outfile(localPath, std::ios::binary);
            if (!outfile.is_open()) {
                std::cout << "Error: Could not open local file '" << localPath << "' for writing.\n";
                return;
            }

            std::string content = result->content;
            outfile.write(content.data(), content.size());
            outfile.close();

            std::cout << "File downloaded successfully to '" << localPath 
                      << "' (" << content.size() << " bytes)\n";
        } else {
            std::cout << "Error: " << result->content << std::endl;
        }
    }

public:
    FileClient(const std::string& server) {
        client = clnt_create(const_cast<char*>(server.c_str()), 
                            FILEPROG, FILEVERS, "tcp");
        if (client == nullptr) {
            clnt_pcreateerror(const_cast<char*>(server.c_str()));
            throw std::runtime_error("Failed to connect to RPC server");
        }
    }

    ~FileClient() {
        if (client != nullptr) {
            clnt_destroy(client);
        }
    }

    void run() {
        while (true) {
            displayMenu();
            int choice = readChoice();

            if (choice == 9) {
                std::cout << "Exiting client.\n";
                break;
            }

            if (choice < 1 || choice > 8) {
                std::cout << "Invalid choice, try again.\n";
                continue;
            }

            try {
                switch (choice) {
                    case 1:
                        handleCreate();
                        break;
                    case 2:
                        handleWrite();
                        break;
                    case 3:
                        handleAppend();
                        break;
                    case 4:
                        handleRead();
                        break;
                    case 5:
                        handleDelete();
                        break;
                    case 6:
                        handleList();
                        break;
                    case 7:
                        handleUpload();
                        break;
                    case 8:
                        handleDownload();
                        break;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <server_host>" << std::endl;
        return 1;
    }

    try {
        FileClient client(argv[1]);
        client.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
