# arduino_msgpck
This library targeted at arduino aims at implementing a light weight serializer and parser for messagepack.

## Install
Download the zip, and import it with your Arduino IDE (Sketch>Include Library>Add .zip library).

## Usage
See the either the .h file, or the examples (led_controller and test_uno_writer).

In short:
* functions like `msgpck_what_next(Stream * s);` watch the next type of data without reading it (without advancing the buffer of Stream s).
* functions like `msgpck_read_bool(Stream * s, bool *b)` read a value from Stream s.
* functions like `msgpck_write_bool(Stream * s, bool b)` write a value on Stream s.

Notes:
* Stream are used as much as possible in order not to add to many overhead with buffers. Therefor you should be able to store the minimum number of value at a given time.
* map and array related function concern only their headers.


## Limitations
Currently the library does not support:
* 8 bytes float
* 2^32 char long (or longer) strings
* 2^32 byte long (or longer) bins
* extention types.
