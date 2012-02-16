/*
 * Routines for discovering the current state of network interfaces
 *
 * Copyright (C) 2009-2010 Olaf Kirch <okir@suse.de>
 */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <wicked/nis.h>
#include <wicked/resolver.h>
#include "netinfo_priv.h"
#include "config.h"

#define CONFIG_WICKED_BACKUP_DIR	CONFIG_WICKED_STATEDIR "/backup"

ni_netconfig_t *
ni_global_state_handle(int refresh)
{
	static ni_netconfig_t *nc = NULL;

	if (nc == NULL) {
		if (__ni_global_netlink == NULL) {
			__ni_global_netlink = __ni_netlink_open(0);
			if (__ni_global_netlink == NULL)
				return NULL;
		}

		nc = calloc(1, sizeof(*nc));
	}
	if (refresh && __ni_system_refresh_interfaces(nc) < 0) {
		ni_error("failed to refresh interface list");
		return NULL;
	}

	return nc;
}

int
__ni_system_hostname_get(char *buffer, size_t size)
{
	return gethostname(buffer, size);
}


int
__ni_system_hostname_put(const char *hostname)
{
	if (!hostname || !*hostname) {
		errno = EINVAL;
		return -1;
	}
	return sethostname(hostname, strlen(hostname));
}

int
__ni_system_nis_domain_get(char *buffer, size_t size)
{
	return getdomainname(buffer, size);
}


int
__ni_system_nis_domain_put(const char *domainname)
{
	if (!domainname || !*domainname)
		return setdomainname("", 0);
	return setdomainname(domainname, strlen(domainname));
}

ni_nis_info_t *
__ni_system_nis_get(void)
{
	char domainname[256];
	ni_nis_info_t *nis;

	if ((nis = ni_nis_parse_yp_conf(_PATH_YP_CONF)) == NULL)
		return NULL;

	if (nis->domainname == NULL
	 && getdomainname(domainname, sizeof(domainname)) >= 0)
		ni_string_dup(&nis->domainname, domainname);

	return nis;
}

int
__ni_system_nis_put(const ni_nis_info_t *nis)
{
	const char *tempfile = _PATH_YP_CONF ".new";

	if (ni_nis_write_yp_conf(tempfile, nis, NULL) < 0) {
		unlink(tempfile);
		return -1;
	}
	if (rename(tempfile, _PATH_YP_CONF) < 0) {
		ni_error("cannot move temp file to %s: %m", _PATH_YP_CONF);
		unlink(tempfile);
		return -1;
	}

	if (__ni_system_nis_domain_put(nis->domainname) < 0) {
		ni_error("cannot set domainname: %m");
		return -1;
	}

	return 0;
}

int
__ni_system_nis_backup(void)
{
	return ni_backup_file_to(_PATH_YP_CONF, CONFIG_WICKED_BACKUP_DIR);
}

int
__ni_system_nis_restore(void)
{
	__ni_system_nis_domain_put(NULL);
	return ni_restore_file_from(_PATH_YP_CONF, CONFIG_WICKED_BACKUP_DIR);
}

ni_resolver_info_t *
__ni_system_resolver_get(void)
{
	return ni_resolver_parse_resolv_conf(_PATH_RESOLV_CONF);
}

int
__ni_system_resolver_put(const ni_resolver_info_t *resolver)
{
	const char *tempfile = _PATH_RESOLV_CONF ".new";

	if (ni_resolver_write_resolv_conf(tempfile, resolver, NULL) < 0) {
		unlink(tempfile);
		return -1;
	}
	if (rename(tempfile, _PATH_RESOLV_CONF) < 0) {
		ni_error("cannot move temp file to %s: %m", _PATH_RESOLV_CONF);
		unlink(tempfile);
		return -1;
	}

	return 0;
}

int
__ni_system_resolver_backup(void)
{
	return ni_backup_file_to(_PATH_RESOLV_CONF, CONFIG_WICKED_BACKUP_DIR);
}

int
__ni_system_resolver_restore(void)
{
	return ni_restore_file_from(_PATH_RESOLV_CONF, CONFIG_WICKED_BACKUP_DIR);
}
