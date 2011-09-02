# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=efixo-headers
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/efixo-headers
  SECTION:=efixo
  CATEGORY:=Efixo applications
  TITLE:=Neufbox headers.
  MAINTAINER:=Miguel GAIO
  BUILDONLY:=1
endef

define Package/efixo-headers/description
 This package contains all headers to build neufbox firmware.
endef

define Build/Prepare
endef

define Build/Compile
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) src/include/nbh $(1)/usr/include/
endef

define Package/efixo-headers/install
endef

$(eval $(call BuildPackage,efixo-headers))
