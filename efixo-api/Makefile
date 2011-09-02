# 
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 10749 2008-04-07 00:24:19Z nbd $

include $(TOPDIR)/rules.mk

PKG_NAME:=efixo-api
PKG_RELEASE:=1
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

ifeq ($(DUMP),)
  STAMP_CONFIGURED:=$(strip $(STAMP_CONFIGURED))_$(shell grep '^CONFIG_efixo-www-api-' $(TOPDIR)/.config | md5s)
endif

define Package/efixo-api/config
	source "$(SOURCE)/src/Config.in"
endef

define Package/efixo-api
  SECTION:=efixo
  CATEGORY:=Efixo applications
  TITLE:=Neufbox API
  MAINTAINER:=Anthony VIALLARD & Raphael HUCK
  DEPENDS:=+libfcgi +clearsilver +efixo-headers +efixo-libnbu +efixo-libnbd +efixo-libetk +libinklevel +libtropicssl +libezxml +libpcre
endef

define Package/efixo-api/description
 This package contains the API of the neufbox
endef

export TOPDIR

define Build/Prepare
	$(CP) src/* $(PKG_BUILD_DIR)
	$(call Build/Prepare/Default)
endef

MAKE_FLAGS+=prefix="$(PKG_INSTALL_DIR)/usr"
MAKE_VARS+=ROOT=$(PKG_INSTALL_DIR)

define Build/InstallDev
	mkdir -p $(1)
	$(CP) $(PKG_INSTALL_DIR)/* $(1)
endef

define Package/efixo-api/install
	$(CP) $(PKG_INSTALL_DIR)/* $(1)
endef

$(eval $(call BuildPackage,efixo-api))
