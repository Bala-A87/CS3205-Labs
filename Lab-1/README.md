# Lab 1

## File names for questions
1. Server - UDPechod, Client - UDPecho
2. Server - UDPmathd, Client - UDPmath
3. Server - TCPfiled, Client - TCPfile

## Executing code
### Compiling
`make <filename>`

### Running
- For server: `./<server-filename> <port>`
- For client: `./<client-filename> <server-address> <port>`

### Example for q1
Commands to be run in shell for server:
- `make UDPechod`
- `./UDPechod 9999`

Commands to be run in shell for client:
- `make UDPecho`
- `./UDPecho localhost 9999`

`make clean` can be run after terminating client and server to clean up object files and executables.

## Known limitations/bugs

- Requires filename with extension for q3
