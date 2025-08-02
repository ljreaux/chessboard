#ifndef STOCKFISH_ENGINE_H
#define STOCKFISH_ENGINE_H

#include <string>

class StockfishEngine
{
public:
  bool start(const std::string &enginePath = "stockfish");
  void sendCommand(const std::string &cmd);
  std::string readLine();
  void stop();
  ~StockfishEngine();

private:
  int toEngine[2];   // Parent → Child (stdin)
  int fromEngine[2]; // Child → Parent (stdout)
  pid_t pid = -1;
};

#endif