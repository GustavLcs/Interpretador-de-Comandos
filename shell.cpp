#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>

std::vector<std::string> history;

void printcwd() {
    int buf_size = 1024;
    char buffer[buf_size];

    if (getcwd(buffer, buf_size) != NULL)
        std::cout << buffer << std::endl;
    else
        std::cerr << "erro em: getcwd()" << std::endl;
}

void print_history() {
    int size = history.size();

    if (size > 10)
        for (int i = 10; i > 0; i--)
            std::cout << i - 1 << " " << history[size - i] << std::endl;
    else
        for (int i = size; i > 0; i--)
            std::cout << i - 1 << " " << history[size - i] << std::endl;
}

std::string find_command(std::string command) {
    if (command[0] == '/' || (command.size() >= 2 && command.substr(0, 2) == "./")) {
        if (access(command.c_str(), F_OK) == 0)
            return command;

        return "";
    }

    char* path = getenv("PATH");

    if (path == nullptr)
        return "";

    std::string path_string(path);
    size_t inicio = 0;
    while (inicio < path_string.size()) {
        size_t fim = path_string.find(':', inicio);
        if (fim == std::string::npos)
            fim = path_string.size();

        std::string diretorio = path_string.substr(inicio, fim - inicio);

        std::string caminho = diretorio + "/" + command;
        if (access(caminho.c_str(), F_OK) == 0)
            return caminho;
        inicio = fim + 1;
    }

    return "";
}

void process_command(std::string command) {
    if (command == "")
        return;

    history.push_back(command);
    std::stringstream ss(command);
    std::vector<std::string> args;
    std::string palavra;
    while (ss >> palavra)
        args.push_back(palavra);
    if (args.empty())
        return;
    std::string comando = args[0];

    // Se for comando interno
    if (comando == "exit")
        exit(0);

    if (comando == "pwd") {
        printcwd();
        return;
    }

    if (comando == "cd") {
        std::string path;

        if (args.size() >= 2) {
            path = args[1];
        } else {
            char* home = getenv("HOME");
            if (home != nullptr)
                path = home;
            else {
                std::cout << "diretório não encontrado" << std::endl;
                return;
            }
        }

        if (chdir(path.c_str()) != 0)
            std::cout << "caminho: '" << path << "' não encontrado" << std::endl;

        return;
    }

    if (comando == "history") {
        if (args.size() == 1) {
            print_history();
            return;
        }
        if (args.size() == 2) {
            std::string arg = args[1];
            if (arg == "-c") {
                history.clear();
                return;
            }
            int value = std::atoi(arg.c_str());
            int size = history.size();

            if (value < 0 || value >= size || value >= 10) {
                std::cout << "invalid arguments: " << arg << std::endl;
                return;
            }

            int index = size - 1 - value;
            if (index >= 0 && index < size) {
                std::string old_command = history[index];
                if (old_command != command) {
                    process_command(old_command);
                    return;
                }
            }
            std::cout << "invalid arguments: " << arg << std::endl;
            return;
        }

        std::cout << "invalid arguments" << std::endl;
        return;
    }

    // Se for comando externo
    std::string absolute_path = find_command(comando);
    if (absolute_path == "") {
        std::cout << "command not found: " << comando << std::endl;
        return;
    }

    if (access(absolute_path.c_str(), X_OK) != 0) {
        std::cout << "permission denied: " << comando << std::endl;
        return;
    }

    pid_t pid = fork();
    if (pid < 0) {
        // Erro
        std::cout << "Erro de execução!" << std::endl;
        return;
    }
    else if (pid == 0) {
        // Processo filho
        std::vector<char*> argv;
        for (size_t i = 0; i < args.size(); i++)
            argv.push_back((char*)args[i].c_str());
        argv.push_back(nullptr);
        extern char **environ;
        execve(absolute_path.c_str(), argv.data(), environ);
        std::cout << "Erro ao executar o comando" << std::endl;
        exit(1);
    }
    else {
        // Processo pai
        waitpid(pid, nullptr, 0);
    }
}

int main() {
    while (true) {
        std::cout << "$";
        std::string command;
        getline(std::cin, command);
        process_command(command);
    }
    return 0;
}
