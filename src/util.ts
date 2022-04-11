export function validateInt(...args: number[]): void {
  args.forEach(num => {
    if (!Number.isInteger(num) || num < -0x80000000 || num > 0x7fffffff) {
      throw RangeError("Number is not a 32-bit signed integer");
    }
  });
}
