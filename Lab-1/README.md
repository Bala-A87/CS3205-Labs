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

## Assumptions

- Operands for q2 lie within 32-bit signed integer range, and operation will be one of the 4 operations mentioned in the question
- Filename for q3 is provided with extension, and the 1 to be appended to the new filename is appended to the whole name, including extension
- Content read from file in q3 is not a prefix of SORRY! and SORRY! is not a prefix of the content read

## Known bugs
None - programs work correctly to the best of my knowledge.

## Credits

- Placeholder text for sample files for q3 generated using [Lorem Ipsum](https://loremipsum.io/)
- C template code used as suggested by instructor, based on Douglas Comer's [Internetworking with TCP/IP](https://books.google.co.in/books/about/Internetworking_with_TCP_IP_Principles_p.html?id=jonyuTASbWAC&source=kp_book_description&redir_esc=y)