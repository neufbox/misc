# 
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: $

include $(TOPDIR)/rules.mk

PKG_NAME:=efixo-www
PKG_RELEASE:=1
PKG_INSTALL:=1

PKG_BUILD_DEPENDS:=efixo-www/host +libdmalloc

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/www_tools

ifeq ($(DUMP),)
  STAMP_CONFIGURED:=$(strip $(STAMP_CONFIGURED))_$(shell grep '^CONFIG_efixo-www-page-' $(TOPDIR)/.config | md5s)
endif

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/package.mk

DEPS:=+libfcgi +clearsilver +efixo-libewf +efixo-headers +efixo-libnbu \
	+efixo-libnbd +libtropicssl +libezxml +libpcre +efixo-libetk

define Package/efixo-www/config
	source "$(SOURCE)/src/Config.in"
endef

define Package/efixo-www
  SECTION:=efixo
  CATEGORY:=Efixo applications
  TITLE:=Neufbox Web User Interface
  MAINTAINER:=Anthony VIALLARD & Raphael HUCK
  DEPENDS:=$(DEPS)
endef

define Package/efixo-www/description
 This package contains the web user interface of the neufbox
endef

export TOPDIR

define Build/Prepare
	$(CP) src/* $(PKG_BUILD_DIR)
	ln -sf $(CONFIG_WEBUI_THEME) $(PKG_BUILD_DIR)/docroot/themes/default
	find $(PKG_BUILD_DIR)/fcgiroot/lng -name "*.hdf" -exec $(SED) 's,$$$$OPERATORNAME,$(call qstrip,$(CONFIG_WEBUI_OPERATOR_NAME)),g' {} \;
	find $(PKG_BUILD_DIR)/fcgiroot/lng -name "*.hdf" -exec $(SED) 's,$$$$BOXNAME,$(call qstrip,$(CONFIG_WEBUI_BOX_NAME)),g' {} \;
	find $(PKG_BUILD_DIR)/fcgiroot/lng -name "*.hdf" -exec $(SED) 's,$$$$HOTSPOTCONDITIONSURL,$(call qstrip,$(CONFIG_WEBUI_HOTSPOT_CONDITIONS_URL)),g' {} \;
	$(call Build/Prepare/Default)
endef

MAKE_FLAGS+=prefix="$(PKG_INSTALL_DIR)/usr"
MAKE_VARS+=ROOT=$(PKG_INSTALL_DIR)

define Build/InstallDev
	mkdir -p $(1)/www
	$(CP) $(PKG_BUILD_DIR)/.buildroot/* $(1)/www
	$(CP) $(PKG_BUILD_DIR)/etc $(1)/www
endef

define Package/efixo-www/install
	$(CP) $(PKG_INSTALL_DIR)/* $(1)
endef

# host tools

define Host/Prepare
	$(CP) src/tools/* $(HOST_BUILD_DIR)
	$(call Host/Prepare/Default)
endef

define Host/Compile
        $(MAKE) -C $(HOST_BUILD_DIR) all
endef

define Host/Install
        $(MAKE) -C $(HOST_BUILD_DIR) install ROOT=$(STAGING_DIR_HOST)
endef

$(eval $(call RequireCommand,gpp, \
	Please install gpp \
))

$(eval $(call HostBuild))
$(eval $(call BuildPackage,efixo-www))
