# learnuv

Learn uv for fun and profit, a self-guided workshop to [the library](https://github.com/libuv/libuv) that powers Node.js.

![screenshot](assets/screenshot.png)

## Before You Start

Read the [learnuv gitbook](http://thlorenz.github.io/learnuv/book) which explains some libuv and related C language
concepts that will help you complete the exercises.

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
