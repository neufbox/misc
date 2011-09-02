# 
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 10749 2008-04-07 00:24:19Z lan-topology $

include $(TOPDIR)/rules.mk

PKG_NAME:=efixo-lan-topology
PKG_RELEASE:=2

include $(INCLUDE_DIR)/package.mk

define Package/efixo-lan-topology
  TITLE:=lan-topology
  SECTION:=efixo
  CATEGORY:=Efixo applications
  MAINTAINER:=Miguel GAIO
  DEPENDS:=+librt +libnl-tiny +efixo-libetk +efixo-libnbd
endef

define Package/efixo-lan-topology/description
 This package contains tool for lan-topology
endef

define Build/Prepare
	$(CP) src/* $(PKG_BUILD_DIR)
	$(call Build/Prepare/Default)
endef

define Package/efixo-lan-topology/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lan-topology $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/lan-topology.init $(1)/etc/init.d/lan-topology
endef

$(eval $(call BuildPackage,efixo-lan-topology))
