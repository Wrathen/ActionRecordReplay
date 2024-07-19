# ActionRecordReplay
Global action recorder&amp;replayer. Listens to your windows keyboard input, records it by time, replays it back.

<h1>Info:</h1>

- **Windows only** code.
- Can record W-A-S-D-Q-E-Tab-Space keys without needing the focus into the process. (Easily extendable)
- Can playback the actions with their recorded times, in sequence.
- You can extend the code to also record mouse clicks & positions.

- I am trying out different code styles nowadays for fun, so don't mind it.


**Recorded data looks like this:**
![image](https://github.com/user-attachments/assets/4c73cb5a-774f-4523-8b97-34caef0a78a7)


<h1>Usage:</h1>

**To record:**
- Run the code/program. (Come on its just one cpp file, you can run it)
- By default it is on Recording Mode.
- After 5 seconds, it will start recording.
- Do whatever you'd like to be repeated.
- Close the program when you no longer need any more action recordings.




**To replay:**
- Go to the source code, change the part where 
  - `#define is_recording 1`
  - change it to 0 instead to switch to replaying mode.
- Start the program again.
- After 5 seconds, it will start replaying back whatever you did in the recording.

- Cheers!
