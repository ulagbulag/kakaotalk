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

## Invisible chats or incorrect click positions (채팅창 표시 및 클릭 위치 복구)

If chats open as invisible or tiny windows, or clicks land in the wrong place,
Wine may be holding stale monitor information. Closing and reopening KakaoTalk
alone can reuse that Wine session. Save any work in applications using
KakaoTalk's Wine prefix, then run:

채팅창이 보이지 않거나 아주 작게 열리고, 클릭 위치도 어긋난다면 Wine에 잘못된
모니터 정보가 남아 있을 수 있습니다. 카카오톡만 다시 실행해도 같은 Wine 세션을
재사용할 수 있습니다. 카카오톡 전용 Wine 환경에서 작업 중인 내용을 저장한 뒤 실행하세요.

```bash
kakaotalk --recover-display
```

This cleanly shuts down all applications in `~/.local/share/kakaotalk`, waits
for its Wine server to exit, and opens KakaoTalk again with fresh display
information. Chat data and settings are preserved. If shutdown fails or takes
too long, the command stops; close any pending Wine dialogs and retry.
Other Wine prefixes are unaffected. Normal launches do not restart the session.

`~/.local/share/kakaotalk`에 속한 프로그램을 정상 종료하고 Wine 서버가 끝날 때까지
기다린 뒤, 화면 정보를 새로 읽어 카카오톡을 실행합니다. 대화 데이터와 설정은
유지됩니다. 종료에 실패하거나 시간이 초과되면 중단하므로, Wine의 확인 창이
남아 있다면 처리한 뒤 다시 실행하세요. 다른 Wine 환경에는 영향을 주지 않으며,
평소 실행 시에는 세션을 재시작하지 않습니다.

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
