#include <iostream>
#include "stockfish_engine.h"
#include <thread>
#include <chrono>
#include <sstream>
#include <vector>

// Helper to split a string into lines
std::vector<std::string> splitLines(const std::string &blob)
{
  std::vector<std::string> lines;
  std::istringstream stream(blob);
  std::string line;
  while (std::getline(stream, line))
  {
    lines.push_back(line);
  }
  return lines;
}

int main()
{
  StockfishEngine engine;
  if (!engine.start())
  {
    std::cerr << "Failed to start Stockfish.\n";
    return 1;
  }

  while (true)
  {
    std::string fromSquare;

    std::cout << "Enter square (e.g. e2), or 'quit' to exit: ";
    std::cin >> fromSquare;

    if (fromSquare == "quit")
    {
      break;
    }

    engine.sendCommand("ucinewgame");
    engine.sendCommand("position startpos");
    engine.sendCommand("go perft 1");

    std::cout << "Valid moves from " << fromSquare << ":\n";

    for (int i = 0; i < 100; ++i)
    {
      std::string blob = engine.readLine();
      if (!blob.empty())
      {
        auto lines = splitLines(blob);
        for (const std::string &line : lines)
        {
          size_t colonPos = line.find(':');
          if (colonPos != std::string::npos)
          {
            std::string move = line.substr(0, colonPos);
            if (move.substr(0, fromSquare.size()) == fromSquare)
            {
              std::cout << "  " << move << "\n";
            }
          }
        }
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    std::cout << "\n";
  }

  return 0;
}