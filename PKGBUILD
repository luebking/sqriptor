# Maintainer: Thomas Lübking <thomas.luebking at gmail>

pkgname=sqriptor
pkgver=r147.5b1a194
pkgrel=1
pkgdesc="Greatest and best TextEditor in the World. Tribute."
arch=('i686' 'x86_64')
url="https://github.com/luebking/sqriptor"
license=('GPL')
depends=(qt6-base qscintilla-qt6)
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
    qmake6 ..
    make
}

package()
{
    install -Dm755 sqriptor -t "${pkgdir}/usr/bin"
    install -Dm644 "../sqriptor.svg" -t "${pkgdir}/usr/share/icons/hicolor/scalable/apps"
    install -Dm644 "../sqriptor256.png" "${pkgdir}/usr/share/pixmaps/sqriptor.png"
    install -Dm644 "../sqriptor.desktop" -t "${pkgdir}/usr/share/applications"
}

# vim:set ts=4 sw=4 et:
