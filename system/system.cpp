#include "system.hpp"
#include <utils.hpp>

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_PIPES          3
 
#define READ_FD  0
#define WRITE_FD 1
#define STDERR_FD 2

#define PARENT_WRITE_PIPE  0
#define PARENT_READ_PIPE   1
#define PARENT_STDERR_PIPE 2

#define PARENT_READ_FD  ( pipes[PARENT_READ_PIPE][READ_FD]   )
#define PARENT_WRITE_FD ( pipes[PARENT_WRITE_PIPE][WRITE_FD] )
#define PARENT_STDERR_FD ( pipes[PARENT_STDERR_PIPE][READ_FD])

#define CHILD_READ_FD   ( pipes[PARENT_WRITE_PIPE][READ_FD]  )
#define CHILD_WRITE_FD  ( pipes[PARENT_READ_PIPE][WRITE_FD]  )
#define CHILD_STDERR_FD ( pipes[PARENT_STDERR_PIPE][WRITE_FD] )

ProgramContext ExecuteProgram(std::string const& filename, std::vector<std::string> args, std::vector<std::string> env)
{
    args.insert(args.begin(), filename);

    char *const* Args = (char* const*)Vector2Array(args, true);
    char *const* Env = (char* const*)Vector2Array(env, true);

    ProgramContext context = {};

    int pipes[NUM_PIPES][2];
    
    pipe(pipes[PARENT_READ_PIPE]);
    pipe(pipes[PARENT_WRITE_PIPE]);
    pipe(pipes[PARENT_STDERR_PIPE]);


    int pid = fork();
    if(pid == 0)
    {
        dup2(CHILD_READ_FD, 0); //stdin
        dup2(CHILD_WRITE_FD, 1); //stdout
        dup2(CHILD_STDERR_FD, 2);

        
        close(CHILD_READ_FD);
        close(CHILD_WRITE_FD);
        close(CHILD_STDERR_FD);

        close(PARENT_STDERR_FD);
        close(PARENT_READ_FD);
        close(PARENT_WRITE_FD);

    
        if(execvpe(filename.c_str(), Args, Env) == -1){
            printf("[ERROR]: error executing %s!\n", filename.c_str());
        }
        exit(EXIT_FAILURE);
    }
    else if(pid < 0){
        printf("[ERROR]: error forking\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        close(CHILD_READ_FD);
        close(CHILD_WRITE_FD);
        close(CHILD_STDERR_FD);

        context.pid = pid;
        context.stdout = fdopen(PARENT_READ_FD, "r");
        context.stdin = fdopen(PARENT_WRITE_FD, "w");
        context.stderr = fdopen(PARENT_STDERR_FD, "r");

        int& status = context.status;
        do {
            waitpid(pid, &status, WUNTRACED);
        }while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    delete[] Args;
    delete[] Env;
    return context;    
}