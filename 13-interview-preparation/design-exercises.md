# Interview design exercises — worked approaches

## 1. Many idle clients, occasional expensive requests

**Prompt:** design a framed service with many idle connections and bursts of
CPU-heavy work.

**Worked approach:** the readiness loop owns sockets and parsing/output queues.
A bounded worker pool owns CPU jobs. Jobs carry request/connection-generation
identifiers rather than permission to write arbitrary descriptors. Workers post
completion back to the loop. The loop verifies the connection is still live,
queues the response and enables write interest. Bound accepted peers, input
lengths, queued jobs and output bytes. Apply deadlines and overload responses
whose own write path is bounded.

**Follow-up:** why not let all workers send directly? It creates socket-write
ordering, lifetime and framing races unless a carefully designed ownership and
serialization policy is added.

## 2. File upload with a lost completion response

**Prompt:** the sender disconnects before seeing success, but the receiver may
have finished writing.

**Worked approach:** define a transfer ID, expected size/digest and an atomic
commit boundary. The server can answer a status query for that ID. A retry with
the same ID should return the same outcome or continue the defined transfer,
not silently overwrite unrelated data. Decide durability scope and how long
status records are retained. The supplied one-shot project only refuses overwrite
and demonstrates acknowledgement ambiguity; it does not implement this design.

**Follow-up:** why does TCP reliability not solve the ambiguity? Bytes delivered
to a kernel do not reveal whether an application transaction committed before
failure or whether the acknowledgement reached the client.

## 3. Broadcast chat with one slow reader

**Prompt:** one participant reads once per minute while others type continuously.

**Worked approach:** retain bounded per-peer output queues. Choose a policy:
drop slow peers, drop old messages with explicit gap notification, or use a
separate replay mechanism. Blocking every sender on the slowest recipient may
violate the product's responsiveness requirement. Track pressure metrics and
make policy visible to users. The supplied chat drops a peer whose queue would
overflow and keeps others progressing.

**Follow-up:** how do you test it? Fill one reader's path without draining it,
continue traffic between normal peers, assert their progress and assert the
specified queue/close behavior. Repeat with fragmented input and simultaneous
joins/leaves; do not rely only on one happy-path demo.

## Evaluation rubric

A strong answer states assumptions, wire format, ownership, bounded resources,
failure semantics and tests. A weaker answer names a fashionable API or claims a
connection count without explaining CPU work, memory or overload behavior.
