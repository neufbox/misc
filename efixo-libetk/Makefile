# 
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 10749 2008-04-07 00:24:19Z libnbu $

include $(TOPDIR)/rules.mk

PKG_NAME:=efixo-libetk
PKG_RELEASE:=1
PKG_INSTALL:=1

PKG_FIXUP:=libtool

PKG_BUILD_DEPENDS:=+libinklevel +libtropicssl

include $(INCLUDE_DIR)/package.mk

define Package/efixo-libetk
  TITLE:=Efixo ToolKit
  SECTION:=efixo
  CATEGORY:=Efixo applications
  MAINTAINER:=Miguel Gaio
endef

define Package/efixo-libetk/description
 This package contains useful functions for any efixo application
endef

define Build/Prepare
	$(CP) src/* $(PKG_BUILD_DIR)
	(cd $(PKG_BUILD_DIR); \
		touch NEWS README AUTHORS ChangeLog && \
		aclocal && \
		autoheader && \
		libtoolize --force && \
		autoconf && \
		automake --add-missing)
	$(call Build/Configure/Default)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libetk.a $(1)/usr/lib
endef

define Package/efixo-libetk/install
	true
endef

$(eval $(call BuildPackage,efixo-libetk))
