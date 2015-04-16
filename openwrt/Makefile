include $(TOPDIR)/rules.mk

PKG_NAME:=BaiduPCS
PKG_VERSION:=0.2.0
PKG_RELEASE:=$(PKG_SOURCE_VERSION)

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=https://github.com/GangZhuo/BaiduPCS/archive
PKG_SOURCE:=$(PKG_VERSION).tar.gz
PKG_BUILD_DEPENDS:=libopenssl curl

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/BaiduPCS
    SECTION:=utils
    CATEGORY:=Utilities
    TITLE:=BaiduPCS
    URL:=https://github.com/GangZhuo/BaiduPCS
	MAINTAINER:=Gang Zhuo <gang.zhuo@gmail.com>
	DEPENDS:=libopenssl curl
endef

define Package/BaiduPCS/description
    The client for baidu net disk.
endef

define Package/BaiduPCS/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/pcs $(1)/usr/bin
endef

$(eval $(call BuildPackage,BaiduPCS))
