
# AWALE

## compiling

see `make help` for more informations

- compile only server in debug `make server`
- compile only client in debug `make client`
- compile client and server `make final`

## main menu

() : actions
else : display HMI

1. (Play) -> user list -> (choice) -> [create game] -> during game
2. (Observer) -> game list -> (choice) -> during game
0. (Quit)

## during game

if player :
- display game -> (move|give up|∅) -> display game -> ...

if observer :
- display game -> (quit|∅) -> display game -> ...

## user list

1. {user 1}
2. {user 2}
...

## game list

1. {user A1} vs {user B1}
2. {user A2} vs {user B2}
...

## commands

- `/help` : list possible commands
- `/c {message}` : send {message} to global chat
- `/q` : quit game or disconect
- `/y {pseudo}` : accept {pseudo}'s game request
- `/n {pseudo}` : decline {pseudo}'s game request
- `/abandon` : give up current game

## diffusion lists

list of players that have subscribed to a diffusion -> correspond to a certain state

- `diffusionChatGlobal[]`	: list of pseudos that have access to the global chat				: no state
- `diffusionMainMenu[]`		: list of pseudos that are in the state main menu					: `m` (main menu)
- `diffusionUsersList[]`	: list of pseudos that are in the state list of user				: `u` (user list)
- `diffusionGamesList[]`	: list of pseudos that are in the state list of in progress games	: `g` (game list)
- `diffusionGames[][]`		: list of pseudos that are in the state specific game				: `p` (playing or viewing)

## tips

- write on the line before : `\033[F` (or `\e[1A\e` but work strangely)
- write N lines before : `\e[{N}A\e`
- seg fault : `gdb {executable}` then `run` then `where` when the program stop
- 

## bugs

- when disconnecting send too many disconnected messages
- message received when start typing are not well printed on the terminal
- show server disconected when quitting (and blind() the server for the next time)
- client can't connect with another port
- get the right user from the number of the user list (may change during the time the client type the number)
- idem for the game
- server stop from some user input
- crash when the input for selecting game/user is bad
- crash when spectator enter input
- 

## todo

- send in diffusion list
- main menu
- game in general
- cleaning (warnings)
- handle exceptions 
- 


