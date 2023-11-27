
# AWALE

## compiling

see `make help`

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

- `diffusionChatGlobal[]` : list of pseudos that have access to the global chat
- `diffusionMainMenu[]` : list of pseudos that are in the state main menu
- `diffusionUsersList[]` : list of pseudos that are in the state list of user
- `diffusionGamesList[]` : list of pseudos that are in the state list of in progress games
- `diffusionGames[][]` : list of pseudos that are in the state specific game

## tips

- write on the line before : `\e[1A\e`
- write N lines before : `\e[{N}A\e`
- 




