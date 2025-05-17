Treasure Hunt Project — User Guide & Explanation

Overview
This project implements a treasure hunt system with multiple processes and inter-process communication.

Components:
- treasure_manager: Command-line tool to manage hunts and treasures.
- treasure_hub: Interactive interface managing a background monitor process.
- monitor: Background process that processes requests from treasure_hub.
- calculate_score: External program to compute user scores in hunts.

How It Works
- Create hunts and add treasures using treasure_manager. Each hunt is a folder storing treasure data.
- Run treasure_hub to start an interactive shell managing the monitor process.
- Interact with hunts and treasures through commands sent to the monitor.
- Calculate scores per user in hunts using the external calculate_score tool.
- Communication between components uses signals, files, and pipes.

Commands

treasure_manager
---------------

Add treasure to hunt:
$ ./treasure_manager --add HuntID
Follow interactive prompts to input treasure details.

List all treasures in a hunt:
$ ./treasure_manager --list HuntID

View specific treasure details:
$ ./treasure_manager --view HuntID TreasureID

Remove a treasure:
$ ./treasure_manager --remove_treasure HuntID TreasureID

Remove an entire hunt:
$ ./treasure_manager --remove_hunt HuntID


treasure_hub (interactive)
--------------------------

Start the program:
$ ./treasure_hub

At the prompt, use:

start_monitor
    Starts the monitor background process.

list_hunts
    Lists all hunts and number of treasures.

list_treasures HuntID
    Lists treasures in a hunt.

view_treasure HuntID TreasureID
    Shows detailed info of a treasure.

calculate_score HuntID
    Calculates and shows users’ scores in a hunt.

stop_monitor
    Stops the monitor process.

exit
    Exits the hub (only if monitor is stopped).

Important Notes
---------------

- After stopping the monitor, other commands are disabled until it fully exits.
- The monitor communicates with treasure_hub using UNIX signals and pipes for IPC.
- Treasure data is stored in binary files inside hunt directories.
- Scores are calculated by the external calculate_score program called via a pipe.
- This setup demonstrates key systems programming concepts: processes, signals, pipes, and IPC.