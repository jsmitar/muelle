import * as F from 'qrc:/shared/functional';

export function from<T>(iterable?: T[]) {
  return new Seq<T>(Array.from(iterable || []));
}

export default { from };

class Seq<T> {
  iterable: T[];

  constructor(iterable: T[]) {
    this.iterable = iterable;
  }

  map<
    Fn extends (item: T, index: number, array: T[]) => any,
    U = Seq<ReturnType<Fn>>
  >(fn: Fn): Seq<U> {
    return (this.iterable = F.map(this.iterable, fn)), <any>this;
  }

  concat<U>(iterable: U): Seq<T | U> {
    return this.iterable.concat(<any>iterable), this;
  }

  filter<Fn extends (item: T, index: number, array: T[]) => any>(
    fn: Fn
  ): Seq<T> {
    return (this.iterable = F.filter(this.iterable, fn)), this;
  }

  each<Fn extends (item: T, index: number, array: T[]) => any>(fn: Fn): Seq<T> {
    return F.each(this.iterable, fn), this;
  }

  get value(): T[] {
    return this.iterable;
  }
}
