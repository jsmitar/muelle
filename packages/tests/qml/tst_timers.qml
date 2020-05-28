import QtQuick 2.14
import QtTest 1.14

TestCase {
  name: "Extensions"

  function test_timeout() {
    print("START 100 timers")
    const timers = Array(100).fill(0)
      .map((_, i) => {
        return setTimeout(() => {
          fail(`${i}: setTimeout completed`)
        }, 1000)
      })

    wait(600)
    timers.forEach(timer => clearTimeout(timer))
    print("STOP 100 timers")
    wait(600)
  }

  function test_interval() {
    print("START 10 timers")
    const timers = Array(4).fill(0)
      .forEach((_, i) => {
        let n = 0
        const timer = setInterval(() => {
          if (++n > 10) {
            fail(`${i}: setInterval triggered`)
          } else if (n === 10) {
            clearInterval(timer)
            print(`STOP timer: ${i}`)
          }
        }, 50)
      })

    wait(50 * 14)
  }
}
