# bqueue library
In-memory circular buffer for binary strings. It supports multi-threading.

## Requirements

- Clang
- GNU Make

## Build and install

```sh
make
sudo make install
```

### Build in debug mode

```sh
make MODE=debug
```

### Build shared library

```sh
make SHARED=yes
```

### Build with a sanitizer

```sh
make MODE=debug SANITIZE=address
```

### Uninstall

```sh
sudo make uninstall
```

## Testing

### Threading test

```sh
make DEBUG=yes
cd test
make
< /var/log/syslog ./test_threads
```

### Fuzzing test

```sh
make DEBUG=yes
cd test
make
./test_fuzzer > /dev/null
```
