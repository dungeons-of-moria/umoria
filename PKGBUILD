pkgname=umoria
pkgver=5.7.15
pkgrel=1
pkgdesc="The Dungeons of Moria is a single player dungeon simulation originally written by Robert Alan Koeneke, with its first public release in 1983. The game was originally developed using VMS Pascal before being ported to the C language by James E. Wilson in 1988, and released as Umoria."
arch=('x86_64')
url="https://umoria.org/"
license=('GPL-3.0-only')
depends=('ncurses' 'glibc' 'gcc-libs' 'bash')
makedepends=('gcc' 'cmake' 'make')
source=("https://github.com/dungeons-of-moria/umoria/archive/refs/tags/v${pkgver}.zip")
sha256sums=('SKIP')

build() {
  cd "$srcdir/$pkgname-$pkgver"
  mkdir ./build
  cd ./build
  cmake ..
  make -j
}

package() {
  cd "$srcdir/$pkgname-$pkgver"

  mkdir -p "$pkgdir/usr/bin"
  install -m755 ./build/umoria/umoria "$pkgdir/usr/bin/umoria.bin"

  mkdir -p "$pkgdir/usr/share/umoria"
  cp -R ./build/umoria/data "$pkgdir/usr/share/umoria/"

  mkdir -p "$pkgdir/usr/share/umoria/historical"
  cp ./historical/* "$pkgdir/usr/share/umoria/historical"

  mkdir -p "$pkgdir/usr/share/licenses/umoria"
  cp LICENSE "$pkgdir/usr/share/licenses/umoria/"

  cat << EOF > "$pkgdir/usr/bin/umoria"
#!/bin/sh
CONFDIR=~/.config/umoria
DATADIR=/usr/share/umoria/data
BIN=/usr/bin/umoria.bin
[ ! -d \$CONFDIR ] && mkdir -p \$CONFDIR && ln -s \$DATADIR \$CONFDIR/data
[ ! -f \$CONFDIR/scores.dat ] && touch \$CONFDIR/scores.dat
(cd \$CONFDIR; \$BIN; \$@)
EOF

  chmod +x "$pkgdir/usr/bin/umoria"
}

