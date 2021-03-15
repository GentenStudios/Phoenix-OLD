# Networking Architecure
This is a generic overview of how the networking system works. If you have additional questions beyond what is
described here, reach out in [#networking](https://discord.gg/6vwjZhC) on Discord. If you are interested in contributing
to the networking system, this is also the place to reach out to coordinate any work you want to do.
## Flow
The data in the networking system is managed in a deterministic lockstep method. This means the server is 100%
authoritative as to what happens. For some background information on deterministic lockstep check out
[this article by GlenFelder (Gaffer On Games) is a good read](https://gafferongames.com/post/deterministic_lockstep/).
We process three types of information during runtime. States are non-critical but time-sensitive pieces of information
that get sent unreliably and are discarded when not received (such as positioning). Events are critical pieces of
information that are sent via Enet's reliable packet system (Such as when a block is broken). Messages are essentially
events but we use a third channel on Enet since they are processed by an entirely separate system.
NOTE: This system's V1 version is a WIP, some information here may describe a future state of the networking system not
yet implemented.
### States
[InputState]s are created by the client every ∆T / game tick (1/20 of a second by default) and queued for transmission.
This happens in a thread so we never run out of time before the next ∆T.
The networking system is then running in another thread `m_iris` watching that queue. It packs states into redundant
packages (of 5 by default) so each packet will include the X most recent states. This helps prevent packet loss from
becoming an issue.
When the server gets packets, the networking thread (`m_iris` again) unpacks the data and fills a new queue system with
any packets it doesn't already have discarding any data it does have or arrived too late. This queue system contains
StateBundles which are a bundle of InputStates, one from each player for that tick (sequence). When either we have an
InputState from each connected player, or we have waited too long (.5 seconds max by default) we mark that bundle ready
for consumption.

A separate game thread on the server then watches that queue for when the networking system has marked that the oldest
stateBundle in the queue is ready. When it is the game thread takes that bundle, as well as any queued events or
messages, and processes them moving the server forward a tick. Once a tick is done processing, the server blasts any
relevant information to clients to clients updating them on the final official state for that tick.

When a client gets information back its usually significantly (up to 2.5 seconds at the extreme by default) later than
it sent that packet. While the client is waiting for that state from the server, it is predicting what is happening
based on velocities, AI logic, or the known player input state. When the client does get the packet for that state, it
checks to see if its predictions up until that point were correct, if they aren't (within a margin of error) then it
re-processes every state from that confirmation re-predicting all future states up to the one the client is currently
on.

### Events
Events are fortunately a lot simpler than States. Events are things such as an inventory movement (client to server) or
a block breaking (server to client). We use events when the player loads a new chunk to avoid sending all loaded
chunks every game-tick (we only update data if it changes).

Events aren't always a circular path, but can be. When an event occurs on the client, lets take inventory movement for
example, we send a message to the server letting the server know it happened. The server then processes that event
determining if that was a valid event, and sends an update event back to the client to confirm that it happened, or a
rejection event back to the client saying it didn't happen. If it happened then the client goes along with its day, if
it didn't actually happen, we roll back that action.

When the client sends the server information, it just sends a packet and the server processes that event on the next
game tick. When the server sends the client an event, it includes the tick number in case the client needs to do any
rollback. This means, the event may happen at a slightly different time on the client as the server (which is okay as
long as the event did actually happen). Not binding the events to ticks on the client side, helps allow the game ticks
to run freely and unbound if the client has the power to do so (of if the client does not have enough power, its okay
for it to lag without doing much thinking).

### Messages
Messages are just like events with a few key differences.
* When processed on the server, they are always sent to the Commander so they don't have header information determining
what system they go to.
* Messages dont have to have a return to the client to ack being received. We trust Enet's reliable packet system to
ensure they are delivered. Its up to the Commander/ the specific command to send the error message back when something
goes wrong.
* Messages from the Server to the Client are purely information and printed to the terminal with little to no client
side processing. If a Message from the client results in an action (EX: /tp) then there is no client side prediction and
the Event system is instead used to relay that the action happened.


[InputState]: @ref phx::InputState

#### </b> {#networking}
