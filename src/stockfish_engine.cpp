#include "stockfish_engine.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>

bool StockfishEngine::start(const std::string &enginePath)
{
  if (pipe(toEngine) != 0 || pipe(fromEngine) != 0)
  {
    std::cerr << "Failed to create pipes.\n";
    return false;
  }

  pid = fork();
  if (pid < 0)
  {
    std::cerr << "Failed to fork.\n";
    return false;
  }

  if (pid == 0)
  {
    // Child process
    dup2(toEngine[0], STDIN_FILENO);
    dup2(fromEngine[1], STDOUT_FILENO);
    close(toEngine[1]);
    close(fromEngine[0]);

    execlp(enginePath.c_str(), enginePath.c_str(), nullptr);
    std::cerr << "Failed to exec Stockfish.\n";
    exit(1);
  }

  // Parent process
  close(toEngine[0]);
  close(fromEngine[1]);

  // Set read non-blocking (optional)
  fcntl(fromEngine[0], F_SETFL, O_NONBLOCK);

  return true;
}

void StockfishEngine::sendCommand(const std::string &cmd)
{
  std::string line = cmd + "\n";
  write(toEngine[1], line.c_str(), line.size());
}

std::string StockfishEngine::readLine()
{
  char buffer[256];
  ssize_t count = read(fromEngine[0], buffer, sizeof(buffer) - 1);
  if (count > 0)
  {
    buffer[count] = '\0';
    return std::string(buffer);
  }
  return "";
}

void StockfishEngine::stop()
{
  if (pid > 0)
  {
    kill(pid, SIGTERM);
    waitpid(pid, nullptr, 0);
  }
}

StockfishEngine::~StockfishEngine()
{
  stop();
}