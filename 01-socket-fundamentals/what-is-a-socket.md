# Socket, descriptor and port


A port names a transport endpoint component. A socket stores communication state
in the kernel. A descriptor is one process-table entry referring to an open
resource. Closing a descriptor removes that reference; another reference can
keep the underlying resource alive.

This explains two important bugs: a fork parent that forgets to close its peer
copy may delay final release, while a thread that closes another thread's peer
can disrupt that shared connection immediately. The same integer can later be
reused for a different resource, so it is not a durable user identity.

**Experiment:** run socket-info several times. The descriptor may repeat even
though each execution creates a new socket. That repetition does not mean the
old connection survived.
