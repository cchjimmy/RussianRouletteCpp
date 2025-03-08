export type revolver = { shots: number; capacity: number };
export function newRevolver(
  numberOfBullets: number = 1,
  capacity: number = 6,
): revolver {
  return {
    shots: randomizeShots(numberOfBullets, capacity),
    capacity,
  };
}

export function randomizeShots(
  numberOfBullets: number,
  capacity: number,
): number {
  numberOfBullets = Math.min(numberOfBullets, capacity);
  let shots = 0;
  while (numberOfBullets) {
    numberOfBullets--;
    let position = Math.random() * capacity;
    while (shots & 1 << position) {
      position++;
      position %= capacity;
    }
    shots |= 1 << position;
  }
  return shots;
}

export function reload(revolver: revolver, numberOfBullets: number = 1) {
  revolver.shots = randomizeShots(numberOfBullets, revolver.capacity);
}

export function load(revolver: revolver, numberOfBullets: number) {
  let position = revolver.capacity;
  while (numberOfBullets) {
    numberOfBullets--;
    while (revolver.shots & 1 << position && position >= 0) {
      position--;
    }
    revolver.shots |= 1 << position;
  }
}

export function check(revolver: revolver): number {
  // 1 means live
  return revolver.shots & 1;
}

export function shoot(revolver: revolver) {
  revolver.shots >>= 1;
}

export function spin(revolver: revolver): number {
  let spins = Math.random() * revolver.capacity;
  while (spins > 0) {
    revolver.shots += (revolver.shots & 1) << revolver.capacity;
    revolver.shots >>= 1;
    spins--;
  }
  return spins;
}
export function liveLeft(revolver: revolver): number {
  let count = revolver.capacity;
  let liveCount = 0;
  while (count) {
    count--;
    liveCount += revolver.shots & 1 << count;
  }
  return liveCount;
}
