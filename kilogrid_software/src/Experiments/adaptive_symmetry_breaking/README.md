# Some notes 
There are 3 options
- option 1 = red
- option 2 = green
- option 3 = blue


### Structure of config file
The structure of the config file (and the setup).

```
x position of the module
y position of the module
role of the module (in my case wall or normal cell)
cell colour - cell 1
cell colour - cell 2
cell colour - cell 3
cell colour - cell 4
```

### Messages 
There are different message types.
- 1 - cell sending its coordinates, role and option

```
x position of the cell
y position of the cell
role of the cell (wall=42 or normal cell=0)
option (1,2,3 or 0 if wall)
```

- 2 - 


