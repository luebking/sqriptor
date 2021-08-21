# Maintainer: Thomas Lübking <thomas.luebking at gmail>

pkgname=sqriptor
pkgver=0.1.0
pkgrel=1
pkgdesc="Greatest and best TextEditor in the World. Tribute."
arch=('i686' 'x86_64')
url="https://github.com/luebking/sqriptor"
license=('GPL')

depends=('qt5-base')
makedepends=('gcc')
license=('GPL')

build()
{
    qmake-qt5 .. && make || return 1
}

package()
{
    mkdir -p $pkgdir/usr/bin
    install sqriptor $pkgdir/usr/bin
    strip $pkgdir/usr/bin/sqriptor
}

# vim:set ts=4 sw=4 et:
