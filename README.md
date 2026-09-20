# fcitx5-vi-cooperative

[English](README.en.md) / 日本語

fcitx5-vi-cooperativeは、fcitx5上でuimの「vi協調モード」と同等の挙動を実現するアドオンです。

## uimのvi協調モードについて

uimにはviのインサートモードからノーマルモードへの遷移をサポートする機能として「vi協調モード (vi-cooperative mode)」という設定があります。これはviに限定せず、IMEがONで未確定文字がない時にESC (または Ctrl+[) を入力するとIMEがOFFになるというものです。

1. 未確定文字がある状態でESCを押下 → 未確定文字がキャンセルされIMEはONのまま (mozc側の動作)
2. 未確定文字がない状態でESCを押下 → IMEをOFF (直接入力) に切り替え、ESCキー自体は入力対象 (vi) にも渡される

本アドオンはfcitx5の `EventWatcherPhase::PostInputMethod` を利用し、入力メソッドのキー処理が終わった後に判定を行い、uimの挙動を再現しています。

uimではこの機能は各入力メソッドで個別に同じ仕様で実装されています。本アドオンでは、fcitx5のAPIの `InputPanel::empty()` や `KeyEvent::filtered()` を使用し、特定の入力メソッドに依存していません。したがって現在の入力メソッドが直接入力 ( `keyboard-` で始まるレイアウト) で、OFFにするものが何もない状態でない限り動作するようになっています。そのためmozc以外の入力メソッドでも動作するはずです。しかし **動作確認はmozcとの組み合わせでのみ行っています** 。他の入力メソッドでの動作は個別にサポートしません。

## 動作要件

Gentoo Linuxの以下の環境で動作確認しました。

- app-i18n/fcitx-5.1.21
- sys-devel/gcc-15.3.0
- app-i18n/mozc-3.33.6133
- dev-build/make-4.4.1-r102
- dev-util/pkgconf-3.0.7

## 必要なパッケージ

### Gentoo

```
emerge --ask app-i18n/fcitx
USE="fcitx5" emerge --ask app-i18n/mozc
```

### Ubuntu

```
sudo apt install fcitx5 fcitx5-mozc libfcitx5core-dev build-essential pkg-config
```

## ビルド方法

```
make
```

## インストール方法

### ホームディレクトリ

```
make install
```

以下の2箇所にインストールされます。

| ファイル | 配置先 |
|---|---|
| `libvicooperative.so` | `~/.local/lib/fcitx5/` |
| `vicooperative.conf` | `~/.local/share/fcitx5/addon/` |

`~/.local/lib/fcitx5/` はfcitx5が自動的に検索しないので環境変数を設定する必要があります。 (後述)

### システム

```
sudo make install-system
```

`pkg-config --variable=libdir Fcitx5Core` と `pkg-config --variable=prefix Fcitx5Core` が返すディレクトリを元にそれぞれインストールされます。

| ファイル | 配置先 (Gentoo) |
|---|---|
| `libvicooperative.so` | `/usr/lib64/fcitx5/` |
| `vicooperative.conf` | `/usr/share/fcitx5/addon/` |

fcitx5標準のディレクトリにインストールされるのでfcitx5を再起動すれば認識され有効になります。


## 起動

`make install` でホームディレクトリ以下にインストールした場合、fcitx5は自動的に検索しないので環境変数を設定する必要があります。 `make install-system` でシステムにインストールした場合はこの手順は不要です。既に起動しているfcitx5のトレイアイコンを右クリックして再起動してください。

fcitx5ではアドオンの検索先を `FCITX_ADDON_DIRS` 環境変数で指定することができます。この変数を設定すると、fcitx5は指定されたディレクトリのみを検索対象とします。そのためホームディレクトリ以下にインストールした場合は、インストールしたディレクトリとシステムのディレクトリの両方をこの環境変数に指定する必要があります。

ホームディレクトリとシステムのディレクトリ両方を含む環境変数は以下のように設定し、fcitx5を再起動します。

```
$ pkg-config --variable=libdir Fcitx5Core
/usr/lib64

$ export FCITX_ADDON_DIRS="$HOME/.local/lib/fcitx5:/usr/lib64/fcitx5"

$ fcitx5 -r -d #もしくはトレイアイコンから再起動
```

次回起動時からも有効にする場合はこの環境変数をfcitx5が起動される前に読み込まれる場所 ( `~/.xprofile` 等) で設定してください。

## トラブルシューティング

- `Could not locate library libvicooperative.so ...` と出る → `FCITX_ADDON_DIRS` の設定漏れ、またはパスの誤り。
- アドオンはロードされるがESCを押しても何も起きない → 現在の入力メソッドが `keyboard-`で始まっていない ( `fcitx5-remote -n` で確認)
