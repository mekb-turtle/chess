<div align="center">
    <h3 align="center">Simple SDL2 chess program</h3>
    <img alt="Version" src="https://img.shields.io/github/v/release/mekb-turtle/chess?display_name=tag&style=for-the-badge" />
    <img alt="Stars" src="https://img.shields.io/github/stars/mekb-turtle/chess?display_name=tag&style=for-the-badge" />
    <img alt="License" src="https://img.shields.io/github/license/mekb-turtle/chess?style=for-the-badge" />
</div>

---
<br/>

## About The Project
A simple chess program written in C using SDL2 and Nuklear.
It supports playing vs a friend or an AI.

<br />

## Building
Find the following dependencies in your package manager or elsewhere:
- `sdl2`
- `sdl2_image`
- `fontconfig`
- `pandoc` (only needed when building man pages)

```bash
git clone https://github.com/mekb-turtle/chess.git
cd chess
git checkout "$(git describe --tags --abbrev=0)" # checkout to latest tag, omit for latest commit
make
# result will be in build/*/bin/chess
```

