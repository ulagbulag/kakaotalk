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
- xrdb

For multimedia playback support, also install the following optional dependencies:  
동영상 재생 등의 기능을 이용하시려면 아래 패키지도 추가로 설치하세요:

- gst-plugins-good
- gst-plugins-bad

### Arch Linux (아치리눅스)

```bash
sudo pacman -S bash curl desktop-file-utils wine xdg-utils xorg-xrdb
```

### Debian / Ubuntu (데비안 / 우분투)

```bash
sudo apt-get update
sudo apt-get install bash curl desktop-file-utils wine xdg-utils x11-xserver-utils
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

## Display Scaling (화면 배율)

`kakaotalk` detects the display scale on every launch and applies it as the Wine DPI.
With the default Wine X11 driver (also on XWayland) it follows the `Xft.dpi` X resource, which is read with `xrdb` (or `xprop` as a fallback).
With the Wine Wayland driver it follows the compositor scale (GNOME, KDE Plasma, Hyprland, Sway).
You can override it with environment variables:

`kakaotalk`은 실행할 때마다 화면 배율을 감지해 Wine DPI로 적용합니다.
기본 Wine X11 드라이버(XWayland 포함)에서는 `xrdb`(없으면 `xprop`)로 읽은 `Xft.dpi` 값을, Wine Wayland 드라이버에서는 컴포지터 배율(GNOME, KDE Plasma, Hyprland, Sway)을 따릅니다.
아래 환경 변수로 직접 조절할 수 있습니다:

```bash
# Use a fixed DPI (96 = 100%, 120 = 125%, 144 = 150%, 192 = 200%)
KAKAOTALK_DPI=144 kakaotalk

# Scale the detected DPI (e.g. 10% larger)
KAKAOTALK_DPI_SCALE=1.1 kakaotalk

# Keep the DPI configured with `winecfg`
KAKAOTALK_DPI=off kakaotalk
```

When the DPI changes, the saved main window size is reset so that KakaoTalk does not open larger than the screen.

DPI가 바뀌면 카카오톡 창이 화면보다 크게 열리지 않도록 저장된 메인 창 크기를 초기화합니다.

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
