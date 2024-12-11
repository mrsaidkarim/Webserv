# kqueue

## filter

| filter          | description              |
| --------------- | ------------------------ |
| EVFILT_READ     | on read                  |
| EVFILT_WRITE    | on write                 |
| EVFILT_AIO      | attached to aio requests |
| EVFILT_VNODE    | attached to vnodes       |
| EVFILT_PROC     | attached to struct proc  |
| EVFILT_SIGNAL   | attached to signal       |
| EVFILT_TIMER    | timers                   |
| EVFILT_MACHPORT | Mach portsets            |

## flags

| flags      | description                                |
| ---------- | ------------------------------------------ |
| EV_ADD     | add event to kq (implies enable)           |
| EV_DELETE  | delete event from kq                       |
| EV_ENABLE  | enable event                               |
| EV_DISABLE | disable event (not reported)               |
| EV_ONESHOT | only report one occurrence                 |
| EV_CLEAR   | clear event state after reporting          |
| EV_RECEIPT | force EV_ERROR on success, data == 0       |
| EV_ERROR   | error, data contains errno(returned value) |
| EV_EOF     | EOF detected(returned value)               |

# fflags

| fflags          | description                                              |
| --------------- | -------------------------------------------------------- |
| NOTE_DELETE     | vnode was removed                                        |
| NOTE_WRITE      | data contents changed                                    |
| NOTE_EXTEND     | size increased                                           |
| NOTE_ATTRIB     | attributes changed                                       |
| NOTE_LINK       | link count changed                                       |
| NOTE_RENAME     | vnode was renamed                                        |
| NOTE_REVOKE     | vnode access was revoked                                 |
| NOTE_EXIT       | process exited                                           |
| NOTE_EXITSTATUS | exit status to be returned, valid for child process only |
| NOTE_FORK       | process forked                                           |
| NOTE_EXEC       | process exec'd                                           |
| NOTE_SIGNAL     | shared with EVFILT_SIGNAL                                |
| NOTE_REAP       | process reaped                                           |

1- srouce for Kqueue: https://people.freebsd.org/~jlemon/papers/kqueue_freenix.pdf
