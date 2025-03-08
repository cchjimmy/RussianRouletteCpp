export enum powerTypes {
	attack,
	defence,
	healing,
}
// a: attack, d: defence
export type stats = { ap: number; dp: number };
export type power = {
	name: string;
	description: string;
	type: powerTypes;
	stats: stats;
};
export const powers: Map<string, power> = new Map();
function newPower(power: power) {
	powers.set(name, power);
}
