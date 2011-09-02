# 
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=efixo-libewf
PKG_RELEASE:=1
PKG_INSTALL:=1

PKG_BUILD_DEPENDS:=+libdmalloc

include $(INCLUDE_DIR)/package.mk

define Package/efixo-libewf
  SECTION:=efixo
  CATEGORY:=Efixo applications
  TITLE:=Embedded Web Framework
  MAINTAINER:=Anthony VIALLARD & Raphael HUCK
  DEPENDS:=+libfcgi +clearsilver +efixo-libnbu +efixo-libetk \
	+libtropicssl +libezxml +libpcre
endef

define Package/efixo-libewf/description
 This package contains a web framework library
endef

define Package/efixo-libewf/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/libewf.so $(1)/usr/lib
endef

define Build/Prepare
	$(CP) src/* $(PKG_BUILD_DIR)
	$(call Build/Prepare/Default)
endef

define Build/InstallDev
	$(CP) $(PKG_INSTALL_DIR)/* $(1)
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include/ClearSilver
MAKE_FLAGS += prefix="$(PKG_INSTALL_DIR)/usr"

$(eval $(call BuildPackage,efixo-libewf))
