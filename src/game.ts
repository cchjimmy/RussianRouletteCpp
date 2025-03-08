import {
  check,
  liveLeft,
  newRevolver,
  reload,
  revolver,
  shoot as shootGun,
  spin,
} from "./revolver.ts";
import { scheduler } from "node:timers/promises";

export type game = {
  playerStatuses: number;
  currentPlayer: number;
  playerCount: number;
  revolver: revolver;
  liveRounds: number;
};
export function newGame(playerCount: number = 5, liveRounds: number = 1): game {
  let playerStatuses = 0;
  let count = playerCount;
  while (count) {
    count--;
    playerStatuses += 1 << count;
  }
  return {
    playerStatuses,
    currentPlayer: Math.floor(Math.random() * playerCount),
    playerCount,
    revolver: newRevolver(liveRounds),
    liveRounds,
  };
}
export function shoot(game: game, playerIdx: number): number {
  const isLive = check(game.revolver);
  game.playerStatuses &= ~(isLive << playerIdx);
  shootGun(game.revolver);
  return isLive;
}
export function botMove(game: game): number {
  let target = Math.round(Math.random() * game.playerCount);
  while (!isAlive(game, target)) {
    target = Math.round(Math.random() * game.playerCount);
  }
  return target;
}
export function gameEnd(game: game): number {
  let count = game.playerCount;
  let aliveCount = 0;
  let winner = -1;
  while (count) {
    count--;
    if (game.playerStatuses & 1 << count) {
      aliveCount++;
      winner = count;
    }
  }
  return aliveCount == 1 ? winner : -1;
}
export function isAlive(game: game, playerIdx: number): number {
  return game.playerStatuses & 1 << playerIdx;
}
export function nextPlayer(game: game) {
  game.currentPlayer++;
  game.currentPlayer %= game.playerCount;
}
export async function step(
  game: game,
  playerIdx: number,
  betIdx: number,
): Promise<number> {
  if (!liveLeft(game.revolver)) {
    console.log("Revolver is empty, reloading ...");
    reload(game.revolver, game.liveRounds);
  }
  if (!isAlive(game, game.currentPlayer)) {
    nextPlayer(game);
    return -1;
  }
  console.log(`Hands revolver to Player ${game.currentPlayer} ...`);
  await scheduler.wait(2000);
  console.log(`Player ${game.currentPlayer} spins the cylinder ...`);
  spin(game.revolver);
  await scheduler.wait(2000);
  console.log(`Player statuses: ${game.playerStatuses.toString(2)}`);
  let target = -1;
  if (
    playerIdx != -1 && isAlive(game, playerIdx) &&
    game.currentPlayer == playerIdx
  ) {
    target = parseInt(
      prompt(
        `Enter player index to shoot (0-${game.playerCount - 1}):`,
      ) || "",
    );
  } else {
    const waitTimeMs = Math.random() * 5000;
    await scheduler.wait(waitTimeMs);
    target = botMove(game);
  }
  console.log(`Player ${game.currentPlayer} shoots Player ${target} ...`);
  if (shoot(game, target)) {
    console.log(`Player ${target} died`);
    if (target == playerIdx) console.log("You died");
  }
  const winner = gameEnd(game);
  nextPlayer(game);
  if (winner < 0) return -1;
  console.log(`Player ${winner} wins`);
  if (betIdx == -1) return 0;
  console.log(
    winner == betIdx
      ? "Congradulations on winning the bet!"
      : "Better luck next time ...",
  );
  return 0;
}
function random(min: number, max: number): number {
  return Math.random() * (max - min) + min;
}
export async function initGame() {
  const isSpectator = confirm("Do you wish to spectate?");
  const liveRounds = parseInt(prompt("How many live rounds?:") || "");
  const game = newGame(Math.round(random(4, 6)), liveRounds || 1);
  let playerIdx = -1, bet = -1;
  if (!isSpectator) {
    playerIdx = Math.floor(Math.random() * game.playerCount);
    console.log(`Welcome, Player ${playerIdx}`);
  } else {
    bet = parseInt(
      prompt(
        `Please place bet on the player you believe would survive (0-${
          game.playerCount - 1
        }):`,
      ) || "",
    );
  }
  let end = 0;
  while (!end) {
    if (await step(game, playerIdx, bet) < 0) continue;
    end = 1;
  }
}
