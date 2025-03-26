#pragma once
#include <cstdlib>

typedef struct {
  int shots;
  int capacity;
} revolverT;

int randomizeShots(int numberOfBullets, int capacity) {
  numberOfBullets = numberOfBullets < capacity ? numberOfBullets : capacity;
  int shots = 0;
  while (numberOfBullets) {
    numberOfBullets--;
    int position = rand() % capacity;
    while (shots & 1 << position) {
      position++;
      position %= capacity;
    }
    shots |= 1 << position;
  }
  return shots;
}

revolverT newRevolver(int numberOfBullets = 1, int capacity = 6) {
  return {
      randomizeShots(numberOfBullets, capacity),
      capacity,
  };
}

void reload(revolverT &revolver, int numberOfBullets = 1) {
  revolver.shots = randomizeShots(numberOfBullets, revolver.capacity);
}

void load(revolverT &revolver, int numberOfBullets) {
  int position = revolver.capacity;
  while (numberOfBullets) {
    numberOfBullets--;
    while (revolver.shots & 1 << position && position >= 0) {
      position--;
    }
    revolver.shots |= 1 << position;
  }
}

int check(revolverT &revolver) {
  // 1 means live
  return revolver.shots & 1;
}

void shoot(revolverT &revolver) { revolver.shots >>= 1; }

int spin(revolverT &revolver) {
  int spins = rand() % revolver.capacity;
  while (spins > 0) {
    revolver.shots += (revolver.shots & 1) << revolver.capacity;
    revolver.shots >>= 1;
    spins--;
  }
  return spins;
}

int isEmpty(revolverT &revolver) {
  int count = revolver.capacity;
  int liveCount = 0;
  while (count) {
    count--;
    liveCount += revolver.shots & 1 << count;
  }
  return liveCount;
}
