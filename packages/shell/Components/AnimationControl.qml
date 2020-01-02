import QtQuick 2.12
import '../libs/functional.ts' as F

SequentialAnimation {
  id: anim
  running: false

  onStopped: {
    const { _pendings } = control
    control._current = null

    for (let k in _pendings) {
      control._current = _pendings[k]
      delete _pendings[k]
      control._current._deliver()
      break;
    }
  }

  readonly property var control: ({
    _current: null,
    _pendings: {},
    _nextId: 0,
    cancelAndStop() {
      this.cancel()
      anim.stop()
      return this
    },
    stop() {
      anim.stop()
      return this
    },
    cancel() {
      if (this._current) {
        this._current._cancel()
        this._current = null
      }
      for (let k in this._pendings) {
        this._pendings[k]._cancel()
      }
      this._pendings = {}
      return this
    },
    start(onstart = F.noop) {
      const control = this

      const ctl = {
        id: control._nextId++,
        _onstart: onstart,
        _oncompleted: F.noop,
        _oncancelled: F.noop,
        _delivering: false,
        _cancel() {
          F.disconnect(anim.stopped, this._oncompleted)
          this._oncancelled()
        },
        _deliver() {
          console.assert(!anim.running, 'control._deliver: you cannot call _deliver if animation is running')
          F.once(anim.stopped, this._oncompleted)
          this._onstart()
          anim.start()
        },
        completed(oncompleted = F.noop) {
          console.assert(!this._delivering, 'control.completed: already setted')

          this._oncompleted = oncompleted
          return this
        },
        cancelled(oncancelled = F.noop) {
          console.assert(!this._delivering, 'control.cancelled: already setted')

          this._oncancelled = oncancelled
          return this
        },
        start: control.start,
        cancel: control.cancel
      }

      if (!control._current && !anim.running) {
        control._current = ctl

        Qt.callLater(() => {
          if (control._current && !anim.running)
            control._current._deliver()
        })
        return ctl
      }
      control._pendings[ctl.id] = ctl

      return ctl
    }
  })
}

