export default function*(i = 0) {
  while (true) yield ++i;
}
