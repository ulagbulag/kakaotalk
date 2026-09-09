# KakaoTalk One-Shot Installer for Linux

This is a simple [`KakaoTalk`](https://www.kakaocorp.com/page/service/service/KakaoTalk) installation script using [`wine`](https://www.winehq.org/).

[`wine`](https://www.winehq.org/)을 활용한 간단한 [카카오톡](https://www.kakaocorp.com/page/service/service/KakaoTalk) 설치 스크립트입니다.

## Quick Start (빠른 설치법)

### Arch Linux (아치리눅스)

```bash
# If you prefer `paru`
paru -S --needed kakaotalk && kakaotalk

# If you prefer `yay`
yay -S --needed kakaotalk && kakaotalk
```

## Dependencies (준비물 패키지)

- bash
- curl
- desktop-file-utils
- grep
- procps
- wine >= 11.0
- winetricks
- xdg-utils

For multimedia playback support, also install the following optional dependencies:  
동영상 재생 등의 기능을 이용하시려면 아래 패키지도 추가로 설치하세요:

- gst-plugins-good
- gst-plugins-bad

### Arch Linux (아치리눅스)

```bash
sudo pacman -S bash curl desktop-file-utils wine xdg-utils
```

### Debian / Ubuntu (데비안 / 우분투)

```bash
sudo apt-get update
sudo apt-get install bash curl desktop-file-utils wine xdg-utils
```

## Installation (설치하기)

### Arch Linux (아치리눅스)

#### Automatic Installation with AUR package manager (AUR 패키지 관리자를 활용한 설치)

```bash
# If you prefer `paru`
paru -S kakaotalk

# If you prefer `yay`
yay -S kakaotalk
```

#### Manual Installation (수동 설치)

```bash
makepkg -scri
```

### Other Distros (우분투 등 일반적인 리눅스 배포판)

#### System-wide Installation (모든 사용자용으로 설치)

```bash
sudo ./install.sh
```

#### User-wide Installation (현재 사용자용으로 설치)

```bash
. ./install.sh
```

## Opening KakaoTalk (카카오톡 열기)

You can init and open KakaoTalk by choosing **one of the methods below**.
After initialization, a shortcut icon will appear on the desktop so you can open it more conveniently.

**아래의 방법 중 하나를 수행**하여 카카오톡을 초기화 및 실행할 수 있습니다.
초기화 후에는 바탕화면에 바로가기 아이콘이 생기므로 더욱 편리하게 실행할 수 있습니다.

### CLI (명령줄 인터페이스를 활용하기)

```bash
kakaotalk
```

### Xfce

Application Finder -> Search KakaoTalk (카카오톡) -> Open

### Mouse input and window focus (마우스 입력과 창 포커스)

The launcher loads a small Wine/X11 helper to keep global raw mouse events
from other applications out of KakaoTalk. This prevents background chat
windows from reacting to clicks, right-clicks, and scrolling in another
window, including focus jumps when switching between multiple applications.
Mouse input over KakaoTalk and ongoing drags are preserved.

다른 앱에서 발생한 클릭·우클릭·휠 입력이 카카오톡 채팅창으로 전달되어
포커스를 빼앗는 문제를 보정합니다. 카카오톡 창 위에서의 마우스 입력과
드래그는 유지됩니다.

The helper applies to Wine's X11 event loop launched by this script. Native
applications, keyboard events, and window-manager activation requests are
unchanged. Existing `LD_PRELOAD` entries are preserved. To disable the helper
for a launch, run `KAKAOTALK_INPUT_FIX=0 kakaotalk`.

For the non-AUR installer, a C compiler and Xlib/XInput development headers are
needed (Debian/Ubuntu: `build-essential libx11-dev libxi-dev`). AUR builds use
`base-devel` and the declared build dependencies.

Run `bash tests/input-guard.sh` to test in an isolated Xvfb display. The tests
exercise real XInput2 mouse events, core events, drags, and an unaffected
native caller. On Arch, install `xorg-server-xvfb`, `libxi`, and `libxtst` to
run them. The tests do not move the pointer on your desktop.

## Uninstallation (삭제법)

### KakaoTalk (카카오톡)

The script below deletes the directory where wine KakaoTalk is installed.

아래 스크립트는 wine 카카오톡이 설치된 디렉토리를 삭제합니다.

```bash
rm -r ~/.local/share/kakaotalk
```

### KakaoTalk Installer (카카오톡 설치 도우미)

TODO: TBD

추후 추가 예정

## LICENSE

`install.sh`, `kakaotalk` 등 레포지토리 내 스크립트에 라이선스를 명시한 파일의 경우 `The Unlicense` 라이선스가 적용됩니다.

그 외의 **모든 파일**들은 해당 레포지토리에서 라이선스를 적용하지 않습니다.

카카오톡 사용자 약관에 관해서는 다음 링크를 참고해주세요: https://www.kakao.com/policy/kakaoTerms?type=s&version=simple&lang=ko
