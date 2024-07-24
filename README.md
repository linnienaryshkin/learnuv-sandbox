# learnuv

This is a fork, where I did my playtime. Make sure to check the original repo [thlorenz/learnuv](https://github.com/thlorenz/learnuv)

## Before You Start

1. C Introduction - https://www.w3schools.com/c/c_intro.php
2. Pointers - https://www.w3schools.com/c/c_pointers.php
3. Address-Of / Dereference
4. Struct - https://www.w3schools.com/c/c_structs.php
5. Automatic Allocation vs Dynamic Allocation (malloc)
6. NULL
7. Typecast - https://www.tutorialspoint.com/cprogramming/c_type_casting.htm
8. LibUV guidebook (5 mins reading) - https://thlorenz.com/learnuv/book/

And of course a magnificent [LibUV Guidebook](https://docs.libuv.org/en/v1.x/guide/introduction.html)

## Setup for `macOS 14.5 23F79`

```sh
npm install
```

```sh
pyenv install 2.7 &&
pyenv local 2.7.18
```

```sh
pip install six # https://github.com/thlorenz/learnuv/issues/45
```

```sh
eval "$(pyenv init -)" # for some reason it's not working from ~/.zshrc
```

```sh
./learnuv
```

```sh
./learnuv xcode
```

```sh
open learnuv.xcodeproj
```

```sh
./learnuv verify
```

## License

MIT
