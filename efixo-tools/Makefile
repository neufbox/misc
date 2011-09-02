# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 10749 2008-04-07 00:24:19Z libnbu $

include $(TOPDIR)/rules.mk

PKG_NAME:=efixo-tools
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/efixo-tools
  SECTION:=efixo
  CATEGORY:=Efixo applications
  TITLE:=Neufbox various tools
  MAINTAINER:=Miguel GAIO
  DEPENDS:=+efixo-libetk
endef

define Package/efixo-tools/description
 This package contains serveral tools from Efixo
endef

define Package/efixo-tools-rescue
$(call Package/efixo-tools)
TITLE:=Neufbox rescue tools
endef

define Build/Prepare
	$(CP) $(LINUX_DIR)/include/neufbox/ $(STAGING_DIR)/usr/include/
	$(CP) src/* $(PKG_BUILD_DIR)
	$(call Build/Prepare/Default)
endef

EXTRA_CFLAGS+=-DOPERATOR_DOMAIN='\"$(CONFIG_OPERATOR_DOMAIN)\"'
define Package/efixo-tools/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/iomem $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ledctl $(1)/usr/bin/
endef

define Package/efixo-tools-rescue/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mac_addr $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/spy-send $(1)/usr/bin/
endef

$(eval $(call BuildPackage,efixo-tools))
$(eval $(call BuildPackage,efixo-tools-rescue))
