# 
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 10749 2008-04-07 00:24:19Z libnbu $

include $(TOPDIR)/rules.mk

PKG_NAME:=efixo-libnbu
PKG_RELEASE:=3
PKG_INSTALL:=1

PKG_BUILD_DEPENDS:=+libdmalloc

include $(INCLUDE_DIR)/package.mk

define Package/efixo-libnbu
  TITLE:=Nbu library
  SECTION:=efixo
  CATEGORY:=Efixo library
  MAINTAINER:=Anthony VIALLARD
endef

define Package/efixo-libnbu/description
 This package contains the nbu library
endef

define Package/efixo-libnbu/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnbu.so $(1)/usr/lib/
endef

define Build/Prepare
	$(CP) src/* $(PKG_BUILD_DIR)
	$(call Build/Prepare/Default)
endef

define Build/InstallDev
	$(CP) $(PKG_INSTALL_DIR)/* $(1)
	$(INSTALL_DIR) $(1)/utests/nbu
	-$(CP) $(PKG_BUILD_DIR)/tests/test-* $(1)/utests/nbu/
endef

MAKE_FLAGS += prefix="$(PKG_INSTALL_DIR)/usr"

$(eval $(call BuildPackage,efixo-libnbu))
