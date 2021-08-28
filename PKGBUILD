# Maintainer: Thomas Lübking <thomas.luebking at gmail>

pkgname=sqriptor
pkgver=r79.d73418a
pkgrel=1
pkgdesc="Greatest and best TextEditor in the World. Tribute."
arch=('i686' 'x86_64')
url="https://github.com/luebking/sqriptor"
license=('GPL')
depends=(qt5-base qscintilla-qt5)
makedepends=('gcc')
license=('GPL')
provides=(sqriptor)
conflicts=(sqriptor-git)

pkgver() {
    cd ..
    printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

build()
{
    qmake-qt5 ..
    make
}

package()
{
    install -Dm755 sqriptor -t "${pkgdir}/usr/bin"
}

# vim:set ts=4 sw=4 et:
