# AES Block Cipher in CTR/ECB Mode

## Structure
```
.
├── [4.0K]  node
│   ├── [3.9K]  config.py
│   ├── [5.2K]  CustomAES.py
│   ├── [1.5K]  GenericMultiThreadedServer.py
│   ├── [   0]  __init__.py
│   ├── [2.2K]  KeyManager.py
│   ├── [ 463]  logging.py
│   └── [8.5K]  PeerNode.py
├── [  36]  README.md
├── [  10]  requirements.txt
├── [3.7K]  run_node.py
├── [2.1M]  sample-2mb-text-file.txt
├── [2.1M]  sample-2mb-text-file.txt.CTR
├── [2.1M]  sample-2mb-text-file.txt.ECB
├── [3.5K]  sample3.txt
└── [1.9K]  tests.py

1 directory, 15 files

```

## Requirements
-   This project was realized using Python 3.9.7 (default, Aug 31 2021, 13:28:12), but my guess is that anything >= Python 3.6 will be fine.
-   Install the required modules:
```bash
python3 -m pip install -r requirements.txt
```

## How do you run it?
-   Set the required environments variables. Reference: [node/config.py](https://github.com/reloadedd/Faculty/blob/cc82f1c2d1435dbb238872f1d162e329e621de0e/0x3-the-end/Information-Security/Homework-01/node/config.py#L30)
```
Moy4TUu7lR4KU475P7OuwRjBbP3Kkz  # The master key
te5ZHIfoAz0vtw7YHgDAEBwea9dIh2  # The first key (k1)
Eq6foZUj2GuAryGdzBLxTCbbd5mBup  # The second key (k2)
```
For instance, setting the master key in `bash` shell:
```bash
export Moy4TUu7lR4KU475P7OuwRjBbP3Kkz="this_is_my_master_key"
```
*(do the same for the other keys)*
-   After that, start the server aka Key Manager:
```bash
python3 run_node.py --mode server
```
*(take a note of the port, you will need it in order to connect the nodes A and B to the Key Manager)*
-   Start the nodes (you will be asked to choose which node do you want to be -- Node A or Node B):
```bash
python3 run_node.py -m peer --server-ip 127.0.0.1 --server-port <the port which I told you to take note of>
```
-   Then follow the instructions that the program will give you.

## Full Example
-   You will to open 3 terminals: one for the Key Manager, one for the Node A and one for the Node B.
-   **Note:** `#` is a comment, you do not need to copy this into your terminal. 
-   **Note 2:** `...` means that what's in there is the output of the command and you do not need to copy into your terminal.
```bash
# Set the environment variables for the first terminal in order to 
# start the server
export Moy4TUu7lR4KU475P7OuwRjBbP3Kkz="this_is_my_master_key"
export te5ZHIfoAz0vtw7YHgDAEBwea9dIh2="this_is_my_first_key"
export Eq6foZUj2GuAryGdzBLxTCbbd5mBup="this_is_my_second_key"

# Start the server
python3 run_node.py --mode server
...
INFO     Opening port 43401 for communication..
...

# Take note of that port and open another terminal. This will be
# Node B
export Moy4TUu7lR4KU475P7OuwRjBbP3Kkz="this_is_my_master_key"

# Start Node B
python3 run_node.py --mode peer --server-ip 127.0.0.1 43401
...
INFO     Which node do you want to be?
>>> 2 
INFO     You are now the Node B!
...

# Take note of that port and open another terminal. This will be
# Node A
export Moy4TUu7lR4KU475P7OuwRjBbP3Kkz="this_is_my_master_key"

# Start Node A
python3 run_node.py --mode peer --server-ip 127.0.0.1 43401
...
INFO     Which node do you want to be?
>>> 1
INFO     You are now the Node A!
...
# Follow the instructions...
```
