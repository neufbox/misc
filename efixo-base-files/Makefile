# 
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 19992 2011-03-25 13:03:37Z mgo $

include $(TOPDIR)/rules.mk

PKG_NAME:=efixo-base-files
PKG_RELEASE:=1

PKG_CONFIG_DEPENDS+=CONFIG_VALID

include $(INCLUDE_DIR)/package.mk

define Package/efixo-base-files-common/config
	source "$(SOURCE)/Config.in"
endef

define Package/efixo-base-files/Template
  define Package/efixo-base-files-$(1)
    SECTION:=efixo
    CATEGORY:=Efixo applications
    MAINTAINER:=Miguel GAIO
    TITLE:=neufbox $(1) base files
    DEPENDS:=$(2)
  endef

  define Package/efixo-base-files-$(1)/description
    This package contains $(1) neufbox base files
  endef

  define Package/efixo-base-files-$(1)/install
  	$(INSTALL_DIR) $$(1)
	$(CP) $$(PKG_BUILD_DIR)/$(1)/* $$(1)/
  endef

  $$(eval $$(call BuildPackage,efixo-base-files-$(1)))
endef

define Build/Prepare
	$(CP) files/* $(PKG_BUILD_DIR)
	$(call Build/Prepare/Default)
	echo $(CONFIG_HOSTNAME) > $(PKG_BUILD_DIR)/common/etc/hostname
	$(if $(CONFIG_VALID),$(SED) 's/^root:[^:]*:/root::/' $(PKG_BUILD_DIR)/common/etc/shadow)
	$(SED) 's,$$$$OPERATOR_DOMAIN,$(call qstrip,$(CONFIG_OPERATOR_DOMAIN)),g' $(PKG_BUILD_DIR)/main/etc/syslog-ng/syslog-ng.conf
	$(SED) 's,$$$$OPERATOR_DOMAIN,$(call qstrip,$(CONFIG_OPERATOR_DOMAIN)),g' $(PKG_BUILD_DIR)/rescue/usr/sbin/autoconf.sh
	$(SED) 's,$$$$E,$(call qstrip,$(CONFIG_FIRMWARE_RELEASE)),g' $(PKG_BUILD_DIR)/rescue/usr/sbin/autoconf.sh
	$(SED) 's,$$$$E,$(call qstrip,$(CONFIG_FIRMWARE_RELEASE)),g' $(PKG_BUILD_DIR)/rescue/etc/init.d/dhcpc
endef

define Build/Configure
endef

define Build/Compile
endef

$(eval $(call Package/efixo-base-files/Template,common))
$(eval $(call Package/efixo-base-files/Template,main,efixo-base-files-common))
$(eval $(call Package/efixo-base-files/Template,nb4-main,@TARGET_brcm63xx efixo-base-files-main))
$(eval $(call Package/efixo-base-files/Template,nb5-main,@TARGET_octeon efixo-base-files-main))
$(eval $(call Package/efixo-base-files/Template,nb5-debug,@TARGET_octeon efixo-base-files-main))
$(eval $(call Package/efixo-base-files/Template,nb5-refurbishing,@TARGET_octeon efixo-base-files-main))
$(eval $(call Package/efixo-base-files/Template,adsl,@neufbox_net_infra_adsl efixo-base-files-main))
$(eval $(call Package/efixo-base-files/Template,gprs,@neufbox_net_infra_gprs efixo-base-files-main))
$(eval $(call Package/efixo-base-files/Template,ipv6,efixo-base-files-main))
$(eval $(call Package/efixo-base-files/Template,nb6-main,@TARGET_brcm63xx efixo-base-files-main))
$(eval $(call Package/efixo-base-files/Template,rescue,efixo-base-files-common))
$(eval $(call Package/efixo-base-files/Template,sound,efixo-base-files-main +kmod-usb-audio))
