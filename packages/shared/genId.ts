export default function*(i = 0): Generator<number, number, unknown> {
  while (true) yield ++i;
}
