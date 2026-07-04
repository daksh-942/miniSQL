# MiniSQL

A miniature **disk-based database engine in C++** built to understand how real databases store, index, and retrieve data internally.

MiniSQL simulates the core ideas of a real storage engine:

- **Fixed-size pages** on disk
- A **Pager** that reads/writes pages
- **Slotted Pages** for variable-length record storage
- A **disk-backed B+ Tree** for efficient lookup
- A simple **CLI** for interacting with the engine

It is intentionally designed as a learning project, but the architecture mirrors the way real database systems separate responsibilities between storage, indexing, and query execution.

---

## Why this project exists

The goal of MiniSQL is to answer a few core database questions:

- How does a database store rows on disk?
- Why do databases use pages instead of raw file offsets?
- Why are slotted pages useful for variable-length records?
- Why are B+ Trees preferred for indexing?
- How does a disk-based index survive restarts?
- How does a user command travel from CLI to disk?

MiniSQL is a hands-on implementation of those ideas.

---

## High-level idea

A hard disk does not work like RAM. Data is accessed in **blocks/pages**, not as individual objects. MiniSQL models that idea directly:

- A database file is treated as an array of fixed-size **pages**
- Each page has a **page ID**
- Records inside a page are identified by **slot IDs**
- The B+ Tree stores **(page_id, slot_id)** references instead of raw memory addresses
- The Pager translates page IDs into byte offsets using:

```cpp
offset = page_id * PAGE_SIZE;
```

---

## Architecture

```text
                    User
                     |
                     v
                    CLI
                     |
                     v
             Command Parsing
                     |
                     v
              Table / Engine
               /           \
              /             \
             v               v
      Slotted Page        Disk B+ Tree
             \               /
              \             /
               v           v
                   Pager
                     |
                     v
              Database File
```

### Storage layout

```text
Database File
+---------+---------+---------+---------+---------+
| Page 0  | Page 1  | Page 2  | Page 3  | Page 4  |
+---------+---------+---------+---------+---------+
```

Each page is fixed-size:

```text
PAGE_SIZE = 4096 bytes
```

---

## Core modules

### 1) Pager
The Pager is the lowest storage layer. It provides:

- page allocation
- page reading
- page writing
- page count tracking
- file persistence

The Pager knows nothing about tables, rows, SQL, or indexes. It only manages raw pages.

### 2) Slotted Page
The Slotted Page stores records inside a page. It supports:

- variable-length record insertion
- record lookup by slot ID
- lazy deletion
- page-level space management

This layer keeps record references stable even if physical bytes move during compaction.

### 3) B+ Tree
The disk-backed B+ Tree provides:

- O(log n) lookup
- efficient insertions
- node splits
- linked leaf traversal for range scans
- persistence across restarts

Each B+ Tree node is stored in a Pager-managed page.

### 4) Table layer
The Table layer connects the B+ Tree, Slotted Page, and Pager to implement row-level operations such as:

- insert
- get
- update
- delete
- scan
- print
- count
- clear

### 5) CLI
The CLI is the user-facing shell. It parses commands like:

- `insert <id> <name>`
- `get <id>`
- `delete <id>`
- `update <id> <name>`
- `scan <l> <r>`
- `print`
- `count`
- `exists <id>`
- `clear`
- `help`
- `exit`

---

## Main design decisions

### Fixed-size pages
MiniSQL uses 4KB pages because this matches the common granularity used by operating systems and database storage engines.

### Page IDs instead of raw offsets
A page ID is stable and easy to recover after restart. The Pager converts page IDs into file offsets.

### Slot IDs instead of raw record offsets
Inside a page, records may move during compaction. Slot IDs remain stable, so indexes do not break.

### B+ Tree instead of a simple binary tree
A B+ Tree keeps height small, minimizes disk reads, and supports range scans through linked leaf nodes.

### Disk-backed nodes
B+ Tree nodes are pages, not heap objects. This makes the index persistent.

### Lazy deletion
Deletion marks a slot as deleted instead of immediately moving memory around. This keeps deletion fast and avoids invalidating references.

---

## Folder structure

```text
MiniSQL/
├── btree/
│   ├── btree_page.h
│   ├── btree_page.cpp
│   ├── diskBtree.h
│   └── diskBtree.cpp
├── cli/
│   ├── cli.h
│   └── cli.cpp
├── page/
│   ├── slotted_page.h
│   └── slotted_page.cpp
├── storage/
│   ├── pager.h
│   └── pager.cpp
├── table/
│   ├── table.h
│   └── table.cpp
├── main.cpp
└── README.md
```

---

## File-by-file overview

### `storage/pager.h` / `storage/pager.cpp`
Implements the persistent page manager.

Responsibilities:
- open/create database file
- maintain page count
- allocate new pages
- read a page by page ID
- write a page by page ID
- validate file format

Key idea:

```cpp
byte_offset = page_id * PAGE_SIZE
```

### `page/slotted_page.h` / `page/slotted_page.cpp`
Implements record storage inside a page.

Responsibilities:
- initialize page header
- insert records into free space
- read records by slot ID
- mark records deleted
- manage slot directory metadata

### `btree/btree_page.h` / `btree/btree_page.cpp`
A thin wrapper over raw page bytes for B+ Tree node interpretation.

Responsibilities:
- read/write node header
- read/write keys
- read/write children
- read/write leaf pointers
- store parent and next-leaf IDs

### `btree/diskBtree.h` / `btree/diskBtree.cpp`
Implements disk-based B+ Tree logic.

Responsibilities:
- search
- insert
- split leaf nodes
- split internal nodes
- update parent nodes
- handle root splits
- persist root page metadata

### `table/table.h` / `table.cpp`
Bridges user operations to the storage and index layers.

Responsibilities:
- insert row
- get row
- update row
- delete row
- scan range
- count rows
- print all rows
- clear database content

### `cli/cli.h` / `cli.cpp`
Implements the interactive shell.

Responsibilities:
- read user commands
- parse input
- validate syntax
- call table operations
- print results and errors

---

## Data model

MiniSQL stores rows as:

```text
(key, record)
```

Within storage, the record is represented through:

- `page_id`
- `slot_id`

The B+ Tree maps a key to a physical location:

```text
key -> (page_id, slot_id)
```

That location is then used by the Slotted Page layer to retrieve the actual record bytes.

---

## How storage works

### Page-level view

A page is a fixed-size block of 4096 bytes.

### Slotted page layout

```text
+---------------------------------------------------+
| Header | Slot Directory | Free Space | Record Data |
+---------------------------------------------------+
```

- The slot directory grows forward from the beginning
- Records grow backward from the end
- Free space stays in the middle

This makes insertions efficient and keeps slot IDs stable.

### Slot directory entry

Each slot stores:

- `is_deleted`
- `length`
- `offset`

The slot ID is the logical record identity. The offset is only the physical location of bytes inside the page.

---

## Lazy deletion

MiniSQL uses lazy deletion in the slotted page layer.

Instead of shifting records immediately when a row is deleted, the slot is marked as deleted. This keeps deletion O(1) and avoids invalidating index references.

Benefits:

- fast deletion
- stable slot IDs
- less page rewriting
- index safety

Later, a compaction step can reclaim fragmented space if needed.

---

## B+ Tree layout

Each node is stored in one page.

### Node header
A B+ Tree page begins with metadata such as:

- `is_leaf`
- `key_count`
- `parent_page`
- `next_leaf_page` (for leaf nodes)

### Leaf node
Leaf nodes store:

- keys
- values `(page_id, slot_id)`

### Internal node
Internal nodes store:

- keys
- child page IDs

---

## Why B+ Tree?

B+ Trees are used in databases because they are excellent for disk-based indexing:

- they keep the tree balanced
- they minimize height
- they reduce disk I/O
- they support range queries through linked leaves
- they are efficient for large datasets

Compared with a binary tree, a B+ Tree stores many keys per node, which means fewer levels and fewer page reads.

---

## Query flow

### INSERT

```text
insert <id> <name>
     |
     v
CLI parses command
     |
     v
Table creates row
     |
     v
Slotted Page stores record
     |
     v
B+ Tree stores key -> (page_id, slot_id)
     |
     v
Pager writes pages to disk
```

### GET

```text
get <id>
   |
   v
CLI parses command
   |
   v
Table asks B+ Tree for location
   |
   v
B+ Tree returns (page_id, slot_id)
   |
   v
Pager reads the page
   |
   v
Slotted Page reads the slot
   |
   v
Record is printed
```

### DELETE

```text
delete <id>
    |
    v
CLI parses command
    |
    v
Table locates row
    |
    v
Lazy delete marks slot as deleted
    |
    v
B+ Tree / page state updated
```

### UPDATE

```text
update <id> <name>
      |
      v
Locate row
      |
      v
Replace record content
      |
      v
Persist updated page
```

### RANGE SCAN

```text
scan <l> <r>
      |
      v
Traverse B+ Tree to first matching leaf
      |
      v
Follow linked leaf pages
      |
      v
Read consecutive keys in sorted order
```

---

## CLI usage

After launching the program, you will see the prompt:

```text
db>
```

### Available commands

```text
insert <id> <name>   : Insert row
get <id>             : Get row
delete <id>          : Delete row
update <id> <name>   : Update row
scan <l> <r>         : Range scan
print                : Print table
count                : Count rows
exists <id>          : Check if exists
clear                : Reset table
help                 : Show commands
exit                 : Exit program
```

### Example session

```text
db> insert 1 Alice
Inserted

db> insert 2 Bob
Inserted

db> get 1
1 -> Alice

db> exists 2
Yes

db> update 2 Bobby
Updated

db> scan 1 5
1 -> Alice
2 -> Bobby

db> count
Total rows: 2

db> delete 1
Deleted

db> get 1
Not found

db> help
... command list ...

db> exit
Bye!
```

---

## Build instructions

### Prerequisites

- C++ compiler with C++17 support
- `g++` or equivalent
- Terminal / shell environment

### Compile

```bash
g++ main.cpp btree/*.cpp page/*.cpp storage/*.cpp table/*.cpp cli/*.cpp -o minisql
```

If you are on Windows with MinGW:

```bash
g++ main.cpp btree/*.cpp page/*.cpp storage/*.cpp table/*.cpp cli/*.cpp -o minisql.exe
```

### Run

Linux / macOS:

```bash
./minisql
```

Windows:

```bash
minisql.exe
```

---

## What happens when the program starts

1. The CLI creates a `Pager`
2. The `Pager` opens or creates the database file
3. The `Table` layer initializes on top of the Pager
4. The B+ Tree root page is loaded or created
5. The CLI prompt appears
6. Commands are accepted interactively

---

## Example internal lifecycle

### First run
If the database file is empty:

- the Pager allocates a root page
- a metadata page is created
- the root page ID is stored in metadata

### Later runs
If the database already exists:

- the metadata page is read
- the root page ID is recovered
- the B+ Tree continues from the previous state

This makes the index persistent across restarts.

---

## Complexity

| Operation | Complexity |
|----------|------------|
| Page read | O(1) |
| Page write | O(1) |
| Page allocation | O(1) |
| Slotted-page insert | O(1) |
| Slotted-page read | O(1) |
| Slotted-page delete | O(1) |
| B+ Tree search | O(log n) |
| B+ Tree insert | O(log n) |
| Range scan | O(log n + k) |

Where `k` is the number of matching records.

---

## Important implementation ideas

### Pager invariants
- page size is fixed
- file size must remain a multiple of page size
- page IDs are stable
- file offset is derived from page ID

### Slotted page invariants
- slot IDs remain stable
- offsets may change during compaction
- deleted records are marked, not immediately removed
- free space remains between slot directory and record region

### B+ Tree invariants
- keys are sorted
- all leaves remain at the same depth
- internal nodes have `children = keys + 1`
- leaf nodes are linked for range scans
- root page must be recoverable from metadata

---

## Common interview explanation

If someone asks what this project is, say:

> I built a MiniSQL database engine in C++ to understand how real databases work internally. I simulated disk storage using fixed-size pages, built a Pager to manage those pages, used slotted pages to store variable-length records, and implemented a disk-backed B+ Tree for efficient indexing and persistence. I also added a CLI so users can interact with the engine using commands like insert, get, update, delete, and scan.

---

## Interview talking points

### Why not store everything in RAM?
Because a real database must persist data across restarts and scale beyond memory.

### Why use page IDs?
Because page IDs are stable, persistent, and easy to map to disk offsets.

### Why not store raw offsets in the index?
Because raw offsets can become invalid when records move during compaction.

### Why B+ Tree instead of binary search tree?
Because B+ Trees reduce height, reduce disk I/O, and support range queries.

### Why use lazy deletion?
Because it keeps deletion fast and avoids expensive page rewrites.

### Why is the Pager separate from the B+ Tree?
Because each layer should have one responsibility: file I/O, record layout, and indexing are different concerns.

---

## Future improvements

Possible extensions to MiniSQL:

- Transaction support
- Write-Ahead Logging (WAL)
- Crash recovery
- MVCC
- Lock manager
- Buffer pool manager
- Free-list page reuse
- Page compaction
- Query optimizer
- Cost-based optimizer
- Secondary indexes
- Joins
- Distributed storage
- Replication
- Sharding

---

## Learning outcomes

This project helped build a practical understanding of:

- storage engine design
- disk I/O
- page-based memory organization
- stable record references
- indexing strategies
- B+ Tree balancing
- database persistence
- query flow through layers
- how a database survives restart

---

## References

- Database System Concepts
- SQLite architecture
- PostgreSQL storage and indexing ideas
- Standard database systems design notes

---

## License

This project is for learning and educational use.
