#pragma once
#include "revolver.hpp"
#include <iostream>
#include <unistd.h>

typedef struct {
  int playerStatuses;
  int currentPlayer;
  int playerCount;
  revolverT revolver;
  int liveRounds;
} gameT;
gameT newGame(int playerCount = 5, int liveRounds = 1) {
  int playerStatuses = 0;
  int count = playerCount;
  while (count) {
    count--;
    playerStatuses += 1 << count;
  }
  return {
      playerStatuses, (int)floor(rand() % playerCount),
      playerCount,    newRevolver(liveRounds),
      liveRounds,
  };
}
int fire(gameT &game, int playerIdx) {
  int isLive = check(game.revolver);
  game.playerStatuses &= ~(isLive << playerIdx);
  shoot(game.revolver);
  return isLive;
}
int isAlive(gameT &game, int playerIdx) {
  return game.playerStatuses & 1 << playerIdx;
}
int botMove(gameT &game) {
  int target = round(rand() % game.playerCount);
  while (!isAlive(game, target)) {
    target = round(rand() % game.playerCount);
  }
  return target;
}
int gameEnd(gameT &game) {
  int count = game.playerCount;
  int aliveCount = 0;
  int winner = -1;
  while (count) {
    count--;
    if (game.playerStatuses & 1 << count) {
      aliveCount++;
      winner = count;
    }
  }
  return aliveCount == 1 ? winner : -1;
}
void nextPlayer(gameT &game) {
  game.currentPlayer++;
  game.currentPlayer %= game.playerCount;
}
std::string ask(std::string question) {
  std::cout << question;
  std::string answer;
  std::cin >> answer;
  return answer;
}
std::string playerStatusesString(gameT &game) {
  std::string out;
  int count = game.playerCount;
  while (count) {
    count--;
    out += game.playerStatuses & 1 << count ? "1" : "0";
  }
  return out;
}
int step(gameT &game, int playerIdx, int betIdx) {
  if (!isAlive(game, game.currentPlayer)) {
    nextPlayer(game);
    return -1;
  }
  if (!isEmpty(game.revolver)) {
    std::cout << "Revolver is empty, reloading\n";
    reload(game.revolver, game.liveRounds);
    usleep(game.liveRounds * 1e6);
  }
  std::cout << "Hands revolver to Player " << game.currentPlayer << "\n";
  usleep(2 * 1e6);
  std::cout << "Player " << game.currentPlayer << " spins the cylinder\n";
  spin(game.revolver);
  usleep(2 * 1e6);
  std::cout << "Player statuses: " << playerStatusesString(game) << "\n";
  int target = -1;
  if (playerIdx != -1 && isAlive(game, playerIdx) &&
      game.currentPlayer == playerIdx) {
    target = std::stoi(ask("Enter player index to shoot (0 - " +
                           std::to_string(game.playerCount - 1) + "): "));
  } else {
    int waitTimeS = rand() % 5;
    usleep(waitTimeS * 1e6);
    target = botMove(game);
  }
  std::cout << "Player " << game.currentPlayer << " shoots Player " << target
            << "\n";
  if (fire(game, target)) {
    std::cout << "Player " << target << " died\n";
    if (target == playerIdx || target == betIdx) {
      if (target == playerIdx)
        std::cout << "You died\n";
      if (ask("Continue to spectate? [y/n] ").compare("y") != 0) {
        return 0;
      }
    }
  }
  int winner = gameEnd(game);
  nextPlayer(game);
  if (winner < 0)
    return -1;
  std::cout << "Player " << winner << " wins\n";
  if (betIdx == -1)
    return 0;
  std::string gameEndString = winner == betIdx
                                  ? "Congradulations on winning the bet!"
                                  : "Better luck next time";
  std::cout << gameEndString << "\n";
  return 0;
}
int random(int min, int max) { return rand() % (max - min) + min; }
void initGame() {
  std::srand(time(NULL));
  int isSpectator = ask("Do you wish to spectate? [y/n] ").compare("y");
  int liveRounds = random(1, 3);
  int playerCount = random(4, 8);
  gameT game = newGame(playerCount, liveRounds);
  std::cout << playerCount << " players, " << liveRounds << " live rounds\n";
  int playerIdx = -1, bet = -1;
  if (isSpectator != 0) {
    playerIdx = rand() % game.playerCount;
    std::cout << "Welcome, Player " << playerIdx << "\n";
  } else {
    bet = std::stoi(
        ask("Please place bet on the player you believe would survive (0 - " +
            std::to_string(game.playerCount - 1) + "): "));
  }
  int end = 0;
  while (!end) {
    if (step(game, playerIdx, bet) != 0)
      continue;
    end = 1;
  }
}
